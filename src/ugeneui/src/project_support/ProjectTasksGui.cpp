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

#include <QDomDocument>
#include <QMainWindow>
#include <QMessageBox>
#include <QMutex>

#include <AppContextImpl.h>

#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GHints.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/RemoveDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UnloadedObject.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/ProjectParsing.h>
#include <U2Gui/UnloadDocumentTask.h>

#include "ProjectImpl.h"
#include "ProjectLoaderImpl.h"
#include "ProjectServiceImpl.h"
#include "ProjectTasksGui.h"
#include "ui_SaveProjectDialog.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
///Close project
CloseProjectTask::CloseProjectTask() : Task(tr("Close project"), TaskFlags(TaskFlag_NoRun) | TaskFlag_CancelOnSubtaskCancel)
{
}

void CloseProjectTask::prepare() {
    if (AppContext::getProject()==NULL) {
        stateInfo.setError(  tr("No active project found") );
        return;
    }
    /* TODO: this is done by project view. Need to cleanup this part! 
    addSubTask(new SaveProjectTask(SaveProjectTaskKind_SaveProjectAndDocumentsAskEach));
    */
    QList<Task*> tasks;
    /**/
    Project *pp = AppContext::getProject();
    if (pp->isTreeItemModified()) {
        tasks.append(AppContext::getProjectService()->saveProjectTask(SaveProjectTaskKind_SaveProjectAndDocumentsAskEach));
    }
    /**/
    ServiceRegistry* sr = AppContext::getServiceRegistry();
    QList<Service*> services = sr->findServices(Service_Project);
    assert(services.size() == 1);
    Service* projectService = services.first();
    tasks.append(sr->unregisterServiceTask(projectService));
    addSubTask(new MultiTask(tr("Save and close project"), tasks));
}


//////////////////////////////////////////////////////////////////////////
/// OpenProjectTask
OpenProjectTask::OpenProjectTask(const QString& _url, const QString& _name) 
    : Task(tr("Open project/document"), TaskFlags_NR_FOSCOE), url(_url), name(_name), loadProjectTask(NULL)
{
}

void OpenProjectTask::prepare() {
    QFileInfo f(url);
    if (f.exists() && !(f.isFile() && f.isReadable())) {
        stateInfo.setError(  tr("Not a readable file: %1").arg(url) );
        return;
    }
    if (AppContext::getProject() != NULL) {
        addSubTask(new CloseProjectTask());
    }        

    if (f.exists()) {
        loadProjectTask = new LoadProjectTask(url);
        addSubTask(loadProjectTask);
    } else {
        ProjectImpl* p =  new ProjectImpl(name, url);
        addSubTask(new RegisterProjectServiceTask(p));
        //     addSubTask(new SaveProjectTask(SaveProjectTaskKind_SaveProjectOnly, p));
    }
}

QList<Task*> OpenProjectTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (!isCanceled() && subTask == loadProjectTask && !loadProjectTask->hasError()) {
        Project* p =  loadProjectTask->detachProject();
        res.append(new RegisterProjectServiceTask(p));
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////
/// Save project
SaveProjectTask::SaveProjectTask(SaveProjectTaskKind _k, Project* p, const QString& _url, bool silentSave_) 
    : Task(tr("Save project"), TaskFlags(TaskFlag_NoRun) | TaskFlag_CancelOnSubtaskCancel), k(_k), proj(p), url(_url), silentSave(silentSave_)
{
}

SaveProjectTask::~SaveProjectTask () {
}

void SaveProjectTask::prepare() {
    if (proj == NULL) {
        proj = AppContext::getProject();
    }
    assert(proj!=NULL);
    if (url.isEmpty()) {
        url = proj->getProjectURL();
    }
    QList<Task *> ssTasks;
    if (url.isEmpty() && (!proj->getGObjectViewStates().isEmpty() || proj->getDocuments().size() > 0)) {
        //show "save project?" dialog, if needed
        int savedSaveProjectState = AppContext::getAppSettings()->getUserAppsSettings()->getAskToSaveProject();

        QWidget* mainWindow = AppContext::getMainWindow()->getQMainWindow();
        int code;
        if(silentSave == true){
            code = QDialogButtonBox::Yes;
        }else{
            code = savedSaveProjectState;
            if (QDialogButtonBox::NoButton == savedSaveProjectState) {
                // QMessageBox::NoButton is a special invalid button state, represents that no saved choise was made
                QObjectScopedPointer<SaveProjectDialogController> saveProjectDialog = new SaveProjectDialogController(mainWindow);
                code = saveProjectDialog->exec();
                CHECK_EXT(!saveProjectDialog.isNull(), setError("dialog is NULL"), );

                if (code == QDialog::Rejected) {
                    code = QDialogButtonBox::Cancel;
                }

                if (QDialogButtonBox::Cancel != code && true == saveProjectDialog->dontAskCheckBox->isChecked()) {
                    AppContext::getAppSettings()->getUserAppsSettings()->setAskToSaveProject(code);
                }
            }
        }

        if (code == QDialogButtonBox::Cancel) {
            cancel();
            return;
        }

        if (code == QDialogButtonBox::Yes) {
            QObjectScopedPointer<ProjectDialogController> d = new ProjectDialogController(ProjectDialogController::Save_Project, mainWindow);
            const int rc = d->exec();
            CHECK_EXT(!d.isNull(), setError("dialog is NULL"), );

            if (rc == QDialog::Accepted) {
                AppContext::getProject()->setProjectName(d->projectNameEdit->text());
                url = d->projectFolderEdit->text() + "/" + d->projectFileEdit->text();
                if (!url.endsWith(PROJECTFILE_EXT)) {
                    url.append(PROJECTFILE_EXT);
                }
                AppContext::getProject()->setProjectURL(url);
            } else {
                cancel();
                return;
            }
        }
    }

    if (k!=SaveProjectTaskKind_SaveProjectOnly) {
        QList<Document*> modifiedDocs = SaveMultipleDocuments::findModifiedDocuments(AppContext::getProject()->getDocuments());
        if (!modifiedDocs.isEmpty()) {
            ssTasks.append(new SaveMultipleDocuments(modifiedDocs, k == SaveProjectTaskKind_SaveProjectAndDocumentsAskEach, SavedNewDoc_Open));
        }
    }
    if (!url.isEmpty()) {
        ssTasks.append(new SaveOnlyProjectTask(proj, url));
    }
    if (!ssTasks.isEmpty()) {
        addSubTask(new MultiTask(tr("Save documents, remove phantom docs, save project"), ssTasks, false, TaskFlags_NR_FOSE_COSC));
    }
}

//////////////////////////////////////////////////////////////////////////
/// SaveOnlyProjectTask

SaveOnlyProjectTask::SaveOnlyProjectTask(Project* p, const QString& _url)
: Task(tr("Save project"), TaskFlag_NoRun), sub(NULL), proj(p), url(_url)
{
        lock = NULL;
}

SaveOnlyProjectTask::~SaveOnlyProjectTask(){
    assert(lock == NULL);
}

void SaveOnlyProjectTask::prepare(){
    if (proj == NULL) {
        proj = AppContext::getProject();
    }
    assert(proj!=NULL);
    if (url.isEmpty()) {
        url = proj->getProjectURL();
    }

    foreach(Document *d, proj->getDocuments()){
        
        QStringList urls = d->getGHintsMap().value(ProjectLoaderHint_MultipleFilesMode_URLDocument, QStringList()).toStringList();
        if(urls.isEmpty()){ // not merged document
            if (d->getURL().isLocalFile()) {
                QFile pathToDoc(d->getURLString());
                if (pathToDoc.exists()){
                    continue;
                }
                phantomDocs.append(d);
            }
        }
        else{ // merged document
            foreach(QString url, urls){
                QFile pathToDoc(url);
                if (!pathToDoc.exists()){
                    phantomDocs.append(d);
                    break;
                }
            }
        }
    }
    if (!phantomDocs.isEmpty()){
        sub = new RemoveMultipleDocumentsTask(proj, phantomDocs, false, false);
        addSubTask(sub);
    } else {
        _run();
    }
}

QList<Task*> SaveOnlyProjectTask::onSubTaskFinished(Task* subTask){
    QList<Task*> ret;
    if (!isCanceled() && subTask == sub){
        _run();
    }
    return ret;
}

void SaveOnlyProjectTask::_run(){
    lock = new StateLock(getTaskName(), StateLockFlag_LiveLock);
    proj->lockState(lock);

    coreLog.info(tr("Saving project %1").arg(url));
    ProjectFileUtils::saveProjectFile(stateInfo, proj, url);
}

Task::ReportResult SaveOnlyProjectTask::report(){
    if (!stateInfo.hasError() && url == proj->getProjectURL()) {
        proj->setModified(false);
    }
    proj->unlockState(lock);
    delete lock;
    lock = NULL;
    return Task::ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
/// LoadProjectTask
LoadProjectTask::LoadProjectTask(const QString& _url) 
    : Task(tr("Load project"), TaskFlag_None), proj(NULL), url(_url)
{
    xmlDoc = new QDomDocument();
}

LoadProjectTask::~LoadProjectTask() {
    delete proj;
    delete xmlDoc;
}

void LoadProjectTask::run() {
    coreLog.details(tr("Loading project from: %1").arg(url));
    ProjectFileUtils::loadXMLProjectModel(url, stateInfo, *xmlDoc, version);
}

Task::ReportResult LoadProjectTask::report() {
    if (!stateInfo.hasError()) {
        ProjectParserRegistry *ppr = ProjectParserRegistry::instance();
        ProjectParser *parser = ppr->getProjectParserByVersion(version);
        if(parser == NULL){
            coreLog.info(tr("Unable to detect parser for project"));
            setError(tr("failed to parse project file %1").arg(url));
        }else{
            proj = parser->createProjectFromXMLModel(url, *xmlDoc, stateInfo);
        }
        if (proj!=NULL) {
            coreLog.info(tr("Project loaded: %1").arg(url));
        } 
    }
    return Task::ReportResult_Finished;
}




RegisterProjectServiceTask::RegisterProjectServiceTask(Project* _proj) 
    : Task(tr("Register project"), TaskFlag_NoRun), proj(_proj)
{
}

void RegisterProjectServiceTask::prepare() {
    ProjectServiceImpl* ps = new ProjectServiceImpl(proj);
    addSubTask(AppContext::getServiceRegistry()->registerServiceTask(ps));
}

ExportProjectTask::ExportProjectTask(const QString& _destinationDir, const QString& _file, bool _compress):
Task(tr("Export project task"), TaskFlags_NR_FOSCOE), compress(_compress), destinationDir(_destinationDir), projectFile(_file)
{
    assert(!destinationDir.isEmpty());
    setVerboseLogMode(true);
}

void ExportProjectTask::prepare(){
    Project *pr = AppContext::getProject();
    if (pr->isItemModified()) {
        //setting url in case of anonymous project
        if (pr->getProjectURL().isEmpty()) {
            QString tempDir = AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath();
            pr->setProjectURL(tempDir + QDir::separator() + projectFile);
        }else{
            addSubTask(new SaveProjectTask(SaveProjectTaskKind_SaveProjectAndDocuments, pr, QString::null, true));  
        }
    }
}

Task::ReportResult ExportProjectTask::report() {
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }
    Project *pr = AppContext::getProject();

    QList<Document*> docList = pr->getDocuments();
    foreach(Document* doc, docList){
        if(doc->getURL().isEmpty()){
            setError(tr("One of the project documents has empty URL"));
            return ReportResult_Finished;
        }
    }
    
    QDir dDir(GUrlUtils::prepareDirLocation(destinationDir, stateInfo));
    CHECK_OP(stateInfo, ReportResult_Finished);
    
    QMap<QString, QString> urlRemap;
    foreach(Document* doc, docList){
        QString origPath = doc->getURLString();
        IOAdapterId id = doc->getIOAdapterFactory()->getAdapterId();
        if (id == BaseIOAdapters::LOCAL_FILE || id == BaseIOAdapters::GZIPPED_LOCAL_FILE){
            QFile f(origPath);
            QFileInfo fi(f);
            QString resultPath = destinationDir + "/" + fi.fileName();
            resultPath = GUrlUtils::rollFileName(resultPath, "_copy" , QSet<QString>());
            if (resultPath != origPath && !f.copy(resultPath)){
                if (QFile::exists(resultPath)) {
                    setError(tr("Error during coping documents: file already exist"));
                } else {
                    setError(tr("Error during coping documents"));
                }
                return ReportResult_Finished;
            }
            urlRemap[origPath] = GUrl(resultPath).getURLString();
        }
    }
    if(pr->getProjectName().isEmpty()){
        QFileInfo fi(projectFile);
        pr->setProjectName(fi.baseName());
    }
 
    ProjectFileUtils::saveProjectFile(stateInfo, pr, destinationDir + "/" + projectFile, urlRemap);
    return ReportResult_Finished;
}


//////////////////////////////////////////////////////////////////////////
// tests

void GTest_LoadProject::init(XMLTestFormat*, const QDomElement& el) {
    addTaskResource(TaskResourceUsage( RESOURCE_PROJECT, 1, true));
    projContextName = el.attribute("index");
    if(!el.attribute("load_from_temp").isEmpty()){
        url = env->getVar("TEMP_DATA_DIR") + "/" + el.attribute("url");
    }else{
        url = env->getVar("COMMON_DATA_DIR") + "/" + el.attribute("url");
    }
}

void GTest_LoadProject::prepare(){
    QList<Task*> tasks;
    Project *previousProject = AppContext::getProject();
    ServiceRegistry* sr = AppContext::getServiceRegistry();
    QList<Service*> services = sr->findServices(Service_Project);
    int servSize = services.size();
    assert(servSize <= 1);
    if(servSize == 1){
        if (previousProject->metaObject() != NULL){
            Task *tt  = new CloseProjectTask();
            tasks.append(tt);
        }
    } else if(previousProject != NULL) {
        AppContextImpl::getApplicationContext()->setProject(NULL);
    }
    contextAdded = false;

    loadTask = new LoadProjectTask(url);
    tasks.append(loadTask);
    mt = new MultiTask(tr("Unload previous project, then load new"), tasks);
    addSubTask(mt);
}

Task::ReportResult GTest_LoadProject::report() {
    if (loadTask!=NULL && loadTask->hasError()) {
        stateInfo.setError( loadTask->getError());
    } else if (!projContextName.isEmpty()) {
        addContext(projContextName, loadTask->getProject());
        contextAdded = true;
    }
    return ReportResult_Finished;
}

QList<Task*> GTest_LoadProject::onSubTaskFinished( Task* subTask ){
    QList<Task*> subTasks;
    if (subTask->hasError()) {
        return subTasks;
    }
    if (subTask == mt) {
        Project *p = loadTask->getProject();
        if(p != NULL){
            AppContextImpl::getApplicationContext()->setProject(p);
        }
    }
    return subTasks;
}

void GTest_LoadProject::cleanup(){
    AppContextImpl::getApplicationContext()->setProject(NULL);
    XmlTest::cleanup();
}


QList<XMLTestFactory*> ProjectTests::createTestFactories(){
    QList<XMLTestFactory*> res;
    res.append(GTest_LoadProject::createFactory());
    res.append(GTest_ExportProject::createFactory());
    res.append(GTest_UnloadProject::createFactory());
    res.append(GTest_LoadDocumentFromProject::createFactory());
    return res;
}

void GTest_ExportProject::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);
    
    exportTask = NULL;
    url = env->getVar("TEMP_DATA_DIR") + el.attribute("url");
}

void GTest_ExportProject::prepare(){
    QDir qd(url);
    if(qd.exists(url)){
        if(!removeDir(qd)){
            stateInfo.setError(tr("GTest_ExportProject::prepare unable to clean folder for exporting"));
            return;
        }
    }   
    if(!qd.mkpath(url)){
        stateInfo.setError(tr("GTest_ExportProject::prepare unable to create folder for exporting"));
        return;
    }
    exportTask = new ExportProjectTask(url, "");
    addSubTask(exportTask);
}

Task::ReportResult GTest_ExportProject::report(){
    if(exportTask!=NULL){
        if (exportTask->hasError()) {
            stateInfo.setError(exportTask->getError());
        }
    }
    return ReportResult_Finished;
}

void GTest_ExportProject::cleanup(){
    QDir qd(url);
    if(qd.exists(url)){
        if(!removeDir(QDir(url))){
            coreLog.info(tr("GTest_ExportProject::cleanup unable to delete exported files"));
        }
    }

    XmlTest::cleanup();
}

bool GTest_ExportProject::removeDir( const QDir &aDir ){
    bool has_err = false;
    if (aDir.exists())//QDir::NoDotAndDotDot
    {
        QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot | 
            QDir::Dirs | QDir::Files);
        int count = entries.size();
        for (int idx = 0; idx < count; idx++)
        {
            QFileInfo entryInfo = entries[idx];
            QString path = entryInfo.canonicalFilePath();
            if (entryInfo.isDir()){
                QDir aaa(path);
                has_err = removeDir( aaa );
            }else{
                QFile file(path);
                if (!file.remove())
                    has_err = true;
            }
        }
        if (!aDir.rmdir(aDir.absolutePath()))
            has_err = true;
    }
    return(has_err);
}
void GTest_UnloadProject::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QString packedList = el.attribute("documents");
    if(packedList.isEmpty()){
        //document list can be empty!
        return;
    }
    unloadDocList = packedList.split(",");
}

void GTest_UnloadProject::prepare(){
    Project *p = AppContext::getProject();
    if(p != NULL){
        foreach(QString doc, unloadDocList){
            removeContext(doc);
        }
        AppContextImpl::getApplicationContext()->setProject(NULL);
    }
}

void GTest_LoadDocumentFromProject::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    documentFileName = el.attribute("document");
    contextAdded = false;
}

void GTest_LoadDocumentFromProject::prepare(){
    Project *pr = AppContext::getProject();
    if (pr == NULL){
        stateInfo.setError(tr("No project loaded"));
        return;
    }
    if (documentFileName.isEmpty()){
        stateInfo.setError(tr("Document name to load is empty"));
        return;
    }
    foreach(Document *doc, pr->getDocuments()){
        QFileInfo fi(doc->getURLString());
        QString tmp  = fi.fileName();
        if (fi.fileName() == documentFileName){
            if(!doc->isLoaded()){
                loadTask = new LoadUnloadedDocumentTask(doc);
                addSubTask(loadTask);
            }
            loadedDoc = doc;
        }
    }
    if (loadedDoc == NULL){
        stateInfo.setError(tr("File %1 not found in project").arg(documentFileName));
        return;
    }
}

void GTest_LoadDocumentFromProject::cleanup(){
    QObject *o = getContext(documentFileName);
    if(contextAdded && o != NULL){
        removeContext(documentFileName);
        if(loadedDoc->isLoaded()){
            AppContext::getTaskScheduler()->registerTopLevelTask(new UnloadDocumentTask(loadedDoc, false));
        }
    }

    XmlTest::cleanup();
}

Task::ReportResult GTest_LoadDocumentFromProject::report(){
    if(loadTask!=NULL){
        if(!loadTask->hasError()){
            addContext(documentFileName, loadedDoc);
            contextAdded = true;
        }
    }
    return ReportResult_Finished;
}

} //namespace
