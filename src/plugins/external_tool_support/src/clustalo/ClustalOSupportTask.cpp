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

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/OpenViewTask.h>

#include "ClustalOSupport.h"
#include "ClustalOSupportTask.h"

namespace U2 {

void ClustalOSupportTaskSettings::reset() {
    numIterations=-1;
    maxGuidetreeIterations=-1;
    maxHMMIterations=-1;

    setAutoOptions=false;
    numberOfProcessors=1;
}

ClustalOSupportTask::ClustalOSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const ClustalOSupportTaskSettings& _settings)
    : ExternalToolSupportTask("Run ClustalO alignment task", TaskFlags_NR_FOSCOE),
      inputMsa(_inputMsa->getExplicitCopy()),
      objRef(_objRef),
      settings(_settings),
      lock(NULL)
{
    GCOUNTER( cvar, tvar, "ClustalOSupportTask" );
    saveTemporaryDocumentTask=NULL;
    loadTemporyDocumentTask=NULL;
    clustalOTask=NULL;
    tmpDoc=NULL;
    resultMA->setName(inputMsa->getName());
    resultMA->setAlphabet(inputMsa->getAlphabet());
}

ClustalOSupportTask::~ClustalOSupportTask() {
    if (NULL != tmpDoc) {
        delete tmpDoc;
    }
    //Unlock the alignment object if the task has been failed
    if(!lock.isNull()) {
        if (objRef.isValid()) {
            GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
            if (NULL != obj) {
                MultipleSequenceAlignmentObject* alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
                CHECK(NULL != alObj, );
                if(alObj->isStateLocked()) {
                    alObj->unlockState(lock);
                }
                delete lock;
                lock = NULL;
            }
        }
    }
}

void ClustalOSupportTask::prepare(){
    algoLog.info(tr("ClustalO alignment started"));

    if (objRef.isValid()) {
        GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
        if (NULL != obj) {
            MultipleSequenceAlignmentObject* alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
            SAFE_POINT(NULL != alObj, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying ClustalW results!",);
            lock = new StateLock("ClustalWAligment");
            alObj->lockState(lock);
        }
    }

    //Add new subdir for temporary files
    //Folder name is ExternalToolName + CurrentDate + CurrentTime

    QString tmpDirName = "ClustalO_"+QString::number(this->getTaskId())+"_"+
                         QDate::currentDate().toString("dd.MM.yyyy")+"_"+
                         QTime::currentTime().toString("hh.mm.ss.zzz")+"_"+
                         QString::number(QCoreApplication::applicationPid())+"/";
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(ClustalOSupport::CLUSTALO_TMP_DIR) + "/" + tmpDirName;
    url= tmpDirPath + "tmp.aln";
    ioLog.details(tr("Saving data to temporary file '%1'").arg(url));

    //Check and remove subdir for temporary files
    QDir tmpDir(tmpDirPath);
    if(tmpDir.exists()){
        foreach(const QString& file, tmpDir.entryList()){
            tmpDir.remove(file);
        }
        if(!tmpDir.rmdir(tmpDir.absolutePath())){
            stateInfo.setError(tr("Subfolder for temporary files exists. Can not remove this folder."));
            return;
        }
    }
    if(!tmpDir.mkpath(tmpDirPath)){
        stateInfo.setError(tr("Can not create folder for temporary files."));
        return;
    }

    saveTemporaryDocumentTask = new SaveAlignmentTask( MSAUtils::setUniqueRowNames(inputMsa), url, BaseDocumentFormats::CLUSTAL_ALN);
    saveTemporaryDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveTemporaryDocumentTask);
}

QList<Task*> ClustalOSupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }
    QString outputUrl = url+".out.aln";
    if(subTask==saveTemporaryDocumentTask){
        QStringList arguments;
        arguments << "-v";
        arguments <<"--infile="+url;
        arguments << "--outfmt=clu";
        arguments << "--outfile="+outputUrl;
        if(settings.numIterations != -1){
            arguments << "--iterations" << QString::number(settings.numIterations);
        }
        if(settings.maxGuidetreeIterations != -1){
            arguments << "--max-guidetree-iterations="+QString::number(settings.maxGuidetreeIterations);
        }
        if(settings.maxHMMIterations!= -1){
            arguments << "--max-hmm-iterations="+QString::number(settings.maxHMMIterations);
        }
        if(settings.setAutoOptions){
            arguments << "--auto";
        }
        arguments <<"--threads="+QString::number(settings.numberOfProcessors);

        clustalOTask = new ExternalToolRunTask(ClustalOSupport::ET_CLUSTALO_ID, arguments, new ClustalOLogParser());
        setListenerForTask(clustalOTask);
        clustalOTask->setSubtaskProgressWeight(95);
        res.append(clustalOTask);
    }
    else if(subTask == clustalOTask){
        if(!QFileInfo(outputUrl).exists()){
            if(AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID)->isValid()){
                stateInfo.setError(tr("Output file %1 not found").arg(outputUrl));
            }else{
                stateInfo.setError(tr("Output file %3 not found. May be %1 tool path '%2' not valid?")
                                   .arg(AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID)->getName())
                                   .arg(AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID)->getPath())
                                   .arg(outputUrl));
            }
            emit si_stateChanged();
            return res;
        }
        ioLog.details(tr("Loading output file '%1'").arg(outputUrl));
        loadTemporyDocumentTask=
                new LoadDocumentTask(BaseDocumentFormats::CLUSTAL_ALN,
                                     outputUrl,
                                     AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE));
        loadTemporyDocumentTask->setSubtaskProgressWeight(5);
        res.append(loadTemporyDocumentTask);
    }
    else if(subTask == loadTemporyDocumentTask){
        tmpDoc=loadTemporyDocumentTask->takeDocument();
        SAFE_POINT(tmpDoc!=NULL, QString("output document '%1' not loaded").arg(tmpDoc->getURLString()), res);
        SAFE_POINT(tmpDoc->getObjects().length()==1, QString("no objects in output document '%1'").arg(tmpDoc->getURLString()), res);

        // Get the result alignment
        MultipleSequenceAlignmentObject* newMAligmentObject = qobject_cast<MultipleSequenceAlignmentObject*>(tmpDoc->getObjects().first());
        SAFE_POINT(newMAligmentObject!=NULL, "newDocument->getObjects().first() is not a MultipleSequenceAlignmentObject", res);

        resultMA = newMAligmentObject->getMsaCopy();
        bool renamed = MSAUtils::restoreRowNames(resultMA, inputMsa->getRowNames());
        SAFE_POINT( renamed, "Failed to restore initial row names!", res);

        // If an alignment object has been specified, save the result to it
        if (objRef.isValid()) {
            GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
            if (NULL != obj) {
                MultipleSequenceAlignmentObject* alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
                SAFE_POINT(NULL != alObj, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying ClustalO results!", res);

                QList<qint64> rowsOrder = MSAUtils::compareRowsAfterAlignment(inputMsa, resultMA, stateInfo);
                CHECK_OP(stateInfo, res);

                // Save data to the database
                {
                    if(!lock.isNull()) {
                        if(alObj->isStateLocked()) {
                            alObj->unlockState(lock);
                        }
                        delete lock;
                        lock = NULL;
                    }
                    else {
                        stateInfo.setError("MultipleSequenceAlignment object has been changed");
                        return res;
                    }
                    U2OpStatus2Log os;
                    U2UseCommonUserModStep userModStep(obj->getEntityRef(), os);
                    Q_UNUSED(userModStep);
                    if (os.hasError()) {
                        stateInfo.setError("Failed to apply the result of the alignment!");
                        return res;
                    }

                    if (rowsOrder.count() != inputMsa->getNumRows()) {
                        // Find rows that were removed by ClustalO and remove them from MSA
                        for (int i = inputMsa->getNumRows() - 1; i >= 0; i--) {
                            qint64 rowId = inputMsa->getMsaRow(i)->getRowDbInfo().rowId;
                            if (!rowsOrder.contains(rowId)) {
                                alObj->removeRow(i);
                            }
                        }
                        MaModificationInfo mi;
                        mi.rowContentChanged = false;
                        alObj->updateCachedMultipleAlignment(mi);
                    }

                    QMap<qint64, QList<U2MsaGap> > rowsGapModel;
                    for (int i = 0, n = resultMA->getNumRows(); i < n; ++i) {
                        qint64 rowId = resultMA->getMsaRow(i)->getRowDbInfo().rowId;
                        const QList<U2MsaGap>& newGapModel = resultMA->getMsaRow(i)->getGapModel();
                        rowsGapModel.insert(rowId, newGapModel);
                    }

                    alObj->updateGapModel(stateInfo, rowsGapModel);
                    SAFE_POINT_OP(stateInfo, res);

                    if (rowsOrder != inputMsa->getRowsIds()) {
                        alObj->updateRowsOrder(stateInfo, rowsOrder);
                        SAFE_POINT_OP(stateInfo, res);
                    }
                }

                Document* currentDocument = alObj->getDocument();
                SAFE_POINT(NULL != currentDocument, "Document is NULL!", res);
                currentDocument->setModified(true);
            }
            else {
                algoLog.error(tr("Failed to apply the result of ClustalO: alignment object is not available!"));
                return res;
            }
        }

        algoLog.info(tr("ClustalO alignment successfully finished"));
        //new document deleted in destructor of LoadDocumentTask
    }
    return res;
}
Task::ReportResult ClustalOSupportTask::report(){
    //Remove subdir for temporary files, that created in prepare
    if(!url.isEmpty()){
        QDir tmpDir(QFileInfo(url).absoluteDir());
        foreach(QString file, tmpDir.entryList()){
            tmpDir.remove(file);
        }
        if(!tmpDir.rmdir(tmpDir.absolutePath())){
            stateInfo.setError(tr("Can not remove folder for temporary files."));
            emit si_stateChanged();
        }
    }

    return ReportResult_Finished;
}

////////////////////////////////////////
//ClustalOWithExtFileSpecifySupportTask
ClustalOWithExtFileSpecifySupportTask::ClustalOWithExtFileSpecifySupportTask(const ClustalOSupportTaskSettings& _settings) :
        Task("Run ClustalO alignment task", TaskFlags_NR_FOSCOE),
        settings(_settings)
{
    GCOUNTER( cvar, tvar, "ClustalOSupportTask" );
    mAObject = NULL;
    currentDocument = NULL;
    saveDocumentTask = NULL;
    loadDocumentTask = NULL;
    clustalOSupportTask = NULL;
    cleanDoc = true;
}

ClustalOWithExtFileSpecifySupportTask::~ClustalOWithExtFileSpecifySupportTask(){
    if (cleanDoc) {
        delete currentDocument;
    }
}

void ClustalOWithExtFileSpecifySupportTask::prepare(){
    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.rawData = IOAdapterUtils::readFileHeader(settings.inputFilePath);
    c.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(c);
    if (formats.isEmpty()) {
        stateInfo.setError(tr("Unrecognized input alignment file format"));
        return;
    }

    DocumentFormatId alnFormat = formats.first();
    QVariantMap hints;
    if(alnFormat == BaseDocumentFormats::FASTA){
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    }
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.inputFilePath));
    loadDocumentTask = new LoadDocumentTask(alnFormat, settings.inputFilePath,iof, hints);

    addSubTask(loadDocumentTask);
}
QList<Task*> ClustalOWithExtFileSpecifySupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask == loadDocumentTask) {
        currentDocument = loadDocumentTask->takeDocument();
        SAFE_POINT(currentDocument != NULL, QString("Failed loading document: %1").arg(loadDocumentTask->getURLString()), res);
        SAFE_POINT(currentDocument->getObjects().length() == 1, QString("Number of objects != 1 : %1").arg(loadDocumentTask->getURLString()), res);

        mAObject = qobject_cast<MultipleSequenceAlignmentObject*>(currentDocument->getObjects().first());
        SAFE_POINT(mAObject != NULL, QString("MA object not found!: %1").arg(loadDocumentTask->getURLString()), res);

        // Launch the task, objRef is empty - the input document maybe not in project
        clustalOSupportTask = new ClustalOSupportTask(mAObject->getMultipleAlignment(), GObjectReference(), settings);
        res.append(clustalOSupportTask);
    }
    else if (subTask == clustalOSupportTask) {
        // Set the result alignment to the alignment object of the current document
        mAObject=qobject_cast<MultipleSequenceAlignmentObject*>(currentDocument->getObjects().first());
        SAFE_POINT(mAObject != NULL, QString("MA object not found!: %1").arg(loadDocumentTask->getURLString()), res);
        mAObject->updateGapModel(clustalOSupportTask->resultMA->getMsaRows());

        // Save the current document
        saveDocumentTask = new SaveDocumentTask(currentDocument,
            AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.outputFilePath)),
            settings.outputFilePath);
        res.append(saveDocumentTask);
    }
    else if (subTask == saveDocumentTask) {
        Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(settings.outputFilePath);
        if (openTask != NULL) {
            res << openTask;
        }
    }
    return res;
}
Task::ReportResult ClustalOWithExtFileSpecifySupportTask::report(){
    return ReportResult_Finished;
}

////////////////////////////////////////
//ClustalOLogParser
ClustalOLogParser::ClustalOLogParser() : ExternalToolLogParser() {

}
void ClustalOLogParser::parseOutput(const QString& partOfLog){
    lastPartOfLog=partOfLog.split(QChar('\r'));
    lastPartOfLog.first()=lastLine+lastPartOfLog.first();
    lastLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        if(buf.contains("error",Qt::CaseInsensitive)){
            ioLog.error(buf);
            lastError = buf;
        }else{
            ioLog.trace(buf);
        }
    }
}
int ClustalOLogParser::getProgress(){
    if(!lastPartOfLog.isEmpty()){
        QString lastMessage=lastPartOfLog.last();
        //0..10% progresss
        if(lastMessage.contains(QRegExp("Pairwise distance calculation progress: \\d+ %"))){
            QRegExp rx("Pairwise distance calculation progress: (\\d+) %");
            rx.indexIn(lastMessage);
            CHECK(rx.captureCount() > 0, 0);
            return rx.cap(1).toInt()/10;
        }
        //10..20% progresss
        if(lastMessage.contains(QRegExp("Distance calculation within sub-clusters: \\d+ %"))){
            QRegExp rx("Distance calculation within sub-clusters: (\\d+) %");
            rx.indexIn(lastMessage);
            CHECK(rx.captureCount() > 0, 0);
            return rx.cap(1).toInt()/10+10;
        }
        //20..100% progresss
        if(lastMessage.contains(QRegExp("Progressive alignment progress: (\\d+) %"))){
            QRegExp rx("Progressive alignment progress: (\\d+) %");
            rx.indexIn(lastMessage);
            CHECK(rx.captureCount() > 0, 0);
            return rx.cap(1).toInt()*0.8+20;
        }

    }
    return 0;
}
}//namespace
