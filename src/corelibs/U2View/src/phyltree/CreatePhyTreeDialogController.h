/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/Msa.h>
#include <U2Core/global.h>

class Ui_CreatePhyTree;

namespace U2 {

class MsaObject;
class CreatePhyTreeWidget;
class SaveDocumentController;

class U2VIEW_EXPORT CreatePhyTreeDialogController : public QDialog {
    Q_OBJECT
public:
    CreatePhyTreeDialogController(QWidget* parent, const MsaObject* msaObject, CreatePhyTreeSettings& settings);
    ~CreatePhyTreeDialogController();

private slots:
    void accept();
    void sl_comboIndexChanged(int index);
    void sl_onStoreSettings();
    void sl_onRestoreDefault();

private:
    bool checkAndPrepareOutputFilePath();
    bool checkSettings();
    bool checkMemory();
    void initSaveController(const MsaObject* msaObject);

    Msa msa;
    CreatePhyTreeSettings& settings;
    CreatePhyTreeWidget* settingsWidget;
    Ui_CreatePhyTree* ui;
    SaveDocumentController* saveController;
};

}  // namespace U2
