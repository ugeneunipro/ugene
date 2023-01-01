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

#ifndef GTSEQUENCEREADINGMODEDIALOGUTILS_H
#define GTSEQUENCEREADINGMODEDIALOGUTILS_H

#include <QSpinBox>

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class GTSequenceReadingModeDialogUtils : public Filler {
public:
    enum sequenceMode { Separate,
                        Merge };
    enum Button { Ok,
                  Cancel };

    GTSequenceReadingModeDialogUtils(HI::GUITestOpStatus& os, CustomScenario* scenario = nullptr);
    virtual void commonScenario();

private:
    void selectMode();
    void setNumSymbolsParts();
    void setNumSymbolsFiles();
    void setNewDocumentName();
    void selectSaveDocument();
    void clickButton();
    void changeSpinBoxValue(QSpinBox*, int);

    QWidget* dialog;
};

}  // namespace U2

#endif  // GTSEQUENCEREADINGMODEDIALOGUTILS_H
