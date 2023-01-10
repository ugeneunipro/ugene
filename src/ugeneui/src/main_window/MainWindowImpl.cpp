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

#include <qglobal.h>
#ifdef Q_OS_DARWIN
#    include <Security/Authorization.h>
#    include <errno.h>
#    include <unistd.h>
#endif
#include <algorithm>

#include <QAction>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ObjectViewModel.h>

#include "AboutDialogController.h"
#include "CheckUpdatesTask.h"
#include "CreateDesktopShortcutTask.h"
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

#define SETTINGS_DIR QString("main_window/")

class MWStub : public QMainWindow {
public:
    MWStub(MainWindowImpl* _owner)
        : owner(_owner) {
        setAttribute(Qt::WA_NativeWindow);
        setAcceptDrops(true);
    }
    virtual QMenu* createPopupMenu() {
        return nullptr;
    }  // todo: decide if we do really need this menu and fix it if yes?
protected:
    virtual void closeEvent(QCloseEvent* e);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
    virtual bool focusNextPrevChild(bool next);

protected:
    MainWindowImpl* owner;
};

void MWStub::closeEvent(QCloseEvent* e) {
    if (owner->getMDIManager() == nullptr) {
        QMainWindow::closeEvent(e);
    } else {
        owner->runClosingTask();
        e->ignore();
    }
}

void MWStub::dragEnterEvent(QDragEnterEvent* event) {
    MainWindowDragNDrop::dragEnterEvent(event);
}

void MainWindowDragNDrop::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls() || event->mimeData()->hasFormat(DocumentMimeData::MIME_TYPE)) {
        event->acceptProposedAction();
    }
}

void MWStub::dropEvent(QDropEvent* event) {
    MainWindowDragNDrop::dropEvent(event);
}

void MainWindowDragNDrop::dropEvent(QDropEvent* event) {
    if (event->source() == nullptr) {
        QList<GUrl> urls;
        if (event->mimeData()->hasUrls()) {
            urls = GUrlUtils::qUrls2gUrls(event->mimeData()->urls());
        } else if (event->mimeData()->hasFormat(DocumentMimeData::MIME_TYPE)) {
            urls = GUrlUtils::qUrls2gUrls(event->mimeData()->urls());
        }
        if (!urls.isEmpty()) {
            QVariantMap hints;
            hints[ProjectLoaderHint_CloseActiveProject] = true;
            Task* t = AppContext::getProjectLoader()->openWithProjectTask(urls, hints);
            if (t) {
                AppContext::getTaskScheduler()->registerTopLevelTask(t);
                event->acceptProposedAction();
            }
        }
    } else {
        if (event->mimeData()->hasFormat(DocumentMimeData::MIME_TYPE)) {
            const DocumentMimeData* docData = static_cast<const DocumentMimeData*>(event->mimeData());

            DocumentSelection ds;
            ds.setSelection(QList<Document*>() << docData->objPtr);
            MultiGSelection ms;
            ms.addSelection(&ds);
            foreach (GObjectViewFactory* f, AppContext::getObjectViewFactoryRegistry()->getAllFactories()) {
                if (f->canCreateView(ms)) {
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

void MWStub::dragMoveEvent(QDragMoveEvent* event) {
    MainWindowDragNDrop::dragMoveEvent(event);
}

void MainWindowDragNDrop::dragMoveEvent(QDragMoveEvent* event) {
    MainWindow* mainWindow = AppContext::getMainWindow();
    SAFE_POINT(nullptr != mainWindow, L10N::nullPointerError("Main Window"), );

    if (event->mimeData()->hasUrls())
        return;
    if (event->source() != nullptr) {
        QObject* par = event->source()->parent();
        while (par != nullptr) {
            if (par == mainWindow->getQMainWindow()) {
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
MainWindowImpl::~MainWindowImpl() {
    SAFE_POINT(mw == nullptr, "main window must be null!", );
}

void MainWindowImpl::prepare() {
    createActions();
    prepareGUI();
}

void MainWindowImpl::close() {
    AppContext::getSettings()->setValue(SETTINGS_DIR + "maximized", mw->isMaximized());
    AppContext::getSettings()->setValue(SETTINGS_DIR + "geometry", mw->geometry());

    dockManager->deleteLater();
    dockManager = nullptr;

    menuManager->deleteLater();
    menuManager = nullptr;

    toolbarManager->deleteLater();
    toolbarManager = nullptr;

    mdiManager->deleteLater();
    mdiManager = nullptr;

    nStack->deleteLater();
    nStack = nullptr;

    mdi->deleteLater();
    mdi = nullptr;

    mw->close();
    mw->deleteLater();
    mw = nullptr;
}

bool MainWindowImpl::eventFilter(QObject* object, QEvent* event) {
    CHECK(mw == object, false);
    CHECK(nullptr != event, false);
    CHECK(event->type() == QEvent::KeyPress, false);

    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
    CHECK(nullptr != keyEvent, false);

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

    createDesktopShortcutAction = new QAction(tr("Create desktop shortcut"), this);
    createDesktopShortcutAction->setObjectName("Create desktop shortcut");
    connect(createDesktopShortcutAction, SIGNAL(triggered()), SLOT(sl_createDesktopShortcutAction()));

    welcomePageAction = new QAction(tr("Open Start Page"), this);
    welcomePageAction->setObjectName("welcome_page");
    connect(welcomePageAction, SIGNAL(triggered()), SIGNAL(si_showWelcomePage()));

    showWhatsNewAction = new QAction(tr("What's New in UGENE"), this);
    showWhatsNewAction->setObjectName("show_whats_new");
    connect(showWhatsNewAction, SIGNAL(triggered()), SLOT(sl_showWhatsNew()));

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
    QWidget* p = qobject_cast<QWidget*>(getQMainWindow());
    QObjectScopedPointer<AboutDialogController> d = new AboutDialogController(visitWebAction, p);
    d->exec();
}

void MainWindowImpl::sl_showWhatsNew() {
    Shtirlitz::showWhatsNewDialog();
}

void MainWindowImpl::sl_checkUpdatesAction() {
    AppContext::getTaskScheduler()->registerTopLevelTask(new CheckUpdatesTask());
}

void MainWindowImpl::sl_createDesktopShortcutAction() {
    AppContext::getTaskScheduler()->registerTopLevelTask(new CreateDesktopShortcutTask());
}

void MainWindowImpl::setWindowTitle(const QString& title) {
    if (title.isEmpty()) {
        mw->setWindowTitle(U2_APP_TITLE);
    } else {
        mw->setWindowTitle(title + " " + U2_APP_TITLE);
    }
}

void MainWindowImpl::registerAction(QAction* action) {
    menuManager->registerAction(action);
}

void MainWindowImpl::prepareGUI() {
    mw = new MWStub(this);  // todo: parents?
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

    nStack = new NotificationStack(mw);

    aboutAction->setObjectName(ACTION__ABOUT);
    aboutAction->setParent(mw);
    QMenu* helpMenu = menuManager->getTopLevelMenu(MWMENU_HELP);
    helpMenu->addAction(viewOnlineDocumentation);
    helpMenu->addSeparator();
    helpMenu->addAction(visitWebAction);
    helpMenu->addAction(showWhatsNewAction);
    helpMenu->addAction(checkUpdateAction);
    helpMenu->addSeparator();
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    // TODO: re-test support for MAC OS before enabling.
    helpMenu->addAction(createDesktopShortcutAction);
    helpMenu->addSeparator();
#endif
    helpMenu->addAction(welcomePageAction);
    helpMenu->addAction(aboutAction);
    if (qgetenv(ENV_TEST_CRASH_HANDLER) == "1") {
        helpMenu->addSeparator();
        helpMenu->addAction(crashUgeneAction);
    }

    if (qgetenv(ENV_TEST_NOTIFICATIONS) == "1") {
        helpMenu->addSeparator();

        static int testNotificationCounter = 0;
        auto addUniqueNotificationAction = new QAction(tr("Add unique notification"), this);
        addUniqueNotificationAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_N);
        connect(addUniqueNotificationAction, &QAction::triggered, [=]() {
            testNotificationCounter++;
            nStack->add("Notification: " + QString::number(testNotificationCounter) + QString("\n...").repeated(testNotificationCounter % 4));
        });
        helpMenu->addAction(addUniqueNotificationAction);

        auto addRepeatingNotificationAction = new QAction(tr("Add repeating notification"), this);
        addRepeatingNotificationAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_M);
        connect(addRepeatingNotificationAction, &QAction::triggered, [=]() { nStack->add("Repeating notification"); });
        helpMenu->addAction(addRepeatingNotificationAction);
    }

    mdiManager = new MWMDIManagerImpl(this, mdi);

    dockManager = new MWDockManagerImpl(this);
}

void MainWindowImpl::runClosingTask() {
    if (!shutDownInProcess) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new ShutdownTask(this));
        setShutDownInProcess(true);
    } else {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(getQMainWindow());
        msgBox->setWindowTitle(U2_APP_TITLE);
        msgBox->setText(tr("Shutdown already in process. Close UGENE immediately?"));
        QPushButton* closeButton = msgBox->addButton(tr("Close"), QMessageBox::ActionRole);
        /*QPushButton *waitButton =*/msgBox->addButton(tr("Wait"), QMessageBox::ActionRole);
        msgBox->exec();
        CHECK_EXT(!msgBox.isNull(), exit(0), );

        if (getQMainWindow()) {
            if (msgBox->clickedButton() == closeButton) {
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
    GUIUtils::runWebBrowser("https://ugene.net");
}
void MainWindowImpl::sl_viewOnlineDocumentation() {
    GUIUtils::runWebBrowser("https://ugene.net/documentation.html");
}

void MainWindowImpl::sl_tempDirPathCheckFailed(QString path) {
    QObjectScopedPointer<TmpDirChangeDialogController> tmpDirChangeDialogController = new TmpDirChangeDialogController(path, mw);
    tmpDirChangeDialogController->exec();
    CHECK(!tmpDirChangeDialogController.isNull(), );

    if (tmpDirChangeDialogController->result() == QDialog::Accepted) {
        AppContext::getAppSettings()->getUserAppsSettings()->setUserTemporaryDirPath(tmpDirChangeDialogController->getTmpDirPath());
    } else {
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
    tools << "ugene"
          << "ugeneui"
          << "ugenecl";

    AuthorizationRef auth;
    if (AuthorizationCreate(nullptr, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &auth) == errAuthorizationSuccess) {
        foreach (QString tool, tools) {
            QByteArray executable = (exePath + tool).toUtf8();
            QByteArray installPath = installationPath.toUtf8();
            QString symlink = installationPath + tool;
            char const* arguments[] = {"-f", "-s", executable.constData(), installPath.constData(), nullptr};
            char const* helperTool = "/bin/ln";

            if (AuthorizationExecuteWithPrivileges(auth, helperTool, kAuthorizationFlagDefaults, (char**)arguments, nullptr) == errAuthorizationSuccess) {
                // HACK: sleep because otherwise QFileInfo::exists might return false
                sleep(100);
                wait(nullptr);
                if (!QFileInfo(symlink).exists()) {
                    QMessageBox::critical(nullptr, tr("Installation failed"), tr("Failed to enable terminal usage: couldn't install '%1'").arg(symlink));
                    success = false;
                    break;
                }
            } else {
                QMessageBox::critical(nullptr, tr("Installation failed"), tr("Failed to enable terminal usage: not authorized"));
                success = false;
                break;
            }
        }

        AuthorizationFree(auth, kAuthorizationFlagDefaults);
    } else {
        QMessageBox::critical(nullptr, tr("Installation failed"), tr("Failed to enable terminal usage: authorization failure"));
        success = false;
    }

    if (success) {
        QMessageBox::information(nullptr, tr("Installation successful"), tr("Terminal usage successfully enabled.\n\nNow you can type ugene in command line to start UGENE."));
    }
}
#endif  // #ifdef _INSTALL_TO_PATH_ACTION

QMenu* MainWindowImpl::getTopLevelMenu(const QString& sysName) const {
    return menuManager->getTopLevelMenu(sysName);
}

QToolBar* MainWindowImpl::getToolbar(const QString& sysName) const {
    return toolbarManager->getToolbar(sysName);
}

///////////////////////////////////////////////////////////////////

FixedMdiArea::FixedMdiArea(QWidget* parent)
    : QMdiArea(parent) {
    setDocumentMode(true);
    setTabShape(QTabWidget::Rounded);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
}

void FixedMdiArea::setViewMode(QMdiArea::ViewMode mode) {
    if (mode == viewMode()) {
        return;
    }
    QMdiArea::setViewMode(mode);
    if (mode == QMdiArea::TabbedView) {
        // FIXME QTBUG-9293, Adding a close button to tabbed QMdiSubWindows
        QList<QTabBar*> tb = findChildren<QTabBar*>();
        for (QTabBar* t : qAsConst(tb)) {
            if (t->parentWidget() == this) {
                t->setTabsClosable(true);
            }
        }
    } else {
        // TODO QTBUG-3269: switching between TabbedView and SubWindowView does not preserve maximized window state
    }
}

// Workaround for QTBUG-17428: Superfluous RestoreAction for tabbed QMdiSubWindows
void FixedMdiArea::sysContextMenuAction(QAction* action) {
    if (viewMode() == QMdiArea::TabbedView && activeSubWindow()) {
        QList<QAction*> lst = activeSubWindow()->actions();
        if (!lst.isEmpty() && action == lst.first()) {  // RestoreAction always comes before CloseAction
            // FIXME better to detect via shortcut or icon ???
            assert(action->icon().pixmap(32).toImage() == style()->standardIcon(QStyle::SP_TitleBarNormalButton).pixmap(32).toImage());
            activeSubWindow()->showMaximized();
        }
    }
}

QMdiSubWindow* FixedMdiArea::addSubWindow(QWidget* widget) {
    QMdiSubWindow* subWindow = QMdiArea::addSubWindow(widget);
    // Workaround for QTBUG-17428
    connect(subWindow->systemMenu(), SIGNAL(triggered(QAction*)), SLOT(sysContextMenuAction(QAction*)));
    return subWindow;
}

void FixedMdiArea::tileSubWindows() {
    // A fix for https://local.ugene.net/tracker/browse/UGENE-4361
    // An appropriate Qt bug: https://bugreports.qt.io/browse/QTBUG-29758
    // After Qt bug fixing just remove this method.

    if (!isOsMac()) {
        QMdiArea::tileSubWindows();
        return;
    }

    QMainWindow* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    SAFE_POINT_EXT(mainWindow != nullptr, QMdiArea::tileSubWindows(), );

    QPoint topLeft = mainWindow->mapToGlobal(QPoint(0, 0));
    static QPoint compensationOffset = QPoint(0, -22);  // I think, it is a menu bar. I'm not sure that it has constant height.

    QMdiArea::tileSubWindows();

    mainWindow->move(topLeft + compensationOffset);

    QPoint topLeftResult = mainWindow->mapToGlobal(QPoint(0, 0));
    if (topLeft != topLeftResult) {
        compensationOffset = topLeft + (topLeft - topLeftResult);
        mainWindow->move(topLeft + compensationOffset);
    }
}

void MainWindowImpl::sl_show() {
    bool maximized = AppContext::getSettings()->getValue(SETTINGS_DIR + "maximized", false).toBool();
    QRect geom = AppContext::getSettings()->getValue(SETTINGS_DIR + "geometry", QRect()).toRect();

    if (mw != nullptr) {
        if (maximized) {
            mw->showMaximized();
        } else {
            mw->show();
            if (!geom.isNull()) {
                mw->setGeometry(geom);
            }
        }
    } else {
        return;
    }
    foreach (Task* t, startupTasklist) {
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    }
    startupTasklist.clear();
    emit si_show();
}

void MainWindowImpl::sl_crashUgene() {
    volatile int* killer = nullptr;
    *killer = 0;
}

void MainWindowImpl::registerStartupChecks(const QList<Task*>& tasks) {
    startupTasklist << tasks;
}

void MainWindowImpl::addNotification(const QString& message, NotificationType type) {
    SAFE_POINT(nStack != nullptr, "Notification stack is null", );
    nStack->add(message, type);
}

}  // namespace U2
