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

#include "GTUtilsDocument.h"
#include "GTGlobals.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include "primitives/GTMenu.h"
#include "GTUtilsProjectTreeView.h"
#include "primitives/PopupChooser.h"
#include <base_dialogs/MessageBoxFiller.h>

#include <U2Gui/ObjectViewModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <QApplication>
#include <QTreeWidgetItem>

namespace U2 {
using namespace HI;

const QString GTUtilsDocument::DocumentUnloaded = "Unloaded";

#define GT_CLASS_NAME "GTUtilsDocument"

#define GT_METHOD_NAME "getDocument"
Document* GTUtilsDocument::getDocument(HI::GUITestOpStatus &os, const QString& documentName) {

    Project* p = AppContext::getProject();
    GT_CHECK_RESULT(p != NULL, "Project does not exist", NULL);

    QList<Document*> docs = p->getDocuments();
    foreach (Document *d, docs) {
        if (d && (d->getName() == documentName)) {
            return d;
        }
    }

    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkDocument"
void GTUtilsDocument::checkDocument(HI::GUITestOpStatus &os, const QString &documentName, const GObjectViewFactoryId &id) {

    GTGlobals::sleep(1000);

    Document *d = getDocument(os, documentName);
    GT_CHECK(d != NULL, "There is no document with name " + documentName);

    if (id.isEmpty()) {
        return;
    }

    GObjectView* view = getDocumentGObjectView(os, d);
    if (id == DocumentUnloaded) {
        GT_CHECK(view == NULL, "GObjectView is not NULL");
        return;
    }

    GT_CHECK(view != NULL, "GObjectView* is NULL");
    GObjectViewFactoryId viewFactoryId = view->getFactoryId();
    GT_CHECK(viewFactoryId == id, "View's GObjectViewFactoryId is " + viewFactoryId + ", not " + id);
}
#undef GT_METHOD_NAME

void GTUtilsDocument::removeDocument(HI::GUITestOpStatus &os, const QString &documentName, GTGlobals::UseMethod method)
{
    switch (method) {
    case GTGlobals::UseMouse:
    {
        Runnable *popupChooser = new PopupChooser(os, QStringList() << ACTION_PROJECT__REMOVE_SELECTED, method);
        GTUtilsDialog::waitForDialog(os, popupChooser);
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, documentName) + QPoint(1,0));//dirty hack

        GTMouseDriver::click(Qt::RightButton);
        break;
    }

    default:
    case GTGlobals::UseKey:
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, documentName) + QPoint(1,0));//dirty hack
        GTMouseDriver::click();

        GTKeyboardDriver::keyClick(Qt::Key_Delete);
        break;
    }

    GTGlobals::sleep(500);
}

#define GT_METHOD_NAME "getDocumentGObjectView"
GObjectView* GTUtilsDocument::getDocumentGObjectView(HI::GUITestOpStatus &os, Document* d) {

    GT_CHECK_RESULT(d != NULL, "Document* is NULL", NULL);

    QList<GObjectView*> gObjectViews = getAllGObjectViews();
    foreach (GObjectView *view, gObjectViews) {
        if (view->containsDocumentObjects(d)) {
            return view;
        }
    }

    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isDocumentLoaded"
bool GTUtilsDocument::isDocumentLoaded(HI::GUITestOpStatus &os, const QString& documentName)
{
    QModelIndex index = GTUtilsProjectTreeView::findIndex(os, GTUtilsProjectTreeView::getTreeView(os), documentName);
    QString s = index.data().toString();
    return !s.contains("unloaded");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "saveDocument"
void GTUtilsDocument::saveDocument(HI::GUITestOpStatus &os, const QString &documentName) {
    Runnable *popupChooser = new PopupChooser(os, QStringList() << ACTION_PROJECT__SAVE_DOCUMENT, GTGlobals::UseMouse);

    GTUtilsDialog::waitForDialog(os, popupChooser);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, documentName) + QPoint(1,0));//dirty hack
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "unloadDocument"
void GTUtilsDocument::unloadDocument(HI::GUITestOpStatus &os, const QString &documentName, bool waitForMessageBox) {
    GT_CHECK_RESULT( isDocumentLoaded(os, documentName), "Document is not loaded", );

    Runnable *popupChooser = new PopupChooser(os, QStringList() << ACTION_PROJECT__UNLOAD_SELECTED, GTGlobals::UseMouse);

    if (waitForMessageBox) {
        MessageBoxDialogFiller *filler = new MessageBoxDialogFiller(os, "Yes");
        GTUtilsDialog::waitForDialog(os, filler);
    }

    GTUtilsDialog::waitForDialog(os, popupChooser);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, documentName) + QPoint(1,0));//dirty hack
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "loadDocument"
void GTUtilsDocument::loadDocument(HI::GUITestOpStatus &os, const QString &documentName) {
    GT_CHECK_RESULT( !isDocumentLoaded(os, documentName), "Document is loaded", );

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, documentName) + QPoint(1,0));//dirty hack
    GTMouseDriver::doubleClick();

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "lockDocument"
void GTUtilsDocument::lockDocument(HI::GUITestOpStatus &os, const QString &documentName) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Lock document for editing"));
    GTUtilsProjectTreeView::click(os, documentName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "unlockDocument"
void GTUtilsDocument::unlockDocument(HI::GUITestOpStatus &os, const QString &documentName) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Unlock document for editing"));
    GTUtilsProjectTreeView::click(os, documentName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkIfDocumentIsLocked"
void GTUtilsDocument::checkIfDocumentIsLocked(GUITestOpStatus &os, const QString &documentName, bool isLocked) {
    const QIcon actualIcon = GTUtilsProjectTreeView::getIcon(os, GTUtilsProjectTreeView::findIndex(os, QStringList() << documentName));
    const QIcon unlockedDocumentIcon(":/core/images/document.png");
    const QIcon lockedDocumentIcon(":/core/images/ro_document.png");
    if (isLocked) {
        GT_CHECK(actualIcon.pixmap(16).toImage() == lockedDocumentIcon.pixmap(16).toImage(), QString("The '%1' document is unexpectedly unlocked").arg(documentName));
    } else {
        GT_CHECK(actualIcon.pixmap(16).toImage() == unlockedDocumentIcon.pixmap(16).toImage(), QString("The '%1' document is unexpectedly locked").arg(documentName));
    }
}
#undef GT_METHOD_NAME

QList<GObjectView*> GTUtilsDocument::getAllGObjectViews() {

    QList<GObjectView*> gObjectViews;

    MWMDIManager* mwMDIManager = AppContext::getMainWindow()->getMDIManager();
    QList<MWMDIWindow*> windows = mwMDIManager->getWindows();

    foreach (MWMDIWindow *w, windows) {
        if (GObjectViewWindow* gObjectViewWindow = qobject_cast<GObjectViewWindow*>(w)) {
            if (GObjectView* gObjectView = gObjectViewWindow->getObjectView()) {
                gObjectViews.append(gObjectView);
            }
        }
    }

    return gObjectViews;
}

#undef GT_CLASS_NAME

}
