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

#include <U2Gui/U2SavableWidget.h>

#include "MfoldSettings.h"
#include "ui_MfoldDialog.h"

class QLineEdit;

namespace U2 {
class ADVSequenceObjectContext;
class DNASequenceSelection;
class GUrl;
class RegionSelector;

// Dialog for Mfold action.
class MfoldDialog final : public QDialog {
    Q_OBJECT

    // GUI fields.
    Ui_MfoldDialog ui;
    RegionSelector* regionSelector = nullptr;
    const QLineEdit* startEdit = nullptr;
    const QLineEdit* endEdit = nullptr;

    // To check the correctness of a region.
    qint64 seqLen = 0;
    bool isCircular = false;

    U2SavableWidget savableWidget;

    // Called once in ctor to create RegionSelector and bind signals.
    // DNASequenceSelection required for RegionSelector ctor.
    void initRegionSelector(DNASequenceSelection*);
    // Called once in ctor to bind signals on Output tab.
    // Takes path to input document to prompt user to save output there.
    void initOutputTab(const GUrl& inpPath);

private slots:
    // Checks the values of region widgets: if the region is incorrect, it enables the error label; if it is correct,
    // it disables the label.
    void validateRegionAndShowError();

public:
    // Uses the context to assign a dialog parent and get sequence params (current selection, isCircular, ...).
    explicit MfoldDialog(const ADVSequenceObjectContext&);
    // Settings specified by the user in the dialog.
    MfoldSettings getSettings() const;

public slots:
    void accept() override;
};
}  // namespace U2
