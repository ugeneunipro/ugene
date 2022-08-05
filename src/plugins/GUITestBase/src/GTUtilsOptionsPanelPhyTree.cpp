/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <primitives/GTWidget.h>
#include <primitives/GTSpinBox.h>

#include "GTUtilsOptionsPanelPhyTree.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsOptionPanelPhyTree"

#define GT_METHOD_NAME "openTab"
QWidget* GTUtilsOptionPanelPhyTree::openTab(HI::GUITestOpStatus& os) {
    QWidget* tabButton = GTWidget::findWidget(os, "OP_TREES_WIDGET");
    GTWidget::click(os, tabButton);
    return getOptionsPanelWidget(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOptionsPanelWidget"
QWidget* GTUtilsOptionPanelPhyTree::getOptionsPanelWidget(HI::GUITestOpStatus& os) {
    return GTWidget::findWidget(os, "TreeOptionsWidget");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandFontSettingsPanel"
void GTUtilsOptionPanelPhyTree::expandFontSettingsPanel(HI::GUITestOpStatus& os) {
    QWidget* optionsPanel = getOptionsPanelWidget(os);
    auto labelsColorButton = GTWidget::findWidget(os, "labelsColorButton", optionsPanel);
    if (!labelsColorButton->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "lblFontSettings", optionsPanel));
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFontSize"
int GTUtilsOptionPanelPhyTree::getFontSize(HI::GUITestOpStatus& os) {
    expandFontSettingsPanel(os);
    return GTSpinBox::getValue(os, "fontSizeSpinBox", getOptionsPanelWidget(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFontSize"
void GTUtilsOptionPanelPhyTree::setFontSize(HI::GUITestOpStatus& os, int fontSize) {
    expandFontSettingsPanel(os);
    GTSpinBox::setValue(os, "fontSizeSpinBox", fontSize, getOptionsPanelWidget(os));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
