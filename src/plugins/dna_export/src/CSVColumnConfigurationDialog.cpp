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

#include "CSVColumnConfigurationDialog.h"

#include <U2Core/L10n.h>
#include <U2Core/Annotation.h>
#include <U2Gui/HelpButton.h>
#include <QPushButton>

#include <QMessageBox>

namespace U2 {

CSVColumnConfigurationDialog::CSVColumnConfigurationDialog(QWidget* w, const ColumnConfig& _config)
: QDialog(w), config(_config)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748791");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(complMarkRB, SIGNAL(toggled(bool)), SLOT(sl_complMarkToggle(bool)));
    connect(startRB, SIGNAL(toggled(bool)), SLOT(sl_startToggle(bool)));

    switch(config.role) {
        case ColumnRole_Ignore:
            ignoreRB->setChecked(true);
            break;
        case ColumnRole_Name:
            nameRB->setChecked(true);
            break;
        case ColumnRole_Qualifier:
            qualifierRB->setChecked(true);
            qualifierNameEdit->setText(config.qualifierName);
            break;
        case ColumnRole_StartPos:
            startRB->setChecked(true);
            startOffsetCheck->setChecked(config.startPositionOffset!=0);
            startOffsetValue->setValue(config.startPositionOffset);
            break;
        case ColumnRole_EndPos:
            endRB->setChecked(true);
            endInclusiveCheck->setChecked(config.endPositionIsInclusive);
            break;
        case ColumnRole_Length:
            lengthRB->setChecked(true);
            break;
        case ColumnRole_ComplMark:
            complMarkRB->setChecked(true);
            complValueEdit->setText(config.complementMark);
            complValueCheck->setChecked(!config.complementMark.isEmpty());
            break;
        case ColumnRole_Group:
            groupRB->setChecked(true);
            break;
        default:
            assert(0);
    }

}


void CSVColumnConfigurationDialog::accept() {
    config.reset();
    if (startRB->isChecked()) {
        config.role = ColumnRole_StartPos;
        config.startPositionOffset = startOffsetCheck->isChecked() ? startOffsetValue->value() : 0;
    } else if (endRB->isChecked()) {
        config.role = ColumnRole_EndPos;
        config.endPositionIsInclusive = endInclusiveCheck->isChecked();
    } else if (lengthRB->isChecked()) {
        config.role = ColumnRole_Length;
    } else if (qualifierRB->isChecked()) {
        config.role = ColumnRole_Qualifier;
        config.qualifierName = qualifierNameEdit->text();
        if (!Annotation::isValidQualifierName(config.qualifierName)) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("Invalid qualifier name!"));
            qualifierNameEdit->setFocus();
            return;
        }
    } else if (nameRB->isChecked()) {
        config.role = ColumnRole_Name;
    } else if (complMarkRB->isChecked()) {
        config.role = ColumnRole_ComplMark;
        config.complementMark = complValueCheck->isChecked() ? complValueEdit->text() : QString();
    } else if (groupRB->isChecked()) {
        config.role = ColumnRole_Group;
    } else {
        assert(ignoreRB->isChecked());
    }
    QDialog::accept();
}


void CSVColumnConfigurationDialog::sl_complMarkToggle(bool checked) {
    complValueCheck->setEnabled(checked);
    complValueEdit->setEnabled(checked && complValueCheck->isChecked());
}

void CSVColumnConfigurationDialog::sl_startToggle(bool checked) {
    startOffsetCheck->setEnabled(checked);
    startOffsetValue->setEnabled(checked && startOffsetCheck->isChecked());
}


} //namespace
