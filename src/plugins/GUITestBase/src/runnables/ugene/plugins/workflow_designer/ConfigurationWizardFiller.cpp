/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QApplication>

#include "ConfigurationWizardFiller.h"
#include "GTUtilsWizard.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "ConfigurationWizardFiller"

#define GT_METHOD_NAME "commonScenario"
void ConfigurationWizardFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");
    GTGlobals::sleep(500);

    GTMouseDriver::moveTo(QPoint(dialog->pos().x() + dialog->rect().width() / 2, dialog->pos().y() + 5));
    GTMouseDriver::click();

    foreach (const QString &s, radioNames) {
        QRadioButton *b = GTWidget::findExactWidget<QRadioButton *>(os, s, dialog);
        GTRadioButton::click(os, b);
    }

    GTUtilsWizard::clickButton(os, GTUtilsWizard::Setup);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
