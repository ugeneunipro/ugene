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

#ifndef _U2_WEIGHT_MATRIX_SET_PARAMETERS_DIALOG_CONTROLLER_H_
#define _U2_WEIGHT_MATRIX_SET_PARAMETERS_DIALOG_CONTROLLER_H_

#include <ui_SetDefaultParametersDialog.h>

#include <QDialog>

#include "WeightMatrixPlugin.h"

namespace U2 {

class SetParametersDialogController : public QDialog, public Ui_SetDefaultParametersDialog {
    Q_OBJECT

public:
    SetParametersDialogController(QWidget* w = NULL);

private slots:
    void sl_onOkButton();
    void sl_onSliderMoved(int);
};

} //namespace

#endif
