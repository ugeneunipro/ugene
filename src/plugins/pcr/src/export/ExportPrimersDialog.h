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

#include <U2Core/Primer.h>

#include "ui_ExportPrimersDialog.h"

namespace U2 {

class Document;
class SaveDocumentController;

class ExportPrimersDialog : public QDialog, private Ui_ExportPrimersDialog {
    Q_OBJECT
public:
    ExportPrimersDialog(const QList<Primer>& primers, QWidget* parent);

private slots:
    void accept() override;

private:
    void init();
    void initSaveController();
    void connectSignals();
    void connectProjectSignals();
    QList<Primer> primers;
    SaveDocumentController* saveController;
};

}  // namespace U2
