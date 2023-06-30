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

#include "CommonImportOptionsDialogFiller.h"
#include <primitives/GTWidget.h>

#include <QApplication>

#include <U2Gui/ImportOptionsWidget.h>

#include "ImportOptionsWidgetFiller.h"

namespace U2 {
using namespace HI;
#define GT_CLASS_NAME "GTUtilsDialog::CommonImportOptionsDialogFiller"

CommonImportOptionsDialogFiller::CommonImportOptionsDialogFiller(const QVariantMap& data)
    : Filler("CommonImportOptionsDialog"),
      data(data) {
}

void CommonImportOptionsDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    GTWidget::clickWindowTitle(dialog);

    auto optionsWidget = GTWidget::findExactWidget<ImportOptionsWidget*>("optionsWidget", dialog);

    ImportOptionsWidgetFiller::fill(optionsWidget, data);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}


#undef GT_CLASS_NAME

}  // namespace U2
