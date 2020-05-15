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

#ifndef _U2_PROJECT_SUPPORT_H_
#define _U2_PROJECT_SUPPORT_H_

#include <assert.h>

#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>

#include <U2Gui/WelcomePageAction.h>

#include "ui_CreateNewProjectWidget.h"
#include "ui_SaveProjectDialog.h"

namespace U2 {

#define SETTINGS_DIR QString("project_loader/")
#define RECENT_ITEMS_SETTINGS_NAME "recentItems"
#define RECENT_PROJECTS_SETTINGS_NAME "recentProjects"

class DocumentProviderTask;
class FormatDetectionResult;

class ProjectLoaderImpl : public ProjectLoader {
    Q_OBJECT
public:
    ProjectLoaderImpl();

    virtual Task *openWithProjectTask(const QList<GUrl> &urls, const QVariantMap &hints = QVariantMap());

    virtual Task *createNewProjectTask(const GUrl &url = GUrl());

    virtual Task *createProjectLoadingTask(const GUrl &url, const QVariantMap &hints = QVariantMap());

    virtual Project *createProject(const QString &name, const QString &url, QList<Document *> &documents, QList<GObjectViewState *> &states);

    virtual QAction *getAddExistingDocumentAction() {
        return addExistingDocumentAction;
    }

    static QString getLastProjectURL();
    static const int maxObjectsInSingleDocument = 50000;
    static bool detectFormat(const GUrl &url, QList<FormatDetectionResult> &formats, const QVariantMap &hints, FormatDetectionResult &selectedResult);
    static bool shouldFormatBeSelected(const QList<FormatDetectionResult> &formats, bool forceSelectFormat);
    static bool processHints(FormatDetectionResult &dr);

signals:
    void si_recentListChanged();

public slots:
    void sl_newDocumentFromText();
    void sl_openProject();

private:
    void updateState();
    void updateRecentProjectsMenu();
    void prependToRecentProjects(const QString &pFile);
    void updateRecentItemsMenu();
    void prependToRecentItems(const QString &url);
    void rememberProjectURL();

private slots:
    void sl_newProject();
    void sl_openRecentFile();
    void sl_openRecentProject();
    void sl_serviceStateChanged(Service *s, ServiceState prevState);
    void sl_documentAdded(Document *doc);
    void sl_paste();
    void sl_documentStateChanged();
    void sl_projectURLChanged(const QString &oldURL);
    void sl_onAddExistingDocument();

    void sl_downloadRemoteFile();
    void sl_accessSharedDatabase();
    void sl_searchGenbankEntry();

    // QT 4.5.0 bug workaround
    void sl_updateRecentItemsMenu();

private:
    QAction *addExistingDocumentAction;
    QAction *newProjectAction;
    QAction *openProjectAction;
    QAction *pasteAction;
    QAction *downloadRemoteFileAction;
    QAction *accessSharedDatabaseAction;
    QAction *searchGenbankEntryAction;
    QAction *newDocumentFromtext;

    QMenu *recentProjectsMenu;
    QMenu *recentItemsMenu;
};

//////////////////////////////////////////////////////////////////////////
/// WelcomePageActions

class LoadDataWelcomePageAction : public WelcomePageAction {
public:
    LoadDataWelcomePageAction(ProjectLoaderImpl *loader);
    void perform();

private:
    QPointer<ProjectLoaderImpl> loader;
};

class CreateSequenceWelcomePageAction : public WelcomePageAction {
public:
    CreateSequenceWelcomePageAction(ProjectLoaderImpl *loader);
    void perform();

private:
    QPointer<ProjectLoaderImpl> loader;
};

//////////////////////////////////////////////////////////////////////////
/// Dialogs

//TODO: merge project dir & project name fields

class SaveProjectDialogController : public QDialog, public Ui_SaveProjectDialog {
    Q_OBJECT
public:
    SaveProjectDialogController(QWidget *p);
public slots:
    void sl_clicked(QAbstractButton *button);
};

class ProjectDialogController : public QDialog, public Ui_CreateNewProjectDialog {
    Q_OBJECT
public:
    enum Mode { New_Project,
                Save_Project };
    ProjectDialogController(Mode m, QWidget *p);

    void accept();
    void updateState();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void sl_fileSelectClicked();
    void sl_fileNameEdited(const QString &);
    void sl_projectNameEdited(const QString &);

private:
    void setupDefaults();
    bool fileEditIsEmpty;
    QPushButton *createButton;
};

class AD2P_DocumentInfo {
public:
    AD2P_DocumentInfo()
        : iof(NULL), openView(false), loadDocuments(false), markLoadedAsModified(false) {
    }
    GUrl url;
    DocumentFormatId formatId;
    IOAdapterFactory *iof;
    QVariantMap hints;
    bool openView;
    bool loadDocuments;
    bool markLoadedAsModified;
};

class AD2P_ProviderInfo {
public:
    AD2P_ProviderInfo()
        : dp(NULL), openView(false) {
    }
    DocumentProviderTask *dp;
    bool openView;
};

class AddDocumentsToProjectTask : public Task {
    Q_OBJECT
public:
    AddDocumentsToProjectTask(const QList<AD2P_DocumentInfo> &docsInfo, const QList<AD2P_ProviderInfo> &providersInfo);
    ~AddDocumentsToProjectTask();

    virtual QList<Task *> onSubTaskFinished(Task *subTask);
    virtual QString generateReport() const;
    const QList<AD2P_DocumentInfo> &getDocsInfoList() const;

private:
    QList<Task *> prepareLoadTasks();
    QList<Document *> docsToMarkAsModified;

    QList<AD2P_DocumentInfo> docsInfo;
    QList<AD2P_ProviderInfo> providersInfo;
    bool loadTasksAdded;
};

class OpenWithProjectTask : public Task {
    Q_OBJECT
public:
    OpenWithProjectTask(const QStringList &urls);
    void prepare();

private:
    QList<GUrl> urls;
};

}    // namespace U2
#endif
