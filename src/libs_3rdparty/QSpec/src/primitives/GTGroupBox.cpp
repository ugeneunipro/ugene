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

#include "primitives/GTGroupBox.h"

#include <QStyle>
#include <QStyleOption>

#include "primitives/GTWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTGroupBox"

bool GTGroupBox::getChecked(QGroupBox* groupBox) {
    GT_CHECK_RESULT(groupBox != NULL, "QGroupBox is NULL", false);
    GT_CHECK_RESULT(groupBox->isEnabled(), "QGroupBox is disabled", false);

    return groupBox->isChecked();
}

bool GTGroupBox::getChecked(const QString& groupBoxName, QWidget* parent) {
    return GTGroupBox::getChecked(GTWidget::findGroupBox(groupBoxName, parent));
}

void GTGroupBox::setChecked(QGroupBox* groupBox, bool checked) {
    GT_CHECK(groupBox != NULL, "QGroupBox is NULL");

    if (groupBox->isChecked() == checked) {
        return;
    }

    GT_CHECK(groupBox->isEnabled(), "QGroupBox is disabled");

    QStyleOptionGroupBox options;
    options.initFrom(groupBox);
    if (groupBox->isFlat()) {
        options.features |= QStyleOptionFrame::Flat;
    }
    options.lineWidth = 1;
    options.midLineWidth = 0;
    options.text = groupBox->title();
    options.textAlignment = groupBox->alignment();
    options.subControls = (QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxCheckBox);
    if (!groupBox->title().isEmpty()) {
        options.subControls |= QStyle::SC_GroupBoxLabel;
    }
    options.state |= (groupBox->isChecked() ? QStyle::State_On : QStyle::State_Off);

    const QRect checkboxRect = groupBox->style()->subControlRect(QStyle::CC_GroupBox, &options, QStyle::SC_GroupBoxCheckBox);

    GTWidget::click(groupBox, Qt::LeftButton, checkboxRect.center());
    GTGlobals::sleep(200);

    GT_CHECK(checked == groupBox->isChecked(), "Can't set a new state");
}

void GTGroupBox::setChecked(const QString& groupBoxName, bool checked, QWidget* parent) {
    GTGroupBox::setChecked(GTWidget::findGroupBox(groupBoxName, parent), checked);
}

void GTGroupBox::setChecked(const QString& groupBoxName, QWidget* parent) {
    GTGroupBox::setChecked(GTWidget::findGroupBox(groupBoxName, parent));
}

QRect GTGroupBox::getCheckBoxRect(QGroupBox* groupBox) {
    QStyleOptionGroupBox options;
    return groupBox->style()->subControlRect(QStyle::CC_GroupBox, &options, QStyle::SC_GroupBoxCheckBox);
}

#undef GT_CLASS_NAME

}  // namespace HI
