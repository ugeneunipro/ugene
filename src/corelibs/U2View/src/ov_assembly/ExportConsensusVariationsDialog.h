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

#include "ExportConsensusVariationsTask.h"

#include <ui_ExportConsensusDialog.h>

namespace U2 {

class RegionSelector;
class SaveDocumentController;

class ExportConsensusVariationsDialog : public QDialog, private Ui_ExportConsensusDialog {
    Q_OBJECT
public:
    ExportConsensusVariationsDialog(QWidget* p, const ExportConsensusVariationsTaskSettings& settings, const U2Region& visibleRegion);

    void accept() override;

    const ExportConsensusVariationsTaskSettings& getSettings() const;

private:
    void initSaveController();

    ExportConsensusVariationsTaskSettings settings;
    SaveDocumentController* saveController;
    RegionSelector* regionSelector;
};

}  // namespace U2
