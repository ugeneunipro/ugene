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

#include <QFile>
#include <QTextStream>

#include "MrBayesTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/DocumentModel.h>

namespace U2 {

#define TMPFILENAME "tmp.nex"
#define TREEFILEEXT ".con.tre"

MrBayesPrepareDataForCalculation::MrBayesPrepareDataForCalculation(const MultipleSequenceAlignment& _ma, const CreatePhyTreeSettings& s, const QString& url)
:Task(tr("Generating input file for MrBayes"), TaskFlags_NR_FOSCOE), ma(_ma), settings(s), tmpDirUrl(url){

    saveDocumentTask = NULL;
}
void MrBayesPrepareDataForCalculation::prepare(){
    inputFileForMrBayes = tmpDirUrl + "/"+TMPFILENAME;
    QVariantMap hints;
    hints.insert(DocumentWritingMode_SimpleNames, DocumentWritingMode_SimpleNames);
    saveDocumentTask=new SaveAlignmentTask(ma, inputFileForMrBayes, BaseDocumentFormats::NEXUS, hints);
    saveDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveDocumentTask);
}
QList<Task*> MrBayesPrepareDataForCalculation::onSubTaskFinished(Task* subTask){
    QList<Task*> res;

    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }

    if(subTask == saveDocumentTask){
        assert(saveDocumentTask->getDocument());

        QString fileUrl = saveDocumentTask->getDocument()->getURLString();

        assert(!fileUrl.isEmpty());

        QFile tmpFile(fileUrl);
        if(!tmpFile.open(QIODevice::Append)){
            setError("Can not open tmp file");
            return res;
        }
        //append MrBayes settings to the file
        QTextStream ts(&tmpFile);
        ts << settings.mrBayesSettingsScript;
    }

    return res;
}

MrBayesSupportTask::MrBayesSupportTask(const MultipleSequenceAlignment& _ma, const CreatePhyTreeSettings& s)
:PhyTreeGeneratorTask(_ma, s)
{
    GCOUNTER( cvar, tvar, "MrBayesSupportTask" );

    setTaskName(tr("MrBayes tree calculation"));

    setMaxParallelSubtasks(1);

    tpm = Task::Progress_SubTasksBased;

    prepareDataTask = NULL;
    mrBayesTask = NULL;
    getTreeTask = NULL;
}

void MrBayesSupportTask::prepare(){
    //Add new subdir for temporary files

    tmpDirUrl = ExternalToolSupportUtils::createTmpDir(MrBayesSupport::MRBAYES_TMP_DIR, stateInfo);
    CHECK_OP(stateInfo, );

    prepareDataTask = new MrBayesPrepareDataForCalculation(inputMA, settings, tmpDirUrl);
    prepareDataTask->setSubtaskProgressWeight(5);
    addSubTask(prepareDataTask);
}

Task::ReportResult MrBayesSupportTask::report(){
    U2OpStatus2Log os;
    ExternalToolSupportUtils::removeTmpDir(tmpDirUrl,os);
    return ReportResult_Finished;
}

QList<Task*> MrBayesSupportTask::onSubTaskFinished(Task* subTask){
    QList<Task*> res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }

    if(subTask == prepareDataTask){
        tmpNexusFile = prepareDataTask->getInputFileUrl();
        QStringList arguments;
        arguments << tmpNexusFile;
        mrBayesTask = new ExternalToolRunTask(MrBayesSupport::ET_MRBAYES_ID, arguments, new MrBayesLogParser(settings.mb_ngen));
        mrBayesTask->setSubtaskProgressWeight(95);
        res.append(mrBayesTask);
    }else if(subTask == mrBayesTask){
        getTreeTask = new MrBayesGetCalculatedTreeTask(tmpNexusFile);
        getTreeTask->setSubtaskProgressWeight(5);
        res.append(getTreeTask);
    }else if(subTask == getTreeTask){
        PhyTreeObject* phyObj = getTreeTask->getPhyObject();
        assert(phyObj);
        result = phyObj->getTree();
    }

    return res;
}

MrBayesLogParser::MrBayesLogParser(int _nchains)
:nchains(_nchains), isMCMCRunning(false), curProgress(0){

}
void MrBayesLogParser::parseOutput(const QString& partOfLog){
    lastPartOfLog=partOfLog.split(QChar('\n'));
    lastPartOfLog.first()=lastLine+lastPartOfLog.first();
    lastLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        ioLog.trace(buf);
    }
}
void MrBayesLogParser::parseErrOutput(const QString& partOfLog){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastErrLine+lastPartOfLog.first();
    lastErrLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
        if(buf.contains(QRegExp("^\\d+"))
            ||buf.contains("WARNING")
            ||buf.contains(QRegExp("^-\\w"))
            ||buf.contains("No trees are sampled")){
                algoLog.trace(buf);
        }else if (buf.contains("lastError")){
            //
        }else{

            algoLog.info(buf);
        }
    }
}
int MrBayesLogParser::getProgress(){
    if(!lastPartOfLog.isEmpty()){
        foreach(QString currentMsg, lastPartOfLog){
            if(currentMsg.contains("Chain results:")){
                isMCMCRunning = true;
                curProgress = 0;
            }else if (currentMsg.contains("Analysis completed in")){
                isMCMCRunning = false;
                curProgress = 100;
            }else if(isMCMCRunning){
                if(currentMsg.contains(QRegExp("\\d+ -- "))){
                    QRegExp rx("(\\d+) -- ");
                    assert(rx.indexIn(currentMsg)>-1);
                    rx.indexIn(currentMsg);
                    curProgress = rx.cap(1).toInt()*100/(float)nchains;
                }
            }
        }
        return curProgress;
    }
    return curProgress;
}

MrBayesGetCalculatedTreeTask::MrBayesGetCalculatedTreeTask(const QString& url)
    : Task(tr("Generating output trees from MrBayes"), TaskFlags_NR_FOSCOE),
      baseFileName(url),
      loadTmpDocumentTask(NULL),
      phyObject(NULL)
{

}

void MrBayesGetCalculatedTreeTask::prepare(){
    QString treeFile = baseFileName+TREEFILEEXT;
    if( !QFile::exists(treeFile)) {
        stateInfo.setError(tr("Output file is not found"));
        return;
    }

    loadTmpDocumentTask=
        new LoadDocumentTask(BaseDocumentFormats::NEXUS,
        treeFile,
        AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE));
    loadTmpDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(loadTmpDocumentTask);
}
QList<Task*> MrBayesGetCalculatedTreeTask::onSubTaskFinished(Task* subTask){
    QList<Task*> res;

    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }
    if(subTask == loadTmpDocumentTask){
        Document* doc = loadTmpDocumentTask->getDocument();
        SAFE_POINT(doc != NULL, "Failed loading result document", res);

        if (doc->getObjects().size() == 0) {
            // TODO: delete new file?
            setError(tr("No trees are found"));
            return res;
        }

        const QList<GObject*>& treeList = doc->getObjects();
        assert(treeList.count() > 0);
        int index = 1; //the second tree in the file is needed
        if(treeList.count() - 1 < index){
            index = 0;
        }
        phyObject = qobject_cast<PhyTreeObject*>( treeList.at(index) );
    }

    return res;
}

}//namespace
