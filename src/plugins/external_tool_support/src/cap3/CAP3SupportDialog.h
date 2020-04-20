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

#ifndef _U2_CAP3_SUPPORT_DIALOG_H_
#define _U2_CAP3_SUPPORT_DIALOG_H_

#include <QDialog>

#include <ui_CAP3SupportDialog.h>

namespace U2 {

class CAP3SupportTaskSettings;
class SaveDocumentController;

class CAP3SupportDialog : public QDialog, public Ui_CAP3SupportDialog {
    Q_OBJECT
public:
    CAP3SupportDialog(CAP3SupportTaskSettings& settings, QWidget* parent);
    void accept();

private slots:
    void sl_onAddButtonClicked();
    void sl_onRemoveButtonClicked();
    void sl_onRemoveAllButtonClicked();

private:
    void initSettings();
    void initSaveController();

    CAP3SupportTaskSettings& settings;
    SaveDocumentController *saveController;
};


}//namespace
#endif // _U2_CAP3_SUPPORT_DIALOG_H_
