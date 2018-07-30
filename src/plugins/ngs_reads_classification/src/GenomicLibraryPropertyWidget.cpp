/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/QObjectScopedPointer.h>

#include "GenomicLibraryDialog.h"
#include "GenomicLibraryPropertyWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString GenomicLibraryPropertyWidget::PLACEHOLDER = QObject::tr("Select genomes...");
const QString GenomicLibraryPropertyWidget::FILLED_VALUE = QObject::tr("Custom genomes");

GenomicLibraryPropertyWidget::GenomicLibraryPropertyWidget(QWidget *parent, DelegateTags *tags)
    : PropertyWidget(parent, tags)
{
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(PLACEHOLDER);
    lineEdit->setReadOnly(true);
    lineEdit->setObjectName("lineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("toolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_showDialog()));
    layout()->addWidget(toolButton);

    setObjectName("GenomicLibraryPropertyWidget");
}

QVariant GenomicLibraryPropertyWidget::value() {
    return QVariant::fromValue<QList<Dataset> >(QList<Dataset>() << dataset);
}

void GenomicLibraryPropertyWidget::setValue(const QVariant &value) {
    lineEdit->clear();
    const QList<Dataset> datasets = value.value<QList<Dataset> >();
    if (datasets.isEmpty()) {
        dataset = Dataset();
    } else {
        dataset = datasets.first();
        lineEdit->setText(FILLED_VALUE);
    }
}

void GenomicLibraryPropertyWidget::sl_showDialog() {
    QObjectScopedPointer<GenomicLibraryDialog> dialog(new GenomicLibraryDialog(dataset, this));
    if (QDialog::Accepted == dialog->exec()) {
        CHECK(!dialog.isNull(), );
        lineEdit->setText(FILLED_VALUE);
        dataset = dialog->getDataset();
        emit si_valueChanged(value());
    }
}

}   // namespace LocalWorkflow
}   // namespace U2
