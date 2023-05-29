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

#include <primitives/GTComboBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <U2View/TreeViewerFactory.h>
#include <U2View/TvBranchItem.h>

#include "GTUtilsMdi.h"
#include "GTUtilsOptionsPanelPhyTree.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsOptionPanelPhyTree"

#define GT_METHOD_NAME "openTab"
QWidget* GTUtilsOptionPanelPhyTree::openTab(HI::GUITestOpStatus& os) {
    QWidget* activeObjectViewWindow = GTUtilsMdi::getActiveObjectViewWindow(os, TreeViewerFactory::ID);
    QWidget* tabButton = GTWidget::findWidget(os, "OP_TREES_WIDGET", activeObjectViewWindow);
    GTWidget::click(os, tabButton);
    return getOptionsPanelWidget(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOptionsPanelWidget"
QWidget* GTUtilsOptionPanelPhyTree::getOptionsPanelWidget(HI::GUITestOpStatus& os) {
    QWidget* activeObjectViewWindow = GTUtilsMdi::getActiveObjectViewWindow(os, TreeViewerFactory::ID);
    return GTWidget::findWidget(os, "TreeOptionsWidget", activeObjectViewWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFontSize"
int GTUtilsOptionPanelPhyTree::getFontSize(HI::GUITestOpStatus& os) {
    return GTSpinBox::getValue(os, "fontSizeSpinBox", getOptionsPanelWidget(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFontSize"
void GTUtilsOptionPanelPhyTree::setFontSize(HI::GUITestOpStatus& os, int fontSize) {
    GTSpinBox::setValue(os, "fontSizeSpinBox", fontSize, getOptionsPanelWidget(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "changeTreeLayout"
void GTUtilsOptionPanelPhyTree::changeTreeLayout(HI::GUITestOpStatus& os, const QString& layoutName) {
    GTComboBox::selectItemByText(os, "layoutCombo", getOptionsPanelWidget(os), layoutName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkBranchDepthScaleMode"
void GTUtilsOptionPanelPhyTree::checkBranchDepthScaleMode(HI::GUITestOpStatus& os, const QString& mode) {
    auto treeViewCombo = GTWidget::findComboBox(os, "treeViewCombo", getOptionsPanelWidget(os));
    CHECK_SET_ERR(mode == treeViewCombo->currentText(), QString("Unexpected mode. Expected: %1, got: %2").arg(mode).arg(treeViewCombo->currentText()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "changeBranchDepthScaleMode"
void GTUtilsOptionPanelPhyTree::changeBranchDepthScaleMode(HI::GUITestOpStatus& os, const QString& mode) {
    GTComboBox::selectItemByText(os, "treeViewCombo", getOptionsPanelWidget(os), mode);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
