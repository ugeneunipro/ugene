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

#pragma once

#include <QDialog>

#include <U2Core/PFMatrix.h>
#include <U2Core/PWMatrix.h>

#include <U2View/AlignmentLogo.h>

#include "WeightMatrixPlugin.h"

#include <ui_MatrixAndLogoWidget.h>
#include <ui_ViewMatrixDialog.h>

namespace U2 {

class MatrixAndLogoController : public QWidget, public Ui_MatrixAndLogoWidget {
    Q_OBJECT
public:
    MatrixAndLogoController(PFMatrix matrix, QWidget* parent = nullptr);
    MatrixAndLogoController(PWMatrix matrix, QWidget* parent = nullptr);

private:
    AlignmentLogoRenderArea* logoArea;
};

class ViewMatrixDialogController : public QDialog, public Ui_ViewMatrixDialog {
    Q_OBJECT

public:
    ViewMatrixDialogController(PFMatrix matrix, QWidget* w = nullptr);
    ViewMatrixDialogController(PWMatrix matrix, QWidget* w = nullptr);
private slots:
    void sl_onCloseButton();

private:
    MatrixAndLogoController* ml;
};

class MatrixViewController : public MWMDIWindow {
    Q_OBJECT
public:
    MatrixViewController(PFMatrix matrix);
    MatrixViewController(PWMatrix matrix);

private:
    QWidget* d;
};

}  // namespace U2
