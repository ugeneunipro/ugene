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

#include "CreateAnnotationDialog.h"
#include "CreateAnnotationWidgetController.h"

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/GObjectRelationRoles.h>

#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <U2Gui/HelpButton.h>
#include "ui_CreateAnnotationDialog.h"

namespace U2 {

CreateAnnotationDialog::CreateAnnotationDialog(QWidget* p, CreateAnnotationModel& m) :
    QDialog(p),
    model(m),
    ui(new Ui_CreateAnnotationDialog)
{
    ui->setupUi(this);
    annWidgetController = new CreateAnnotationWidgetController(m, this, CreateAnnotationWidgetController::Full);

    helpButton = new HelpButton(this, ui->buttonBox, "24748780");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Create"));

    ui->mainLayout->insertWidget(0, annWidgetController->getWidget());

    annWidgetController->setFocusToAnnotationType();
}

CreateAnnotationDialog::~CreateAnnotationDialog() {
    delete ui;
}

void CreateAnnotationDialog::updateAppearance(const QString &newTitle, const QString &newHelpPage, const QString &newOkButtonName) {
    setWindowTitle(newTitle);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(newOkButtonName);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    helpButton->updatePageId(newHelpPage);
}

void CreateAnnotationDialog::accept() {
    QString err = annWidgetController->validate();
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), err);
        return;
    }
    bool objectPrepared = annWidgetController->prepareAnnotationObject();
    if (!objectPrepared){
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }
    model = annWidgetController->getModel();
    QDialog::accept();
}

} // namespace
