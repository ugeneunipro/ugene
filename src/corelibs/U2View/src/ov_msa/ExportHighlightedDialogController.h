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

#include "MsaEditor.h"

class Ui_ExportHighlightedDialog;

namespace U2 {

class SaveDocumentController;

class ExportHighligtingDialogController : public QDialog {
    Q_OBJECT
public:
    ExportHighligtingDialogController(MaEditorWgt* msaui_, QWidget* p);
    ~ExportHighligtingDialogController();

    void accept() override;
    void lockKeepGaps();

    int startPos;
    int endPos;
    int startingIndex;
    bool keepGaps;
    bool dots;
    bool transpose;
    GUrl url;

private slots:
    void sl_regionChanged();

private:
    void initSaveController();

    MaEditorWgt* msaui;
    SaveDocumentController* saveController;
    Ui_ExportHighlightedDialog* ui;
};

}  // namespace U2
