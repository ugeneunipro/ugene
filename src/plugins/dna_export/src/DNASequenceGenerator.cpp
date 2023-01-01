/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "DNASequenceGenerator.h"

#include <QRandomGenerator>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FileFilters.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Timer.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Gui/OpenViewTask.h>

static const int MBYTE_TO_BYTE = 1048576;

namespace U2 {

const QString DNASequenceGenerator::ID("dna_generator");

QString DNASequenceGenerator::prepareReferenceFileFilter() {
    return FileFilters::createFileFilterByObjectTypes({GObjectTypes::SEQUENCE, GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT});
}

void DNASequenceGenerator::generateSequence(const QMap<char, qreal>& charFreqs,
                                            int length,
                                            QByteArray& result,
                                            QRandomGenerator& randomGenerator,
                                            U2OpStatus& os) {
    GTIMER(cvar, tvar, "DNASequenceGenerator::generateSequence");
    SAFE_POINT(!charFreqs.isEmpty(), "charFreqs is empty", );
    SAFE_POINT(length >= 0, "Invalid sequence length: " + QString::number(length), );
    CHECK_EXT(length > 0, result.clear(), );

    QList<char> chars = charFreqs.keys();
    QVector<char> characterBySlotIndex;
    int sensitivity = 1000;  // Round after 3 digits.
    characterBySlotIndex.reserve(sensitivity);
    for (char ch : qAsConst(chars)) {
        qreal frequency = charFreqs[ch];
        SAFE_POINT(frequency >= 0 && frequency <= 1, "Invalid character frequency: " + QString(ch) + "=" + QString::number(frequency), );
        int nSlotsPerCharacter = (int)qRound64(frequency * sensitivity);
        for (int i = 0; i < nSlotsPerCharacter; i++) {
            characterBySlotIndex.append(ch);
        }
    }
    result.resize(length);
    CHECK_EXT(result.size() == length, os.setError(GenerateDNASequenceTask::tr("Failed to allocate memory for the result sequence.")), )

    for (int idx = 0; idx < length; idx++) {
        int randomCharIndex = (int)(randomGenerator.generate() % characterBySlotIndex.length());
        char ch = characterBySlotIndex[randomCharIndex];
        result[idx] = ch;
    }
}

static void evaluate(const QByteArray& seq, QMap<char, qreal>& result) {
    QMap<char, int> occurrencesMap;
    for (char ch : qAsConst(seq)) {
        if (!occurrencesMap.contains(ch)) {
            occurrencesMap.insert(ch, 1);
        } else {
            ++occurrencesMap[ch];
        }
    }

    qreal len = seq.length();
    QMapIterator<char, int> i(occurrencesMap);
    while (i.hasNext()) {
        i.next();
        int count = i.value();
        qreal freq = count / len;
        result.insertMulti(i.key(), freq);
    }
}

void DNASequenceGenerator::evaluateBaseContent(const DNASequence& sequence, QMap<char, qreal>& result) {
    evaluate(sequence.seq, result);
}

void DNASequenceGenerator::evaluateBaseContent(const MultipleSequenceAlignment& ma, QMap<char, qreal>& result) {
    QList<QMap<char, qreal>> rowsContents;
    foreach (const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        QMap<char, qreal> rowContent;
        evaluate(row->getData(), rowContent);
        rowsContents.append(rowContent);
    }

    QListIterator<QMap<char, qreal>> listIter(rowsContents);
    while (listIter.hasNext()) {
        const QMap<char, qreal>& cm = listIter.next();
        QMapIterator<char, qreal> mapIter(cm);
        while (mapIter.hasNext()) {
            mapIter.next();
            char ch = mapIter.key();
            qreal freq = mapIter.value();
            if (!result.contains(ch)) {
                result.insertMulti(ch, freq);
            } else {
                result[ch] += freq;
            }
        }
    }

    int rowsNum = ma->getRowCount();
    QMutableMapIterator<char, qreal> i(result);
    while (i.hasNext()) {
        i.next();
        i.value() /= rowsNum;
    }
}

// DNASequenceGeneratorTask
//////////////////////////////////////////////////////////////////////////

/** Returns estimated progress weight per phase in DNASequenceGeneratorTask. */
static float getTaskProgressWeightPerPhase(const DNASequenceGeneratorConfig& config) {
    int nPhases = 2;  // Generate + Save.
    if (config.useReference()) {
        nPhases += 2;  // Load + Evaluate.
    }
    if (config.addToProj) {
        nPhases += 1;  // Add to project.
    }
    return (float)(1.0 / nPhases);
}

EvaluateBaseContentTask* DNASequenceGeneratorTask::createEvaluationTask(Document* doc, QString& err) {
    SAFE_POINT(doc->isLoaded(), "Document must be loaded", nullptr);
    QList<GObject*> gobjects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    gobjects << doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (!gobjects.isEmpty()) {
        return new EvaluateBaseContentTask(gobjects.first());
    }
    err = tr("Reference for sequence generator has to be a sequence or an alignment");
    return nullptr;
}

DNASequenceGeneratorTask::DNASequenceGeneratorTask(const DNASequenceGeneratorConfig& cfg)
    : Task(tr("Generate sequence task"), TaskFlag_NoRun),
      cfg(cfg),
      loadRefTask(nullptr),
      evalTask(nullptr),
      generateTask(nullptr),
      saveTask(nullptr) {
    GCOUNTER(cvar, "DNASequenceGeneratorTask");
    if (cfg.useReference()) {
        // do not load reference file if it is already in project and has loaded state
        const QString& docUrl = cfg.getReferenceUrl();
        Project* project = AppContext::getProject();
        if (project) {
            Document* doc = project->findDocumentByURL(docUrl);
            if (doc && doc->isLoaded()) {
                QString err;
                evalTask = createEvaluationTask(doc, err);
                if (evalTask) {
                    addSubTask(evalTask);
                } else {
                    stateInfo.setError(err);
                }
                return;
            }
        }

        loadRefTask = LoadDocumentTask::getDefaultLoadDocTask(stateInfo, GUrl(docUrl));
        CHECK_OP(stateInfo, );
        SAFE_POINT_EXT(loadRefTask != nullptr, stateInfo.setError(tr("Incorrect reference file: %1").arg(docUrl)), );
        loadRefTask->setSubtaskProgressWeight(getTaskProgressWeightPerPhase(cfg));
        addSubTask(loadRefTask);
    } else {
        generateTask = new GenerateDNASequenceTask(cfg.getContent(), cfg.getLength(), cfg.window, cfg.getNumberOfSequences(), cfg.seed);
        generateTask->setSubtaskProgressWeight(getTaskProgressWeightPerPhase(cfg));
        addSubTask(generateTask);
    }
}

QList<Task*> DNASequenceGeneratorTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> tasks;
    propagateSubtaskError();
    if (hasError() || isCanceled() || subTask->isCanceled()) {
        return tasks;
    }

    if (subTask == loadRefTask) {
        tasks << onLoadRefTaskFinished();
    } else if (subTask == evalTask) {
        tasks << onEvalTaskFinished();
    } else if (subTask == generateTask) {
        tasks << onGenerateTaskFinished();
    } else if (saveTask == subTask) {
        tasks << onSaveTaskFinished();
    }
    for (Task* task : tasks) {
        task->setSubtaskProgressWeight(getTaskProgressWeightPerPhase(cfg));
    }
    return tasks;
}

QList<Task*> DNASequenceGeneratorTask::onLoadRefTaskFinished() {
    QList<Task*> resultTasks;
    SAFE_POINT(loadRefTask->isFinished() && !loadRefTask->getStateInfo().isCoR(),
               "Invalid task encountered",
               resultTasks);
    QString err;
    Document* doc = loadRefTask->getDocument();
    evalTask = createEvaluationTask(doc, err);
    if (evalTask) {
        resultTasks << evalTask;
    } else {
        stateInfo.setError(err);
    }
    return resultTasks;
}

QList<Task*> DNASequenceGeneratorTask::onEvalTaskFinished() {
    QList<Task*> resultTasks;
    SAFE_POINT(evalTask->isFinished() && !evalTask->getStateInfo().isCoR(),
               "Invalid task encountered",
               resultTasks);
    cfg.alphabet = evalTask->getAlphabet();
    QMap<char, qreal> content = evalTask->getResult();
    generateTask = new GenerateDNASequenceTask(content, cfg.getLength(), cfg.window, cfg.getNumberOfSequences(), cfg.seed);
    resultTasks << generateTask;
    return resultTasks;
}

QList<Task*> DNASequenceGeneratorTask::onGenerateTaskFinished() {
    QList<Task*> resultTasks;
    SAFE_POINT(generateTask->isFinished() && !generateTask->getStateInfo().isCoR(),
               "Invalid task encountered",
               resultTasks);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(
        IOAdapterUtils::url2io(cfg.getOutUrlString()));

    if (cfg.saveDoc) {
        DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(cfg.getDocumentFormatId());
        Document* doc = format->createNewLoadedDocument(iof, cfg.getOutUrlString(), stateInfo);
        CHECK_OP(stateInfo, resultTasks);
        const QSet<QString>& supportedFormats = format->getSupportedObjectTypes();
        bool isSequenceFormat = supportedFormats.contains(GObjectTypes::SEQUENCE);
        if (isSequenceFormat) {
            addSequencesToSeqDoc(doc);
        } else {  // consider alignment format
            SAFE_POINT(supportedFormats.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT),
                       "Unexpected format encountered",
                       resultTasks);
            addSequencesToMsaDoc(doc);
        }
        saveTask = new SaveDocumentTask(doc);
        resultTasks << saveTask;
    } else {  // TODO: avoid high memory consumption here
        const DNAAlphabet* alp = cfg.getAlphabet();
        SAFE_POINT(alp != nullptr, "Generated sequence has invalid alphabet", resultTasks);
        U2DbiRef dbiRef = generateTask->getDbiRef();
        QString baseSeqName = cfg.getSequenceName();
        QList<U2Sequence> seqs = generateTask->getResults();

        for (int sequenceIndex = 0, totalSeqCount = seqs.size(); sequenceIndex < totalSeqCount; sequenceIndex++) {
            QString seqName = totalSeqCount > 1 ? baseSeqName + " " + QString::number(sequenceIndex + 1) : baseSeqName;
            DbiConnection con(dbiRef, stateInfo);
            CHECK_OP(stateInfo, resultTasks);
            U2SequenceDbi* sequenceDbi = con.dbi->getSequenceDbi();
            QByteArray seqContent = sequenceDbi->getSequenceData(seqs[sequenceIndex].id, U2_REGION_MAX, stateInfo);
            results << DNASequence(seqName, seqContent, alp);
        }
    }
    return resultTasks;
}

void DNASequenceGeneratorTask::addSequencesToMsaDoc(Document* source) {
    const QSet<QString>& supportedFormats = source->getDocumentFormat()->getSupportedObjectTypes();
    SAFE_POINT(supportedFormats.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT),
               "Invalid document format", );
    SAFE_POINT(generateTask != nullptr, "Invalid generate task", );
    U2DbiRef dbiRef = generateTask->getDbiRef();
    const DNAAlphabet* alp = cfg.alphabet;
    SAFE_POINT(alp != nullptr, "Generated sequence has invalid alphabet", );
    QString baseSeqName = cfg.getSequenceName();
    QList<U2Sequence> seqs = generateTask->getResults();

    MultipleSequenceAlignment msa(tr("Generated MSA"), alp);
    DbiConnection con(dbiRef, stateInfo);

    for (int sequenceIndex = 0, totalSeqCount = seqs.size(); sequenceIndex < totalSeqCount; sequenceIndex++) {
        QString seqName = totalSeqCount > 1 ? baseSeqName + " " + QString::number(sequenceIndex + 1) : baseSeqName;
        // TODO: large sequences will cause out of memory error here
        QByteArray seqContent = con.dbi->getSequenceDbi()->getSequenceData(seqs[sequenceIndex].id, U2_REGION_MAX, stateInfo);
        msa->addRow(seqName, seqContent, sequenceIndex);
    }
    MultipleSequenceAlignmentObject* alnObject = MultipleSequenceAlignmentImporter::createAlignment(source->getDbiRef(), msa, stateInfo);
    CHECK_OP(stateInfo, );
    source->addObject(alnObject);
}

void DNASequenceGeneratorTask::addSequencesToSeqDoc(Document* source) {
    const QSet<QString>& supportedFormats = source->getDocumentFormat()->getSupportedObjectTypes();
    SAFE_POINT(supportedFormats.contains(GObjectTypes::SEQUENCE), "Invalid document format", );
    SAFE_POINT(generateTask != nullptr, "Invalid generate task", );
    U2DbiRef dbiRef = generateTask->getDbiRef();
    QString baseSeqName = cfg.getSequenceName();
    QList<U2Sequence> seqs = generateTask->getResults();

    for (int sequenceIndex = 0, totalSeqCount = seqs.size(); sequenceIndex < totalSeqCount; sequenceIndex++) {
        QString seqName = totalSeqCount > 1 ? baseSeqName + " " + QString::number(sequenceIndex + 1) : baseSeqName;
        U2EntityRef entityRef(dbiRef, seqs[sequenceIndex].id);
        source->addObject(new U2SequenceObject(seqName, entityRef));
    }
}

QList<Task*> DNASequenceGeneratorTask::onSaveTaskFinished() {
    Document* doc = saveTask->getDocument();
    QList<Task*> resultTasks;

    if (!cfg.addToProj) {
        doc->unload();
        delete doc;
    } else {
        Project* prj = AppContext::getProject();
        if (prj) {
            Document* d = prj->findDocumentByURL(doc->getURL());
            if (d == nullptr) {
                prj->addDocument(doc);
                resultTasks << new OpenViewTask(doc);
            } else {
                // if document with same url is already exists in project
                // it will be reloaded by DocumentUpdater => delete this instance
                doc->unload();
                delete doc;
            }
        } else {
            Task* openWithProjectTask = AppContext::getProjectLoader()->openWithProjectTask(QList<GUrl>() << doc->getURL());
            if (openWithProjectTask != nullptr) {
                resultTasks << openWithProjectTask;
            }
            // open project task will load supplied url
            doc->unload();
            delete doc;
        }
    }
    return resultTasks;
}

// EvaluateBaseContentTask
EvaluateBaseContentTask::EvaluateBaseContentTask(GObject* obj)
    : Task(tr("Evaluate base content task"), TaskFlag_None), _obj(obj), alp(nullptr) {
}

void EvaluateBaseContentTask::run() {
    if (_obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        auto dnaObj = qobject_cast<U2SequenceObject*>(_obj);
        alp = dnaObj->getAlphabet();
        DNASequenceGenerator::evaluateBaseContent(dnaObj->getWholeSequence(stateInfo), result);
    } else if (_obj->getGObjectType() == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
        auto maObj = qobject_cast<MultipleSequenceAlignmentObject*>(_obj);
        alp = maObj->getAlphabet();
        DNASequenceGenerator::evaluateBaseContent(maObj->getMultipleAlignment(), result);
    } else {
        stateInfo.setError(tr("Base content can be evaluated for sequence or sequence alignment"));
    }
}

// GenerateTask
GenerateDNASequenceTask::GenerateDNASequenceTask(const QMap<char, qreal>& baseContent_,
                                                 int length_,
                                                 int window_,
                                                 int count_,
                                                 int seed_)
    : Task(tr("Generate DNA sequence task"), TaskFlag_None), baseContent(baseContent_),
      length(length_), window(window_), count(count_), seed(seed_), dbiRef() {
    tpm = Progress_Manual;
    window = qMin(window, length);
}

void GenerateDNASequenceTask::prepare() {
    int memUseMB = window / MBYTE_TO_BYTE;
    coreLog.trace(QString("Generate DNA sequence task: Memory resource %1").arg(memUseMB));
    addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB));
}

void GenerateDNASequenceTask::run() {
    GTIMER(cvar, tvar, "GenerateDNASequenceTask");
    dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    CHECK_OP(stateInfo, );
    DbiConnection con(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    results.reserve(count);

    QRandomGenerator generator(seed >= 0 ? (quint32)seed : QRandomGenerator::system()->generate());
    for (int sequenceIndex = 0; sequenceIndex < count; sequenceIndex++) {
        U2SequenceImporter seqImporter(QVariantMap(), true);

        QByteArray sequenceChunk;
        seqImporter.startSequence(stateInfo, dbiRef, U2ObjectDbi::ROOT_FOLDER, "sequence_" + QString::number(sequenceIndex + 1), false);
        CHECK_OP_BREAK(stateInfo);

        for (int chunkCount = 0; chunkCount < length / window && !isCanceled(); chunkCount++) {
            DNASequenceGenerator::generateSequence(baseContent, window, sequenceChunk, generator, stateInfo);
            CHECK_OP_BREAK(stateInfo);
            seqImporter.addBlock(sequenceChunk.constData(), sequenceChunk.length(), stateInfo);
            CHECK_OP_BREAK(stateInfo);
            int currentProgress = int(100 * (sequenceIndex + chunkCount * (double)window / length) / count);
            stateInfo.setProgress(currentProgress);
        }
        CHECK_OP_BREAK(stateInfo);

        // Add the last chunk.
        if (length % window > 0) {
            DNASequenceGenerator::generateSequence(baseContent, length % window, sequenceChunk, generator, stateInfo);
            CHECK_OP_BREAK(stateInfo);
            seqImporter.addBlock(sequenceChunk.constData(), sequenceChunk.length(), stateInfo);
            CHECK_OP_BREAK(stateInfo);
        }

        U2Sequence seq = seqImporter.finalizeSequence(stateInfo);
        CHECK_OP_BREAK(stateInfo);
        results.append(seq);

        stateInfo.setProgress(int(100 * (double)sequenceIndex / count));
    }
}

}  // namespace U2
