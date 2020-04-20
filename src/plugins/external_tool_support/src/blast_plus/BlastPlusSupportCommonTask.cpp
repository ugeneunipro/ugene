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
#include <U2Core/ExternalToolRegistry.h>
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

#include "BlastNPlusSupportTask.h"
#include "BlastPPlusSupportTask.h"
#include "BlastXPlusSupportTask.h"
#include "TBlastNPlusSupportTask.h"
#include "TBlastXPlusSupportTask.h"
#include "RPSBlastSupportTask.h"

#include "BlastPlusSupport.h"
#include "BlastPlusSupportCommonTask.h"

namespace U2 {

BlastPlusSupportCommonTask::BlastPlusSupportCommonTask(const BlastTaskSettings& _settings) :
        ExternalToolSupportTask("Run NCBI Blast+ task", TaskFlags_NR_FOSCOE | TaskFlag_ReportingIsSupported),
        settings(_settings)
{
    GCOUNTER(cvar, tvar, "BlastPlusSupportCommonTask");
    blastPlusTask=NULL;
    tmpDoc=NULL;
    saveTemporaryDocumentTask=NULL;
    sequenceObject=NULL;
    circularization = new U2PseudoCircularization(this, settings.isSequenceCircular, settings.querySequence);
    addTaskResource(TaskResourceUsage(RESOURCE_THREAD, settings.numberOfProcessors));
    if (NULL != settings.querySequenceObject) {
        TaskWatchdog::trackResourceExistence(settings.querySequenceObject, this, tr("A problem occurred during doing BLAST+. The sequence is no more available."));
    }
}

void BlastPlusSupportCommonTask::prepare(){
    if(settings.databaseNameAndPath.contains(" ")){
        stateInfo.setError("Database path have space(s). Try select any other folder without spaces.");
        return;
    }
    //Add new subdir for temporary files
    //Folder name is ExternalToolName + CurrentDate + CurrentTime

    QString tmpDirPath = getAcceptableTempDir();
    CHECK_EXT(!tmpDirPath.isEmpty(), setError(tr("The task uses a temporary folder to process the data. The folder path is required not to have spaces. "
        "Please set up an appropriate path for the \"Temporary files\" parameter on the \"Directories\" tab of the UGENE Application Settings.")), );

    QDir tmpDir(tmpDirPath);
    if(tmpDir.exists()) {
        foreach(const QString& file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }
        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
            stateInfo.setError(tr("Subfolder for temporary files exists. Can not remove this folder."));
            return;
        }
    }
    if(!tmpDir.mkpath(tmpDirPath)) {
        stateInfo.setError(tr("Can not create folder for temporary files."));
        return;
    }
    //Create ncbi.ini for windows or .ncbirc for unix like systems
    //See issue UGENE-791 (https://ugene.net/tracker/browse/UGENE-791)
#ifdef Q_OS_UNIX
    QString iniNCBIFile=tmpDir.absolutePath()+QString("/.ncbirc");
#else
    QString iniNCBIFile=tmpDir.absolutePath()+QString("\\ncbi.ini");
#endif
    if(!QFileInfo(iniNCBIFile).exists()){
        QFile file(iniNCBIFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            algoLog.details(tr("Can not create fake NCBI ini file"));
        }else{
            file.close();
        }
    }
    url = tmpDirPath + "tmp.fa";
    if (url.contains(" ")){
        stateInfo.setError("Temporary folder path have space(s). Try select any other folder without spaces.");
        return;
    }
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA);
    tmpDoc = df->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(url), stateInfo);
    CHECK_OP(stateInfo,);

    U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, tmpDoc->getDbiRef(), DNASequence(settings.querySequence, settings.alphabet));
    CHECK_OP(stateInfo,);
    sequenceObject = new U2SequenceObject("input sequence", seqRef);
    tmpDoc->addObject(sequenceObject);

    saveTemporaryDocumentTask = new SaveDocumentTask(tmpDoc, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE), url);
    saveTemporaryDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveTemporaryDocumentTask);
}

QString BlastPlusSupportCommonTask::getAcceptableTempDir() const {
    QString tmpDirName = "BlastPlus_" + QString::number(this->getTaskId()) + "_" +
        QDate::currentDate().toString("dd.MM.yyyy") + "_" +
        QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
        QString::number(QCoreApplication::applicationPid()) + "/";

    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(BlastPlusSupport::BLASTPLUS_TMP_DIR);
    if (!GUrlUtils::containSpaces(tmpDirPath)) {
        return tmpDirPath + "/" + tmpDirName;
    }
    return QString();
}

QList<Task*> BlastPlusSupportCommonTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }
    if(subTask==saveTemporaryDocumentTask){
        delete tmpDoc;//sequenceObject also deleted at this place
        blastPlusTask=createBlastPlusTask();
        blastPlusTask->setSubtaskProgressWeight(95);
        res.append(blastPlusTask);
    }
    else if(subTask==blastPlusTask){
        if(settings.outputType == 5 || settings.outputType == 6){
            if(!QFileInfo(settings.outputOriginalFile).exists()){
                QString curToolId = toolIdByProgram(settings.programName);
                if(AppContext::getExternalToolRegistry()->getById(curToolId)->isValid()){
                    stateInfo.setError(tr("Output file not found"));
                }else{
                    stateInfo.setError(tr("Output file not found. May be %1 tool path '%2' not valid?")
                                       .arg(AppContext::getExternalToolRegistry()->getById(curToolId)->getName())
                                       .arg(AppContext::getExternalToolRegistry()->getById(curToolId)->getPath()));
                }
                return res;
            }
            if(settings.outputType == 5){
                parseXMLResult();
            }else if(settings.outputType == 6){
                parseTabularResult();
            }
            if((!result.isEmpty())&&(settings.needCreateAnnotations)) {
                if(!settings.outputResFile.isEmpty()) {
                    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
                    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
                    Document *d = df->createNewLoadedDocument(iof, settings.outputResFile, stateInfo);
                    CHECK_OP(stateInfo, res);
                    d->addObject(settings.aobj);
                    AppContext::getProject()->addDocument(d);
                }

                for (QMutableListIterator<SharedAnnotationData> it_ad(result); it_ad.hasNext();) {
                    SharedAnnotationData &ad = it_ad.next();
                    U2Region::shift(settings.offsInGlobalSeq, ad->location->regions);
                }
                res.append(new CreateAnnotationsTask(settings.aobj, result, settings.groupName));
            }
            if (res.isEmpty()) {
                setReportingEnabled(true);
                taskLog.info(tr("There were no hits found for your BLAST search."));
            }
        }
    }
    return res;
}
Task::ReportResult BlastPlusSupportCommonTask::report(){
    if(url.isEmpty()) {
        return ReportResult_Finished;
    }

    //Remove subdir for temporary files, that created in prepare
    QDir tmpDir(QFileInfo(url).absoluteDir());
    foreach(QString file, tmpDir.entryList(QDir::Files|QDir::Hidden)){
        tmpDir.remove(file);
    }
    if(!tmpDir.rmdir(tmpDir.absolutePath())){
        stateInfo.setError(tr("Can not remove folder for temporary files."));
        emit si_stateChanged();
    }
    return ReportResult_Finished;
}

QString BlastPlusSupportCommonTask::generateReport() const {
    if (result.isEmpty()) {
        return tr("There were no hits found for your BLAST search.");
    }
    return QString();
}

QList<SharedAnnotationData> BlastPlusSupportCommonTask::getResultedAnnotations() const {
    return result;
}
BlastTaskSettings BlastPlusSupportCommonTask::getSettings() const {
    return settings;
}
void BlastPlusSupportCommonTask::parseTabularResult() {
    QFile file(settings.outputOriginalFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        stateInfo.setError("Can't open output file");
        return;
    }
    while(!file.atEnd()){
        QByteArray line = file.readLine();
        if (line.startsWith("#")){//skip comment
            continue;
        }
        parseTabularLine(line);
    }
    file.close();
}
void BlastPlusSupportCommonTask::parseTabularLine(const QByteArray &line){
    SharedAnnotationData ad(new AnnotationData);
    bool isOk;
    int from = -1,to = -1,align_len = -1,gaps = -1, hitFrom = -1,hitTo = -1;
    //Fields: Query id (0), Subject id(1), % identity(2), alignment length(3), mismatches(4), gap openings(5), q. start(6), q. end(7), s. start(8), s. end(9), e-value(10), bit score(11)
    QList<QByteArray> elements=line.split('\t');
    if(elements.size()!=12){
        stateInfo.setError(tr("Incorrect number of fields in line: %1").arg(elements.size()));
        return;
    }
    from = elements.at(6).toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }
    to = elements.at(7).toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }
    if(from != -1 && to != -1) {
        if(from <= to){
            ad->location->regions << U2Region(from - 1, to - from + 1);
        }else{
            ad->location->regions << U2Region(to-1, from - to + 1);
        }
        circularization->uncircularizeLocation(ad->location);
        CHECK(!ad->location->regions.isEmpty(), );
    } else {
        stateInfo.setError(tr("Can't evaluate location"));
        return;
    }

    hitFrom = elements.at(8).toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get hit location"));
        return;
    }
    hitTo = elements.at(9).toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get hit location"));
        return;
    }
    if(hitFrom != -1 && hitTo != -1) {
        if(hitFrom <= hitTo){
            ad->setStrand(U2Strand::Direct);
            ad->qualifiers.push_back(U2Qualifier("source_frame","direct"));
            ad->qualifiers.push_back(U2Qualifier("hit-from", QString::number(hitFrom)));
            ad->qualifiers.push_back(U2Qualifier("hit-to", QString::number(hitTo)));
        }else{
            ad->setStrand(U2Strand::Complementary);
            ad->qualifiers.push_back(U2Qualifier("source_frame","complement"));
            ad->qualifiers.push_back(U2Qualifier("hit-to", QString::number(hitFrom)));
            ad->qualifiers.push_back(U2Qualifier("hit-from", QString::number(hitTo)));
        }
    } else {
        stateInfo.setError(tr("Can't evaluate hit location"));
        return;
    }
    QString elem=QString(elements.at(11));
    if(elem.endsWith('\n')){
        elem.resize(elem.size()-1);//remove \n symbol
    }
    double bitScore = elem.toDouble(&isOk);
    if (isOk){
        ad->qualifiers.push_back(U2Qualifier("bit-score", QString::number(bitScore)));
    }

    align_len = elements.at(3).toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get align length"));
        return;
    }
    gaps = elements.at(4).toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get gaps"));
        return;
    }
    float identitiesPercent = elements.at(2).toFloat(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get identity"));
        return;
    }
    if(align_len != -1) {
        if(gaps != -1) {
            float percent = (float)gaps / (float)align_len * 100;
            QString str = QString::number(gaps) + "/" + QString::number(align_len) + " (" + QString::number(percent,'g',4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("gaps", str));
        }
        if(identitiesPercent != -1) {
            //float percent = (float)identities / (float)align_len * 100;
            int identities=(float)align_len*identitiesPercent/100.;
            QString str = QString::number(identities) + '/' + QString::number(align_len) + " (" + QString::number(identitiesPercent,'g',4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("identities", str));
        }
    }
    if(!elements.at(10).isEmpty()) {
        ad->qualifiers.push_back(U2Qualifier("E-value", elements.at(10)));
    }

    ad->qualifiers.push_back(U2Qualifier("id",elements.at(1)));
    ad->name = "blast result";
    result.append(ad);
}

void BlastPlusSupportCommonTask::parseXMLResult() {
    QDomDocument xmlDoc;
    QFile file(settings.outputOriginalFile);
    if (!file.open(QIODevice::ReadOnly)){
        stateInfo.setError(tr("Can't open output file"));
        return;
    }
    if (!xmlDoc.setContent(&file)) {
        stateInfo.setError(tr("Can't read output file"));
        file.close();
        return;
    }
    file.close();

    QDomNodeList hits = xmlDoc.elementsByTagName("Hit");
    for(int i = 0; i<hits.count();i++) {
        parseXMLHit(hits.at(i));
    }
}

void BlastPlusSupportCommonTask::parseXMLHit(const QDomNode &xml) {
    QString id,def,accession;

    QDomElement tmp = xml.lastChildElement("Hit_id");
    id = tmp.text();
    tmp = xml.lastChildElement("Hit_def");
    def = tmp.text();
    tmp = xml.lastChildElement("Hit_accession");
    accession = tmp.text();

    QDomNodeList nodes = xml.childNodes();
    for(int i = 0; i < nodes.count(); i++) {
        if(nodes.at(i).isElement()) {
            if(nodes.at(i).toElement().tagName()=="Hit_hsps") {
                QDomNodeList hsps = nodes.at(i).childNodes();
                for(int j = 0;j<hsps.count();j++)
                    if(hsps.at(j).toElement().tagName()=="Hsp")
                        parseXMLHsp(hsps.at(j),id,def,accession);
            }
        }
    }
}

void BlastPlusSupportCommonTask::parseXMLHsp(const QDomNode &xml,const QString &id, const QString &def, const QString &accession) {
    SharedAnnotationData ad(new AnnotationData);
    bool isOk;
    int from = -1,to = -1,align_len = -1,gaps = -1,identities = -1;

    QDomElement elem = xml.lastChildElement("Hsp_bit-score");
    if(!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("bit-score", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_score");
    if(!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("score", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_evalue");
    if(!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("E-value", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_query-from");
    QString fr = elem.text();
    from = elem.text().toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }

    elem = xml.lastChildElement("Hsp_query-to");
    to = elem.text().toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get location"));
        return;
    }

    elem = xml.lastChildElement("Hsp_hit-from");
    if(!elem.isNull()) {
        ad->qualifiers.push_back(U2Qualifier("hit-from", elem.text()));
    }

    elem = xml.lastChildElement("Hsp_hit-to");
    if(!elem.isNull()) {
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
        break;
    }
    elem = xml.lastChildElement(strandTag);
    int frame = elem.text().toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(tr("Can't get location. %1[%2]").arg(strandTag).arg(elem.text()));
        return;
    }

    QString frame_txt = (frame < 0) ? "complement" : "direct";
    ad->qualifiers.push_back(U2Qualifier("source_frame", frame_txt));
    ad->setStrand(frame < 0 ? U2Strand::Complementary: U2Strand::Direct);

    elem = xml.lastChildElement("Hsp_identity");
    identities = elem.text().toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get identity"));
        return;
    }

    elem = xml.lastChildElement("Hsp_gaps");
    if(!elem.isNull()) {
        gaps = elem.text().toInt(&isOk);
        if(!isOk) {
            stateInfo.setError(tr("Can't get gaps"));
            return;
        }
    }

    elem = xml.lastChildElement("Hsp_align-len");
    align_len = elem.text().toInt(&isOk);
    if(!isOk) {
        stateInfo.setError(tr("Can't get align length"));
        return;
    }
    //at new blast+ not need check strand
    ad->location->regions << U2Region(from-1, to - from + 1);
    circularization->uncircularizeLocation(ad->location);
    CHECK(!ad->location->regions.isEmpty(),);

    if(align_len != -1) {
        if(gaps != -1) {
            float percent = (float)gaps / (float)align_len * 100.;
            QString str = QString::number(gaps) + "/" + QString::number(align_len) + " (" + QString::number(percent,'g',4) + "%)";
            ad->qualifiers.push_back(U2Qualifier("gaps", str));
        }
        if(identities != -1) {
            float percent = (float)identities / (float)align_len * 100.;
            QString str = QString::number(identities) + '/' + QString::number(align_len) + " (" + QString::number(percent,'g',4) + "%)";
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
                } else if (hSeq.at(i) == '-')  {
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

    ad->qualifiers.push_back(U2Qualifier("id",id));
    ad->qualifiers.push_back(U2Qualifier("def",def));
    ad->qualifiers.push_back(U2Qualifier("accession",accession));
    U1AnnotationUtils::addDescriptionQualifier(ad, settings.annDescription);
    ad->name = "blast result";
    result.append(ad);
}

///////////////////////////////////////
//BlastPlusSupportMultiTask
BlastPlusSupportMultiTask::BlastPlusSupportMultiTask(QList<BlastTaskSettings>& _settingsList, QString& _url):
        Task("Run NCBI BlastAll multitask", TaskFlags_NR_FOSCOE | TaskFlag_ReportingIsSupported),
        settingsList(_settingsList), doc(NULL), url(_url)
{
}
void BlastPlusSupportMultiTask::prepare(){
    //create document
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    //url=settingsList[0].outputResFile;
    doc = df->createNewLoadedDocument(iof, url, stateInfo);
    CHECK_OP(stateInfo,);

    foreach(BlastTaskSettings settings, settingsList){
        settings.needCreateAnnotations=false;
        Task * t=NULL;
        if(settings.programName == "blastn"){
            t = new BlastNPlusSupportTask(settings);
        }else if(settings.programName == "blastp" || settings.programName == "gpu-blastp"){
            t = new BlastPPlusSupportTask(settings);
        }else if(settings.programName == "blastx"){
            t = new BlastXPlusSupportTask(settings);
        }else if(settings.programName == "tblastn"){
            t = new TBlastNPlusSupportTask(settings);
        }else if(settings.programName == "tblastx"){
            t = new TBlastXPlusSupportTask(settings);
        }else if(settings.programName == "rpsblast"){
            t = new RPSBlastSupportTask(settings);
        }
        addSubTask(t);
    }
}
QList<Task*> BlastPlusSupportMultiTask::onSubTaskFinished(Task *subTask){
    QList<Task*> res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }
    BlastPlusSupportCommonTask* s=qobject_cast<BlastPlusSupportCommonTask*>(subTask);
    if(s != NULL){
        BlastTaskSettings settings=s->getSettings();
        assert(settings.aobj!=NULL);
        QList<SharedAnnotationData> result = s->getResultedAnnotations();
        if (!result.isEmpty()) {
            doc->addObject(settings.aobj);
            for (QMutableListIterator<SharedAnnotationData> it_ad(result); it_ad.hasNext();) {
                SharedAnnotationData &ad = it_ad.next();
                U2Region::shift(settings.offsInGlobalSeq, ad->location->regions);
            }
            res.append(new CreateAnnotationsTask(settings.aobj, result, settings.groupName));
        }
    }
    return res;
}

Task::ReportResult BlastPlusSupportMultiTask::report(){
    if(!hasError()){
        if(doc->getObjects().length() > 0){
            AppContext::getProject()->addDocument(doc);
        }else{
            setReportingEnabled(true);
        }
    }
    return ReportResult_Finished;
}

QString BlastPlusSupportMultiTask::generateReport() const {
    QString res;

    res+="<table>";
    res+="<tr><td width=200><b>" + tr("Source file") + "</b></td><td>" + settingsList.at(0).queryFile + "</td></tr>";
    res+="<tr><td width=200><b>" + tr("Used database") + "</b></td><td>" + settingsList.at(0).databaseNameAndPath + "</td></tr>";
    res+="<tr></tr>";
    res+="<tr><td width=200><b>" + tr("No any results found") + "</b></td><td></td></tr>";
    res+="</table>";
    return res;
}

QString BlastPlusSupportCommonTask::toolIdByProgram(const QString &program) {
    QString result;
    if("blastn" == program){
        result = BlastPlusSupport::ET_BLASTN_ID;
    } else if ("blastp" == program){
        result = BlastPlusSupport::ET_BLASTP_ID;
    } else if ("blastx" == program){
        result = BlastPlusSupport::ET_BLASTX_ID;
    } else if ("tblastn" == program){
        result = BlastPlusSupport::ET_TBLASTN_ID;
    } else if ("tblastx" == program){
        result = BlastPlusSupport::ET_TBLASTX_ID;
    } else if ("rpsblast" == program) {
        result = BlastPlusSupport::ET_RPSBLAST_ID;
    }
    return result;
}

}//namespace
