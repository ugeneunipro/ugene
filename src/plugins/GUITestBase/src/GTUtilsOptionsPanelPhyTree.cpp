/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

QWidget* GTUtilsOptionPanelPhyTree::openTab() {
    QWidget* activeObjectViewWindow = GTUtilsMdi::getActiveObjectViewWindow(TreeViewerFactory::ID);
    QWidget* tabButton = GTWidget::findWidget("OP_TREES_WIDGET", activeObjectViewWindow);
    GTWidget::click(tabButton);
    return getOptionsPanelWidget();
}

QWidget* GTUtilsOptionPanelPhyTree::getOptionsPanelWidget() {
    QWidget* activeObjectViewWindow = GTUtilsMdi::getActiveObjectViewWindow(TreeViewerFactory::ID);
    return GTWidget::findWidget("TreeOptionsWidget", activeObjectViewWindow);
}

int GTUtilsOptionPanelPhyTree::getFontSize() {
    return GTSpinBox::getValue("fontSizeSpinBox", getOptionsPanelWidget());
}

void GTUtilsOptionPanelPhyTree::setFontSize(int fontSize) {
    GTSpinBox::setValue("fontSizeSpinBox", fontSize, getOptionsPanelWidget());
}

void GTUtilsOptionPanelPhyTree::changeTreeLayout(const QString& layoutName) {
    GTComboBox::selectItemByText("layoutCombo", getOptionsPanelWidget(), layoutName);
}

void GTUtilsOptionPanelPhyTree::checkBranchDepthScaleMode(const QString& mode) {
    auto treeViewCombo = GTWidget::findComboBox("treeViewCombo", getOptionsPanelWidget());
    CHECK_SET_ERR(mode == treeViewCombo->currentText(), QString("Unexpected mode. Expected: %1, got: %2").arg(mode).arg(treeViewCombo->currentText()));
}

void GTUtilsOptionPanelPhyTree::changeBranchDepthScaleMode(const QString& mode) {
    GTComboBox::selectItemByText("treeViewCombo", getOptionsPanelWidget(), mode);
}

#undef GT_CLASS_NAME
}  // namespace U2
