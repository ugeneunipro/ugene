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

#ifndef _U2_PROJECT_MODEL_H_
#define _U2_PROJECT_MODEL_H_

#include <QAction>

#include "DocumentModel.h"
#include "StateLockableDataModel.h"

#define CURRENT_PROJECT_VERSION "1.0"

namespace U2 {

class GObjectViewState;
class Document;
class Project;
class Task;
class U2OpStatus;

#define PROJECT_FILE_PURE_EXT QString("uprj")
#define PROJECTFILE_EXT QString("." + PROJECT_FILE_PURE_EXT)
#define DIALOG_FILTER_PROJECT_EXT QString(" (*" + PROJECTFILE_EXT + ")")

//data loader hints
#define ProjectLoaderHint_CloseActiveProject "close-active-project"
#define ProjectLoaderHint_ForceFormatOptions "force-format-options"
#define ProjectLoaderHint_LoadWithoutView "load-without-view"
#define ProjectLoaderHint_LoadUnloadedDocument "load-unloaded-document"
#define ProjectLoaderHint_UseImporters "use-importers"
#define ProjectLoaderHint_MultipleFilesMode_URLDocument "multiple-files-mode-url-document"
#define ProjectLoaderHint_MultipleFilesMode_URLsDocumentConsistOf "multiple-files-mode-urls-document-consist-of"
#define ProjectLoaderHint_MultipleFilesMode_SaveDocumentFlag "multiple-files-mode-save-document-flag"
#define ProjectLoaderHint_MultipleFilesMode_Flag "multiple-files-mode-flag"
#define ProjectLoaderHint_MultipleFilesMode_RealDocumentFormat "multiple-files-mode-real-document-format"
#define ProjectLoaderHint_MergeMode_DifferentAlphabets "merge-sequences-different-alphabets"
#define ProjectLoaderHint_DontCheckForExistence "dont-check-for-existence"
#define ProjectLoaderHint_OpenBySystemIfFormatDetectionFailed "open-by-system-if-format-detection-failed"
#define ProjectLoaderHint_DoNotAddToRecentDocuments "do-not-add-to-recent-documents"

/// Service responsible for project loading / unloading
class U2CORE_EXPORT ProjectLoader : public QObject {
public:
    /**
        Opens files and adds them to the current project. If project does not exists - creates anonymous one
        If the file is project file - loads it.
    */
    virtual Task *openWithProjectTask(const QList<GUrl> &urls, const QVariantMap &hints = QVariantMap()) = 0;

    /** Creates new project. If URL is empty the project created is anonymous */
    virtual Task *createNewProjectTask(const GUrl &url = GUrl()) = 0;

    /** Loads project from the specified location */
    virtual Task *createProjectLoadingTask(const GUrl &url, const QVariantMap &hints = QVariantMap()) = 0;

    /** Creates new project instance */
    virtual Project *createProject(const QString &name, const QString &url, QList<Document *> &documents, QList<GObjectViewState *> &states) = 0;

    virtual QAction *getAddExistingDocumentAction() = 0;

    // This function can return NULL, don't forget to check the result.
    Task *openWithProjectTask(const GUrl &url, const QVariantMap &hints = QVariantMap()) {
        QList<GUrl> urls;
        urls << url;
        return openWithProjectTask(urls, hints);
    };
};

/// Project model
class U2CORE_EXPORT Project : public StateLockableTreeItem {
    Q_OBJECT
    Q_PROPERTY(QString name WRITE setProjectName READ getProjectName)
    Q_PROPERTY(QString url WRITE setProjectURL READ getProjectURL)
    Q_PROPERTY(QList<Document *> docs READ getDocuments)

public:
    virtual ~Project();

    virtual const QString &getProjectName() const = 0;

    virtual void setProjectName(const QString &name) = 0;

    virtual const QString &getProjectURL() const = 0;

    virtual void setProjectURL(const QString &) = 0;

    virtual const QList<Document *> &getDocuments() const = 0;

    Q_INVOKABLE virtual void addDocument(Document *d) = 0;

    Q_INVOKABLE virtual void removeDocument(Document *d, bool autodelete = true) = 0;

    Q_INVOKABLE virtual Document *findDocumentByURL(const QString &url) const = 0;

    Q_INVOKABLE virtual Document *findDocumentByURL(const GUrl &url) const = 0;

    virtual bool lockResources(int sizeMB, const QString &url, QString &error) = 0;

    virtual const QList<GObjectViewState *> &getGObjectViewStates() const = 0;

    virtual void addGObjectViewState(GObjectViewState *s) = 0;

    virtual void removeGObjectViewState(GObjectViewState *s) = 0;

    virtual void makeClean() = 0;

    virtual quint64 getObjectIdCounter() const = 0;

    virtual void setObjectIdCounter(quint64 c) = 0;

    static void setupToEngine(QScriptEngine *engine);

    virtual void removeRelations(const QString &docUrl) = 0;

    virtual void updateDocInRelations(const QString &oldDocUrl, const QString &newDocUrl) = 0;

private:
    static QScriptValue toScriptValue(QScriptEngine *engine, Project *const &in);
    static void fromScriptValue(const QScriptValue &object, Project *&out);
signals:
    void si_projectURLChanged(const QString &oldURL);

    void si_projectRenamed(Project *p);

    void si_documentAdded(Document *d);

    void si_documentRemoved(Document *d);

    void si_objectViewStateAdded(GObjectViewState *);

    void si_objectViewStateRemoved(GObjectViewState *);
};

class DocumentSerialState {
};

}    // namespace U2
Q_DECLARE_METATYPE(U2::Project *)
Q_DECLARE_METATYPE(QList<U2::Document *>)

#endif
