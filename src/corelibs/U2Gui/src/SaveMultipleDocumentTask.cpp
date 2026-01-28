/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "SaveMultipleDocumentTask.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/U2FileDialog.h>

#include <QApplication>
#include <QMessageBox>

namespace U2 {

SaveMultipleDocuments::SaveMultipleDocuments(const QList<Document*>& docs, bool askBeforeSave, SavedNewDocFlag saveAndOpenFlag)
    : Task(tr("Save multiple documents"), TaskFlags_NR_FOSE_COSC) {
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
            if (!FileAndDirectoryUtils::isNoWritePermission(url)) {
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
        QObjectScopedPointer<QMessageBox> msgBox(new QMessageBox);
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setWindowTitle(U2_APP_TITLE);

        msgBox->setText(tr("You have no permission to write to '%1' file.\nUGENE contains unsaved modifications.").arg(doc->getURL().fileName()));
        msgBox->setInformativeText(tr("Do you want to save changes to another file?"));

        QPushButton* saveButton = msgBox->addButton(QMessageBox::Save);
        msgBox->addButton(QMessageBox::Cancel);
        msgBox->setDefaultButton(saveButton);
        msgBox->setObjectName("permissionBox");
        int res = msgBox->exec();

        if (res == QMessageBox::Save) {
            QString newFileUrl = GUrlUtils::rollFileName(doc->getURLString(), "_modified_", DocumentUtils::getNewDocFileNameExcludesHint());
            QString saveFileFilter = doc->getDocumentFormat()->getSupportedDocumentFileExtensions().join(" *.").prepend("*.");
            auto activeWindow = qobject_cast<QWidget*>(QApplication::activeWindow());
            QString fileName = U2FileDialog::getSaveFileName(activeWindow, tr("Save as"), newFileUrl, saveFileFilter);
            if (isOsMac()) {
                activeWindow->activateWindow();
            }
            if (!fileName.isEmpty()) {
                url = fileName;
            } else {  // Cancel in "Save as" dialog clicked
                cancel();
                break;
            }
        } else if (res == QMessageBox::Cancel) {
            cancel();
            break;
        } else {
            return GUrl();
        }

    } while (FileAndDirectoryUtils::isNoWritePermission(url));

    return url;
}

}  // namespace U2
