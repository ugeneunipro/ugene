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

#include "SetParametersDialogController.h"

#include <U2Core/AppContext.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>
#include <U2Gui/HelpButton.h>
#include <QPushButton>

namespace U2 {

SetParametersDialogController::SetParametersDialogController(QWidget *w)
    : QDialog(w) {

    setupUi(this);
    new HelpButton(this, buttonBox, "24749002");

    QStringList algo = AppContext::getPWMConversionAlgorithmRegistry()->getAlgorithmIds();
    algorithmComboBox->addItems(algo);

    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    connect(okButton, SIGNAL(clicked()), SLOT(sl_onOkButton()));
    connect(scoreSlider, SIGNAL(valueChanged(int)), SLOT(sl_onSliderMoved(int)));

}

void SetParametersDialogController::sl_onSliderMoved(int value) {
    scoreValueLabel->setText(QString("%1%").arg(value));
}

void SetParametersDialogController::sl_onOkButton() {
    QDialog::accept();
}

}
