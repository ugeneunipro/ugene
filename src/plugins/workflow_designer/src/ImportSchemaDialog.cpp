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

#include <QMessageBox>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Gui/HelpButton.h>

#include "ImportSchemaDialog.h"

namespace U2 {

ImportSchemaDialog::ImportSchemaDialog(QWidget* p) : QDialog(p) {
    setupUi(this);
    new HelpButton(this, buttonBox, "28967074");
}

void ImportSchemaDialog::accept() {
    QString name = nameEdit->text();
    if (name.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Element name is empty"));
        return;
    }

    if (name.contains(QRegExp("\\s"))) {
        QMessageBox::critical(this, tr("Error"), tr("Element name contains spaces"));
        return;
    }

    if (name.contains(".")) {
        QMessageBox::critical(this, tr("Error"), tr("Element name contains dots"));
        return;
    }

    if (name.contains("@")) {
        QMessageBox::critical(this, tr("Error"), tr("Element name contains @"));
        return;
    }

    if (NULL != Workflow::WorkflowEnv::getProtoRegistry()->getProto(name)) {
        QMessageBox::critical(this, tr("Error"), tr("An element with this name already exists"));
        return;
    }

    return QDialog::accept();
}

QString ImportSchemaDialog::getTypeName() {
    return nameEdit->text();
}

} // U2
