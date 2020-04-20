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

#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QTreeView>
#include <QVBoxLayout>
#include <QAction>

#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Core/QObjectScopedPointer.h>

#include "ProjectTreeItemSelectorDialog.h"
#include "ProjectTreeItemSelectorDialogImpl.h"

namespace U2 {

ProjectTreeItemSelectorDialogImpl::ProjectTreeItemSelectorDialogImpl(QWidget* p, const ProjectTreeControllerModeSettings& s)
    : QDialog(p)
{
    setupUi(this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    controller = new ProjectTreeController(treeView, s, this);
    connect(controller, SIGNAL(si_doubleClicked(GObject*)), this, SLOT(sl_objectClicked(GObject*)));
    acceptByDoubleClick = false;
}

void ProjectTreeItemSelectorDialogImpl::sl_objectClicked(GObject* obj) {

    Document* d = obj->getDocument();
    assert(d != NULL);
    if (!d->isLoaded()) { 
        QAction* loadSelectedDocumentsAction = controller->getLoadSeletectedDocumentsAction();
        loadSelectedDocumentsAction->trigger();
        return;
    }
   
    if (acceptByDoubleClick) {
         accept();
    }
}

ProjectTreeItemSelectorDialogImpl::~ProjectTreeItemSelectorDialogImpl() {
    delete controller;
}

QList<Document*> ProjectTreeItemSelectorDialog::selectDocuments(const ProjectTreeControllerModeSettings& s, QWidget* p) {
    QList<Document*> res;
    QObjectScopedPointer<ProjectTreeItemSelectorDialogImpl> d = new ProjectTreeItemSelectorDialogImpl(p, s);
    const int rc = d->exec();
    CHECK(!d.isNull(), res);

    if (rc == QDialog::Accepted) {
        const DocumentSelection* ds = d->controller->getDocumentSelection();
        res << ds->getSelectedDocuments();
    }
    return res;    
}

QList<GObject*> ProjectTreeItemSelectorDialog::selectObjects(const ProjectTreeControllerModeSettings& s, QWidget* p) {
    QList<GObject*> res;
    QObjectScopedPointer<ProjectTreeItemSelectorDialogImpl> d = new ProjectTreeItemSelectorDialogImpl(p, s);
    const int rc = d->exec();
    CHECK(!d.isNull(), res);

    if (rc == QDialog::Accepted) {
        const GObjectSelection* os = d->controller->getGObjectSelection();
        res << os->getSelectedObjects();
    }
    return res;
}

void ProjectTreeItemSelectorDialog::selectObjectsAndDocuments(const ProjectTreeControllerModeSettings& s, QWidget* p, QList<Document*>& docList,
    QList<GObject*>& objList)
{
    QObjectScopedPointer<ProjectTreeItemSelectorDialogImpl> d = new ProjectTreeItemSelectorDialogImpl(p, s);
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc == QDialog::Accepted) {
        const GObjectSelection* os = d->controller->getGObjectSelection();
        const DocumentSelection* ds = d->controller->getDocumentSelection();

        docList << ds->getSelectedDocuments();
        foreach (GObject* obj, os->getSelectedObjects()) {
            if (!docList.contains(obj->getDocument())) {
                objList << obj;
            }
        }
    }
}

void ProjectTreeItemSelectorDialog::selectObjectsAndFolders(const ProjectTreeControllerModeSettings &s, QWidget *p, QList<Folder> &folderList,
    QList<GObject *> &objList)
{
    QObjectScopedPointer<ProjectTreeItemSelectorDialogImpl> d = new ProjectTreeItemSelectorDialogImpl(p, s);
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc == QDialog::Accepted) {
        SAFE_POINT(NULL != d->controller, "Invalid project tree controller", );
        folderList << d->controller->getSelectedFolders(); // add folders selected by a user

        const GObjectSelection* os = d->controller->getGObjectSelection();
        SAFE_POINT(NULL != os, "Invalid object selection", );
        foreach (GObject* obj, os->getSelectedObjects()) {
            bool objectIsAlreadySelected = false;
            foreach (const Folder &selectedFolder, folderList) {
                if (d->controller->isObjectInFolder(obj, selectedFolder)) {
                    objectIsAlreadySelected = true;
                    break;
                }
            }
            if (!objectIsAlreadySelected) {
                objList << obj; // add objects selected by the user that are not located in chosen folders
            }
        }
    }
}

Folder ProjectTreeItemSelectorDialog::selectFolder(QWidget *parent) {
    ProjectTreeControllerModeSettings settings;
    settings.allowMultipleSelection = false;

    QObjectScopedPointer<ProjectTreeItemSelectorDialogImpl> d = new ProjectTreeItemSelectorDialogImpl(parent, settings);
    const int rc = d->exec();
    CHECK(!d.isNull(), Folder());

    if (rc == QDialog::Accepted) {
        SAFE_POINT(NULL != d->controller, "Invalid project tree controller", Folder());
        const QList<Folder> folders = d->controller->getSelectedFolders();
        if (!folders.isEmpty()) {
            return folders.first();
        }
    }
    return Folder();
}

}//namespace
