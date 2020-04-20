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

#include <U2Core/U2SafePoints.h>
#include "ADVGraphModel.h"

#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMessageBox>
#include <U2Gui/HelpButton.h>

#include "GraphLabelsSelectDialog.h"

#define BACKGROUND_COLOR "QPushButton { background-color : %1;}"

namespace U2 {

GraphLabelsSelectDialog::GraphLabelsSelectDialog(int maxWindowSize, QWidget* parent )
:QDialog(parent) {
    stepSpinBox = new QSpinBox(this);
    stepSpinBox->setRange(maxWindowSize/50, maxWindowSize);
    stepSpinBox->setObjectName("stepSpinBox");
    usedIntervalsCheck = new QCheckBox(this);
    usedIntervalsCheck->setObjectName("usedIntervalsCheck");
    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QHBoxLayout* spinLayout = new QHBoxLayout();
    buttonsLayout->addStretch(10);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->setObjectName("buttonBox");

    QLabel* spinLabel = new QLabel(tr("Window size"), this);
    buttonsLayout->addWidget(buttonBox);
    spinLayout->addWidget(spinLabel);
    spinLayout->addWidget(stepSpinBox);
    usedIntervalsCheck->setText(tr("Mark labels only in selected area"));

    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    new HelpButton(this, buttonBox, "24748816");

    mainLayout->addLayout(spinLayout);
    mainLayout->addWidget(usedIntervalsCheck);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("Graph Settings"));
    setWindowIcon(QIcon(":core/images/graphs.png"));

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMinimumWidth(200);

    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_onCancelClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_onOkClicked()));

    okButton->setDefault(true);

    this->setObjectName("GraphLabelsSelectDialog");
}
int GraphLabelsSelectDialog::getWindowSize() {
    return stepSpinBox->value();
}
bool GraphLabelsSelectDialog::isUsedIntervals() {
    CHECK(Qt::Checked != usedIntervalsCheck->checkState(), true);
    return false;
}

void GraphLabelsSelectDialog::sl_onCancelClicked() {
    reject();
}

void GraphLabelsSelectDialog::sl_onOkClicked() {
    accept();
    return;
}

} // namespace
