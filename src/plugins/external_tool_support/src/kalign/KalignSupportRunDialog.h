/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2Core/GAutoDeleteList.h>
#include <U2Core/Msa.h>

#include "KalignSupportTask.h"

#include <ui_KalignSupportRunDialog.h>

namespace U2 {

class SaveDocumentController;

class Kalign3DialogWithMsaInput : public QDialog, public Ui_KalignSupportRunDialog {
    Q_OBJECT
public:
    Kalign3DialogWithMsaInput(QWidget* w, const Msa& ma, Kalign3Settings& settings);

public slots:
    void accept() override;

private:
    Msa ma;
    Kalign3Settings& settings;
};

class Kalign3DialogWithFileInput : public QDialog, public Ui_KalignSupportRunDialog {
    Q_OBJECT
public:
    Kalign3DialogWithFileInput(QWidget* w, Kalign3Settings& settings);

public slots:
    void accept() override;

private slots:
    void sl_inputPathButtonClicked();

private:
    void initSaveController();
    Kalign3Settings& settings;
    SaveDocumentController* saveController = nullptr;
};

}  // namespace U2
