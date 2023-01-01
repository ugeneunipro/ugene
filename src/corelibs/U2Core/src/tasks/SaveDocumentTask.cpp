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

#include "SaveDocumentTask.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

bool isNoWritePermission(GUrl& url) {
    if (!QFile::exists(url.getURLString())) {
        return !FileAndDirectoryUtils::isDirectoryWritable(url.dirPath());
    }
    return !QFile::permissions(url.getURLString()).testFlag(QFile::WriteUser);
}

SaveDocumentTask::SaveDocumentTask(Document* _doc, IOAdapterFactory* _io, const GUrl& _url, SaveDocFlags _flags)
    : Task(tr("Save document"), TaskFlag_None), doc(_doc), iof(_io), url(_url), flags(_flags) {
    assert(doc != nullptr);
    if (iof == nullptr) {
        iof = doc->getIOAdapterFactory();
    }
    if (url.isEmpty()) {
        url = doc->getURLString();
    }
    if (isNoWritePermission(url)) {
        stateInfo.setError(tr("No permission to write to '%1' file.").arg(url.getURLString()));
    }

    lock = nullptr;
}

SaveDocumentTask::SaveDocumentTask(Document* _doc, SaveDocFlags f, const QSet<QString>& _excludeFileNames)
    : Task(tr("Save document"), TaskFlag_None),
      doc(_doc), iof(doc->getIOAdapterFactory()), url(doc->getURL()), flags(f), excludeFileNames(_excludeFileNames) {
    assert(doc != nullptr);

    if (isNoWritePermission(url)) {
        stateInfo.setError(tr("No permission to write to '%1' file.").arg(url.getURLString()));
    }
}

void SaveDocumentTask::addFlag(SaveDocFlag f) {
    flags |= f;
}

void SaveDocumentTask::prepare() {
    if (doc.isNull()) {
        setError(tr("Document was removed"));
        return;
    }
    lock = new StateLock(getTaskName(), StateLockFlag_LiveLock);
    doc->lockState(lock);
}

void SaveDocumentTask::run() {
    if (flags.testFlag(SaveDoc_Roll) && !GUrlUtils::renameFileWithNameRoll(url.getURLString(), stateInfo, excludeFileNames, &coreLog)) {
        return;
    }

    const QString message = tr("Saving document %1\n").arg(url.getURLString());
    if (flags.testFlag(SaveDoc_ReduceLoggingLevel)) {
        coreLog.details(message);
    } else {
        coreLog.info(message);
    }
    DocumentFormat* df = doc->getDocumentFormat();

    QString originalFilePath = url.getURLString();
    QFile originalFile(originalFilePath);

    // If we do not check for 0 size here XML tests (cmd-line/bedtools) fail on Windows.
    // TODO: understand &  document why this size() != 0 is needed here for Windows.
    bool isOriginalFileExist = url.isLocalFile() && originalFile.exists() && originalFile.size() != 0;

    if (isOriginalFileExist && df->checkFlags(DocumentFormatFlag_DirectWriteOperations)) {
        // Changes are already applied, the file shouldn't be saved
        coreLog.trace(QString("Document with 'direct write operations' flag saving: "
                              "file '%1' exists, all changes are already applied, finishing the task")
                          .arg(url.getURLString()));
        return;
    }

    if (isOriginalFileExist) {  // The file exist and is local.
        coreLog.trace(QString("Local file '%1' already exists, going to overwrite it").arg(url.getURLString()));

        // make tmp file
        QString tmpFileName = GUrlUtils::prepareTmpFileLocation(url.dirPath(), url.fileName(), "tmp", stateInfo);

        QFile tmpFile(tmpFileName);
        bool created = tmpFile.open(QIODevice::WriteOnly);
        tmpFile.close();
        CHECK_EXT(created == true, stateInfo.setError(tr("Can't create tmp file")), );

        if (flags.testFlag(SaveDoc_Append)) {
            QFile::remove(tmpFileName);
            bool copied = QFile::copy(originalFilePath, tmpFileName);
            CHECK_EXT(copied == true, stateInfo.setError(tr("Can't copy file to tmp file while trying to save document by append")), );
        }

        // save document to tmp file, QScopedPointer will release file in destructor
        {
            QScopedPointer<IOAdapter> io(IOAdapterUtils::open(GUrl(tmpFileName), stateInfo, flags.testFlag(SaveDoc_Append) ? IOAdapterMode_Append : IOAdapterMode_Write, doc->getIOAdapterFactory()));
            CHECK_OP(stateInfo, );
            df->storeDocument(doc, io.data(), stateInfo);
            CHECK_OP(stateInfo, );
        }

        // remove old file and rename tmp file
        GUrlUtils::removeFile(originalFilePath, stateInfo);
        CHECK_OP(stateInfo, );

        bool isRenamed = QFile::rename(tmpFileName, originalFilePath);
        CHECK_EXT(isRenamed, stateInfo.setError(tr("Can't rename saved tmp file to original file: %1").arg(originalFilePath)), );
    } else {
        coreLog.trace(QString("File '%1' doesn't exist, going to write it directly").arg(url.getURLString()));
        QScopedPointer<IOAdapter> io(IOAdapterUtils::open(url, stateInfo, flags.testFlag(SaveDoc_Append) ? IOAdapterMode_Append : IOAdapterMode_Write, doc->getIOAdapterFactory()));
        CHECK_OP(stateInfo, );
        df->storeDocument(doc, io.data(), stateInfo);
        if (stateInfo.isCoR() && url.isLocalFile()) {
            GUrlUtils::removeFile(originalFilePath, stateInfo);
        }
    }
}

Task::ReportResult SaveDocumentTask::report() {
    if (lock != nullptr) {
        SAFE_POINT(!doc.isNull(), "document is null!", ReportResult_Finished);
        doc->unlockState(lock);
        delete lock;
        lock = nullptr;
    }
    CHECK_OP(stateInfo, ReportResult_Finished);

    if (doc && url == doc->getURL() && iof == doc->getIOAdapterFactory()) {
        doc->makeClean();
    }
    if (doc) {
        doc->setLastUpdateTime();
        doc->getGHints()->remove(ProjectLoaderHint_DontCheckForExistence);
    }
    bool dontUnload = flags.testFlag(SaveDoc_DestroyButDontUnload);
    if (flags.testFlag(SaveDoc_DestroyAfter) || dontUnload) {
        if (!dontUnload) {
            doc->unload();
        }
        CHECK(AppContext::getProject() != nullptr, ReportResult_Finished);
        AppContext::getProject()->removeDocument(doc, true);
    }
    if (flags.testFlag(SaveDoc_UnloadAfter)) {
        if (!doc->unload()) {
            stateInfo.setError(tr("Document '%1' can't be unloaded: ").arg(doc->getName()) + tr("unexpected error"));
            coreLog.error(stateInfo.getError());
        }
    }
    if (flags.testFlag(SaveDoc_OpenAfter)) {
        Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(url, openDocumentWithProjectHints);
        if (nullptr != openTask) {
            AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
        }
    }
    return Task::ReportResult_Finished;
}

/** Returns current set of 'openDocumentWithProjectHints'. See 'openDocumentWithProjectHints' for details. */
QVariantMap SaveDocumentTask::getOpenDocumentWithProjectHints() const {
    return openDocumentWithProjectHints;
}

/** Sets new 'openDocumentWithProjectHints'. See 'openDocumentWithProjectHints' for details. */
void SaveDocumentTask::setOpenDocumentWithProjectHints(const QVariantMap& hints) {
    openDocumentWithProjectHints = hints;
}

//////////////////////////////////////////////////////////////////////////
/// save multiple

SaveMultipleDocuments::SaveMultipleDocuments(const QList<Document*>& docs, bool askBeforeSave, SavedNewDocFlag saveAndOpenFlag)
    : Task(tr("Save multiple documents"), TaskFlag_NoRun) {
    bool saveAll = false;
    foreach (Document* doc, docs) {
        bool save = true;
        if (askBeforeSave) {
            QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes) | QMessageBox::No | QMessageBox::Cancel;
            if (docs.size() > 1) {
                buttons = buttons | QMessageBox::YesToAll | QMessageBox::NoToAll;
            }

            QObjectScopedPointer<QMessageBox> messageBox(new QMessageBox(QMessageBox::Question,
                                                                         tr("Question?"),
                                                                         tr("Save document: %1").arg(doc->getURLString()),
                                                                         buttons,
                                                                         QApplication::activeWindow()));

            int res = saveAll ? QMessageBox::YesToAll : messageBox->exec();

            if (res == QMessageBox::NoToAll) {
                break;
            }
            if (res == QMessageBox::YesToAll) {
                saveAll = true;
            }
            if (res == QMessageBox::No) {
                save = false;
            }
            if (res == QMessageBox::Cancel) {
                cancel();
                break;
            }
        }
        if (save) {
            GUrl url = doc->getURL();
            if (isNoWritePermission(url)) {
                url = chooseAnotherUrl(doc);
                if (!url.isEmpty()) {
                    if (saveAndOpenFlag == SavedNewDoc_Open) {
                        addSubTask(new SaveDocumentTask(doc, doc->getIOAdapterFactory(), url, SaveDocFlags(SaveDoc_DestroyAfter | SaveDoc_OpenAfter)));
                    } else {
                        addSubTask(new SaveDocumentTask(doc, doc->getIOAdapterFactory(), url));
                    }
                }
            } else {
                addSubTask(new SaveDocumentTask(doc));
            }
        }
    }
}

QList<Document*> SaveMultipleDocuments::findModifiedDocuments(const QList<Document*>& docs) {
    QList<Document*> res;
    foreach (Document* doc, docs) {
        if (doc->isTreeItemModified()) {
            res.append(doc);
        }
    }
    return res;
}

GUrl SaveMultipleDocuments::chooseAnotherUrl(Document* doc) {
    GUrl url;
    do {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setWindowTitle(U2_APP_TITLE);

        msgBox->setText(tr("You have no permission to write to '%1' file.\nUGENE contains unsaved modifications.").arg(doc->getURL().fileName()));
        msgBox->setInformativeText(tr("Do you want to save changes to another file?"));

        QPushButton* saveButton = msgBox->addButton(QMessageBox::Save);
        QPushButton* cancelButton = msgBox->addButton(QMessageBox::Cancel);
        msgBox->setDefaultButton(saveButton);
        msgBox->setObjectName("permissionBox");
        msgBox->exec();
        CHECK(!msgBox.isNull(), url);

        if (msgBox->clickedButton() == saveButton) {
            QString newFileUrl = GUrlUtils::rollFileName(doc->getURLString(), "_modified_", DocumentUtils::getNewDocFileNameExcludesHint());
            QString saveFileFilter = doc->getDocumentFormat()->getSupportedDocumentFileExtensions().join(" *.").prepend("*.");
            QWidget* activeWindow = qobject_cast<QWidget*>(QApplication::activeWindow());
            QFileDialog::Options options(qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0 ? QFileDialog::DontUseNativeDialog : 0);
            const QString fileName = QFileDialog::getSaveFileName(activeWindow, tr("Save as"), newFileUrl, saveFileFilter, 0, options);
            if (isOsMac()) {
                activeWindow->activateWindow();
            }
            if (!fileName.isEmpty()) {
                url = fileName;
            } else {  // Cancel in "Save as" dialog clicked
                cancel();
                break;
            }
        } else if (msgBox->clickedButton() == cancelButton) {
            cancel();
            break;
        } else {
            return GUrl();
        }

    } while (isNoWritePermission(url));

    return url;
}

//////////////////////////////////////////////////////////////////////////
// save a copy and add to project
SaveCopyAndAddToProjectTask::SaveCopyAndAddToProjectTask(Document* doc, IOAdapterFactory* iof, const GUrl& _url)
    : Task(tr("Save a copy %1").arg(_url.getURLString()), TaskFlags_NR_FOSCOE), url(_url) {
    origURL = doc->getURL();
    df = doc->getDocumentFormat();
    hints = doc->getGHintsMap();

    saveTask = new SaveDocumentTask(doc, iof, url);
    saveTask->setExcludeFileNames(DocumentUtils::getNewDocFileNameExcludesHint());
    addSubTask(saveTask);

    foreach (GObject* obj, doc->getObjects()) {
        info.append(UnloadedObjectInfo(obj));
    }
}

Task::ReportResult SaveCopyAndAddToProjectTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);
    Project* project = AppContext::getProject();
    CHECK_EXT(project != nullptr, setError(tr("No active project found")), ReportResult_Finished);
    CHECK_EXT(!project->isStateLocked(), setError(tr("Project is locked")), ReportResult_Finished);

    const GUrl& saveUrl = saveTask->getURL();
    if (project->findDocumentByURL(saveUrl)) {
        setError(tr("Document is already added to the project %1").arg(saveUrl.getURLString()));
        return ReportResult_Finished;
    }
    Document* doc = df->createNewUnloadedDocument(saveTask->getIOAdapterFactory(), saveUrl, stateInfo, hints, info);
    CHECK_OP(stateInfo, ReportResult_Finished);
    foreach (GObject* o, doc->getObjects()) {
        GObjectUtils::updateRelationsURL(o, origURL, saveUrl);
    }
    doc->setModified(false);
    project->addDocument(doc);
    return ReportResult_Finished;
}

///////////////////////////////////////////////////////////////////////////
// relocate task

RelocateDocumentTask::RelocateDocumentTask(const GUrl& fu, const GUrl& tu)
    : Task(tr("Relocate document %1 -> %2").arg(fu.getURLString()).arg(tu.getURLString()), TaskFlag_NoRun), fromURL(fu), toURL(tu) {
}

Task::ReportResult RelocateDocumentTask::report() {
    Project* project = AppContext::getProject();
    if (project == nullptr) {
        setError(tr("No active project found"));
        return ReportResult_Finished;
    }
    if (project->isStateLocked()) {
        setError(tr("Project is locked"));
        return ReportResult_Finished;
    }
    Document* fromDocument = project->findDocumentByURL(fromURL);
    if (fromDocument == nullptr) {
        setError(L10N::errorDocumentNotFound(fromURL));
        return ReportResult_Finished;
    }
    if (fromDocument->isLoaded()) {
        setError(tr("Only unloaded objects can be relocated"));
        return ReportResult_Finished;
    }

    fromDocument->setURL(toURL);
    if (fromURL.baseFileName() == fromDocument->getName() || fromURL.fileName() == fromDocument->getName()) {  // if document name is default -> update it too
        fromDocument->setName(toURL.baseFileName());
    }

    // Update relations to the new url.
    const QList<Document*>& projectDocs = project->getDocuments();
    for (Document* projectDoc : qAsConst(projectDocs)) {
        for (GObject* o : qAsConst(projectDoc->getObjects())) {
            GObjectUtils::updateRelationsURL(o, fromURL, toURL);
        }
    }

    return ReportResult_Finished;
}

}  // namespace U2
