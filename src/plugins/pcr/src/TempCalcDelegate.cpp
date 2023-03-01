/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "TempCalcDelegate.h"

#include <U2Algorithm/BaseTempCalc.h>
#include <U2Algorithm/TempCalcRegistry.h>

#include <U2Core/AppContext.h>

#include "TempCalcPropertyWidget.h"

namespace U2 {

TempCalcDelegate::TempCalcDelegate(QObject* parent)
    : PropertyDelegate(parent) {
}

QVariant TempCalcDelegate::getDisplayValue(const QVariant& value) const {
    if (!value.isValid()) {
        auto defaultFactory = AppContext::getTempCalcRegistry()->getDefaultTempCalcFactory();
        return defaultFactory->getId();
    }
    return value.toMap().value(BaseTempCalc::KEY_ID).toString();
}

PropertyDelegate* TempCalcDelegate::clone() {
    return new TempCalcDelegate(parent());
}

QWidget* TempCalcDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    auto editor = new TempCalcPropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget* TempCalcDelegate::createWizardWidget(U2OpStatus&, QWidget* parent) const {
    return new TempCalcPropertyWidget(parent);
}

void TempCalcDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    auto propertyWidget = qobject_cast<TempCalcPropertyWidget*>(editor);
    propertyWidget->setValue(value);
}

void TempCalcDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto propertyWidget = qobject_cast<TempCalcPropertyWidget*>(editor);
    model->setData(index, propertyWidget->value(), ConfigurationEditor::ItemValueRole);
}

void TempCalcDelegate::sl_commit() {
    auto editor = qobject_cast<TempCalcPropertyWidget*>(sender());
    CHECK(editor != nullptr, );

    emit commitData(editor);
}

}  // namespace U2
