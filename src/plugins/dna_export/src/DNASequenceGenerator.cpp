/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Gui/OpenViewTask.h>

#include <U2Gui/DialogUtils.h>

#include "DNASequenceGenerator.h"

static const int MBYTE_TO_BYTE = 1048576;

namespace U2 {

int getRandomInt(int max) {
    return qrand() % (max + 1);
}

const QString DNASequenceGenerator::ID("dna_generator");

QString DNASequenceGenerator::prepareReferenceFileFilter() {
    QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true) +
        ";;" + DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, false);
    return filter;
}

void DNASequenceGenerator::generateSequence(const QMap<char, qreal>& charFreqs, int length, QByteArray& result) {
    QMap<char, int> content;
    QMapIterator<char, qreal> iter(charFreqs);
    assert(iter.hasNext());
    iter.next();
    char ch = iter.key();
    int total = 0;
    while (iter.hasNext()) {
        iter.next();
        char ch = iter.key();
        qreal percent = iter.value();
        assert(percent>=0 && percent<=1);
        int num = length * percent;
        total += num;
        content.insertMulti(ch, num);
    }
    assert(total>=0 && total<=length);
    content[ch] = length - total;

    result.resize(length);
    for (int idx = 0; idx < length; idx++) {
        int rnd = getRandomInt(length - idx - 1);
        assert(rnd < length - idx);
        int charRange = 0;
        foreach(char ch, content.keys()) {
            charRange += content.value(ch);
            if (rnd < charRange) {
                --content[ch];
                result[idx] = ch;
                break;
            }
        }
    }
}

void evaluate(const QByteArray& seq, QMap<char, qreal>& result) {
    QMap<char, int> occurrencesMap;
    foreach(char ch, seq) {
        if (!occurrencesMap.keys().contains(ch)) {
            occurrencesMap.insertMulti(ch, 1);
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
    QList< QMap<char, qreal> > rowsContents;
    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        QMap<char, qreal> rowContent;
        evaluate(row->getData(), rowContent);
        rowsContents.append(rowContent);
    }

    QListIterator< QMap<char, qreal> > listIter(rowsContents);
    while (listIter.hasNext()) {
        const QMap<char, qreal>& cm = listIter.next();
        QMapIterator<char, qreal> mapIter(cm);
        while (mapIter.hasNext()) {
            mapIter.next();
            char ch = mapIter.key();
            qreal freq = mapIter.value();
            if (!result.keys().contains(ch)) {
                result.insertMulti(ch, freq);
            } else {
                result[ch] += freq;
            }
        }
    }

    int rowsNum = ma->getNumRows();
    QMutableMapIterator<char, qreal> i(result);
    while (i.hasNext()) {
        i.next();
        i.value() /= rowsNum;
    }
}

// DNASequenceGeneratorTask
//////////////////////////////////////////////////////////////////////////

EvaluateBaseContentTask* DNASequenceGeneratorTask::createEvaluationTask(Document* doc, QString& err) {
    assert(doc->isLoaded());
    QList<GObject*> gobjects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    gobjects << doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (!gobjects.isEmpty()) {
        return new EvaluateBaseContentTask(gobjects.first());
    }
    err = tr("Reference for sequence generator has to be a sequence or an alignment");
    return NULL;
}

DNASequenceGeneratorTask::DNASequenceGeneratorTask(const DNASequenceGeneratorConfig& cfg)
    : Task(tr("Generate sequence task"), TaskFlag_NoRun),
      cfg(cfg),
      loadRefTask(NULL),
      evalTask(NULL),
      generateTask(NULL),
      saveTask(NULL) {
    GCOUNTER( cvar, tvar, "DNASequenceGeneratorTask" );
    if (cfg.useReference()) {
        // do not load reference file if it is already in project and has loaded state
        const QString& docUrl = cfg.getReferenceUrl();
        Project* prj = AppContext::getProject();
        if (prj) {
            Document* doc = prj->findDocumentByURL(docUrl);
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

        loadRefTask = LoadDocumentTask::getDefaultLoadDocTask(GUrl(docUrl));
        if (loadRefTask) {
            addSubTask(loadRefTask);
        } else {
            stateInfo.setError(tr("Incorrect reference file"));
            return;
        }
    } else {
        generateTask = new GenerateDNASequenceTask(cfg.getContent(), cfg.getLength(), cfg.window, cfg.getNumberOfSequences(), cfg.seed);
        addSubTask(generateTask);
    }
}

QList<Task*> DNASequenceGeneratorTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> tasks;
    if (hasError() || isCanceled() || subTask->isCanceled()) {
        return tasks;
    }
    propagateSubtaskError();

    if (subTask == loadRefTask) {
        tasks << onLoadRefTaskFinished( );
    } else if (subTask == evalTask) {
        tasks << onEvalTaskFinished( );
    } else if ( subTask == generateTask ) {
        tasks << onGenerateTaskFinished( );
    } else if (saveTask == subTask) {
        tasks << onSaveTaskFinished( );
    }
    return tasks;
}

QList<Task *> DNASequenceGeneratorTask::onLoadRefTaskFinished( ) {
    QList<Task *> resultTasks;
    SAFE_POINT( loadRefTask->isFinished( ) && !loadRefTask->getStateInfo( ).isCoR( ),
        "Invalid task encountered", resultTasks );
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

QList<Task*> DNASequenceGeneratorTask::onEvalTaskFinished( ) {
    QList<Task *> resultTasks;
    SAFE_POINT( evalTask->isFinished( ) && !evalTask->getStateInfo( ).isCoR( ),
        "Invalid task encountered", resultTasks );
    cfg.alphabet = evalTask->getAlphabet();
    QMap<char, qreal> content = evalTask->getResult();
    generateTask = new GenerateDNASequenceTask(content, cfg.getLength(), cfg.window,
        cfg.getNumberOfSequences(), cfg.seed);
    resultTasks << generateTask;
    return resultTasks;
}

QList<Task*> DNASequenceGeneratorTask::onGenerateTaskFinished( ) {
    QList<Task *> resultTasks;
    SAFE_POINT( generateTask->isFinished( ) && !generateTask->getStateInfo( ).isCoR( ),
        "Invalid task encountered", resultTasks );
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry( )->getIOAdapterFactoryById(
        IOAdapterUtils::url2io( cfg.getOutUrlString( ) ) );

    if (cfg.saveDoc) {
        DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(cfg.getDocumentFormatId());
        Document* doc = format->createNewLoadedDocument(iof, cfg.getOutUrlString(), stateInfo);
        CHECK_OP( stateInfo, resultTasks );
        const QSet<QString> &supportedFormats = format->getSupportedObjectTypes( );
        const bool isSequenceFormat = supportedFormats.contains( GObjectTypes::SEQUENCE );
        if (  isSequenceFormat) {
            addSequencesToSeqDoc( doc );
        } else { // consider alignment format
            SAFE_POINT( supportedFormats.contains( GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT ),
                "Unexpected format encountered", resultTasks );
            addSequencesToMsaDoc( doc );
        }
        saveTask = new SaveDocumentTask(doc, SaveDoc_Overwrite);
        resultTasks << saveTask;
    } else { // TODO: avoid high memory consumption here
        const DNAAlphabet *alp = cfg.getAlphabet( );
        SAFE_POINT( NULL != alp, "Generated sequence has invalid alphabet", resultTasks );
        const U2DbiRef dbiRef = generateTask->getDbiRef( );
        const QString baseSeqName = cfg.getSequenceName( );
        QList<U2Sequence> seqs = generateTask->getResults( );

        for ( int sequenceNum = 0, totalSeqCount = seqs.size( ); sequenceNum < totalSeqCount;
            ++sequenceNum )
        {
            const QString seqName = ( 1 < totalSeqCount )
                ? ( baseSeqName + " " + QString::number( sequenceNum + 1 ) ) : baseSeqName;

            DbiConnection con( dbiRef, stateInfo );
            CHECK_OP( stateInfo, resultTasks );
            const QByteArray seqContent = con.dbi->getSequenceDbi( )->getSequenceData(
                seqs[sequenceNum].id, U2_REGION_MAX, stateInfo );
            results << DNASequence( seqName, seqContent, alp );
        }
    }
    return resultTasks;
}

void DNASequenceGeneratorTask::addSequencesToMsaDoc( Document *source )
{
    const QSet<QString> &supportedFormats = source->getDocumentFormat( )->getSupportedObjectTypes( );
    SAFE_POINT( supportedFormats.contains( GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT ),
        "Invalid document format", );
    SAFE_POINT( NULL != generateTask, "Invalid generate task", );
    const U2DbiRef dbiRef = generateTask->getDbiRef( );
    const DNAAlphabet *alp = cfg.alphabet;
    SAFE_POINT( NULL != alp, "Generated sequence has invalid alphabet", );
    const QString baseSeqName = cfg.getSequenceName( );
    const QList<U2Sequence> seqs = generateTask->getResults( );

    MultipleSequenceAlignment msa(tr( "Generated MSA" ), alp);
    DbiConnection con( dbiRef, stateInfo );

    for ( int sequenceNum = 0, totalSeqCount = seqs.size( ); sequenceNum < totalSeqCount;
        ++sequenceNum )
    {
        const QString seqName = ( 1 < totalSeqCount )
            ? ( baseSeqName + " " + QString::number( sequenceNum + 1 ) ) : baseSeqName;
        // TODO: large sequences will cause out of memory error here
        const QByteArray seqContent = con.dbi->getSequenceDbi( )->getSequenceData(
            seqs[sequenceNum].id, U2_REGION_MAX, stateInfo );
        msa->addRow( seqName, seqContent, sequenceNum);
    }
    MultipleSequenceAlignmentObject *alnObject = MultipleSequenceAlignmentImporter::createAlignment( source->getDbiRef( ), msa, stateInfo );
    CHECK_OP( stateInfo, );
    source->addObject( alnObject );
}

void DNASequenceGeneratorTask::addSequencesToSeqDoc( Document *source )
{
    const QSet<QString> &supportedFormats = source->getDocumentFormat( )->getSupportedObjectTypes( );
    SAFE_POINT( supportedFormats.contains( GObjectTypes::SEQUENCE ), "Invalid document format", );
    SAFE_POINT( NULL != generateTask, "Invalid generate task", );
    const U2DbiRef dbiRef = generateTask->getDbiRef( );
    const QString baseSeqName = cfg.getSequenceName( );
    const QList<U2Sequence> seqs = generateTask->getResults( );

    for ( int sequenceNum = 0, totalSeqCount = seqs.size( ); sequenceNum < totalSeqCount;
        ++sequenceNum )
    {
        const QString seqName = ( 1 < totalSeqCount )
            ? ( baseSeqName + " " + QString::number( sequenceNum + 1 ) ) : baseSeqName;
        U2EntityRef entityRef( dbiRef, seqs[sequenceNum].id );
        source->addObject( new U2SequenceObject( seqName, entityRef ) );
    }
}

QList<Task*> DNASequenceGeneratorTask::onSaveTaskFinished( ) {
    Document* doc = saveTask->getDocument();
    QList<Task *> resultTasks;

    if (!cfg.addToProj) {
        doc->unload();
        delete doc;
    } else {
        Project* prj = AppContext::getProject();
        if (prj) {
            Document* d = prj->findDocumentByURL(doc->getURL());
            if (d==NULL) {
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
            if (openWithProjectTask != NULL) {
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
: Task(tr("Evaluate base content task"), TaskFlag_None), _obj(obj), alp(NULL) {
}

void EvaluateBaseContentTask::run() {
    if (_obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(_obj);
        alp = dnaObj->getAlphabet();
        DNASequenceGenerator::evaluateBaseContent(dnaObj->getWholeSequence(stateInfo), result);
    } else if (_obj->getGObjectType() == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
        MultipleSequenceAlignmentObject* maObj = qobject_cast<MultipleSequenceAlignmentObject*>(_obj);
        alp = maObj->getAlphabet();
        DNASequenceGenerator::evaluateBaseContent(maObj->getMultipleAlignment(), result);
    } else {
        stateInfo.setError(tr("Base content can be evaluated for sequence or sequence alignment"));
    }
}

// GenerateTask
GenerateDNASequenceTask::GenerateDNASequenceTask(const QMap<char, qreal> &baseContent_,
    int length_, int window_, int count_, int seed_)
    : Task(tr("Generate DNA sequence task"), TaskFlag_None), baseContent(baseContent_),
    length(length_), window(window_), count(count_), seed(seed_), dbiRef( )
{

}

void GenerateDNASequenceTask::prepare() {
    const qint64 memUseMB = window / MBYTE_TO_BYTE;
    coreLog.trace( QString( "Generate DNA sequence task: Memory resource %1" ).arg( memUseMB ) );
    addTaskResource( TaskResourceUsage( RESOURCE_MEMORY, memUseMB ) );
}

void GenerateDNASequenceTask::run( ) {
    if ( seed < 0 ) {
        qsrand( QDateTime::currentDateTime( ).toTime_t( ) );
    } else {
        qsrand( seed );
    }
    dbiRef = AppContext::getDbiRegistry( )->getSessionTmpDbiRef( stateInfo );
    CHECK_OP( stateInfo, );
    DbiConnection con( dbiRef, stateInfo );
    CHECK_OP( stateInfo, );

    results.reserve( count );
    for ( int seqCount = 0; seqCount < count; seqCount++ ) {
        U2SequenceImporter seqImporter( QVariantMap( ), true );

        QByteArray sequenceChunk;
        if( window > length ) {
            window = length;
        }

        seqImporter.startSequence(stateInfo, dbiRef, U2ObjectDbi::ROOT_FOLDER, QString( "default" ), false);
        CHECK_OP_BREAK( stateInfo );

        for( int chunkCount = 0; chunkCount < length / window && !isCanceled( );
            chunkCount++ )
        {
            DNASequenceGenerator::generateSequence( baseContent, window, sequenceChunk );
            seqImporter.addBlock( sequenceChunk.constData( ), sequenceChunk.length( ), stateInfo );
            CHECK_OP_BREAK( stateInfo );
            const int currentProgress = int( 100 * ( seqCount + chunkCount * double ( window )
                / double ( length ) ) / double( count ) );
            stateInfo.setProgress( currentProgress );
        }
        // append last chunk
        DNASequenceGenerator::generateSequence( baseContent, length % window, sequenceChunk );
        seqImporter.addBlock( sequenceChunk.constData( ), sequenceChunk.length( ), stateInfo );
        CHECK_OP_BREAK( stateInfo );
        U2Sequence seq = seqImporter.finalizeSequence( stateInfo );
        CHECK_OP_BREAK( stateInfo );

        results.append( seq );
        stateInfo.setProgress( seqCount / count * 100 );
    }
}

} //namespace
