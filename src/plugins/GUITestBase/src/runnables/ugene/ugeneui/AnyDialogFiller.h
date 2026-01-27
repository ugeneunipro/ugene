/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <QDialogButtonBox>

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class AnyDialogFiller : public Filler {
public:
    AnyDialogFiller(const QString& objectName, QDialogButtonBox::StandardButton _b)
        : Filler(objectName), b(_b) {
    }
    // Creates a filler with scenario for any dialog possible
    // Useful in case of rarely-used dialogs, when you do not want to write a separate filler
    AnyDialogFiller(const QString& objectName, CustomScenario* _scenario)
        : Filler(objectName, _scenario) {
    }

    void commonScenario() override;

private:
    QDialogButtonBox::StandardButton b = QDialogButtonBox::Ok;
};
}  // namespace U2
