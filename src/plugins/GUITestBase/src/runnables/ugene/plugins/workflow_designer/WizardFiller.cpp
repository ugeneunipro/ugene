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
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QLabel>
#include <QRadioButton>
#include <QScrollArea>
#include <QToolButton>
#include <QWizard>

#include "../../src/corelibs/U2Designer/src/wizard/WDWizardPage.h"
#include "GTUtilsWizard.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::WizardFiller"

#define GT_METHOD_NAME "commonScenario"
void WizardFiller::commonScenario() {
    GTGlobals::sleep();
    if (inputFiles.count() != 0 && !inputFiles.first().isEmpty()) {
        GTUtilsWizard::setInputFiles(os, inputFiles);
    }
    GTUtilsWizard::setAllParameters(os, map);

    GTUtilsWizard::clickButton(os, GTUtilsWizard::Apply);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "WizardFiller::getExpandButton"
QToolButton* WizardFiller::getExpandButton(HI::GUITestOpStatus& os) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QWizard* wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK_RESULT(wizard, "activeModalWidget is not of wizard type", nullptr);

    QList<QWidget*> widList = wizard->currentPage()->findChildren<QWidget*>();
    QList<QToolButton*> plusList;
    for (QWidget* w : qAsConst(widList)) {
        QToolButton* but = qobject_cast<QToolButton*>(w);
        if (but && but->text() == "+" && abs(but->rect().width() - 19) < 2) {
            plusList.append(but);
        }
    }
    // There can be one or more '+' buttons at wizard page which are invisible. TODO:detect them
    return plusList.isEmpty() ? nullptr : plusList.takeLast();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
