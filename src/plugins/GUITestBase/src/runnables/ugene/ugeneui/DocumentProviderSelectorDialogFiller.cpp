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

#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include "DocumentProviderSelectorDialogFiller.h"

namespace U2 {

QMap<DocumentProviderSelectorDialogFiller::OpenWith, QString> DocumentProviderSelectorDialogFiller::initMap(){
    QMap<DocumentProviderSelectorDialogFiller::OpenWith, QString> result;
    result.insert(DocumentProviderSelectorDialogFiller::AlignmentEditor,
                  "Multiple sequence alignment in the Alignment Editor_radio");
    result.insert(DocumentProviderSelectorDialogFiller::AssemblyBrowser,
                  "Short reads assembly in the Assembly Browser_radio");
    return result;
}
const QMap<DocumentProviderSelectorDialogFiller::OpenWith, QString> DocumentProviderSelectorDialogFiller::openWithMap =
        DocumentProviderSelectorDialogFiller::initMap();

#define GT_CLASS_NAME "DocumentProviderSelectorDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void DocumentProviderSelectorDialogFiller::commonScenario(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QRadioButton* radio = GTWidget::findExactWidget<QRadioButton*>(os, openWithMap.value(openWith), dialog);
    GTWidget::click(os, radio);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
