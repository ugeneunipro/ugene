/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "BlastCommonTask.h"

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "BlastNTask.h"
#include "BlastPTask.h"
#include "BlastSupport.h"
#include "BlastXTask.h"
#include "RPSBlastTask.h"
#include "TBlastNTask.h"
#include "TBlastXTask.h"

namespace U2 {

/** Name prefix used for query sequences when query sequences are stored to the BLAST input FASTA file. */
static const QString QUERY_SEQUENCE_NAME_PREFIX = "query-";

BlastCommonTask::BlastCommonTask(const BlastTaskSettings& _settings)
    : ExternalToolSupportTask(tr("Run NCBI Blast task"), TaskFlags_NR_FOSCOE | TaskFlag_ReportingIsSupported),
      settings(_settings) {
    GCOUNTER(cvar, "BlastCommonTask");
    for (const QByteArray& querySequence : qAsConst(settings.querySequences)) {
        querySequences << (settings.isSequenceCircular ? U2PseudoCircularization::createSequenceWithCircularOverlaps(querySequence) : querySequence);
    }
    addTaskResource(TaskResourceUsage(RESOURCE_THREAD, settings.numberOfProcessors));
    if (settings.querySequenceObject != nullptr) {
        TaskWatchdog::trackResourceExistence(settings.querySequenceObject, this, tr("A problem occurred during doing BLAST. The sequence is no more available."));
    }
}

void BlastCommonTask::prepare() {
    if (settings.databaseNameAndPath.contains(" ")) {
        stateInfo.setError(tr("Database path have space(s). Try select any other folder without spaces."));
        return;
    }
    // Add new subdir for temporary files
    // Folder name is ExternalToolName + CurrentDate + CurrentTime

    QString tmpDirPath = getAcceptableTempDir();
    CHECK_EXT(!tmpDirPath.isEmpty(), setError(tr("The task uses a temporary folder to process the data. The folder path is required not to have spaces. "
                                                 "Please set up an appropriate path for the \"Temporary files\" parameter on the \"Directories\" tab of the UGENE Application Settings.")), );

    QDir tmpDir(tmpDirPath);
    if (tmpDir.exists()) {
        foreach (const QString& file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }
        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
            stateInfo.setError(tr("Subfolder for temporary files exists. Can not remove this folder."));
            return;
        }
    }
    if (!tmpDir.mkpath(tmpDirPath)) {
        stateInfo.setError(tr("Can not create folder for temporary files."));
        return;
    }
    // Create 'ncbi.ini' for windows or '.ncbirc' for unix like systems
    // See issue UGENE-791 (https://ugene.net/tracker/browse/UGENE-791)
    QString iniNCBIFile = tmpDir.absolutePath() + QString(isOsWindows() ? "/ncbi.ini" : "/.ncbirc");
    if (!QFileInfo::exists(iniNCBIFile)) {
        QFile file(iniNCBIFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            algoLog.details(tr("Can not create fake NCBI ini file"));
        }
    }
    url = tmpDirPath + "tmp.fa";
    if (url.contains(" ")) {
        stateInfo.setError(tr("Temporary folder path have space(s). Try select any other folder without spaces."));
        return;
    }
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA);
    tmpDoc = df->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(url), stateInfo);
    CHECK_OP(stateInfo, );

    for (int i = 0; i < querySequences.length(); i++) {
        QString querySequenceName = QUERY_SEQUENCE_NAME_PREFIX + QString::number(i);
        const QByteArray& querySequence = querySequences[i];
        U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, tmpDoc->getDbiRef(), DNASequence(querySequenceName, querySequence, settings.alphabet));
        CHECK_OP(stateInfo, );
        sequenceObject = new U2SequenceObject("input sequence", seqRef);
        tmpDoc->addObject(sequenceObject);
    }

    saveTemporaryDocumentTask = new SaveDocumentTask(tmpDoc, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE), url);
    saveTemporaryDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveTemporaryDocumentTask);
}

QString BlastCommonTask::getAcceptableTempDir() const {
    QString tmpDirName = "Blast_" + QString::number(this->getTaskId()) + "_" +
                         QDate::currentDate().toString("dd.MM.yyyy") + "_" +
                         QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
                         QString::number(QCoreApplication::applicationPid()) + "/";

    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(BlastSupport::BLAST_TMP_DIR);
    return !GUrlUtils::containSpaces(tmpDirPath) ? tmpDirPath + "/" + tmpDirName : "";
}

QList<Task*> BlastCommonTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask == saveTemporaryDocumentTask) {
        delete tmpDoc;  // sequenceObject also deleted at this place
        blastTask = createBlastTask();
        blastTask->setSubtaskProgressWeight(95);
        res.append(blastTask);
    } else if (subTask == blastTask) {
        if (settings.outputType == 5 || settings.outputType == 6) {
            if (!QFileInfo::exists(settings.outputOriginalFile)) {
                QString curToolId = BlastSupport::getToolIdByProgramName(settings.programName);
                if (AppContext::getExternalToolRegistry()->getById(curToolId)->isValid()) {
                    stateInfo.setError(tr("Output file not found"));
                } else {
                    stateInfo.setError(tr("Output file not found. May be %1 tool path '%2' not valid?")
                                           .arg(AppContext::getExternalToolRegistry()->getById(curToolId)->getName())
                                           .arg(AppContext::getExternalToolRegistry()->getById(curToolId)->getPath()));
                }
                return res;
            }
            if (settings.outputType == 5) {
                parseXMLResult();
            } else if (settings.outputType == 6) {
                parseTabularResult();
            }
            if (!resultsPerQuerySequence.isEmpty() && settings.needCreateAnnotations) {
                if (!settings.outputResFile.isEmpty()) {
                    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
                    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
                    Document* d = df->createNewLoadedDocument(iof, settings.outputResFile, stateInfo);
                    CHECK_OP(stateInfo, res);
                    d->addObject(settings.aobj);
                    AppContext::getProject()->addDocument(d);
                }

                QList<SharedAnnotationData> resultAnnotations = getResultAnnotations();
                res.append(new CreateAnnotationsTask(settings.aobj, {{settings.groupName, resultAnnotations}}));
            }
            if (res.isEmpty()) {
                setReportingEnabled(true);
                taskLog.info(tr("There were no hits found for your BLAST search."));
            }
        }
    }
    return res;
}

QList<SharedAnnotationData> BlastCommonTask::getResultAnnotations() const {
    QList<SharedAnnotationData> resultAnnotations;
    QList<int> resultKeys = resultsPerQuerySequence.keys();
    for (int key : qAsConst(resultKeys)) {
        resultAnnotations << resultsPerQuerySequence.value(key);
    }
    return resultAnnotations;
}

QList<SharedAnnotationData> BlastCommonTask::getResultPerQuerySequence(int querySequenceIndex) const {
    return resultsPerQuerySequence.value(querySequenceIndex);
}

Task::ReportResult BlastCommonTask::report() {
    if (url.isEmpty()) {
        return ReportResult_Finished;
    }

    // Remove subdir for temporary files, that created in prepare
    QDir tmpDir(QFileInfo(url).absoluteDir());
    foreach (QString file, tmpDir.entryList(QDir::Files | QDir::Hidden)) {
        tmpDir.remove(file);
    }
    if (!tmpDir.rmdir(tmpDir.absolutePath())) {
        stateInfo.setError(tr("Can not remove folder for temporary files."));
        emit si_stateChanged();
    }
    return ReportResult_Finished;
}

QString BlastCommonTask::generateReport() const {
    return resultsPerQuerySequence.isEmpty() ? tr("There were no hits found for your BLAST search.") : "";
}

BlastTaskSettings BlastCommonTask::getSettings() const {
    return settings;
}

void BlastCommonTask::parseTabularResult() {
    QFile file(settings.outputOriginalFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        stateInfo.setError("Can't open output file");
        return;
    }
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (line.startsWith("#")) {  // skip comment
            continue;
        }
        parseTabularLine(line);
    }
    file.close();
}

void BlastCommonTask::parseTabularLine(const QByteArray& line) {
    SharedAnnotationData ad(new AnnotationData);
    // Fields: Query id (0), Subject id(1), % identity(2), alignment length(3), mismatches(4), gap openings(5), q. start(6), q. end(7), s. start(8), s. end(9), e-value(10), bit score(11)
    QList<QByteArray> elements = line.split('\t');
    if (elements.size() != 12) {
        stateInfo.setError(tr("Incorrect number of fields in line: %1").arg(elements.size()));
        return;
    }
    int querySequenceIndex = parseQuerySequenceIndex(elements[0]);
    SAFE_POINT(querySequenceIndex >= 0, "Invalid querySequenceIndex", );

    bool isOk;
    int from = elements.at(6).toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }
    int to = elements.at(7).toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }
    if (from != -1 && to != -1) {
        int querySequenceLength = settings.querySequences[querySequenceIndex].length();
        U2Region resultRegion = {qMin(from, to) - 1, qAbs(to - from) + 1};
        bool isOverflowAreaDuplicate = resultRegion.startPos >= querySequenceLength;
        CHECK(!isOverflowAreaDuplicate, );
        ad->location->regions = {resultRegion};
        U2PseudoCircularization::convertToOriginalSequenceCoordinates(ad->location, querySequenceLength);
        SAFE_POINT(!ad->location->regions.isEmpty(), "Result location can't be empty", );
        U2Region::shift(settings.resultRegionOffset, ad->location->regions);
    } else {
        stateInfo.setError(tr("Can't evaluate location"));
        return;
    }

    int hitFrom = elements.at(8).toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get hit location"));
        return;
    }
    int hitTo = elements.at(9).toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get hit location"));
        return;
    }
    if (hitFrom != -1 && hitTo != -1) {
        if (hitFrom <= hitTo) {
            ad->setStrand(U2Strand::Direct);
            ad->qualifiers.push_back(U2Qualifier("source_frame", "direct"));
            ad->qualifiers.push_back(U2Qualifier("hit-from", QString::number(hitFrom)));
            ad->qualifiers.push_back(U2Qualifier("hit-to", QString::number(hitTo)));
        } else {
            ad->setStrand(U2Strand::Complementary);
            ad->qualifiers.push_back(U2Qualifier("source_frame", "complement"));
            ad->qualifiers.push_back(U2Qualifier("hit-to", QString::number(hitFrom)));
            ad->qualifiers.push_back(U2Qualifier("hit-from", QString::number(hitTo)));
        }
    } else {
        stateInfo.setError(tr("Can't evaluate hit location"));
        return;
    }
    QString elem = QString(elements.at(11));
    if (elem.endsWith('\n')) {
        elem.resize(elem.size() - 1);  // remove \n symbol
    }
    double bitScore = elem.toDouble(&isOk);
    if (isOk) {
        ad->qualifiers.push_back(U2Qualifier("bit-score", QString::number(bitScore)));
    }

    int align_len = elements.at(3).toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get align length"));
        return;
    }
    int gaps = elements.at(4).toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get gaps"));
        return;
    }
    float identitiesPercent = elements.at(2).toFloat(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get identity"));
        return;
    }
    if (align_len != -1) {
        if (gaps != -1) {
            float percent = (float)gaps / (float)align_len * 100;
            QString str = QString::number(gaps) + "/" + QString::number(align_len) + " (" + QString::number(percent, 'g', 4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("gaps", str));
        }
        if (identitiesPercent != -1) {
            // float percent = (float)identities / (float)align_len * 100;
            int identities = qRound(align_len * (double)identitiesPercent / 100);
            QString str = QString::number(identities) + '/' + QString::number(align_len) + " (" + QString::number(identitiesPercent, 'g', 4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("identities", str));
        }
    }
    if (!elements.at(10).isEmpty()) {
        ad->qualifiers.push_back(U2Qualifier("E-value", elements.at(10)));
    }

    ad->qualifiers.push_back(U2Qualifier("id", elements.at(1)));
    ad->name = "blast result";

    QList<SharedAnnotationData> resultPerQuerySequence = resultsPerQuerySequence.value(querySequenceIndex);
    resultPerQuerySequence << ad;
    resultsPerQuerySequence[querySequenceIndex] = resultPerQuerySequence;
}

void BlastCommonTask::parseXMLResult() {
    QDomDocument xmlDoc;
    {
        QFile file(settings.outputOriginalFile);
        if (!file.open(QIODevice::ReadOnly)) {
            stateInfo.setError(tr("Can't open output file"));
            return;
        }
        if (!xmlDoc.setContent(&file)) {
            stateInfo.setError(tr("Can't read output file"));
            return;
        }
    }

    QDomNodeList iterationDomElements = xmlDoc.elementsByTagName("Iteration");
    for (int iterationIndex = 0; iterationIndex < iterationDomElements.count(); iterationIndex++) {
        QDomElement iterationDomElement = iterationDomElements.at(iterationIndex).toElement();

        int querySequenceIndex = parseQuerySequenceIndex(iterationDomElement.firstChildElement("Iteration_query-def").text());
        SAFE_POINT(querySequenceIndex >= 0, "Invalid querySequenceIndex", );
        QDomNodeList hits = iterationDomElement.elementsByTagName("Hit");
        for (int i = 0; i < hits.count(); i++) {
            parseXMLHit(hits.at(i), querySequenceIndex);
        }
    }
}

int BlastCommonTask::parseQuerySequenceIndex(const QString& querySequenceName) const {
    SAFE_POINT_EXT(querySequenceName.startsWith(QUERY_SEQUENCE_NAME_PREFIX), uiLog.trace("Unexpected query sequence name: " + querySequenceName), -1);
    bool isOk;
    int querySequenceIndex = querySequenceName.mid(QUERY_SEQUENCE_NAME_PREFIX.length()).toInt(&isOk);
    SAFE_POINT_EXT(isOk && querySequenceIndex >= 0 && querySequenceIndex < settings.querySequences.size(),
                   uiLog.trace("Unexpected query sequence index: " + querySequenceName),
                   -1);
    return querySequenceIndex;
}

void BlastCommonTask::parseXMLHit(const QDomNode& xml, int querySequenceIndex) {
    QDomElement tmp = xml.lastChildElement("Hit_id");
    QString id = tmp.text();
    tmp = xml.lastChildElement("Hit_def");
    QString def = tmp.text();
    tmp = xml.lastChildElement("Hit_accession");
    QString accession = tmp.text();

    QDomNodeList nodes = xml.childNodes();
    for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i).isElement()) {
            if (nodes.at(i).toElement().tagName() == "Hit_hsps") {
                QDomNodeList hsps = nodes.at(i).childNodes();
                for (int j = 0; j < hsps.count(); j++) {
                    if (hsps.at(j).toElement().tagName() == "Hsp") {
                        parseXMLHsp(hsps.at(j), querySequenceIndex, id, def, accession);
                    }
                }
            }
        }
    }
}

void BlastCommonTask::parseXMLHsp(const QDomNode& xml, int querySequenceIndex, const QString& id, const QString& def, const QString& accession) {
    SharedAnnotationData ad(new AnnotationData);

    QDomElement elem = xml.lastChildElement("Hsp_bit-score");
    if (!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("bit-score", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_score");
    if (!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("score", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_evalue");
    if (!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("E-value", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_query-from");
    bool isOk;
    int from = elem.text().toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }

    elem = xml.lastChildElement("Hsp_query-to");
    int to = elem.text().toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }

    elem = xml.lastChildElement("Hsp_hit-from");
    if (!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("hit-from", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_hit-to");
    if (!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("hit-to", elem.text()));
    }

    QString strandTag;
    switch (settings.strandSource) {
        case BlastTaskSettings::HitFrame:
            strandTag = "Hsp_hit-frame";
            break;
        case BlastTaskSettings::QueryFrame:
            strandTag = "Hsp_query-frame";
            break;
        default:
            SAFE_POINT_EXT(false, stateInfo.setError(tr("Unknown strand source setting")), );
    }
    elem = xml.lastChildElement(strandTag);
    int frame = elem.text().toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get location. %1[%2]").arg(strandTag).arg(elem.text()));
        return;
    }

    QString frame_txt = (frame < 0) ? "complement" : "direct";
    ad->qualifiers.push_back(U2Qualifier("source_frame", frame_txt));
    ad->setStrand(frame < 0 ? U2Strand::Complementary : U2Strand::Direct);

    elem = xml.lastChildElement("Hsp_identity");
    int identities = elem.text().toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get identity"));
        return;
    }

    elem = xml.lastChildElement("Hsp_gaps");
    int gaps = -1;
    if (!elem.isNull()) {
        gaps = elem.text().toInt(&isOk);
        if (!isOk) {
            stateInfo.setError(tr("Can't get gaps"));
            return;
        }
    }

    elem = xml.lastChildElement("Hsp_align-len");
    int align_len = elem.text().toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get align length"));
        return;
    }
    // No need to check strand with BLAST
    U2Region resultRegion(from - 1, to - from + 1);
    int querySequenceLength = settings.querySequences[querySequenceIndex].length();
    bool isOverflowAreaDuplicate = resultRegion.startPos >= querySequenceLength;
    CHECK(!isOverflowAreaDuplicate, );
    ad->location->regions = {resultRegion};
    U2PseudoCircularization::convertToOriginalSequenceCoordinates(ad->location, querySequenceLength);
    SAFE_POINT(!ad->location->regions.isEmpty(), "Result location can't be empty", );
    U2Region::shift(settings.resultRegionOffset, ad->location->regions);

    if (align_len != -1) {
        if (gaps != -1) {
            double percent = gaps / (double)align_len * 100;
            QString str = QString::number(gaps) + "/" + QString::number(align_len) + " (" + QString::number(percent, 'g', 4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("gaps", str));
        }
        if (identities != -1) {
            double percent = identities / (double)align_len * 100.;
            QString str = QString::number(identities) + '/' + QString::number(align_len) + " (" + QString::number(percent, 'g', 4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("identities", str));
            ad->qualifiers.push_back(U2Qualifier("identity_percent", QString::number(percent)));
        }
    }

    // parse alignment
    {
        elem = xml.lastChildElement("Hsp_qseq");
        QByteArray qSeq = elem.text().toLatin1();

        elem = xml.lastChildElement("Hsp_hseq");
        QByteArray hSeq = elem.text().toLatin1();

        elem = xml.lastChildElement("Hsp_midline");
        QByteArray midline = elem.text().toLatin1();

        QByteArray cigar;
        int length = midline.length();
        char prevCigarChar = '*';
        int cigarCount = 0;

        for (int i = 0; i < length; ++i) {
            char cigarChar;
            char gapChar = midline.at(i);
            if (gapChar == '|') {
                cigarChar = 'M';
            } else {
                if (qSeq.at(i) == '-') {
                    cigarChar = 'D';
                } else if (hSeq.at(i) == '-') {
                    cigarChar = 'I';
                } else {
                    cigarChar = 'X';
                }
            }

            if (i > 0 && prevCigarChar != cigarChar) {
                cigar.append(QByteArray::number(cigarCount)).append(prevCigarChar);
                cigarCount = 0;
            }

            prevCigarChar = cigarChar;
            cigarCount++;

            if (i == length - 1) {
                cigar.append(QByteArray::number(cigarCount)).append(cigarChar);
            }
        }

        ad->qualifiers.append(U2Qualifier("subj_seq", hSeq));
        ad->qualifiers.append(U2Qualifier("cigar", cigar));
    }

    ad->qualifiers.push_back(U2Qualifier("id", id));
    ad->qualifiers.push_back(U2Qualifier("def", def));
    ad->qualifiers.push_back(U2Qualifier("accession", accession));
    U1AnnotationUtils::addDescriptionQualifier(ad, settings.annDescription);
    ad->name = "blast result";

    QList<SharedAnnotationData> resultPerQuerySequence = resultsPerQuerySequence.value(querySequenceIndex);
    resultPerQuerySequence << ad;
    resultsPerQuerySequence[querySequenceIndex] = resultPerQuerySequence;
}

///////////////////////////////////////
// BlastMultiTask
BlastMultiTask::BlastMultiTask(QList<BlastTaskSettings>& _settingsList, QString& _url)
    : Task("Run NCBI BlastAll multitask", TaskFlags_NR_FOSCOE | TaskFlag_ReportingIsSupported),
      settingsList(_settingsList), doc(nullptr), url(_url) {
}
void BlastMultiTask::prepare() {
    // create document
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    // url=settingsList[0].outputResFile;
    doc = df->createNewLoadedDocument(iof, url, stateInfo);
    CHECK_OP(stateInfo, );

    foreach (BlastTaskSettings settings, settingsList) {
        settings.needCreateAnnotations = false;
        Task* t = nullptr;
        if (settings.programName == "blastn") {
            t = new BlastNTask(settings);
        } else if (settings.programName == "blastp") {
            t = new BlastPTask(settings);
        } else if (settings.programName == "blastx") {
            t = new BlastXTask(settings);
        } else if (settings.programName == "tblastn") {
            t = new TBlastNTask(settings);
        } else if (settings.programName == "tblastx") {
            t = new TBlastXTask(settings);
        } else if (settings.programName == "rpsblast") {
            t = new RPSBlastTask(settings);
        }
        addSubTask(t);
    }
}
QList<Task*> BlastMultiTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if (hasError() || isCanceled()) {
        return res;
    }
    if (auto blastTask = qobject_cast<BlastCommonTask*>(subTask)) {
        BlastTaskSettings settings = blastTask->getSettings();
        SAFE_POINT_EXT(settings.aobj != nullptr, setError("Result annotation object is null!"), {});
        QList<SharedAnnotationData> resultAnnotations = blastTask->getResultAnnotations();
        if (!resultAnnotations.isEmpty()) {
            doc->addObject(settings.aobj);
            res.append(new CreateAnnotationsTask(settings.aobj, {{settings.groupName, resultAnnotations}}));
        }
    }
    return res;
}

Task::ReportResult BlastMultiTask::report() {
    if (!hasError()) {
        if (doc->getObjects().length() > 0) {
            AppContext::getProject()->addDocument(doc);
        } else {
            setReportingEnabled(true);
        }
    }
    return ReportResult_Finished;
}

QString BlastMultiTask::generateReport() const {
    QString res;

    res += "<table>";
    res += "<tr><td width=200><b>" + tr("Source file") + "</b></td><td>" + settingsList.at(0).queryFile + "</td></tr>";
    res += "<tr><td width=200><b>" + tr("Used database") + "</b></td><td>" + settingsList.at(0).databaseNameAndPath + "</td></tr>";
    res += "<tr></tr>";
    res += "<tr><td width=200><b>" + tr("No results found") + "</b></td><td></td></tr>";
    res += "</table>";
    return res;
}

}  // namespace U2
