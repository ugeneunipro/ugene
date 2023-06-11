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

#include "GTUtilsDocument.h"
#include <api/GTUtils.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QTreeWidgetItem>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/ObjectViewModel.h>

#include "GTGlobals.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"

namespace U2 {
using namespace HI;

const QString GTUtilsDocument::DocumentUnloaded = "Unloaded";

#define GT_CLASS_NAME "GTUtilsDocument"

#define GT_METHOD_NAME "getDocument"
Document* GTUtilsDocument::getDocument(const QString& documentName, const GTGlobals::FindOptions& options) {
    Project* project = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        project = AppContext::getProject();
        if (project == nullptr) {  // Wait up to 'GT_OP_WAIT_MILLIS' before failing.
            continue;
        }
        QList<Document*> documents = project->getDocuments();
        for (Document* document : qAsConst(documents)) {
            if (GTUtils::matchText(documentName, document->getName(), options.matchPolicy)) {
                // Wait, so we 100% sure that document is not only in the project model but on the project view model too.
                GTThread::waitForMainThread();
                return document;
            }
        }
        if (!options.failIfNotFound) {
            return nullptr;
        }
    }
    if (project == nullptr) {
        GT_FAIL("There is no project to check if document is present or not: " + documentName, nullptr);
    }
    GT_FAIL("Document is not found: " + documentName, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkDocument"
void GTUtilsDocument::checkDocument(const QString& documentName, const GObjectViewFactoryId& id) {
    // TODO: rework the whole method to use ProjectTreeView to check if document is available.
    Document* document = getDocument(documentName);
    if (id.isEmpty()) {
        return;
    }
    GObjectViewController* view = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && view == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        view = getDocumentGObjectView(document);
        if (id == DocumentUnloaded) {
            GT_CHECK(view == nullptr, "GObjectView is not for document: " + documentName + ", view id: " + id);
            return;
        }
    }
    GT_CHECK(view != nullptr, "GObjectView is not found for document: " + documentName + ", view id: " + id);
    GObjectViewFactoryId viewFactoryId = view->getFactoryId();
    GT_CHECK(viewFactoryId == id, "View's GObjectViewFactoryId is " + viewFactoryId + ", not " + id + ", document: " + documentName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeDocument"
void GTUtilsDocument::removeDocument(const QString& documentName, GTGlobals::UseMethod method) {
    switch (method) {
        case GTGlobals::UseMouse: {
            GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}, method));
            GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(documentName));
            GTMouseDriver::click(Qt::RightButton);
            break;
        }
        case GTGlobals::UseKey:
        default:
            GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(documentName));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_Delete);
            break;
    }
    GTUtilsTaskTreeView::waitTaskFinished();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDocumentGObjectView"
GObjectViewController* GTUtilsDocument::getDocumentGObjectView(Document* d) {
    GT_CHECK_RESULT(d != nullptr, "Document* is NULL", nullptr);

    QList<GObjectViewController*> gObjectViews = getAllGObjectViews();
    foreach (GObjectViewController* view, gObjectViews) {
        if (view->containsDocumentObjects(d)) {
            return view;
        }
    }

    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isDocumentLoaded"
bool GTUtilsDocument::isDocumentLoaded(const QString& documentName) {
    QModelIndex index = GTUtilsProjectTreeView::findIndex(GTUtilsProjectTreeView::getTreeView(), documentName);
    QString s = index.data().toString();
    return !s.contains("unloaded");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "saveDocument"
void GTUtilsDocument::saveDocument(const QString& documentName) {
    Runnable* popupChooser = new PopupChooser({ACTION_PROJECT__SAVE_DOCUMENT}, GTGlobals::UseMouse);

    GTUtilsDialog::waitForDialog(popupChooser);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(documentName) + QPoint(1, 0));  // dirty hack
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "unloadDocument"
void GTUtilsDocument::unloadDocument(const QString& documentName, bool waitForMessageBox) {
    GT_CHECK_RESULT(isDocumentLoaded(documentName), "Document is not loaded", );

    Runnable* popupChooser = new PopupChooser({ACTION_PROJECT__UNLOAD_SELECTED}, GTGlobals::UseMouse);

    if (waitForMessageBox) {
        MessageBoxDialogFiller* filler = new MessageBoxDialogFiller("Yes");
        GTUtilsDialog::waitForDialog(filler);
    }

    GTUtilsDialog::waitForDialog(popupChooser);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(documentName) + QPoint(1, 0));  // dirty hack
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "loadDocument"
void GTUtilsDocument::loadDocument(const QString& documentName) {
    GT_CHECK_RESULT(!isDocumentLoaded(documentName), "Document is loaded", );

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(documentName) + QPoint(1, 0));  // dirty hack
    GTMouseDriver::doubleClick();

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "lockDocument"
void GTUtilsDocument::lockDocument(const QString& documentName) {
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Lock document for editing"}));
    GTUtilsProjectTreeView::click(documentName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "unlockDocument"
void GTUtilsDocument::unlockDocument(const QString& documentName) {
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Unlock document for editing"}));
    GTUtilsProjectTreeView::click(documentName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkIfDocumentIsLocked"
void GTUtilsDocument::checkIfDocumentIsLocked(const QString& documentName, bool isLocked) {
    const QIcon actualIcon = GTUtilsProjectTreeView::getIcon(GTUtilsProjectTreeView::findIndex({documentName}));
    const QIcon unlockedDocumentIcon(":/core/images/document.png");
    const QIcon lockedDocumentIcon(":/core/images/ro_document.png");
    if (isLocked) {
        GT_CHECK(actualIcon.pixmap(16).toImage() == lockedDocumentIcon.pixmap(16).toImage(), QString("The '%1' document is unexpectedly unlocked").arg(documentName));
    } else {
        GT_CHECK(actualIcon.pixmap(16).toImage() == unlockedDocumentIcon.pixmap(16).toImage(), QString("The '%1' document is unexpectedly locked").arg(documentName));
    }
}
#undef GT_METHOD_NAME

QList<GObjectViewController*> GTUtilsDocument::getAllGObjectViews() {
    QList<GObjectViewController*> gObjectViews;

    MWMDIManager* mwMDIManager = AppContext::getMainWindow()->getMDIManager();
    QList<MWMDIWindow*> windows = mwMDIManager->getWindows();

    foreach (MWMDIWindow* w, windows) {
        if (auto gObjectViewWindow = qobject_cast<GObjectViewWindow*>(w)) {
            if (GObjectViewController* gObjectView = gObjectViewWindow->getObjectView()) {
                gObjectViews.append(gObjectView);
            }
        }
    }

    return gObjectViews;
}

#undef GT_CLASS_NAME

}  // namespace U2
