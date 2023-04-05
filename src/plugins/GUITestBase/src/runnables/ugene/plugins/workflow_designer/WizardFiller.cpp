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

#include "WizardFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>

#include "GTUtilsWizard.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::WizardFiller"

#define GT_METHOD_NAME "commonScenario"
void WizardFiller::commonScenario() {
    if (!inputFiles.isEmpty() && !inputFiles.first().isEmpty()) {
        GTUtilsWizard::setInputFiles(os, inputFiles);
    }
    GTUtilsWizard::setAllParameters(os, map);
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Apply);
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
