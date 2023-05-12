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

#include <AppContextImpl.h>

#include <QDesktopServices>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QUrl>

#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DbiDocumentFormat.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/LocalFileAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/ExportDocumentDialogController.h>
#include <U2Gui/ExportObjectUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/ReloadDocumentsTask.h>

#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>

#include "ProjectViewImpl.h"
#include "project_support/DocumentFormatSelectorController.h"
#include "project_support/DocumentReadingModeSelectorController.h"
#include "project_support/ProjectLoaderImpl.h"

namespace U2 {

/* TRANSLATOR U2::ProjectViewImpl */
/* TRANSLATOR U2::ProjectTreeController */

const QString ProjectViewImpl::SETTINGS_ROOT("projecview/");
static const char* NOTIFICATION_TITLE = "File Modification Detected";

#define UPDATER_TIMEOUT 3000

DocumentUpdater::DocumentUpdater(QObject* p)
    : QObject(p) {
    auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sl_update()));
    timer->start(UPDATER_TIMEOUT);
    recursion = false;
    updateTask = nullptr;
}

void DocumentUpdater::sl_update() {
    if (recursion || updateTask != nullptr) {
        return;
    }
    recursion = true;
    update();
    recursion = false;
}

static bool hasActiveDialogs(QObject* o) {
    const QObjectList& childObjects = o->children();

    for (QObject* childObject : qAsConst(childObjects)) {
        if (hasActiveDialogs(childObject)) {
            return true;
        }
    }
    auto d = qobject_cast<QDialog*>(o);
    if (d != nullptr && d->isVisible()) {
        // coreLog.trace(QString("Rejecting dialog %1").arg(o->metaObject()->className()));
        return true;
    }
    return false;
}

void DocumentUpdater::update() {
    // This check is necessary, because if a document is removed from the project
    // while a modal dialog is active, it can lead to invalid pointers to GObjects.
    CHECK(QApplication::activeModalWidget() == nullptr, );
    Project* prj = AppContext::getProject();
    assert(prj);

    // don't check documents currently used by save/load tasks
    QList<Document*> docs2check = prj->getDocuments();
    excludeDocumentsInTasks(AppContext::getTaskScheduler()->getTopLevelTasks(), docs2check);

    // build list of documents which files were modified between calls to sl_update()
    QList<Document*> outdatedDocs;
    QList<Document*> removedDocs;
    foreach (Document* doc, docs2check) {
        SAFE_POINT(doc != nullptr, tr("Project contains NULL document"), );

        if (!doc->isLoaded()) {
            continue;
        }

        if (GUrl_Network == doc->getURL().getType()) {
            // It is something like a network connection, skip it
            continue;
        }

        QFileInfo fi(doc->getURLString());
        bool fileCheckAllowedByHints = !(doc->getGHintsMap().value(ProjectLoaderHint_DontCheckForExistence, false).toBool());

        if (!fileCheckAllowedByHints && !doc->isModified()) {
            doc->setModified(true);
        }
        if (!fileCheckAllowedByHints) {
            continue;
        }
        if (!doc->isModified() && !fi.exists()) {  // file was removed from its folder
            removedDocs.append(doc);
        }

        auto dbiFormat = qobject_cast<DbiDocumentFormat*>(doc->getDocumentFormat());
        if (dbiFormat) {
            continue;
        }

        // TODO: changes within 1 second are not detected! If file is modified right after opening UGENE keeps a stale version of the document.
        QDateTime updTime = doc->getLastUpdateTime();

        // last update time is updated by save/load tasks
        // if it's a null the document was not loaded or saved => reload is pointless
        // if it's not a null and file not exists => file was deleted (don't reload)
        if (updTime.isNull()) {
            continue;
        }
        if (fi.lastModified() != updTime && fi.exists()) {  // file was modified
            outdatedDocs.append(doc);
        }
    }

    if (!outdatedDocs.isEmpty())
        notifyUserAndReloadDocuments(outdatedDocs);
    if (!removedDocs.isEmpty())
        notifyUserAndProcessRemovedDocuments(removedDocs);
}

bool DocumentUpdater::isAnyDialogOpened() const {
    foreach (GObjectViewWindow* vw, GObjectViewUtils::getAllActiveViews()) {
        if (hasActiveDialogs(vw)) {
            coreLog.trace(QString("View: '%1' has active dialogs, skipping reload").arg(vw->windowTitle()));
            return true;
        }
    }
    return false;
}

namespace {

void removeDocFromProject(Project* proj, Document* doc) {
    SAFE_POINT(nullptr != proj, L10N::nullPointerError("Project"), );
    SAFE_POINT(nullptr != doc, L10N::nullPointerError("Document"), );

    proj->removeRelations(doc->getURLString());
    proj->removeDocument(doc);
}

}  // namespace

bool DocumentUpdater::makeDecision(Document* doc, QListIterator<Document*>& iter) {
    QMessageBox::StandardButton btn = QMessageBox::question(AppContext::getMainWindow()->getQMainWindow(),
                                                            tr(NOTIFICATION_TITLE),
                                                            tr("The document '%1' was removed from its original folder. Do you wish to save it? "
                                                               "Otherwise, it will be removed from the current project.")
                                                                .arg(doc->getName()),
                                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll);

    Project* activeProject = AppContext::getProject();
    SAFE_POINT(nullptr != activeProject, L10N::nullPointerError("Project"), false);

    switch (btn) {
        case QMessageBox::Yes: {
            QString saveFileFilter = doc->getDocumentFormat()->getSupportedDocumentFileExtensions().join(" *.").prepend("*.");
            QString newFileUrl = U2FileDialog::getSaveFileName(dynamic_cast<QWidget*>(AppContext::getMainWindow()), tr("Save as"), doc->getURLString(), saveFileFilter);
            CHECK(!newFileUrl.isEmpty(), false);

            activeProject->updateDocInRelations(doc->getURLString(), newFileUrl);

            Task* saveDoc = new SaveDocumentTask(doc, doc->getIOAdapterFactory(), newFileUrl);
            AppContext::getTaskScheduler()->registerTopLevelTask(saveDoc);

            doc->setURL(GUrl(newFileUrl));
            break;
        }
        case QMessageBox::No:
            removeDocFromProject(activeProject, doc);
            break;
        case QMessageBox::NoToAll:
            removeDocFromProject(activeProject, doc);
            while (iter.hasNext()) {
                doc = iter.next();
                removeDocFromProject(activeProject, doc);
            }
            break;
        default:
            FAIL("Unexpected user response", false);
    }
    return true;
}

void DocumentUpdater::notifyUserAndProcessRemovedDocuments(const QList<Document*>& removedDocs) {
    coreLog.trace(QString("Found %1 changed doc(s)!").arg(removedDocs.size()));
    if (isAnyDialogOpened())
        return;

    QList<Document*> dbiDocs;

    // query user what documents he wants to reload
    // reloaded document modification time will be updated in load task
    QListIterator<Document*> iter(removedDocs);
    while (iter.hasNext()) {
        Document* doc = iter.next();
        bool decisionIsMade = false;
        do {
            // don't try to save dbi format files, just delete from project
            if (qobject_cast<DbiDocumentFormat*>(doc->getDocumentFormat())) {
                dbiDocs.append(doc);
                decisionIsMade = true;
            } else {
                decisionIsMade = makeDecision(doc, iter);
            }
        } while (!decisionIsMade);
    }

    if (!dbiDocs.isEmpty()) {
        const bool severalDocRemoved = dbiDocs.size() > 1;
        const QString warningMessageText = severalDocRemoved ? tr("Several documents were removed from their original folders. Therefore, they will be deleted from the current project. "
                                                                  "Find the full list below.")
                                                             : tr("The document '%1' was removed from its original folder. Therefore, it will be deleted from the current project.")
                                                                   .arg(dbiDocs.first()->getName());

        QObjectScopedPointer<QMessageBox> warningBox = new QMessageBox(dynamic_cast<QWidget*>(AppContext::getMainWindow()));
        warningBox->setIcon(QMessageBox::Warning);
        warningBox->setWindowTitle(tr(NOTIFICATION_TITLE));
        warningBox->setText(warningMessageText);
        if (severalDocRemoved) {
            QString removedDocNameList;
            foreach (Document* doc, dbiDocs) {
                removedDocNameList += doc->getURLString() + '\n';
            }
            removedDocNameList.chop(1);  // remove the last new line character
            warningBox->setDetailedText(removedDocNameList);
        }
        warningBox->exec();
        CHECK(!warningBox.isNull(), );

        Project* activeProject = AppContext::getProject();
        SAFE_POINT(nullptr != activeProject, L10N::nullPointerError("Project"), );
        foreach (Document* doc, dbiDocs) {
            removeDocFromProject(activeProject, doc);
        }
    }
}

void DocumentUpdater::notifyUserAndReloadDocuments(const QList<Document*>& outdatedDocs) {
    coreLog.trace(QString("Found %1 outdated docs!").arg(outdatedDocs.size()));
    if (isAnyDialogOpened())
        return;

    // query user what documents he wants to reload
    // reloaded document modification time will be updated in load task
    QList<Document*> docs2Reload;
    QListIterator<Document*> iter(outdatedDocs);
    while (iter.hasNext()) {
        Document* doc = iter.next();
        QMessageBox::StandardButton btn = QMessageBox::question(
            dynamic_cast<QWidget*>(AppContext::getMainWindow()),
            tr(NOTIFICATION_TITLE),
            tr("Document '%1' was modified. Do you want to reload it?\n"
               "Note that reloading may cause closing of some views associated with objects from the document.")
                .arg(doc->getName()),
            QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll);

        switch (btn) {
            case QMessageBox::Yes:
                docs2Reload.append(doc);
                break;

            case QMessageBox::YesToAll:
                docs2Reload.append(doc);
                while (iter.hasNext()) {
                    doc = iter.next();
                    docs2Reload.append(doc);
                }
                break;

            case QMessageBox::No:
                doc->setLastUpdateTime();
                break;

            case QMessageBox::NoToAll:
                doc->setLastUpdateTime();
                while (iter.hasNext()) {
                    doc = iter.next();
                    doc->setLastUpdateTime();
                }
                break;

            default:
                assert(0);
        }
    }

    if (docs2Reload.isEmpty()) {
        return;
    }

    // setup multi task : reload documents + open views

    reloadDocuments(docs2Reload);
}

void DocumentUpdater::sl_updateTaskStateChanged() {
    SAFE_POINT(updateTask != nullptr, "updateTask is NULL?", );
    if (updateTask->isFinished()) {
        updateTask = nullptr;
    }
}

void DocumentUpdater::excludeDocumentsInTasks(const QList<Task*>& tasks, QList<Document*>& documents) {
    foreach (Task* task, tasks) {
        excludeDocumentsInTasks(task->getPureSubtasks(), documents);
        auto saveTask = qobject_cast<SaveDocumentTask*>(task);
        if (saveTask) {
            documents.removeAll(saveTask->getDocument());
        } else {
            auto loadTask = qobject_cast<LoadDocumentTask*>(task);
            if (loadTask) {
                documents.removeAll(loadTask->getDocument(false));
            }
        }
    }
}

void DocumentUpdater::reloadDocuments(QList<Document*> docs2Reload) {
    QList<GObjectViewState*> states;
    QList<GObjectViewWindow*> viewWindows;

    for (Document* doc : qAsConst(docs2Reload)) {
        QList<GObjectViewWindow*> viewWnds = GObjectViewUtils::findViewsWithAnyOfObjects(doc->getObjects());
        foreach (GObjectViewWindow* vw, viewWnds) {
            viewWindows.append(vw);

            GObjectViewFactoryId id = vw->getViewFactoryId();
            QVariantMap stateData = vw->getObjectView()->saveState();
            if (stateData.isEmpty()) {
                continue;
            }
            states << new GObjectViewState(id, vw->getViewName(), "", stateData);

            vw->closeView();
        }
    }

    auto reloadTask = new ReloadDocumentsTask(docs2Reload);
    auto updateViewTask = new Task(tr("Restore state task"), TaskFlag_NoRun);

    foreach (GObjectViewState* state, states) {
        GObjectViewWindow* view = GObjectViewUtils::findViewByName(state->getViewName());
        if (view != nullptr) {
            assert(view->isPersistent());
            AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
            updateViewTask->addSubTask(view->getObjectView()->updateViewTask(state->getStateName(), state->getStateData()));
        }
        delete state;
    }

    QList<Task*> subs;
    subs << reloadTask << updateViewTask;
    updateTask = new MultiTask(tr("Reload documents and restore view state task"), subs);
    connect(updateTask, SIGNAL(si_stateChanged()), SLOT(sl_updateTaskStateChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(updateTask);
}

ProjectViewWidget::ProjectViewWidget() {
    setupUi(this);
    setObjectName(DOCK_PROJECT_VIEW);
    setWindowTitle(tr("Project"));
    setWindowIcon(QIcon(":ugene/images/project.png"));

    updater = new DocumentUpdater(this);
}

void showWarningAndWriteToLog(const QString& message) {
    coreLog.error(message);
    QMessageBox::critical(AppContext::getMainWindow()->getQMainWindow(), L10N::errorTitle(), message);
}

static ProjectTreeGroupMode getLastGroupMode() {
    int n = AppContext::getSettings()->getValue(ProjectViewImpl::SETTINGS_ROOT + "groupMode", ProjectTreeGroupMode_ByDocument).toInt();
    n = qBound((int)ProjectTreeGroupMode_Min, n, (int)ProjectTreeGroupMode_Max);
    return (ProjectTreeGroupMode)n;
}

static void saveGroupMode(ProjectTreeGroupMode m) {
    AppContext::getSettings()->setValue(ProjectViewImpl::SETTINGS_ROOT + "groupMode", (int)m);
}

//////////////////////////////////////////////////////////////////////////
// ProjectViewImpl
ProjectViewImpl::ProjectViewImpl()
    : ProjectView(tr("ProjectView"), tr("ProjectView service provides basic project visualization and manipulation functionality")) {
    // todo: move it somewhere else -> object views could be openend without project view service active
    registerBuiltInObjectViews();
}

ProjectViewImpl::~ProjectViewImpl() {
    unregisterBuiltInObjectViews();
}

/// returns NULL if no actions are required to enable service
Task* ProjectViewImpl::createServiceEnablingTask() {
    return new EnableProjectViewTask(this);
}

/// returns NULL if no actions are required to disable service
Task* ProjectViewImpl::createServiceDisablingTask() {
    return new DisableProjectViewTask(this, saveProjectOnClose);
}

void ProjectViewImpl::enable() {
    Project* pr = AppContext::getProject();
    connect(pr, SIGNAL(si_documentRemoved(Document*)), SLOT(sl_onDocumentRemoved(Document*)));
    connect(pr, SIGNAL(si_modifiedStateChanged()), SLOT(sl_onProjectModifiedStateChanged()));

    pr->setMainThreadModificationOnly(true);

    MWMDIManager* mdi = AppContext::getMainWindow()->getMDIManager();
    connect(mdi, SIGNAL(si_windowAdded(MWMDIWindow*)), SLOT(sl_onMDIWindowAdded(MWMDIWindow*)));

    SAFE_POINT(w == nullptr, "Project widget is already initialized", );
    w = new ProjectViewWidget();

    saveSelectedDocsAction = new QAction(QIcon(":ugene/images/save_selected_documents.png"), tr("Save selected documents"), w);
    saveSelectedDocsAction->setObjectName(ACTION_PROJECT__SAVE_DOCUMENT);
    connect(saveSelectedDocsAction, SIGNAL(triggered()), SLOT(sl_onSaveSelectedDocs()));

    toggleCircularAction = new QAction(tr("Mark as circular"), w);
    toggleCircularAction->setCheckable(true);
    connect(toggleCircularAction, SIGNAL(triggered()), SLOT(sl_onToggleCircular()));

    relocateDocumentAction = new QAction(tr("Relocate..."), w);
    relocateDocumentAction->setIcon(QIcon(":ugene/images/relocate.png"));
    connect(relocateDocumentAction, SIGNAL(triggered()), SLOT(sl_relocate()));

    exportDocumentAction = new QAction(tr("Export document..."), w);
    exportDocumentAction->setObjectName("Export document");
    exportDocumentAction->setIcon(QIcon(":ugene/images/save_copy.png"));
    connect(exportDocumentAction, SIGNAL(triggered()), SLOT(sl_exportDocument()));

    openContainingFolderAction = new QAction(tr("Open containing folder"), w);
    openContainingFolderAction->setObjectName("openContainingFolderAction");
    connect(openContainingFolderAction, &QAction::triggered, this, &ProjectViewImpl::sl_onOpenContainingFolder);

    initView();

    MainWindow* mw = AppContext::getMainWindow();
    MWDockManager* dm = mw->getDockManager();
    w->setObjectName("project_view");
    dm->registerDock(MWDockArea_Left, w, QKeySequence(Qt::ALT | Qt::Key_1));
    if (AppContext::getSettings()->getValue(SETTINGS_ROOT + "firstShow", true).toBool()) {
        dm->activateDock(w->objectName());
        AppContext::getSettings()->setValue(SETTINGS_ROOT + "firstShow", false);
    }

    AppContextImpl::getApplicationContext()->setProjectView(this);

    updateMWTitle();
    foreach (MWMDIWindow* w, mdi->getWindows()) {
        sl_onMDIWindowAdded(w);
    }
}

void ProjectViewImpl::disable() {
    MainWindow* mw = AppContext::getMainWindow();
    if (w != nullptr) {
        saveWidgetState(w);
        saveGroupMode(projectTreeController->getModeSettings().groupMode);
    }

    Project* pr = AppContext::getProject();
    pr->disconnect(this);

    // All these QObjects are autodeleted when 'w' is deleted;
    projectTreeController = nullptr;
    objectViewController = nullptr;

    AppContextImpl::getApplicationContext()->setProjectView(nullptr);

    // close all views;
    MWMDIManager* mdiManager = mw->getMDIManager();
    const QList<GObjectViewWindow*> views = GObjectViewUtils::getAllActiveViews();
    foreach (GObjectViewWindow* view, views) {
        mdiManager->closeMDIWindow(view);
    }
    mw->setWindowTitle("");

    delete w;
    w = nullptr;
}

void ProjectViewImpl::saveWidgetState(ProjectViewWidget* w) {
    QByteArray splitState = w->splitter->saveState();
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "splitterState", splitState);
}

void ProjectViewImpl::restoreWidgetState(ProjectViewWidget* w) {
    QByteArray splitState = AppContext::getSettings()->getValue(SETTINGS_ROOT + "splitterState", QByteArray()).toByteArray();
    if (!splitState.isEmpty()) {
        w->splitter->restoreState(splitState);
    }
}

void ProjectViewImpl::initView() {
    assert(projectTreeController == nullptr);
    ProjectTreeControllerModeSettings s;
    s.groupMode = getLastGroupMode();
    s.loadTaskProvider = this;
    s.markActive = true;
    s.activeFont.setWeight(QFont::Bold);
    projectTreeController = new ProjectTreeController(w->documentTreeWidget, s, w);
    connect(projectTreeController, SIGNAL(si_doubleClicked(GObject*)), SLOT(sl_onActivated(GObject*)));
    connect(projectTreeController, SIGNAL(si_doubleClicked(Document*)), SLOT(sl_onActivated(Document*)));
    connect(projectTreeController, SIGNAL(si_onPopupMenuRequested(QMenu&)), SLOT(sl_onDocTreePopupMenuRequested(QMenu&)));
    projectTreeController->setObjectName("document_Filter_Tree_Controller");
    connect(projectTreeController, SIGNAL(si_returnPressed(GObject*)), SLOT(sl_onActivated(GObject*)));
    connect(projectTreeController, SIGNAL(si_returnPressed(Document*)), SLOT(sl_onActivated(Document*)));
    connect(projectTreeController, SIGNAL(si_filteringStarted()), w->nameFilterEdit, SLOT(sl_filteringStarted()));
    connect(projectTreeController, SIGNAL(si_filteringFinished()), w->nameFilterEdit, SLOT(sl_filteringFinished()));

    connect(w->nameFilterEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_filterTextChanged(const QString&)));
    w->nameFilterEdit->installEventFilter(this);
    w->nameFilterEdit->setMaxLength(MAX_SEARCH_PATTERN_LENGTH + 1);

    assert(objectViewController == nullptr);
    objectViewController = new ObjectViewTreeController(w->viewTreeWidget);

    restoreWidgetState(w);
}

bool ProjectViewImpl::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Close) {
        auto ov = qobject_cast<GObjectViewWindow*>(obj);
        assert(ov);
        if (ov->isPersistent()) {
            saveViewState(ov, GObjectViewState::APP_CLOSING_STATE_NAME);
        }
    } else if (w != nullptr && w->nameFilterEdit == obj) {
        if (event->type() == QEvent::KeyPress) {
            auto keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->modifiers() == Qt::NoModifier && keyEvent->key() == Qt::Key_Escape) {
                w->nameFilterEdit->clear();
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void ProjectViewImpl::saveViewState(GObjectViewWindow* v, const QString& stateName) {
    Project* p = AppContext::getProject();
    GObjectViewFactoryId id = v->getViewFactoryId();
    GObjectViewState* state = GObjectViewUtils::findStateInList(v->getViewName(), stateName, p->getGObjectViewStates());
    QVariantMap stateData = v->getObjectView()->saveState();
    if (stateData.isEmpty()) {
        return;
    }
    if (state == nullptr) {
        state = new GObjectViewState(id, v->getViewName(), stateName, stateData);
        p->addGObjectViewState(state);
    } else {
        assert(state->getViewFactoryId() == id);  // TODO: handle this error;
        state->setStateData(stateData);
    }
}

void ProjectViewImpl::sl_onMDIWindowAdded(MWMDIWindow* m) {
    auto v = qobject_cast<GObjectViewWindow*>(m);
    if (v) {
        v->installEventFilter(this);
        connect(v, SIGNAL(si_persistentStateChanged(GObjectViewWindow*)), SLOT(sl_onViewPersistentStateChanged(GObjectViewWindow*)));
    }
}

void ProjectViewImpl::sl_onDocumentRemoved(Document* doc) {
    doc->disconnect(this);
}

void ProjectViewImpl::sl_onSaveSelectedDocs() {
    const DocumentSelection* docSelection = getDocumentSelection();
    QList<Document*> modifiedDocs;
    foreach (Document* doc, docSelection->getSelectedDocuments()) {
        if (doc->isTreeItemModified()) {
            modifiedDocs.append(doc);
        }
    }
    if (!modifiedDocs.isEmpty()) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new SaveMultipleDocuments(modifiedDocs, false, SavedNewDoc_Open));
    }
}

void ProjectViewImpl::updateMWTitle() {
    Project* p = AppContext::getProject();
    QString title = p->getProjectName();
    if (title.isEmpty()) {
        title = tr("-");
    }
    if (p->isTreeItemModified()) {
        title += "*";
    }
    AppContext::getMainWindow()->setWindowTitle(title);
}

void ProjectViewImpl::sl_onProjectModifiedStateChanged() {
    updateMWTitle();
}

void ProjectViewImpl::sl_onViewPersistentStateChanged(GObjectViewWindow* v) {
    if (v->isPersistent()) {
        // add last saved state
        saveViewState(v, GObjectViewState::APP_CLOSING_STATE_NAME);
    } else {
        // remove all states
        QList<GObjectViewState*> states = GObjectViewUtils::findStatesByViewName(v->getViewName());
        foreach (GObjectViewState* s, states) {
            AppContext::getProject()->removeGObjectViewState(s);
        }
    }
}

void ProjectViewImpl::sl_onDocTreePopupMenuRequested(QMenu& m) {
    buildViewMenu(m);
    emit si_onDocTreePopupMenuRequested(m);
}

#define MAX_DOCS_TO_OPEN_VIEWS 5
QList<Task*> ProjectViewImpl::createLoadDocumentTasks(const QList<Document*>& docs) const {
    bool openViews = docs.size() <= MAX_DOCS_TO_OPEN_VIEWS;
    QList<Task*> res;
    foreach (Document* doc, docs) {
        Task* t;
        if (openViews) {
            t = new LoadUnloadedDocumentAndOpenViewTask(doc);
        } else {
            t = new LoadUnloadedDocumentTask(doc);
        }
        res.append(t);
    }
    return res;
}

// TODO: selection could be modified before slot activation!
class OpenViewContext : public QObject {
public:
    OpenViewContext()
        : state(nullptr), factory(nullptr) {
    }
    OpenViewContext(QObject* p, const MultiGSelection& s, GObjectViewFactory* f)
        : QObject(p), selection(s), state(nullptr), factory(f) {
    }
    OpenViewContext(QObject* p, const GObjectViewState* s, GObjectViewFactory* f)
        : QObject(p), state(s), factory(f) {
    }
    OpenViewContext(QObject* p, const QString& _viewName)
        : QObject(p), state(nullptr), factory(nullptr), viewName(_viewName) {
    }

    MultiGSelection selection;
    const GObjectViewState* state;
    GObjectViewFactory* factory;
    QString viewName;
};

class AddToViewContext : public QObject {
public:
    AddToViewContext(QObject* p, GObjectViewController* v, QList<GObject*> objs)
        : QObject(p), view(v) {
        foreach (GObject* o, objs) {
            objects.append(o);
        }
    }
    QPointer<GObjectViewController> view;
    QList<QPointer<GObject>> objects;
};

void ProjectViewImpl::sl_onActivated(GObject* o) {
    SAFE_POINT(o != nullptr, "No double-clicked object found", );

    GObjectSelection os;
    os.addToSelection(o);
    MultiGSelection ms;
    ms.addSelection(&os);

    QMenu activeViewsMenu(tr("Active views"), nullptr);
    QList<QAction*> openActions;
    QList<GObjectViewFactory*> fs = AppContext::getObjectViewFactoryRegistry()->getAllFactories();
    foreach (GObjectViewFactory* f, fs) {
        QList<QAction*> tmp = selectOpenViewActions(f, ms, &activeViewsMenu, true);
        openActions << tmp;
    }
    if (openActions.size() == 1) {
        QAction* a = openActions.first();
        a->trigger();
        return;
    }
    if (openActions.isEmpty()) {
        if (o->isUnloaded()) {
            AppContext::getTaskScheduler()->registerTopLevelTask(new LoadUnloadedDocumentTask(o->getDocument()));
        }
        return;
    }
    foreach (QAction* a, openActions) {
        activeViewsMenu.addAction(a);
    }
    activeViewsMenu.exec(QCursor::pos());
}

void ProjectViewImpl::sl_onActivated(Document* d) {
    SAFE_POINT(d != nullptr, "No double-clicked document found", );
    MultiGSelection ms;
    GObjectSelection gs;
    DocumentSelection ds;

    if (d->isLoaded()) {
        // find view for loaded objects in document
        gs.addToSelection(d->getObjects());
        ms.addSelection(&gs);
    } else {
        // try create view for unloaded
        ds.addToSelection(QList<Document*>() << d);
        ms.addSelection(&ds);
    }

    QMenu activeViewsMenu(tr("Active views"), nullptr);
    QList<QAction*> openActions;
    QList<GObjectViewFactory*> fs = AppContext::getObjectViewFactoryRegistry()->getAllFactories();
    foreach (GObjectViewFactory* f, fs) {
        QList<QAction*> tmp = selectOpenViewActions(f, ms, &activeViewsMenu, true);
        openActions << tmp;
    }
    if (openActions.isEmpty()) {
        if (!d->isLoaded()) {
            AppContext::getTaskScheduler()->registerTopLevelTask(new LoadUnloadedDocumentTask(d));
        }
        return;
    }
    if (openActions.size() == 1) {
        QAction* a = openActions.first();
        a->trigger();
        return;
    }
    foreach (QAction* a, openActions) {
        activeViewsMenu.addAction(a);
    }
    activeViewsMenu.exec(QCursor::pos());
}

QList<QAction*> ProjectViewImpl::selectOpenViewActions(GObjectViewFactory* f, const MultiGSelection& ms, QObject* actionsParent, bool tryActivate) {
    QList<QAction*> res;

    // check if object is already displayed in some view.
    QList<MWMDIWindow*> windows = AppContext::getMainWindow()->getMDIManager()->getWindows();
    auto objectsSelection = static_cast<const GObjectSelection*>(ms.findSelectionByType(GSelectionTypes::GOBJECTS));
    if (objectsSelection != nullptr) {
        QSet<GObject*> objectsInSelection = objectsSelection->getSelectedObjects().toSet();
        foreach (MWMDIWindow* w, windows) {
            auto ov = qobject_cast<GObjectViewWindow*>(w);
            if (ov == nullptr) {
                continue;
            }
            if (ov->getViewFactoryId() != f->getId()) {
                continue;
            }
            QList<GObject*> viewObjects = ov->getObjects();
            bool contains = false;
            for (GObject* o : qAsConst(viewObjects)) {
                if (objectsInSelection.contains(o)) {
                    contains = true;
                    break;
                }
            }
            if (!contains) {
                continue;
            }
            auto action = new QAction(tr("Activate view: %1").arg(ov->getViewName()), actionsParent);
            auto c = new OpenViewContext(action, ov->getViewName());
            action->setData(QVariant::fromValue((void*)c));
            connect(action, SIGNAL(triggered()), SLOT(sl_activateView()));
            res.append(action);
        }
    }

    if (tryActivate && res.size() == 1) {
        return res;
    }

    // Check if a new view can be created.
    if (f->canCreateView(ms)) {
        auto action = new QAction(tr("Open new view: %1").arg(f->getName()), actionsParent);
        action->setObjectName("action_open_view");
        auto c = new OpenViewContext(action, ms, f);
        action->setData(QVariant::fromValue((void*)c));
        connect(action, SIGNAL(triggered()), SLOT(sl_openNewView()));
        res.append(action);
    }

    if (tryActivate && res.size() == 1) {
        return res;
    }

    // check saved state can be activated
    QList<GObjectViewState*> viewStates = GObjectViewUtils::selectStates(f, ms, AppContext::getProject()->getGObjectViewStates());
    foreach (GObjectViewState* s, viewStates) {
        auto action = new QAction(tr("Open saved view '%1' with a state '%2'").arg(s->getViewName()).arg(s->getStateName()), actionsParent);
        auto c = new OpenViewContext(action, s, f);
        action->setData(QVariant::fromValue((void*)c));
        connect(action, SIGNAL(triggered()), SLOT(sl_openStateView()));
        res.append(action);
    }

    return res;
}

void ProjectViewImpl::buildOpenViewMenu(const MultiGSelection& ms, QMenu* m) {
    QList<GObjectViewFactory*> fs = AppContext::getObjectViewFactoryRegistry()->getAllFactories();
    for (GObjectViewFactory* f : qAsConst(fs)) {
        QList<QAction*> openActions = selectOpenViewActions(f, ms, m);
        if (openActions.isEmpty()) {
            continue;
        }
        if (openActions.size() == 1) {
            QAction* openAction = openActions.first();
            openAction->setObjectName("action_open_view");
            m->addAction(openAction);
            continue;
        }
        for (QAction* a : qAsConst(openActions)) {
            m->addAction(a);
        }
    }
}

void ProjectViewImpl::buildAddToViewMenu(const MultiGSelection& ms, QMenu* m) {
    MWMDIWindow* w = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    if (w == nullptr) {
        return;
    }
    auto ow = qobject_cast<GObjectViewWindow*>(w);
    if (ow == nullptr) {
        return;
    }
    QList<GObject*> objects = SelectionUtils::getSelectedObjects(ms);
    if (objects.isEmpty()) {
        return;
    }
    foreach (GObject* obj, objects) {
        bool canBeAdded = ow->getObjectView()->canAddObject(obj);
        if (!canBeAdded) {
            return;
        }
    }
    auto action = new QAction(tr("Add to view: %1").arg(ow->getViewName()), m);
    auto ac = new AddToViewContext(action, ow->getObjectView(), objects);
    action->setData(QVariant::fromValue((void*)ac));
    action->setObjectName("action_add_view");
    connect(action, SIGNAL(triggered()), SLOT(sl_addToView()));
    m->addAction(action);
}

void ProjectViewImpl::buildRelocateMenu(QMenu* m) {
    const DocumentSelection* docSelection = getDocumentSelection();
    const GObjectSelection* objSelection = getGObjectSelection();
    if (!objSelection->isEmpty() || docSelection->getSelectedDocuments().size() != 1) {
        return;
    }
    Document* doc = docSelection->getSelectedDocuments().first();
    if (doc->isLoaded()) {
        DocumentFormatRegistry* dfr = AppContext::getDocumentFormatRegistry();
        QList<DocumentFormatId> ids = dfr->getRegisteredFormats();
        QList<DocumentFormat*> allWritableFormats;
        foreach (DocumentFormatId id, ids) {
            DocumentFormat* format = dfr->getFormatById(id);
            if (format->checkFlags(DocumentFormatFlag_SupportWriting) && !format->checkFlags(DocumentFormatFlag_CannotBeCreated)) {
                allWritableFormats.append(format);
            }
        }
        for (DocumentFormat* f : qAsConst(allWritableFormats)) {
            const QSet<GObjectType>& supportedObjectTypes = f->getSupportedObjectTypes();
            bool allObjectsWitable = true;
            foreach (GObject* gobj, doc->getObjects()) {
                if (!supportedObjectTypes.contains(gobj->getGObjectType())) {
                    allObjectsWitable = false;
                }
            }
            if (allObjectsWitable) {
                m->addAction(exportDocumentAction);
                return;
            }
        }
    } else {
        m->addAction(relocateDocumentAction);
    }
}

void ProjectViewImpl::buildViewMenu(QMenu& m) {
    auto openInMenu = new QMenu(tr("Open In"), &m);
    auto addToViewMenu = new QMenu(tr("Add to view"), &m);

    const DocumentSelection* docsSelection = getDocumentSelection();
    const GObjectSelection* objsSelection = getGObjectSelection();

    SAFE_POINT(docsSelection != nullptr, "Document selection is NULL", );
    SAFE_POINT(objsSelection != nullptr, "Object selection is NULL", );

    MultiGSelection multiSelection;
    if (!objsSelection->isEmpty()) {
        multiSelection.addSelection(objsSelection);
    }
    if (!docsSelection->isEmpty()) {
        multiSelection.addSelection(docsSelection);
    }

    buildOpenViewMenu(multiSelection, openInMenu);
    openInMenu->menuAction()->setObjectName("openInMenu");

    buildAddToViewMenu(multiSelection, addToViewMenu);
    addToViewMenu->menuAction()->setObjectName("submenu_add_view");

    bool submenusWereAdded = false;

    addToViewMenu->setDisabled(addToViewMenu->isEmpty());
    if (addToViewMenu->isEnabled()) {
        m.insertMenu(m.actions().first(), addToViewMenu);
        submenusWereAdded = true;
    }

    openInMenu->setDisabled(openInMenu->isEmpty());
    if (openInMenu->isEnabled()) {
        m.insertMenu(m.actions().first(), openInMenu);
        submenusWereAdded = true;
    }
    if (submenusWereAdded) {
        m.addSeparator();
    }

    bool hasModifiedDocs = false;
    foreach (Document* doc, docsSelection->getSelectedDocuments()) {
        if (doc->isTreeItemModified()) {
            hasModifiedDocs = true;
            break;
        }
    }

    buildRelocateMenu(&m);

    saveSelectedDocsAction->setEnabled(hasModifiedDocs);
    if (hasModifiedDocs) {
        m.addAction(saveSelectedDocsAction);
    }

    if (!objsSelection->isEmpty()) {
        bool seqobjFound = false;
        bool allCirc = true;
        bool allNucl = true;
        foreach (GObject* obj, objsSelection->getSelectedObjects()) {
            const bool objectIsModifiable = (!obj->isStateLocked());
            if (obj->getGObjectType() == GObjectTypes::SEQUENCE && objectIsModifiable) {
                seqobjFound = true;
                auto casted = qobject_cast<U2SequenceObject*>(obj);
                if (!casted->getAlphabet()->isNucleic()) {
                    allNucl = false;
                }
                if (!casted->isCircular()) {
                    allCirc = false;
                }
            }
        }
        if (seqobjFound && allNucl) {
            toggleCircularAction->setChecked(allCirc);
            m.addAction(toggleCircularAction);
        }
    }

    QSet<Document*> implicitDocumentSelection = projectTreeController->getDocumentSelectionDerivedFromObjects();
    if (std::any_of(implicitDocumentSelection.begin(), implicitDocumentSelection.end(), [](auto d) { return d->getURL().isLocalFile(); })) {
        openInMenu->addSeparator();
        openInMenu->addAction(openContainingFolderAction);
    }
}

void ProjectViewImpl::sl_activateView() {
    auto action = (QAction*)sender();
    auto c = static_cast<OpenViewContext*>(action->data().value<void*>());
    assert(!c->viewName.isEmpty());
    GObjectViewWindow* ov = GObjectViewUtils::findViewByName(c->viewName);
    if (ov != nullptr) {
        AppContext::getMainWindow()->getMDIManager()->activateWindow(ov);
    }
}

void ProjectViewImpl::sl_openNewView() {
    auto action = (QAction*)sender();
    auto c = static_cast<OpenViewContext*>(action->data().value<void*>());
    SAFE_POINT(c->factory->canCreateView(c->selection), "Invalid object view factory!", );
    Task* openViewTask = c->factory->createViewTask(c->selection);
    if (nullptr != openViewTask) {
        AppContext::getTaskScheduler()->registerTopLevelTask(openViewTask);
    }
}

void ProjectViewImpl::sl_addToView() {
    // TODO: create specialized action classes instead of using ->data().value<void*>() casts
    auto action = (QAction*)sender();
    auto ac = static_cast<AddToViewContext*>(action->data().value<void*>());
    GObjectViewController* view = ac->view;
    if (view == nullptr) {
        return;
    }
    foreach (GObject* o, ac->objects) {
        if (o != nullptr) {
            QString err = view->addObject(o);
            if (!err.isEmpty()) {
                QMessageBox::critical(nullptr, tr("Error"), err);
            }
        }
    }
}

void ProjectViewImpl::sl_openStateView() {
    auto action = (QAction*)sender();
    auto c = static_cast<OpenViewContext*>(action->data().value<void*>());
    const GObjectViewState* state = c->state;
    assert(state);

    // todo: 70% of code duplication with ObjectViewTreeController::sl_activateView -> create util
    GObjectViewWindow* view = GObjectViewUtils::findViewByName(state->getViewName());
    if (view != nullptr) {
        assert(view->isPersistent());
        AppContext::getTaskScheduler()->registerTopLevelTask(view->getObjectView()->updateViewTask(state->getStateName(), state->getStateData()));
    } else {
        GObjectViewFactory* f = AppContext::getObjectViewFactoryRegistry()->getFactoryById(state->getViewFactoryId());
        assert(f != nullptr);
        AppContext::getTaskScheduler()->registerTopLevelTask(f->createViewTask(state->getViewName(), state->getStateData()));
    }
}

void ProjectViewImpl::sl_filterTextChanged(const QString& str) {
    SAFE_POINT(nullptr != projectTreeController, "NULL controller", );
    QString changedText = str;
    ProjectTreeControllerModeSettings settings = projectTreeController->getModeSettings();
    if (str.length() > MAX_SEARCH_PATTERN_LENGTH) {
        changedText = str.mid(0, MAX_SEARCH_PATTERN_LENGTH);
        QString warning = ProjectViewImpl::tr("The search pattern is too long. Pattern was truncated to 1000 symbols.");
        coreLog.info(warning);
        QMessageBox::warning(AppContext::getMainWindow()->getQMainWindow(), L10N::warningTitle(), warning);
    }
    settings.tokensToShow = changedText.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    projectTreeController->updateSettings(settings);
}

void ProjectViewImpl::sl_relocate() {
    const DocumentSelection* ds = getDocumentSelection();
    Document* d = ds->isEmpty() ? nullptr : ds->getSelectedDocuments().first();
    if (d == nullptr) {
        return;
    }
    if (d->isLoaded()) {  // TODO: support loaded docs relocation?
        return;
    }
    LastUsedDirHelper h;
    h.url = U2FileDialog::getOpenFileName(w, tr("Select new file location"), h.dir);
    if (h.url.isEmpty()) {
        return;
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(new RelocateDocumentTask(d->getURL(), GUrl(h.url, GUrl_File)));
}

void ProjectViewImpl::sl_exportDocument() {
    const DocumentSelection* ds = getDocumentSelection();
    Document* srcDoc = ds->isEmpty() ? nullptr : ds->getSelectedDocuments().first();
    if (srcDoc == nullptr) {
        return;
    }
    if (!srcDoc->isLoaded()) {
        return;
    }
    QObjectScopedPointer<ExportDocumentDialogController> dialog = new ExportDocumentDialogController(srcDoc, w);
    ExportObjectUtils::export2Document(dialog);
}

void ProjectViewImpl::highlightItem(Document* doc) {
    assert(doc);
    projectTreeController->highlightItem(doc);
}

void ProjectViewImpl::sl_onToggleCircular() {
    const GObjectSelection* objSelection = getGObjectSelection();
    foreach (GObject* obj, objSelection->getSelectedObjects()) {
        const bool objectIsModifiable = (!obj->isStateLocked());
        if (objectIsModifiable && obj->getGObjectType() == GObjectTypes::SEQUENCE) {
            auto casted = qobject_cast<U2SequenceObject*>(obj);
            SAFE_POINT(obj != nullptr, "casting to 'U2SequenceObject' failed", );
            casted->setCircular(toggleCircularAction->isChecked());
            projectTreeController->refreshObject(casted);
        }
    }
}

void ProjectViewImpl::sl_onOpenContainingFolder() {
    QSet<Document*> docToOpen = projectTreeController->getDocumentSelectionDerivedFromObjects();
    int nOpened = 0;
    int maxToOpen = 10;  // Limit number of opened folders, so user is not overflown with new File Manager's windows.
    for (Document* doc : qAsConst(docToOpen)) {
        const GUrl& url = doc->getURL();
        if (url.isLocalFile()) {
            QDesktopServices::openUrl(QUrl("file:///" + url.dirPath()));
            nOpened++;
            if (nOpened == maxToOpen) {
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Tasks

// EnableProjectViewTask

EnableProjectViewTask::EnableProjectViewTask(ProjectViewImpl* _pvi)
    : Task(tr("Enable ProjectView"), TaskFlag_NoRun), pvi(_pvi) {
}

Task::ReportResult EnableProjectViewTask::report() {
    assert(AppContext::getProject() != nullptr);
    pvi->enable();
    return ReportResult_Finished;
}

DisableProjectViewTask::DisableProjectViewTask(ProjectViewImpl* _pvi, bool saveProjectOnClose)
    : Task(tr("Disable project viewer"), TaskFlags_NR_FOSCOE), pvi(_pvi), saveProject(saveProjectOnClose) {
}

void DisableProjectViewTask::prepare() {
    // TODO: this should be removed from here
    // because save project can be canceled by user.
    if (AppContext::getProject()->isTreeItemModified() && saveProject) {
        addSubTask(AppContext::getProjectService()->saveProjectTask(SaveProjectTaskKind_SaveProjectAndDocumentsAskEach));
    }
}

Task::ReportResult DisableProjectViewTask::report() {
    if (propagateSubtaskError()) {
        return ReportResult_Finished;
    }
    pvi->disable();
    return ReportResult_Finished;
}

}  // namespace U2
