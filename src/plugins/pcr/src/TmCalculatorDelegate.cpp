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

#include "TmCalculatorDelegate.h"

#include <U2Algorithm/TmCalculator.h>
#include <U2Algorithm/TmCalculatorFactory.h>
#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>

#include "TmCalculatorPropertyWidget.h"

namespace U2 {

TmCalculatorDelegate::TmCalculatorDelegate(QObject* parent)
    : PropertyDelegate(parent) {
}

QVariant TmCalculatorDelegate::getDisplayValue(const QVariant& value) const {
    if (!value.isValid()) {
        auto defaultFactory = AppContext::getTmCalculatorRegistry()->getDefaultTmCalculatorFactory();
        return defaultFactory->visualName;
    }
    auto currentFactory = AppContext::getTmCalculatorRegistry()->getById(value.toMap().value(TmCalculator::KEY_ID).toString());
    return currentFactory->visualName;
}

PropertyDelegate* TmCalculatorDelegate::clone() {
    return new TmCalculatorDelegate(parent());
}

QWidget* TmCalculatorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    auto editor = new TmCalculatorPropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget* TmCalculatorDelegate::createWizardWidget(U2OpStatus&, QWidget* parent) const {
    return new TmCalculatorPropertyWidget(parent);
}

void TmCalculatorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    auto propertyWidget = qobject_cast<TmCalculatorPropertyWidget*>(editor);
    propertyWidget->setValue(value);
}

void TmCalculatorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto propertyWidget = qobject_cast<TmCalculatorPropertyWidget*>(editor);
    model->setData(index, propertyWidget->value(), ConfigurationEditor::ItemValueRole);
}

void TmCalculatorDelegate::sl_commit() {
    auto editor = qobject_cast<TmCalculatorPropertyWidget*>(sender());
    CHECK(editor != nullptr, );

    emit commitData(editor);
}

}  // namespace U2
