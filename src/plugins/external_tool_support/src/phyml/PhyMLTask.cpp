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

#include "PhyMLTask.h"

#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Core/DocumentModel.h>
#include <QDir>

namespace U2 {

const QString PhyMLSupportTask::TMP_FILE_NAME("tmp.phy");
const QString PhyMLSupportTask::RESULT_BOOTSTRAP_EXT("_phyml_boot_trees.txt");
const QString PhyMLSupportTask::RESULT_TREE_EXT("_phyml_tree.txt");

PhyMLPrepareDataForCalculation::PhyMLPrepareDataForCalculation(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s, const QString& url)
    : Task(tr("Generating input file for PhyML"), TaskFlags_NR_FOSE_COSC),
      ma(ma),
      settings(s),
      tmpDirUrl(url),
      saveDocumentTask(NULL)
{
}
void PhyMLPrepareDataForCalculation::prepare(){
    inputFileForPhyML = tmpDirUrl + '/' + PhyMLSupportTask::TMP_FILE_NAME;
    QVariantMap hints;
    hints.insert(DocumentWritingMode_SimpleNames, DocumentWritingMode_SimpleNames);
    saveDocumentTask = new SaveAlignmentTask(ma, inputFileForPhyML, BaseDocumentFormats::PHYLIP_INTERLEAVED, hints);
    saveDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveDocumentTask);
}
QList<Task*> PhyMLPrepareDataForCalculation::onSubTaskFinished(Task* subTask){
    QList<Task*> res;

    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }

    if(subTask == saveDocumentTask){
        SAFE_POINT_EXT(NULL != saveDocumentTask->getDocument(), setError(tr("Internal UGENE error")), res);

        QString fileUrl = saveDocumentTask->getDocument()->getURLString();

        SAFE_POINT_EXT(!fileUrl.isEmpty(), setError(tr("Internal UGENE error")), res);

        QFile tmpFile(fileUrl);
        if(!tmpFile.open(QIODevice::Append)){
            setError(tr("Can not open tmp file"));
            return res;
        }
    }

    return res;
}

PhyMLSupportTask::PhyMLSupportTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s)
    : PhyTreeGeneratorTask(ma, s),
      prepareDataTask(NULL),
      phyMlTask(NULL),
      getTreeTask(NULL),
      sequencesNumber(0)
{
    GCOUNTER( cvar, tvar, "PhyMLSupportTask" );

    sequencesNumber = ma->getNumRows();

    setTaskName(tr("PhyML tree calculation"));

    setMaxParallelSubtasks(1);

    tpm = Task::Progress_SubTasksBased;
}

void PhyMLSupportTask::prepare(){
    //Add new subdir for temporary files

    tmpDirUrl = ExternalToolSupportUtils::createTmpDir(PhyMLSupport::PHYML_TEMP_DIR, stateInfo);
    CHECK_OP(stateInfo, );

    prepareDataTask = new PhyMLPrepareDataForCalculation(inputMA, settings, tmpDirUrl);
    prepareDataTask->setSubtaskProgressWeight(5);
    addSubTask(prepareDataTask);
}

Task::ReportResult PhyMLSupportTask::report(){
    U2OpStatus2Log os;
    ExternalToolSupportUtils::removeTmpDir(tmpDirUrl,os);
    return ReportResult_Finished;
}

QList<Task*> PhyMLSupportTask::onSubTaskFinished(Task* subTask){
    QList<Task*> res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }

    if(subTask == prepareDataTask){
        tmpPhylipFile = prepareDataTask->getInputFileUrl();
        QStringList arguments;
        arguments << "-i";
        arguments << tmpPhylipFile;
        arguments << "--no_memory_check";
        arguments << settings.extToolArguments;
        phyMlTask = new ExternalToolRunTask(PhyMLSupport::PHYML_ID, arguments, new PhyMLLogParser(this, sequencesNumber));
        phyMlTask->setSubtaskProgressWeight(95);
        res.append(phyMlTask);
    }else if(subTask == phyMlTask){
        getTreeTask = new PhyMLGetCalculatedTreeTask(tmpPhylipFile + PhyMLSupportTask::RESULT_TREE_EXT);
        getTreeTask->setSubtaskProgressWeight(5);
        res.append(getTreeTask);
    }else if(subTask == getTreeTask){
        PhyTreeObject* phyObj = getTreeTask->getPhyObject();
        SAFE_POINT_EXT(NULL != phyObj, setError(tr("UGENE internal error")), QList<Task*>());
        result = phyObj->getTree();
    }

    return res;
}

void PhyMLSupportTask::onExternalToolFailed(const QString& err) {
    if(NULL != phyMlTask) {
        phyMlTask->setError(err);
    }
}

PhyMLLogParser::PhyMLLogParser(PhyMLSupportTask* parentTask, int sequencesNumber)
    : parentTask(parentTask),
      isMCMCRunning(false),
      curProgress(0),
      processedBranches(0),
      sequencesNumber(sequencesNumber)
{
}
void PhyMLLogParser::parseOutput(const QString& partOfLog){
    lastPartOfLog = partOfLog.split(QChar('\n'));
    lastPartOfLog.first() = lastLine + lastPartOfLog.first();
    lastLine = lastPartOfLog.takeLast();

    foreach(QString buf, lastPartOfLog){
        if(buf.contains("Type any key to exit")) {
            int index = lastPartOfLog.indexOf(buf);
            QString errorString = tr("PhyML finished with error");
            if(index >= 2) {
                errorString += lastPartOfLog.at(index - 2);
            }
            parentTask->onExternalToolFailed(errorString);
            return;
        }
        else if(buf.contains("[Branch lengths")) {
            processedBranches++;
        }
        ioLog.trace(buf);
    }
}
void PhyMLLogParser::parseErrOutput(const QString& partOfLog){
    parseOutput(partOfLog);
}
int PhyMLLogParser::getProgress(){
    SAFE_POINT(sequencesNumber > 0, tr("UGENE internal error"), 0);
    return qMin((processedBranches * 100) / sequencesNumber, 99);
}

PhyMLGetCalculatedTreeTask::PhyMLGetCalculatedTreeTask(const QString& url)
    : Task(tr("Generating output trees from PhyML"),
      TaskFlags_NR_FOSE_COSC),
      baseFileName(url),
      loadTmpDocumentTask(NULL),
      phyObject(NULL)
{
}

void PhyMLGetCalculatedTreeTask::prepare(){
    if( !QFile::exists(baseFileName)) {
        stateInfo.setError(tr("Output file is not found"));
        return;
    }

    loadTmpDocumentTask =
        new LoadDocumentTask(BaseDocumentFormats::NEWICK,
        baseFileName,
        AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE));
    loadTmpDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(loadTmpDocumentTask);
}
QList<Task*> PhyMLGetCalculatedTreeTask::onSubTaskFinished(Task* subTask){
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
            setError(tr("No trees are found"));
            return res;
        }

        const QList<GObject*>& treeList = doc->getObjects();
        SAFE_POINT_EXT(treeList.count() > 0, setError(tr("No result tree in PhyML output")), res);
        int index = 1; //the second tree in the file is needed
        if(treeList.count() - 1 < index){
            index = 0;
        }
        phyObject = qobject_cast<PhyTreeObject*>( treeList.at(index) );
        SAFE_POINT_EXT(NULL != phyObject, setError(tr("No result tree in PhyML output")), res);
    }

    return res;
}


}//namespace
