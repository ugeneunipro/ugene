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

#include <U2Core/L10n.h>

#include <U2Lang/URLContainer.h>

#include "GenomicLibraryDelegate.h"
#include "GenomicLibraryPropertyWidget.h"

namespace U2 {
namespace LocalWorkflow {

GenomicLibraryDelegate::GenomicLibraryDelegate(QObject *parent)
    : PropertyDelegate(parent)
{

}

QVariant GenomicLibraryDelegate::getDisplayValue(const QVariant &value) const {
    const QList<Dataset> datasets = value.value<QList<Dataset> >();
    const bool isEmpty = datasets.isEmpty() || datasets.first().getUrls().isEmpty();
    return isEmpty ? GenomicLibraryPropertyWidget::PLACEHOLDER : GenomicLibraryPropertyWidget::FILLED_VALUE;
}

QWidget *GenomicLibraryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    GenomicLibraryPropertyWidget* editor = new GenomicLibraryPropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget *GenomicLibraryDelegate::createWizardWidget(U2OpStatus &, QWidget *parent) const {
    return new GenomicLibraryPropertyWidget(parent);
}

void GenomicLibraryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    const QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    GenomicLibraryPropertyWidget *propertyWidget = qobject_cast<GenomicLibraryPropertyWidget *>(editor);
    SAFE_POINT(NULL != editor, L10N::nullPointerError("GenomicLibraryPropertyWidget"), );
    propertyWidget->setValue(value);
}

void GenomicLibraryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    GenomicLibraryPropertyWidget *propertyWidget = qobject_cast<GenomicLibraryPropertyWidget *>(editor);
    model->setData(index, propertyWidget->value(), ConfigurationEditor::ItemValueRole);
}

PropertyDelegate *GenomicLibraryDelegate::clone() {
    return new GenomicLibraryDelegate(parent());
}

void GenomicLibraryDelegate::sl_commit() {
    GenomicLibraryPropertyWidget* editor = qobject_cast<GenomicLibraryPropertyWidget *>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

}   // namespace LocalWorkflow
}   // namespace U2
