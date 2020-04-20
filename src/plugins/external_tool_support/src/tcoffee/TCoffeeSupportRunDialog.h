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

#ifndef _U2_TCOFFEE_SUPPORT_RUN_DIALOG_H
#define _U2_TCOFFEE_SUPPORT_RUN_DIALOG_H

#include <QDialog>

#include <U2Gui/DialogUtils.h>
#include <ui_TCoffeeSupportRunDialog.h>
#include "TCoffeeSupportTask.h"

namespace U2 {

class SaveDocumentController;

class TCoffeeSupportRunDialog : public QDialog, public Ui_TCoffeeSupportRunDialog {
    Q_OBJECT
public:
    TCoffeeSupportRunDialog(TCoffeeSupportTaskSettings& settings, QWidget* parent);

private slots:
    void accept();

private:
    TCoffeeSupportTaskSettings&        settings;
};

class TCoffeeWithExtFileSpecifySupportRunDialog : public QDialog, public Ui_TCoffeeSupportRunDialog{
    Q_OBJECT
public:
    TCoffeeWithExtFileSpecifySupportRunDialog(TCoffeeSupportTaskSettings& settings, QWidget* parent);

private slots:
    void accept();
    void sl_inputPathButtonClicked();

private:
    void initSaveController();

    TCoffeeSupportTaskSettings&     settings;
    SaveDocumentController *        saveController;
};

}//namespace
#endif // _U2_TCOFFEE_SUPPORT_RUN_DIALOG_H
