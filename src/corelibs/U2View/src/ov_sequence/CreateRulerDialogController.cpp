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

#include "CreateRulerDialogController.h"

#include <U2Core/TextUtils.h>

#include <QPushButton>
#include <QMessageBox>
#include <QColorDialog>
#include <U2Gui/HelpButton.h>

namespace U2 {

CreateRulerDialogController::CreateRulerDialogController(const QSet<QString>& namesToFilter, int defaultOffset, QWidget* p)
: QDialog(p)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748759");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Create"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    setMaximumHeight(layout()->minimumSize().height());

    filter = namesToFilter;

    sampleLabel->setAutoFillBackground(true);

    nameEdit->setText(TextUtils::variate(tr("New ruler"), "_", filter));

    spinBox->setMinimum(INT_MIN);
    spinBox->setMaximum(INT_MAX);
    spinBox->setValue(defaultOffset);

    color = Qt::darkBlue;

    updateColorSample();

    connect(colorButton, SIGNAL(clicked()), SLOT(sl_colorButtonClicked()));
    setWindowIcon(QIcon(":/ugene/images/ugene_16.png"));

}

void CreateRulerDialogController::updateColorSample() {
    QPalette lPal;
    lPal.setColor(QPalette::Window, Qt::white);
    lPal.setColor(QPalette::WindowText, color);
    sampleLabel->setPalette(lPal);
}

void CreateRulerDialogController::sl_colorButtonClicked() {
    QColor c = QColorDialog::getColor(color, this);
    if (!c.isValid()) {
        return;
    }
    color = c;
    updateColorSample();
}

void CreateRulerDialogController::accept() {
    QString n = nameEdit->text();
    if (n.isEmpty()) {
        QMessageBox::critical(NULL, tr("Error"), tr("Ruler name is empty!"));
        nameEdit->setFocus();
        return;
    }
    if (filter.contains(n)) {
        QMessageBox::critical(NULL, tr("Error"), tr("Ruler with the same name is already exists!"));
        nameEdit->setFocus();
        return;
    }
    name = n;
    offset = spinBox->value() - 1;
    QDialog::accept();
}

}//namespace


