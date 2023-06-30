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

#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QTreeWidget>

#include "DashboardsManagerDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "DashboardsManagerDialogFiller"
void DashboardsManagerDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void DashboardsManagerDialogFiller::selectDashboards(QStringList names) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto listWidget = GTWidget::findTreeWidget("listWidget", dialog);
    foreach (QString name, names) {
        QTreeWidgetItem* item = GTTreeWidget::findItem(listWidget, name);
        GTKeyboardDriver::keyPress(Qt::Key_Control);
        GTTreeWidget::click(item);
        GTKeyboardDriver::keyRelease(Qt::Key_Control);
    }
}

bool DashboardsManagerDialogFiller::isDashboardPresent(QString name) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto listWidget = GTWidget::findTreeWidget("listWidget", dialog);
    QTreeWidgetItem* item = GTTreeWidget::findItem(listWidget, name, nullptr, 0, {false});
    return item != nullptr;
}

QList<QPair<QString, bool>> DashboardsManagerDialogFiller::getDashboardsState() {
    QList<QPair<QString, bool>> result;

    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto treeWidget = GTWidget::findTreeWidget("listWidget", dialog);
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        result << QPair<QString, bool>(item->text(0), Qt::Checked == item->checkState(0));
    }
    return result;
}

#undef GT_CLASS_NAME

}  // namespace U2
