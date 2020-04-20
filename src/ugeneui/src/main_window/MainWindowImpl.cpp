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

#include <qglobal.h>
#ifdef Q_OS_MACX
#include <Security/Authorization.h>
#include <errno.h>
#include <unistd.h>
#endif
#include <algorithm>

#include <QAction>
#include <QDesktopServices>
#include <QFont>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ObjectViewModel.h>

#include "AboutDialogController.h"
#include "CheckUpdatesTask.h"
#include "DockManagerImpl.h"
#include "MDIManagerImpl.h"
#include "MainWindowImpl.h"
#include "MenuManager.h"
#include "ShutdownTask.h"
#include "TmpDirChangeDialogController.h"
#include "ToolBarManager.h"
#include "shtirlitz/Shtirlitz.h"
#include "update/UgeneUpdater.h"

namespace U2 {

#define USER_MANUAL_FILE_NAME "UniproUGENE_UserManual.pdf"
#define WD_USER_MANUAL_FILE_NAME "WorkflowDesigner_UserManual.pdf"
#define QD_USER_MANUAL_FILE_NAME "QueryDesigner_UserManual.pdf"

#define SETTINGS_DIR QString("main_window/")

class MWStub : public QMainWindow {
public:
    MWStub(MainWindowImpl* _owner)  : owner(_owner){
        setAttribute(Qt::WA_NativeWindow);
        setAcceptDrops(true);
    }
    virtual QMenu * createPopupMenu () {return NULL;} //todo: decide if we do really need this menu and fix it if yes?
protected:
    virtual void closeEvent(QCloseEvent* e);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent ( QDropEvent * event );
    virtual void dragMoveEvent ( QDragMoveEvent * event );
    virtual bool focusNextPrevChild ( bool next );
protected:
    MainWindowImpl* owner;
};

void MWStub::closeEvent(QCloseEvent* e) {
    if (owner->getMDIManager() == NULL) {
        QMainWindow::closeEvent(e);
    } else {
        owner->runClosingTask();
        e->ignore();
    }
}

void MWStub::dragEnterEvent(QDragEnterEvent *event) {
    MainWindowDragNDrop::dragEnterEvent(event);
}

void MainWindowDragNDrop::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat(DocumentMimeData::MIME_TYPE)) {
        event->acceptProposedAction();
    }
}

void MWStub::dropEvent(QDropEvent *event) {
    MainWindowDragNDrop::dropEvent(event);
}

void MainWindowDragNDrop::dropEvent(QDropEvent *event) {
    if (event->source() == NULL) {
        QList<GUrl> urls;
        if (event->mimeData()->hasUrls()) {
            urls = GUrlUtils::qUrls2gUrls(event->mimeData()->urls());
        }else if(event->mimeData()->hasFormat(DocumentMimeData::MIME_TYPE)){
            urls = GUrlUtils::qUrls2gUrls(event->mimeData()->urls());
        }
        if(!urls.isEmpty()){
            QVariantMap hints;
            hints[ProjectLoaderHint_CloseActiveProject] = true;
            Task* t = AppContext::getProjectLoader()->openWithProjectTask(urls, hints);
            if (t) {
                AppContext::getTaskScheduler()->registerTopLevelTask(t);
                event->acceptProposedAction();
            }
        }
    } else {
        if(event->mimeData()->hasFormat(DocumentMimeData::MIME_TYPE)) {
            const DocumentMimeData *docData = static_cast<const DocumentMimeData *>(event->mimeData());

            DocumentSelection ds;
            ds.setSelection(QList<Document*>() << docData->objPtr);
            MultiGSelection ms;
            ms.addSelection(&ds);
            foreach(GObjectViewFactory *f, AppContext::getObjectViewFactoryRegistry()->getAllFactories()) {
                if(f->canCreateView(ms)) {
                    AppContext::getTaskScheduler()->registerTopLevelTask(f->createViewTask(ms));
                    break;
                }
            }
        }
    }
}

bool MWStub::focusNextPrevChild(bool /*next*/) {
    return false;
}

void MWStub::dragMoveEvent(QDragMoveEvent *event) {
    MainWindowDragNDrop::dragMoveEvent(event);
}

void MainWindowDragNDrop::dragMoveEvent(QDragMoveEvent *event) {
    MainWindow *mainWindow = AppContext::getMainWindow();
    SAFE_POINT(NULL != mainWindow, L10N::nullPointerError("Main Window"), );

    if(event->mimeData()->hasUrls()) return;
    if(event->source() != NULL){
        QObject *par = event->source()->parent();
        while(par != NULL){
            if(par == mainWindow->getQMainWindow()) {
                return;
            }
            par = par->parent();
        }
        event->ignore();
    }
}

//////////////////////////////////////////////////////////////////////////
// MainWindowController
//////////////////////////////////////////////////////////////////////////
MainWindowImpl::MainWindowImpl() {
    mw = NULL;
    mdi = NULL;
    menuManager = NULL;
    toolbarManager = NULL;
    mdiManager = NULL;
    dockManager = NULL;
    exitAction = NULL;
    visitWebAction = NULL;
    viewOnlineDocumentation = NULL;
    checkUpdateAction = NULL;
    aboutAction = NULL;
    openManualAction = NULL;
    openWDManualAction = NULL;
    openQDManualAction = NULL;
    welcomePageAction = NULL;
    crashUgeneAction = NULL;
    shutDownInProcess = false;
#ifdef _INSTALL_TO_PATH_ACTION
    installToPathAction = NULL;
#endif
    nStack = NULL;
}

MainWindowImpl::~MainWindowImpl() {
    assert(mw == NULL);
}

void MainWindowImpl::prepare() {
    nStack = new NotificationStack();
    createActions();
    prepareGUI();
}

void MainWindowImpl::close() {
    AppContext::getSettings()->setValue(SETTINGS_DIR + "maximized", mw->isMaximized());
    AppContext::getSettings()->setValue(SETTINGS_DIR + "geometry", mw->geometry());

    dockManager->deleteLater();
    dockManager = NULL;

    menuManager->deleteLater();
    menuManager = NULL;

    toolbarManager->deleteLater();
    toolbarManager = NULL;

    mdiManager->deleteLater();
    mdiManager = NULL;

    nStack->deleteLater();
    nStack = NULL;

    mdi->deleteLater();
    mdi = NULL;

    mw->close();
    mw->deleteLater();
    mw = NULL;
}

bool MainWindowImpl::eventFilter(QObject *object, QEvent *event) {
    CHECK(mw == object, false);
    CHECK(NULL != event, false);
    CHECK(event->type() == QEvent::KeyPress, false);

    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    CHECK(NULL != keyEvent, false);

    if (keyEvent->matches(QKeySequence::Paste)) {
        uiLog.details(tr("Application paste shortcut is triggered"));
        emit si_paste();
    }
    return false;
}

void MainWindowImpl::createActions() {
    exitAction = new QAction(tr("Exit"), this);
    exitAction->setShortcutContext(Qt::WindowShortcut);
    connect(exitAction, SIGNAL(triggered()), SLOT(sl_exitAction()));

    aboutAction = new QAction(tr("About"), this);
    aboutAction->setObjectName("About");
    aboutAction->setShortcut(QKeySequence(Qt::Key_F1));
    aboutAction->setShortcutContext(Qt::ApplicationShortcut);
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, SIGNAL(triggered()), SLOT(sl_aboutAction()));

    visitWebAction = new QAction(tr("Visit UGENE Web Site"), this);
    visitWebAction->setObjectName("Visit UGENE Web Site");
    connect(visitWebAction, SIGNAL(triggered()), SLOT(sl_visitWeb()));

    viewOnlineDocumentation = new QAction(tr("View UGENE Documentation Online"), this);
    viewOnlineDocumentation->setObjectName("View UGENE Documentation Online");
    connect(viewOnlineDocumentation, SIGNAL(triggered()), SLOT(sl_viewOnlineDocumentation()));

    checkUpdateAction = new QAction(tr("Check for Updates"), this);
    checkUpdateAction->setObjectName("Check for Updates");
    connect(checkUpdateAction, SIGNAL(triggered()), SLOT(sl_checkUpdatesAction()));

    openManualAction = new QAction(tr("Open UGENE User Manual"), this);
    openManualAction->setObjectName("Open UGENE User Manual");
    connect(openManualAction, SIGNAL(triggered()),SLOT(sl_openManualAction()));

    welcomePageAction = new QAction(tr("Open Start Page"), this);
    welcomePageAction->setObjectName("welcome_page");
    connect(welcomePageAction, SIGNAL(triggered()), SIGNAL(si_showWelcomePage()));

    crashUgeneAction = new QAction(tr("Crash UGENE"), this);
    crashUgeneAction->setObjectName("crash_ugene");
    connect(crashUgeneAction, SIGNAL(triggered()), SLOT(sl_crashUgene()));

#ifdef _INSTALL_TO_PATH_ACTION
    installToPathAction = new QAction(tr("Enable Terminal Usage..."), this);
    connect(installToPathAction, SIGNAL(triggered()), SLOT(sl_installToPathAction()));
#endif
}

void MainWindowImpl::sl_exitAction() {
    runClosingTask();
}

void MainWindowImpl::sl_aboutAction() {
    QWidget *p = qobject_cast<QWidget*>(getQMainWindow());
    QObjectScopedPointer<AboutDialogController> d = new AboutDialogController(visitWebAction, p);
    d->exec();
}


void MainWindowImpl::sl_checkUpdatesAction() {
    AppContext::getTaskScheduler()->registerTopLevelTask(new CheckUpdatesTask());
}


void MainWindowImpl::setWindowTitle(const QString& title) {
    if (title.isEmpty()) {
        mw->setWindowTitle(U2_APP_TITLE);
    } else {
        mw->setWindowTitle(title + " " + U2_APP_TITLE);
    }
}

void MainWindowImpl::registerAction(QAction *action) {
    menuManager->registerAction(action);
}

void MainWindowImpl::prepareGUI() {
    mw = new MWStub(this); //todo: parents?
    mw->setObjectName("main_window");
    setWindowTitle("");

    mdi = new FixedMdiArea(mw);
    mdi->setObjectName("MDI_Area");

    mw->setCentralWidget(mdi);
    mw->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    mw->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    mw->installEventFilter(this);

    toolbarManager = new MWToolBarManagerImpl(mw);

    menuManager = new MWMenuManagerImpl(this, mw->menuBar());
    menuManager->registerAction(aboutAction);

    exitAction->setObjectName(ACTION__EXIT);
    exitAction->setParent(mw);
    menuManager->getTopLevelMenu(MWMENU_FILE)->addAction(exitAction);
#ifdef _INSTALL_TO_PATH_ACTION
    menuManager->getTopLevelMenu(MWMENU_FILE)->addAction(installToPathAction);
#endif

    aboutAction->setObjectName(ACTION__ABOUT);
    aboutAction->setParent(mw);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(openManualAction);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(openWDManualAction);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(openQDManualAction);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(viewOnlineDocumentation);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addSeparator();
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(visitWebAction);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(checkUpdateAction);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(welcomePageAction);
    menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(aboutAction);
    if ("1" == qgetenv(ENV_TEST_CRASH_HANDLER)) {
        menuManager->getTopLevelMenu(MWMENU_HELP)->addSeparator();
        menuManager->getTopLevelMenu(MWMENU_HELP)->addAction(crashUgeneAction);
    }

    mdiManager = new MWMDIManagerImpl(this, mdi);

    dockManager = new MWDockManagerImpl(this);
}


void MainWindowImpl::runClosingTask() {
    if(!shutDownInProcess) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new ShutdownTask(this));
        setShutDownInProcess(true);
    } else {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(getQMainWindow());
        msgBox->setWindowTitle(U2_APP_TITLE);
        msgBox->setText(tr("Shutdown already in process. Close UGENE immediately?"));
        QPushButton *closeButton = msgBox->addButton(tr("Close"), QMessageBox::ActionRole);
        /*QPushButton *waitButton =*/ msgBox->addButton(tr("Wait"), QMessageBox::ActionRole);
        msgBox->exec();
        CHECK_EXT(!msgBox.isNull(), exit(0), );

        if(getQMainWindow()) {
            if(msgBox->clickedButton() == closeButton) {
                UgeneUpdater::onClose();
                exit(0);
            }
        }
    }
}

void MainWindowImpl::setShutDownInProcess(bool flag) {
    shutDownInProcess = flag;
//    mw->setEnabled(!flag);
    menuManager->setMenuBarEnabled(!flag);
}

void MainWindowImpl::sl_visitWeb() {
    GUIUtils::runWebBrowser("http://ugene.net");
}
void MainWindowImpl::sl_viewOnlineDocumentation(){
    GUIUtils::runWebBrowser("http://ugene.net/documentation.html");
}

void MainWindowImpl::sl_openManualAction()
{
    openManual(USER_MANUAL_FILE_NAME);
}
void MainWindowImpl::sl_openWDManualAction()
{
    openManual(WD_USER_MANUAL_FILE_NAME);
}
void MainWindowImpl::sl_openQDManualAction()
{
    openManual(QD_USER_MANUAL_FILE_NAME);
}
void MainWindowImpl::sl_tempDirPathCheckFailed(QString path) {
    QObjectScopedPointer<TmpDirChangeDialogController> tmpDirChangeDialogController = new TmpDirChangeDialogController(path, mw);
    tmpDirChangeDialogController->exec();
    CHECK(!tmpDirChangeDialogController.isNull(), );

    if (tmpDirChangeDialogController->result() == QDialog::Accepted) {
        AppContext::getAppSettings()->getUserAppsSettings()->setUserTemporaryDirPath(tmpDirChangeDialogController->getTmpDirPath());
    }
    else {
        AppContext::getTaskScheduler()->cancelAllTasks();
        sl_exitAction();
    }
}

#ifdef _INSTALL_TO_PATH_ACTION
void MainWindowImpl::sl_installToPathAction() {
    // This feature is inspired by GitX and its original implementation is here:
    // https://github.com/pieter/gitx/blob/85322728facbd2a2df84e5fee3e7239fce18fd22/ApplicationController.m#L121

    bool success = true;
    QString exePath = AppContext::getWorkingDirectoryPath() + "/";
    QString installationPath = "/usr/bin/";
    QStringList tools;
    tools << "ugene" << "ugeneui" << "ugenecl";

    AuthorizationRef auth;
    if (AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &auth) == errAuthorizationSuccess) {

        foreach(QString tool, tools) {
            QByteArray executable = (exePath + tool).toUtf8();
            QByteArray installPath = installationPath.toUtf8();
            QString symlink = installationPath + tool;
            char const* arguments[] = { "-f", "-s", executable.constData(), installPath.constData(),  NULL };
            char const* helperTool  = "/bin/ln";

            if (AuthorizationExecuteWithPrivileges(auth, helperTool, kAuthorizationFlagDefaults, (char**)arguments, NULL) == errAuthorizationSuccess) {
                // HACK: sleep because otherwise QFileInfo::exists might return false
                sleep(100);
                wait(NULL);
                if ( ! QFileInfo(symlink).exists() ) {
                    QMessageBox::critical(NULL, tr("Installation failed"), tr("Failed to enable terminal usage: couldn't install '%1'").arg(symlink));
                    success = false;
                    break;
                }
            } else {
                QMessageBox::critical(NULL, tr("Installation failed"), tr("Failed to enable terminal usage: not authorized"));
                success = false;
                break;
            }
        }

        AuthorizationFree(auth, kAuthorizationFlagDefaults);
    } else {
        QMessageBox::critical(NULL, tr("Installation failed"), tr("Failed to enable terminal usage: authorization failure"));
        success = false;
    }

    if(success) {
        QMessageBox::information(NULL, tr("Installation successful"), tr("Terminal usage successfully enabled.\n\nNow you can type ugene in command line to start UGENE."));
    }
}
#endif // #ifdef _INSTALL_TO_PATH_ACTION

void MainWindowImpl::openManual(const QString& name){
    QFileInfo fileInfo( QString(PATH_PREFIX_DATA)+":"+"/manuals/" + name );
    if(!fileInfo.exists()){
        GUIUtils::runWebBrowser(QString("http://ugene.net/downloads/") + name);
    }else{
        if(!QDesktopServices::openUrl(QUrl("file:///"+fileInfo.absoluteFilePath()))){
            QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
            msgBox->setWindowTitle(L10N::warningTitle());
            msgBox->setText(tr("Can not open %1 file. ").arg(name));
            msgBox->setInformativeText(tr("You can try open it manualy from here: %1 \nor view online documentation.\n\nDo you want view online documentation?").arg(fileInfo.absolutePath()));
            msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::Yes);
            int ret = msgBox->exec();
            CHECK(!msgBox.isNull(), );

            switch (ret) {
               case QMessageBox::Yes:
                   GUIUtils::runWebBrowser("http://ugene.net/documentation.html");
                   break;
               case QMessageBox::No:
                   return;
                   break;
               default:
                   assert(false);
                   break;
             }
        }
    }
}

QMenu* MainWindowImpl::getTopLevelMenu( const QString& sysName ) const
{
    return menuManager->getTopLevelMenu(sysName);
}

QToolBar* MainWindowImpl::getToolbar( const QString& sysName ) const
{
    return toolbarManager->getToolbar(sysName);
}

///////////////////////////////////////////////////////////////////

FixedMdiArea::FixedMdiArea(QWidget * parent) : QMdiArea(parent)
{
    setDocumentMode(true);
    setTabShape(QTabWidget::Rounded);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
}

void FixedMdiArea::setViewMode( QMdiArea::ViewMode mode )
{
    if (mode == viewMode()) {
        return;
    }
    QMdiArea::setViewMode(mode);
    if (mode == QMdiArea::TabbedView) {
        //FIXME QTBUG-9293, Adding a close button to tabbed QMdiSubWindows
        QList<QTabBar*> tb = findChildren<QTabBar*>();
        foreach(QTabBar *t ,tb){
            if(t->parentWidget() == this){
                t->setTabsClosable(true);
                connect(t, SIGNAL(tabCloseRequested(int)), SLOT(closeSubWindow(int)));
            }
        }
    } else {
        //TODO QTBUG-3269: switching between TabbedView and SubWindowView does not preserve maximized window state
    }
}

void FixedMdiArea::closeSubWindow(int idx) {
    Q_UNUSED(idx);
    // We use Qt greater than 4.8.0
    // this workaround can be removed
    // do it accurately
#if QT_VERSION < 0x040800 //In Qt version 4.8.0 was added default behavior for closing tab.
    subWindowList().at(idx)->close();
#endif
}

//Workaround for QTBUG-17428: Superfluous RestoreAction for tabbed QMdiSubWindows
void FixedMdiArea::sysContextMenuAction(QAction* action) {
    if (viewMode() == QMdiArea::TabbedView && activeSubWindow())
    {
        QList<QAction*> lst = activeSubWindow()->actions();
        if (!lst.isEmpty() && action == lst.first() ) { //RestoreAction always comes before CloseAction
            //FIXME better to detect via shortcut or icon ???
            assert(action->icon().pixmap(32).toImage() == style()->standardIcon(QStyle::SP_TitleBarNormalButton).pixmap(32).toImage() );
            activeSubWindow()->showMaximized();
        }
    }
}

QMdiSubWindow* FixedMdiArea::addSubWindow(QWidget* widget)
{
    QMdiSubWindow* subWindow = QMdiArea::addSubWindow(widget);
    //Workaround for QTBUG-17428
    connect(subWindow->systemMenu(), SIGNAL(triggered(QAction*)), SLOT(sysContextMenuAction(QAction*)));
    return subWindow;
}

void FixedMdiArea::tileSubWindows() {
    // A fix for https://local.ugene.net/tracker/browse/UGENE-4361
    // An appropriate Qt bug: https://bugreports.qt.io/browse/QTBUG-29758
    // After Qt bug fixing just remove this method.

#ifndef Q_OS_MAC
    QMdiArea::tileSubWindows();
    return;
#endif

    QMainWindow *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    SAFE_POINT_EXT(NULL != mainWindow, QMdiArea::tileSubWindows(), );

    QPoint topLeft = mainWindow->mapToGlobal(QPoint(0,0));
    static QPoint compensationOffset = QPoint(0, -22);      // I think, it is a menu bar. I'm not sure that it has constant height.

    QMdiArea::tileSubWindows();

    mainWindow->move(topLeft + compensationOffset);

    QPoint topLeftResult = mainWindow->mapToGlobal(QPoint(0,0));
    if (topLeft != topLeftResult) {
        compensationOffset = topLeft + (topLeft - topLeftResult);
        mainWindow->move(topLeft + compensationOffset);
    }
}

void MainWindowImpl::sl_show(){
    if(qobject_cast<TaskScheduler*> (sender()) == qobject_cast<TaskScheduler*> (AppContext::getTaskScheduler())){
        QObject::disconnect(AppContext::getTaskScheduler(), SIGNAL(si_noTasksInScheduler()), this, SLOT(sl_show()));
    }
    bool maximized =AppContext::getSettings()->getValue(SETTINGS_DIR + "maximized", false).toBool();
    QRect geom =AppContext::getSettings()->getValue(SETTINGS_DIR + "geometry", QRect()).toRect();

    if(mw != NULL){
        if (maximized) {
            mw->showMaximized();
        } else {
            mw->show();
            if (!geom.isNull()) {
                mw->setGeometry(geom);
            }
        }
    }else{
        return;
    }
    foreach(Task *t, startupTasklist){
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    }
    foreach(Notification* notification, startupNotificationsList) {
        nStack->addNotification(notification);
    }
    startupTasklist.clear();
    emit si_show();
}

void MainWindowImpl::sl_crashUgene() {
    volatile int *killer = NULL;
    *killer = 0;
}

void MainWindowImpl::registerStartupChecks( QList<Task*> tasks ){
    startupTasklist << tasks;
}
void MainWindowImpl::addNotification(const QString& message, NotificationType type) {
    Notification* notification = new Notification(message, type);
    if(mw->isVisible()) {
        nStack->addNotification(notification);
    }
    else {
        startupNotificationsList << notification;
    }
}

}//namespace
