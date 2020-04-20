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

#ifndef _DELETE_GAPS_DIALOG_
#define _DELETE_GAPS_DIALOG_

#include <QDialog>

class Ui_DeleteGapsDialog;

namespace U2 {

enum DeleteMode {
    DeleteByAbsoluteVal,
    DeleteByRelativeVal,
    DeleteAll
};

class DeleteGapsDialog: public QDialog {
    Q_OBJECT
public:
    DeleteGapsDialog(QWidget* parent, int alignmentLen);
    ~DeleteGapsDialog();
    DeleteMode getDeleteMode() const {return deleteMode;}
    int getValue() const {return value;}
private slots:
    void sl_onRadioButtonClicked();
    void sl_onOkClicked();
    void sl_onCancelClicked();

private:
    DeleteMode deleteMode;
    int value;
    Ui_DeleteGapsDialog* ui;
};

}

#endif
