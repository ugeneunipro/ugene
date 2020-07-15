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

#include "ProjectLoaderImpl.h"

#include <QAction>
#include <QDesktopServices>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GHints.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/IdRegistry.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/ServiceTypes.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateDocumentFromTextDialogController.h>
#include <U2Gui/DownloadRemoteFileDialog.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/PasteController.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/ProjectView.h>
#include <U2Gui/SearchGenbankSequenceDialogController.h>
#include <U2Gui/SharedConnectionsDialog.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/DnaAssemblyGUIExtension.h>

#include "DocumentFormatSelectorController.h"
#include "DocumentProviderSelectorController.h"
#include "DocumentReadingModeSelectorController.h"
#include "MultipleDocumentsReadingModeSelectorController.h"
#include "ProjectImpl.h"
#include "ProjectTasksGui.h"
#include "project_view/ProjectViewImpl.h"

namespace U2 {

static QString getProjectFilePathFromPathEdit(const QLineEdit *projectFilePathEdit) {
    QString path = projectFilePathEdit->text();
    if (path.isEmpty()) {
        return path;
    }
    if (!path.endsWith(PROJECTFILE_EXT)) {
        path.append(PROJECTFILE_EXT);
    }
    return path;
}

//////////////////////////////////////////////////////////////////////////
/// ProjectLoaderImpl
//////////////////////////////////////////////////////////////////////////

ProjectLoaderImpl::ProjectLoaderImpl() {
    pasteAction = openProjectAction = newProjectAction = NULL;
    recentProjectsMenu = NULL;

    assert(AppContext::getProject() == NULL);
    assert(AppContext::getProjectLoader() == NULL);

    ServiceRegistry *sr = AppContext::getServiceRegistry();
    connect(sr, SIGNAL(si_serviceStateChanged(Service *, ServiceState)), SLOT(sl_serviceStateChanged(Service *, ServiceState)));

    newProjectAction = new QAction(QIcon(":ugene/images/project_new.png"), tr("&New project..."), this);
    newProjectAction->setObjectName(ACTION_PROJECTSUPPORT__NEW_PROJECT);
    //    newProjectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    newProjectAction->setShortcutContext(Qt::WindowShortcut);
    connect(newProjectAction, SIGNAL(triggered()), SLOT(sl_newProject()));

    addExistingDocumentAction = new QAction(QIcon(":ugene/images/advanced_open.png"), tr("Open as..."), this);
    addExistingDocumentAction->setObjectName(ACTION_PROJECTSUPPORT__OPEN_AS);
    addExistingDocumentAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    addExistingDocumentAction->setShortcutContext(Qt::ApplicationShortcut);
    connect(addExistingDocumentAction, SIGNAL(triggered()), SLOT(sl_onAddExistingDocument()));

    newDocumentFromtext = new QAction(QIcon(), tr("New document from text..."), this);
    newDocumentFromtext->setObjectName("NewDocumentFromText");
    newDocumentFromtext->setShortcutContext(Qt::WindowShortcut);
    connect(newDocumentFromtext, SIGNAL(triggered()), SLOT(sl_newDocumentFromText()));

    pasteAction = new QAction(QIcon(":ugene/images/paste.png"), tr("Open from clipboard..."), this);
    pasteAction->setObjectName(ACTION_PROJECTSUPPORT__PASTE);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setShortcutContext(Qt::WidgetShortcut);
    connect(pasteAction, SIGNAL(triggered()), SLOT(sl_paste()));

    openProjectAction = new QAction(QIcon(":ugene/images/project_open.png"), tr("Open..."), this);
    openProjectAction->setObjectName(ACTION_PROJECTSUPPORT__OPEN_PROJECT);
    openProjectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    openProjectAction->setShortcutContext(Qt::WindowShortcut);
    connect(openProjectAction, SIGNAL(triggered()), SLOT(sl_openProject()));

    downloadRemoteFileAction = new QAction(tr("Access remote database..."), this);
    downloadRemoteFileAction->setObjectName(ACTION_PROJECTSUPPORT__ACCESS_REMOTE_DB);
    downloadRemoteFileAction->setIcon(QIcon(":ugene/images/world_go.png"));
    connect(downloadRemoteFileAction, SIGNAL(triggered()), SLOT(sl_downloadRemoteFile()));

    accessSharedDatabaseAction = new QAction(tr("Connect to UGENE shared database..."), this);
    accessSharedDatabaseAction->setObjectName(ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);
    accessSharedDatabaseAction->setIcon(QIcon(":core/images/db/database_go.png"));
    connect(accessSharedDatabaseAction, SIGNAL(triggered()), SLOT(sl_accessSharedDatabase()));

    searchGenbankEntryAction = new QAction(tr("Search NCBI GenBank..."), this);
    searchGenbankEntryAction->setObjectName(ACTION_PROJECTSUPPORT__SEARCH_GENBANK);
    searchGenbankEntryAction->setIcon(QIcon(":ugene/images/world_go.png"));
    connect(searchGenbankEntryAction, SIGNAL(triggered()), SLOT(sl_searchGenbankEntry()));

    //add load/close actions to menu and toolbar
    MainWindow *mw = AppContext::getMainWindow();
    QMenu *fileMenu = mw->getTopLevelMenu(MWMENU_FILE);

    recentProjectsMenu = new QMenu(tr("Recent projects"));
    recentProjectsMenu->menuAction()->setObjectName(ACTION_PROJECTSUPPORT__RECENT_PROJECTS_MENU);
    updateRecentProjectsMenu();

    recentItemsMenu = new QMenu(tr("Recent files"));
    recentItemsMenu->menuAction()->setObjectName("recent_docs_menu_action");
    updateRecentItemsMenu();

    QAction *newSectionSeparator = new QAction("", this);
    newSectionSeparator->setSeparator(true);
    newSectionSeparator->setObjectName(ACTION_PROJECTSUPPORT__NEW_SECTION_SEPARATOR);

    QAction *openSectionSeparator = new QAction("", this);
    openSectionSeparator->setSeparator(true);

    QAction *remoteSectionSeparator = new QAction("", this);
    remoteSectionSeparator->setSeparator(true);

    QAction *recentSectionSeparator = new QAction("", this);
    recentSectionSeparator->setSeparator(true);

    QList<QAction *> actions;
    actions << newProjectAction
            << newDocumentFromtext
            << newSectionSeparator
            << openProjectAction
            << addExistingDocumentAction
            << pasteAction
            << openSectionSeparator
            << downloadRemoteFileAction
            << searchGenbankEntryAction
            << accessSharedDatabaseAction
            << remoteSectionSeparator
            << recentItemsMenu->menuAction()
            << recentProjectsMenu->menuAction()
            << recentSectionSeparator;

    fileMenu->insertActions(fileMenu->actions().first(), actions);

    QToolBar *tb = mw->getToolbar(MWTOOLBAR_MAIN);
    tb->addAction(newProjectAction);
    tb->addAction(openProjectAction);

    updateState();

    IdRegistry<WelcomePageAction> *welcomePageActions = AppContext::getWelcomePageActionRegistry();
    CHECK(NULL != welcomePageActions, );
    welcomePageActions->registerEntry(new LoadDataWelcomePageAction(this));
    welcomePageActions->registerEntry(new CreateSequenceWelcomePageAction(this));
}

void ProjectLoaderImpl::updateState() {
    recentProjectsMenu->setDisabled(recentProjectsMenu->isEmpty());
}

#define MAX_RECENT_FILES 7

void ProjectLoaderImpl::sl_newProject() {
    QWidget *p = (QWidget *)AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<ProjectDialogController> d = new ProjectDialogController(ProjectDialogController::New_Project, p);
    int rc = d->exec();
    CHECK(!d.isNull(), );
    QFileInfo projectPathFileInfo(getProjectFilePathFromPathEdit(d->projectFilePathEdit));
    AppContext::getSettings()->setValue(SETTINGS_DIR + "last_dir", projectPathFileInfo.absoluteDir().absolutePath(), true);

    if (rc == QDialog::Rejected) {
        updateState();
        return;
    }

    QString projectPath = projectPathFileInfo.absoluteFilePath();
    if (projectPathFileInfo.exists()) {
        QFile::remove(projectPath);
    }

    QString projectName = d->projectNameEdit->text();
    AppContext::getTaskScheduler()->registerTopLevelTask(new OpenProjectTask(projectPath, projectName));
}

void ProjectLoaderImpl::sl_openProject() {
    LastUsedDirHelper h;
    QString filter = DialogUtils::prepareDocumentsFileFilter(true);

    filter.append("\n" + tr("UGENE project file") + " (*" + PROJECTFILE_EXT + ")");

    QStringList files;

    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        files = U2FileDialog::getOpenFileNames(QApplication::activeWindow(), tr("Select files to open"), h.dir, filter, 0, QFileDialog::DontUseNativeDialog);
    } else {
        files = U2FileDialog::getOpenFileNames(QApplication::activeWindow(), tr("Select files to open"), h.dir, filter);
    }

    if (files.isEmpty()) {
        return;
    }

    if (QFileInfo(files.first()).exists()) {
        h.url = files.first();
    }
    QList<GUrl> urls;
    foreach (QString file, files) {
        urls << GUrl(file, GUrl_File);
    }
    //updateRecentItemsMenu();
    Task *openTask = openWithProjectTask(urls);
    if (openTask != NULL) {
        AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
    }
}

void ProjectLoaderImpl::sl_openRecentProject() {
    QAction *action = qobject_cast<QAction *>(sender());
    assert(action);
    QString url = action->data().toString();
    AppContext::getTaskScheduler()->registerTopLevelTask(new OpenProjectTask(url));
}

void ProjectLoaderImpl::sl_openRecentFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    assert(action);
    GUrl url = action->data().toString();
    Task *task = ProjectLoader::openWithProjectTask(url);
    if (task == NULL) {
        return;
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    prependToRecentItems(url.getURLString());
    updateRecentItemsMenu();
}

void ProjectLoaderImpl::prependToRecentProjects(const QString &url) {
    assert(!url.isEmpty());
    CHECK(GUrl(url).isLocalFile(), );
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, QStringList(), true).toStringList();
    recentFiles.removeAll(QString());    //remove all empty tokens if fount (a kind of cleanup)
    recentFiles.removeAll(url);    // remove URL from the old position
    recentFiles.prepend(url);    // make URL first
    while (recentFiles.size() > MAX_RECENT_FILES) {
        recentFiles.pop_back();
    }
    AppContext::getSettings()->setValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, recentFiles, true);
    emit si_recentListChanged();
}

void ProjectLoaderImpl::updateRecentProjectsMenu() {
    assert(recentProjectsMenu != NULL);
    recentProjectsMenu->clear();
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, QStringList(), true).toStringList();
    Project *p = AppContext::getProject();
    foreach (QString f, recentFiles) {
        if ((p == NULL || f != p->getProjectURL()) && !f.isEmpty()) {
            QAction *a = recentProjectsMenu->addAction(f, this, SLOT(sl_openRecentProject()));
            a->setData(f);
            a->setDisabled(!QFile::exists(f));
        }
    }
}

namespace {
/**
 * If there are only unsupported documents which are needed to load
 * then it is not needed to show the project because it will be empty
 */
void prepareDocTab(const QList<AD2P_DocumentInfo> &docsInfo, const QList<AD2P_ProviderInfo> &docProviders) {
    CHECK(docProviders.isEmpty(), );
    foreach (const AD2P_DocumentInfo &info, docsInfo) {
        const DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(info.formatId);
        if (NULL == df) {
            continue;
        }
        const GObjectType t = df->getSupportedObjectTypes().toList().first();
        if (GObjectTypes::getTypeInfo(t).type != GObjectTypes::UNKNOWN) {
            // the project will not be empty
            return;
        }
    }

    const MainWindow *mw = AppContext::getMainWindow();
    CHECK(NULL != mw, );
    MWDockManager *dm = mw->getDockManager();
    CHECK(NULL != dm, );

    {    // do not activate the tab
        dm->dontActivateNextTime(MWDockArea_Left);
        AppContext::getSettings()->setValue(ProjectViewImpl::SETTINGS_ROOT + "firstShow", false);
    }
}

bool haveFormatsRelations(const FormatDetectionResult &firstFormat, const FormatDetectionResult &secondFormat) {
    if (NULL != firstFormat.format && NULL != secondFormat.format) {
        return false;
    }
    if (NULL != firstFormat.format && NULL != secondFormat.importer) {
        return secondFormat.importer->getFormatIds().contains(firstFormat.format->getFormatId());
    }
    if (NULL != firstFormat.importer && NULL != secondFormat.format) {
        return firstFormat.importer->getFormatIds().contains(secondFormat.format->getFormatId());
    }
    if (NULL != firstFormat.importer && NULL != secondFormat.importer) {
        return !firstFormat.importer->getFormatIds().toSet().intersect(secondFormat.importer->getFormatIds().toSet()).isEmpty();
    }
    return false;
}

FormatDetectionResult getFirstUnrelatedFormat(const QList<FormatDetectionResult> &formats) {
    CHECK(formats.size() > 1, FormatDetectionResult());
    const FormatDetectionResult firstFormat = formats[0];

    for (int i = 1; i < formats.size(); i++) {
        if (!haveFormatsRelations(firstFormat, formats[i])) {
            return formats[i];
        }
    }
    return FormatDetectionResult();
}

QList<FormatDetectionResult> getRelatedFormats(const QList<FormatDetectionResult> &formats, int idx) {
    SAFE_POINT(0 <= idx && idx < formats.size(), "Format index is out of range", QList<FormatDetectionResult>());
    QList<FormatDetectionResult> result;
    result << formats[idx];
    for (int i = 0; i < formats.size(); i++) {
        if (Q_LIKELY(idx != i) && haveFormatsRelations(formats[idx], formats[i])) {
            result << formats[i];
        }
    }
    return result;
}
}    // namespace

bool ProjectLoaderImpl::shouldFormatBeSelected(const QList<FormatDetectionResult> &formats, bool forceSelectFormat) {
    CHECK(formats.size() > 1, false);

    const FormatDetectionResult firstFormat = formats[0];
    const FormatDetectionResult firstUnrelatedFormat = getFirstUnrelatedFormat(formats);
    CHECK(FormatDetection_NotMatched != firstUnrelatedFormat.score(), false);

    int firstFormatScore = firstFormat.score();
    int firstUnrelatedFormatScore = firstUnrelatedFormat.score();
    bool isFirstFormatEqualFirstUnrelatedFormat = firstFormatScore == firstUnrelatedFormatScore;
    bool isFirstUnrelatedFormatMoreThenFormatDetectionAverageSimilarity = firstUnrelatedFormatScore > FormatDetection_AverageSimilarity;
    bool isFirstFormatLessThenFormatDetectionMatched = firstFormatScore < FormatDetection_Matched;
    bool isFirstFormatLessOrEqualThenFormatDetectionAverageSimilarity = firstFormatScore <= FormatDetection_AverageSimilarity;
    return isFirstFormatEqualFirstUnrelatedFormat || (isFirstUnrelatedFormatMoreThenFormatDetectionAverageSimilarity && isFirstFormatLessThenFormatDetectionMatched) || isFirstFormatLessOrEqualThenFormatDetectionAverageSimilarity || forceSelectFormat;
}

int ProjectLoaderImpl::getMaxObjectsInSingleDocument() {
    int maxObjects = qgetenv("UGENE_MAX_OBJECTS_PER_DOCUMENT").toInt();
    return maxObjects < 10 ? 50000 : maxObjects;
}

bool ProjectLoaderImpl::detectFormat(const GUrl &url, QList<FormatDetectionResult> &formats, const QVariantMap &hints, FormatDetectionResult &selectedResult) {
    CHECK(!formats.isEmpty(), false);
    int idx = 0;
    if (shouldFormatBeSelected(formats, hints.value(ProjectLoaderHint_ForceFormatOptions, false).toBool())) {
        idx = DocumentFormatSelectorController::selectResult(url, formats.first().rawData, formats);
        if (idx >= 0) {
            selectedResult = formats[idx];
            return true;
        } else {
            return false;
        }
    } else {
        QList<FormatDetectionResult> relatedFormats = getRelatedFormats(formats, idx);
        if (relatedFormats.size() > 1) {
            int indexInRelatedList = DocumentProviderSelectorController::selectResult(url, relatedFormats);
            if (indexInRelatedList >= 0) {
                selectedResult = relatedFormats[indexInRelatedList];
                return true;
            } else {
                return false;
            }
        }
    }
    selectedResult = formats[0];
    return true;
}

Task *ProjectLoaderImpl::openWithProjectTask(const QList<GUrl> &_urls, const QVariantMap &hints) {
    QList<GUrl> urls = _urls;
    // detect if we open real UGENE project file
    bool projectsOnly = true;
    foreach (const GUrl &url, urls) {
        projectsOnly = projectsOnly && url.lastFileSuffix() == PROJECT_FILE_PURE_EXT;
        if (!projectsOnly) {
            break;
        }
    }
    if (projectsOnly) {
        GUrl projectUrl = urls.isEmpty() ? QString() : urls.last();
        QVariantMap h2 = hints;
        h2[ProjectLoaderHint_CloseActiveProject] = true;
        return createProjectLoadingTask(projectUrl, h2);
    }
    bool abilityUniteDocuments = true;

    QVariantMap hintsOverDocuments;
    QMap<QString, qint64> headerSequenceLengths;

    if (urls.size() >= 2) {
        foreach (const GUrl &url, urls) {
            FormatDetectionResult dr;
            FormatDetectionConfig conf;
            conf.useImporters = hints.value(ProjectLoaderHint_UseImporters, true).toBool();
            conf.bestMatchesOnly = false;
            QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, conf);
            if (formats.isEmpty()) {
                abilityUniteDocuments = false;
                break;
            }
            dr = formats[0];
            bool matchCurrentDocument = MultipleDocumentsReadingModeSelectorController::mergeDocumentOption(dr, &headerSequenceLengths);

            if (!matchCurrentDocument) {
                abilityUniteDocuments = false;
                break;
            }
        }
    } else {
        abilityUniteDocuments = false;
    }

    if (abilityUniteDocuments) {
        bool ok = MultipleDocumentsReadingModeSelectorController::adjustReadingMode(hintsOverDocuments, urls, headerSequenceLengths);
        if (!ok) {
            return NULL;
        }
    }

    // detect all formats from urls list and add files to project
    QList<AD2P_DocumentInfo> docsInfo;
    QList<AD2P_ProviderInfo> docProviders;
    int nViews = 0;
    foreach (const GUrl &url, urls) {
        if (url.lastFileSuffix() == PROJECT_FILE_PURE_EXT) {
            // skip extra project files
            coreLog.info(tr("Project file '%1' ignored").arg(url.getURLString()));
            continue;
        }
        Project *project = AppContext::getProject();
        Document *doc = project == NULL ? NULL : project->findDocumentByURL(url);
        if (doc != NULL) {
            coreLog.details(tr("The document with the same URL is already added to the project"));
            if (doc->isLoaded()) {
                const QList<GObject *> &docObjects = doc->getObjects();
                QList<GObjectViewWindow *> viewsList = GObjectViewUtils::findViewsWithAnyOfObjects(docObjects);
                if (viewsList.isEmpty()) {
                    AppContext::getTaskScheduler()->registerTopLevelTask(new OpenViewTask(doc));
                } else {
                    AppContext::getMainWindow()->getMDIManager()->activateWindow(viewsList.first());
                }
                coreLog.info(tr("The document is already loaded and added to project: %1").arg(url.fileName()));
            } else if (!doc->isLoaded() && AppContext::getProjectView()) {
                if (NULL == ProjectUtils::findLoadTask(url.getURLString())) {
                    AppContext::getTaskScheduler()->registerTopLevelTask(new LoadUnloadedDocumentAndOpenViewTask(doc));
                } else {
                    coreLog.details(tr("The document with the same URL is already loading"));
                }
            }
        } else {
            QList<FormatDetectionResult> formats;
            if (hintsOverDocuments.value(ProjectLoaderHint_MultipleFilesMode_Flag, false).toBool() == false) {
                FormatDetectionConfig conf;
                conf.useImporters = hints.value(ProjectLoaderHint_UseImporters, true).toBool();
                conf.bestMatchesOnly = false;
                formats = DocumentUtils::detectFormat(url, conf);
            } else {
                FormatDetectionResult result;
                result.format = AppContext::getDocumentFormatRegistry()->getFormatById(hintsOverDocuments[ProjectLoaderHint_MultipleFilesMode_RealDocumentFormat].toString());
                formats << result;
            }

            if (!formats.isEmpty()) {
                FormatDetectionResult dr;
                const bool accepted = detectFormat(url, formats, hints, dr);
                if (accepted) {
                    dr.rawDataCheckResult.properties.unite(hints);
                    dr.rawDataCheckResult.properties.unite(hintsOverDocuments);
                    if (dr.format != NULL) {
                        bool forceReadingOptions = hints.value(ProjectLoaderHint_ForceFormatOptions, false).toBool();
                        bool optionsAlreadyChosen = hints.value((ProjectLoaderHint_MultipleFilesMode_Flag), false).toBool() || hints.value((DocumentReadingMode_SequenceMergeGapSize), false).toBool() || hints.value((DocumentReadingMode_SequenceAsAlignmentHint), false).toBool() || hints.value((DocumentReadingMode_SequenceAsShortReadsHint), false).toBool() || hints.value((DocumentReadingMode_SequenceAsSeparateHint), false).toBool();
                        bool ok = DocumentReadingModeSelectorController::adjustReadingMode(dr, forceReadingOptions, optionsAlreadyChosen);
                        if (!ok) {
                            continue;
                        }
                        bool documentProcessingFinished = processHints(dr);
                        if (documentProcessingFinished) {
                            continue;
                        }
                        AD2P_DocumentInfo info;
                        if (hints.value(ProjectLoaderHint_LoadWithoutView, false).toBool() == true) {
                            info.openView = false;
                        } else {
                            info.openView = nViews++ < OpenViewTask::MAX_DOC_NUMBER_TO_OPEN_VIEWS;
                        }
                        info.loadDocuments = hints.value(ProjectLoaderHint_LoadUnloadedDocument, true).toBool();
                        info.url = url;
                        info.hints = dr.rawDataCheckResult.properties;
                        if (!info.hints.contains(DocumentReadingMode_MaxObjectsInDoc)) {
                            info.hints[DocumentReadingMode_MaxObjectsInDoc] = getMaxObjectsInSingleDocument();
                        }
                        info.formatId = dr.format->getFormatId();
                        info.iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
                        docsInfo << info;
                    } else {
                        assert(dr.importer != nullptr);
                        AD2P_ProviderInfo info;
                        if (hints.value(ProjectLoaderHint_LoadWithoutView, false).toBool() == true) {
                            info.openView = false;
                        } else {
                            info.openView = nViews++ < OpenViewTask::MAX_DOC_NUMBER_TO_OPEN_VIEWS;
                        }
                        QVariantMap _hints = dr.rawDataCheckResult.properties;
                        info.dp = dr.importer->createImportTask(dr, true, _hints);
                        docProviders << info;
                    }
                }
            } else {
                if (hints.value(ProjectLoaderHint_OpenBySystemIfFormatDetectionFailed, false).toBool()) {
                    QDesktopServices::openUrl(QUrl(url.getURLString(), QUrl::TolerantMode));
                } else {
                    QString message = tr("Failed to detect file format: %1").arg(url.getURLString());
                    QFileInfo finfo(url.getURLString());
                    if (finfo.exists() && finfo.size() == 0) {
                        message = tr("File is empty: %1").arg(url.getURLString());
                    }
                    coreLog.error(message);
                    QMessageBox::critical(AppContext::getMainWindow()->getQMainWindow(), L10N::errorTitle(), message);
                }
            }
        }
    }
    if (docsInfo.isEmpty() && docProviders.isEmpty()) {
        return NULL;
    }

    prepareDocTab(docsInfo, docProviders);
    return new AddDocumentsToProjectTask(docsInfo, docProviders);
}

bool ProjectLoaderImpl::processHints(FormatDetectionResult &dr) {
    bool alignAsShortReads = dr.rawDataCheckResult.properties.value(DocumentReadingMode_SequenceAsShortReadsHint).toBool();
    if (alignAsShortReads) {
        DnaAssemblyGUIUtils::runAssembly2ReferenceDialog(QStringList() << dr.url.getURLString());
        return true;
    }
    return false;
}

Task *ProjectLoaderImpl::createNewProjectTask(const GUrl &url) {
    return createProjectLoadingTask(url);
}

Task *ProjectLoaderImpl::createProjectLoadingTask(const GUrl &url, const QVariantMap &hints) {
    Project *p = AppContext::getProject();
    if (p == NULL) {
        return new OpenProjectTask(url.getURLString());
    }
    if (url == p->getProjectURL()) {
        QString message = tr("Project is already opened");
        QMessageBox::critical(AppContext::getMainWindow()->getQMainWindow(), "UGENE", message);
        return NULL;
    }
    QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(AppContext::getMainWindow()->getQMainWindow());
    msgBox->setWindowTitle(U2_APP_TITLE);
    msgBox->setText(tr("New project can either be opened in a new window or replace the project in the existing. How would you like to open the project?"));
    QPushButton *newWindow = msgBox->addButton(tr("New Window"), QMessageBox::ActionRole);
    newWindow->setObjectName("New Window");
    QPushButton *oldWindow = msgBox->addButton(tr("This Window"), QMessageBox::ActionRole);
    oldWindow->setObjectName("This Window");
    msgBox->addButton(QMessageBox::Abort);
    msgBox->exec();
    CHECK(!msgBox.isNull(), NULL);

    if (msgBox->clickedButton() == newWindow) {
        QStringList params = CMDLineRegistryUtils::getPureValues(0);
        params.append("--" + CMDLineCoreOptions::INI_FILE + "=" + AppContext::getSettings()->fileName());
        bool b = QProcess::startDetached(params.first(), QStringList() << url.getURLString() << params[1]);
        if (!b) {
            coreLog.error(tr("Failed to open new instance of UGENE"));
        }
        return NULL;
    } else if (msgBox->clickedButton() == oldWindow) {
        bool closeActiveProject = hints.value(ProjectLoaderHint_CloseActiveProject, QVariant::fromValue(false)).toBool();
        if (!closeActiveProject) {
            coreLog.error(tr("Stopped loading project: %1. Reason: active project found").arg(url.getURLString()));
            return NULL;
        }
    } else {
        return NULL;
    }
    return new OpenProjectTask(url.getURLString());
}

void ProjectLoaderImpl::sl_projectURLChanged(const QString &oldURL) {
    if (!oldURL.isEmpty()) {
        prependToRecentProjects(oldURL);
    }
    rememberProjectURL();
}

void ProjectLoaderImpl::rememberProjectURL() {
    Project *p = AppContext::getProject();
    QString url = p == NULL ? QString() : p->getProjectURL();
    if (!url.isEmpty()) {
        prependToRecentProjects(url);
    }
    updateRecentProjectsMenu();
}

void ProjectLoaderImpl::sl_serviceStateChanged(Service *s, ServiceState prevState) {
    Q_UNUSED(prevState);

    if (s->getType() != Service_Project) {
        return;
    }
    if (s->isEnabled()) {
        Project *p = AppContext::getProject();
        connect(p, SIGNAL(si_projectURLChanged(const QString &)), SLOT(sl_projectURLChanged(const QString &)));
        connect(p, SIGNAL(si_documentAdded(Document *)), SLOT(sl_documentAdded(Document *)));
    }
    rememberProjectURL();
    updateState();
}

QString ProjectLoaderImpl::getLastProjectURL() {
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, QStringList(), true).toStringList();
    if (!recentFiles.isEmpty()) {
        return recentFiles.first();
    }
    return QString();
}

void ProjectLoaderImpl::prependToRecentItems(const QString &url) {
    SAFE_POINT(!url.isEmpty(), "Invalid URL string!", );
    CHECK(GUrl(url).isLocalFile(), );
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_ITEMS_SETTINGS_NAME, QStringList(), true).toStringList();
    recentFiles.removeAll(url);
    recentFiles.prepend(url);
    while (recentFiles.size() > MAX_RECENT_FILES) {
        recentFiles.pop_back();
    }
    AppContext::getSettings()->setValue(SETTINGS_DIR + RECENT_ITEMS_SETTINGS_NAME, recentFiles, true);
    emit si_recentListChanged();
}

// QT 4.5.0 bug workaround
void ProjectLoaderImpl::sl_updateRecentItemsMenu() {
    updateRecentItemsMenu();
}

void ProjectLoaderImpl::updateRecentItemsMenu() {
    assert(recentItemsMenu != NULL);
    recentItemsMenu->clear();
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_ITEMS_SETTINGS_NAME, QStringList(), true).toStringList();
    recentItemsMenu->menuAction()->setEnabled(!recentFiles.isEmpty());
    Project *p = AppContext::getProject();
    foreach (QString f, recentFiles) {
        if ((p == NULL || f != p->getProjectURL()) && !f.isEmpty()) {
            QAction *a = recentItemsMenu->addAction(f, this, SLOT(sl_openRecentFile()));
            a->setData(f);
            a->setDisabled(!QFile::exists(f));
        }
    }
}

void ProjectLoaderImpl::sl_paste() {
    PasteFactory *pasteFactory = AppContext::getPasteFactory();
    SAFE_POINT(pasteFactory != nullptr, "PasteFactory is null", );

    PasteTask *task = pasteFactory->createPasteTask(true);
    CHECK(task != nullptr,);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void ProjectLoaderImpl::sl_documentAdded(Document *doc) {
    bool doNotAddToRecent = doc->getGHints()->get(ProjectLoaderHint_DoNotAddToRecentDocuments, false).toBool();
    if (!doc->isModified()) {
        if (!doNotAddToRecent) {
            prependToRecentItems(doc->getURLString());
            updateRecentItemsMenu();
        }
    } else {
        connect(doc, SIGNAL(si_modifiedStateChanged()), SLOT(sl_documentStateChanged()));
    }
    doc->getGHints()->remove(ProjectLoaderHint_DoNotAddToRecentDocuments);
}

void ProjectLoaderImpl::sl_documentStateChanged() {
    Document *doc = qobject_cast<Document *>(QObject::sender());
    if (doc != NULL) {
        if (!doc->isModified()) {
            prependToRecentItems(doc->getURLString());
            updateRecentItemsMenu();
        }
    }
}

void ProjectLoaderImpl::sl_newDocumentFromText() {
    QWidget *p = (QWidget *)AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<CreateDocumentFromTextDialogController> dialog = new CreateDocumentFromTextDialogController(p);
    dialog->exec();
}

void ProjectLoaderImpl::sl_downloadRemoteFile() {
    QWidget *p = (QWidget *)(AppContext::getMainWindow()->getQMainWindow());
    QObjectScopedPointer<DownloadRemoteFileDialog> dlg = new DownloadRemoteFileDialog(p);
    dlg->exec();
}

void ProjectLoaderImpl::sl_accessSharedDatabase() {
    QWidget *p = (QWidget *)(AppContext::getMainWindow()->getQMainWindow());
    QObjectScopedPointer<SharedConnectionsDialog> dlg = new SharedConnectionsDialog(p);
    dlg->exec();
}

void ProjectLoaderImpl::sl_searchGenbankEntry() {
    QWidget *p = (QWidget *)(AppContext::getMainWindow()->getQMainWindow());
    QObjectScopedPointer<SearchGenbankSequenceDialogController> dlg = new SearchGenbankSequenceDialogController(p);
    dlg->exec();
}

//////////////////////////////////////////////////////////////////////////
//WelcomePageActions
//////////////////////////////////////////////////////////////////////////
LoadDataWelcomePageAction::LoadDataWelcomePageAction(ProjectLoaderImpl *loader)
    : WelcomePageAction(BaseWelcomePageActions::LOAD_DATA), loader(loader) {
}

void LoadDataWelcomePageAction::perform() {
    SAFE_POINT(!loader.isNull(), L10N::nullPointerError("Project Loader"), );
    loader->sl_openProject();
}

CreateSequenceWelcomePageAction::CreateSequenceWelcomePageAction(ProjectLoaderImpl *loader)
    : WelcomePageAction(BaseWelcomePageActions::CREATE_SEQUENCE), loader(loader) {
}

void CreateSequenceWelcomePageAction::perform() {
    SAFE_POINT(!loader.isNull(), L10N::nullPointerError("Project Loader"), );
    loader->sl_newDocumentFromText();
}

//////////////////////////////////////////////////////////////////////////
//SaveProjectDialogController
//////////////////////////////////////////////////////////////////////////

SaveProjectDialogController::SaveProjectDialogController(QWidget *w)
    : QDialog(w) {
    setupUi(this);
    setModal(true);
    buttonBox->button(QDialogButtonBox::Yes)->setText(tr("Yes"));
    buttonBox->button(QDialogButtonBox::No)->setText(tr("No"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(sl_clicked(QAbstractButton *)));
}

void SaveProjectDialogController::sl_clicked(QAbstractButton *button) {
    done(buttonBox->standardButton(button));
}

//////////////////////////////////////////////////////////////////////////
//ProjectDialogController
//////////////////////////////////////////////////////////////////////////
ProjectDialogController::ProjectDialogController(ProjectDialogController::Mode m, QWidget *p)
    : QDialog(p) {
    setupUi(this);
    new HelpButton(this, buttonBox, "46499627");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Create"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    createButton = buttonBox->button(QDialogButtonBox::Ok);
    setModal(true);
    fileEditIsEmpty = false;

    if (m == Save_Project) {
        setWindowTitle(ProjectLoaderImpl::tr("Save project as"));
        createButton->setText(ProjectLoaderImpl::tr("Save"));
        projectNameEdit->setText(AppContext::getProject()->getProjectName());
        QString url = AppContext::getProject()->getProjectURL();
        if (!url.isEmpty()) {
            QFileInfo fi(url);
            projectFilePathEdit->setText(fi.absoluteFilePath());
        } else {
            setupDefaults();
        }
    } else {
        setupDefaults();
    }
    //projectFolderEdit->setReadOnly(true);
    if (projectFilePathEdit->text().isEmpty()) {
        fileEditIsEmpty = true;
    }
    connect(fileSelectButton, SIGNAL(clicked()), SLOT(sl_fileSelectClicked()));
    connect(projectFilePathEdit, SIGNAL(textEdited(const QString &)), SLOT(sl_fileNameEdited(const QString &)));
    connect(projectNameEdit, SIGNAL(textEdited(const QString &)), SLOT(sl_projectNameEdited(const QString &)));
    updateState();
}

void ProjectDialogController::updateState() {
    bool ready = true;

    const QString &file = projectFilePathEdit->text();
    const QString &name = projectNameEdit->text();

    //todo: improve check
    if (file.isEmpty() || name.isEmpty()) {
        ready = false;
    }

    createButton->setEnabled(ready);
}

void ProjectDialogController::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (event->modifiers() == Qt::NoModifier && (key == Qt::Key_Enter || key == Qt::Key_Return)) {
        createButton->animateClick();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void ProjectDialogController::sl_fileSelectClicked() {
    QString filepath = U2FileDialog::getSaveFileName(this, tr("Save file"), AppContext::getSettings()->getValue(SETTINGS_DIR + "last_dir").toString(), tr("Project files") + DIALOG_FILTER_PROJECT_EXT);
    if (filepath.isEmpty())
        return;
    projectFilePathEdit->setText(filepath);
    updateState();
}

void ProjectDialogController::sl_fileNameEdited(const QString &) {
    //TODO: warn about overwrite
    fileEditIsEmpty = false;
    updateState();
}

void ProjectDialogController::sl_projectNameEdited(const QString &text) {
    if (fileEditIsEmpty) {
        projectFilePathEdit->setText(text);
    }
    updateState();
}

void ProjectDialogController::setupDefaults() {
    projectNameEdit->setText(ProjectLoaderImpl::tr("New Project"));
    projectFilePathEdit->setText(QFileInfo("project" + PROJECTFILE_EXT).absoluteFilePath());
}

void ProjectDialogController::accept() {
    QString projectPath = getProjectFilePathFromPathEdit(projectFilePathEdit);
    if (QFileInfo(projectPath).exists()) {
        int rc = QMessageBox::question(this, windowTitle(), tr("<html><body align=\"center\"><br>Project file already exists.<br>Are you sure you want to overwrite it?<body></html>"), QMessageBox::Yes, QMessageBox::No);
        if (rc != QMessageBox::Yes) {
            return;
        }
    }
    QDialog::accept();
}

Project *ProjectLoaderImpl::createProject(const QString &name, const QString &url, QList<Document *> &documents, QList<GObjectViewState *> &states) {
    ProjectImpl *pi = new ProjectImpl(name, url, documents, states);
    return pi;
}

void ProjectLoaderImpl::sl_onAddExistingDocument() {
    LastUsedDirHelper h;
    QString filter = DialogUtils::prepareDocumentsFileFilter(true);
    QString file = U2FileDialog::getOpenFileName(NULL, tr("Select files to open"), h.dir, filter);
    if (file.isEmpty()) {
        return;
    }
    if (QFileInfo(file).exists()) {
        h.url = file;
    }
    QList<GUrl> urls;
    urls << GUrl(file, GUrl_File);
    QVariantMap hints;
    hints[ProjectLoaderHint_ForceFormatOptions] = true;
    Task *openTask = AppContext::getProjectLoader()->openWithProjectTask(urls, hints);
    if (openTask != NULL) {
        AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
    }
}

//////////////////////////////////////////////////////////////////////////
// Add documents to project task

AddDocumentsToProjectTask::AddDocumentsToProjectTask(const QList<AD2P_DocumentInfo> &_docsInfo, const QList<AD2P_ProviderInfo> &_provInfo)
    : Task(tr("Loading documents"), TaskFlags_NR_FOSE_COSC | TaskFlag_CollectChildrenWarnings), docsInfo(_docsInfo), providersInfo(_provInfo), loadTasksAdded(false) {
    setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS_AUTO);

    Project *p = AppContext::getProject();
    if (!p) {
        // create anonymous project
        Task *rpt = AppContext::getProjectLoader()->createNewProjectTask();
        rpt->setSubtaskProgressWeight(0);
        addSubTask(rpt);
    } else {
        QList<Task *> tasks = prepareLoadTasks();
        foreach (Task *t, tasks) {
            addSubTask(t);
        }
        loadTasksAdded = true;
    }
}

AddDocumentsToProjectTask::~AddDocumentsToProjectTask() {
    if (!loadTasksAdded) {
        foreach (const AD2P_ProviderInfo &info, providersInfo) {
            delete info.dp;
        }
    }
}

QList<Task *> AddDocumentsToProjectTask::onSubTaskFinished(Task *t) {
    QList<Task *> res;
    if (!loadTasksAdded) {
        res = prepareLoadTasks();
        loadTasksAdded = true;
    } else if (t->hasError()) {
        coreLog.error(t->getError());
    } else if (t->hasWarning()) {
        setReportingSupported(true);
        setReportingEnabled(true);
    }
    foreach (Document *d, docsToMarkAsModified) {
        if (d->isLoaded() && !d->isModified()) {
            d->setModified(true);
            docsToMarkAsModified.removeOne(d);
        }
    }
    return res;
}

QString AddDocumentsToProjectTask::generateReport() const {
    SAFE_POINT(stateInfo.hasWarnings(), L10N::internalError("No warnings to show"), "");
    QString warnings = stateInfo.getWarnings().join("<br>");
    warnings.replace("\n", "<br>");
    return warnings;
}

const QList<AD2P_DocumentInfo> &AddDocumentsToProjectTask::getDocsInfoList() const {
    return docsInfo;
}

QList<Task *> AddDocumentsToProjectTask::prepareLoadTasks() {
    QList<Task *> res;

    Project *p = AppContext::getProject();
    SAFE_POINT(p != NULL, tr("No active project found!"), res);

    foreach (const AD2P_DocumentInfo &info, docsInfo) {
        Document *doc = p->findDocumentByURL(info.url);
        bool unsupportedObjectType = false;
        if (doc == NULL) {
            DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(info.formatId);
            GObjectType t = df->getSupportedObjectTypes().toList().first();
            if (GObjectTypes::getTypeInfo(t).type == GObjectTypes::UNKNOWN) {
                unsupportedObjectType = true;
            }
            U2OpStatus2Log os;
            doc = df->createNewUnloadedDocument(info.iof, info.url, os, info.hints);
            if (doc == NULL) {
                continue;
            }
            if (info.markLoadedAsModified) {
                docsToMarkAsModified << doc;
            }
        }
        if (unsupportedObjectType) {
            if (info.openView) {
                res << new LoadUnloadedDocumentAndOpenViewTask(doc);
            } else {
                coreLog.trace(QString("View limit exceed for the document: %1").arg(info.url.getURLString()));
                delete doc;
            }
        } else {
            if (info.openView) {
                res << new AddDocumentAndOpenViewTask(doc);
            } else {
                Task *addDocTask = new AddDocumentTask(doc);
                if (info.loadDocuments) {
                    QList<Task *> tasks;
                    tasks << addDocTask;
                    tasks << new LoadUnloadedDocumentTask(doc);
                    SequentialMultiTask *multiTask = new SequentialMultiTask(tr("Load document and add to project: %1").arg(doc->getName()), tasks);
                    res << multiTask;
                } else {
                    res << addDocTask;
                }
            }
        }
    }

    AddDocumentTaskConfig conf;
    conf.unloadExistingDocument = true;    // -> re-import kills old document version
    foreach (const AD2P_ProviderInfo &info, providersInfo) {
        if (info.openView) {
            res << new AddDocumentAndOpenViewTask(info.dp, conf);
        } else {
            res << new AddDocumentTask(info.dp, conf);
        }
    }

    return res;
}

OpenWithProjectTask::OpenWithProjectTask(const QStringList &_urls)
    : Task(tr(""), TaskFlags_NR_FOSCOE) {
    foreach (const QString &u, _urls) {
        urls << GUrl(u);
    }

    if (urls.size() == 1) {
        setTaskName(tr("Opening document: %1").arg(urls.first().getURLString()));
    } else {
        setTaskName(tr("Opening %1 documents").arg(urls.size()));
    }
}

void OpenWithProjectTask::prepare() {
    Task *t = AppContext::getProjectLoader()->openWithProjectTask(urls);
    if (t != NULL) {
        addSubTask(t);
    }
}

}    // namespace U2
