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

#include "CreateObjectRelationDialogController.h"
#include <ui_CreateObjectRelationDialog.h>

#include <U2Core/AnnotationTableObjectConstraints.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>

#include <QIcon>
#include <QMessageBox>

#include <U2Gui/HelpButton.h>
#include <QPushButton>

namespace U2 {

CreateObjectRelationDialogController::CreateObjectRelationDialogController(GObject* _assObj,
    const QList<GObject*>& _objects, GObjectRelationRole _role, bool rd, const QString& relationHint, QWidget* p)
    : QDialog(p), selectedObject(NULL), assObj(_assObj), objects(_objects), role(_role), removeDuplicates(rd)
{
    assert(!objects.isEmpty());
    assert(assObj!=NULL);
    ui = new Ui_CreateObjectRelationDialog;

    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    QIcon objectIcon(":/core/images/gobject.png");
    foreach(GObject* obj, objects) {
        ui->listWidget->addItem(new QListWidgetItem(objectIcon, obj->getGObjectName()));
    }
    ui->listWidget->setItemSelected(ui->listWidget->item(0), true);
    ui->relationHintLabel->setText(relationHint);
}

void CreateObjectRelationDialogController::accept() {
    int idx = ui->listWidget->currentRow();
    assert(idx>=0 && idx < objects.size());
    
    GObject* selObj = objects[idx];
    if (role == ObjectRole_Sequence && assObj->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(selObj);
        AnnotationTableObject *ao = qobject_cast<AnnotationTableObject *>(assObj);
        AnnotationTableObjectConstraints c;
        c.sequenceSizeToFit = dnaObj->getSequenceLength();
        bool res = ao->checkConstraints(&c);
        if (!res) {
            int rc = QMessageBox::question(this, tr("Warning"), 
                tr("Found annotations that are out of the sequence range, continue?"), QMessageBox::Yes, QMessageBox::No);
            if (rc == QMessageBox::No) {
                return;
            }
        }
        if (removeDuplicates) {
            QList<GObjectRelation> oldRel = assObj->findRelatedObjectsByRole(role);
            foreach(const GObjectRelation& r, oldRel) {
                assObj->removeObjectRelation(r);
            }
        }
        assObj->addObjectRelation(selObj, role);
    }

    selectedObject = selObj;
    QDialog::accept();
}

CreateObjectRelationDialogController::~CreateObjectRelationDialogController()
{
    delete ui;
}
}//namespace
