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

#include "primitives/GTMenu.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTTabWidget.h>

#include "GTUtilsWorkflowDesigner.h"

#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "GTTestsWorkflowNameFilter.h"

#include <U2Gui/ToolsMenu.h>
#include <U2Core/U2SafePoints.h>

#include <QTreeWidget>

namespace U2 {

namespace GUITest_common_scenarios_workflow_name_filter {
using namespace HI;

GUI_TEST_CLASS_DEFINITION( test_0001 ) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Open the samples tab.
    QTabWidget* tabs = qobject_cast<QTabWidget*>(GTWidget::findWidget(os, "tabs"));
    GTTabWidget::setCurrentIndex(os,tabs,1);
    //GTWidget::click(os, GTWidget::findWidget(os, "samples"));

    // 3. Click the "Name filter" line edit.
    QWidget *parent = GTWidget::findWidget(os, "palette");
    QLineEdit *nameFilter = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "nameFilterLineEdit", parent));
    CHECK(nameFilter, );
    //hack. GTLineEdit can not set focus on widget. Don't know why
    GTWidget::click(os, nameFilter);
    GTKeyboardDriver::keySequence("HMM");
    GTGlobals::sleep();
    // 4. Write "HMM".

    // Expected: There are two samples after filtering.
    QTreeWidget *samples;
    samples = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "samples"));
    CHECK(samples, );

    int count = 0;
    QList<QTreeWidgetItem*> outerList = samples->findItems("",Qt::MatchContains);
    for (int i = 0; i < outerList.size(); i++) {
        QList<QTreeWidgetItem*> innerList;
        for (int j = 0;j < outerList.value(i)->childCount(); j++){
           innerList.append(outerList.value(i)->child(j));
        }
        foreach (QTreeWidgetItem* item, innerList) {
            if (!item->isHidden()) {
                count++;
            }
        }
    }
    CHECK_SET_ERR(count == 2, QString("Wrong number of visible items in sample tree: expected: 2, actual: %1").arg(count));
}

GUI_TEST_CLASS_DEFINITION( test_0002 ) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Open the samples tab.
    GTUtilsWorkflowDesigner::setCurrentTab(os, GTUtilsWorkflowDesigner::samples);

    // 3. Press Ctrl+F.
    //Expected: the "Name filter" line edit has the focus
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTGlobals::sleep(200);
    // 4. Write "align muscle".
    //Expected: There is the muscle alignment sample after filtering.
    GTKeyboardDriver::keySequence("align muscle");
    GTGlobals::sleep(500);

    // Expected: There are two samples after filtering.
    QTreeWidget *samples;
    samples = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "samples"));
    CHECK(samples, );

    int count = 0;
    QList<QTreeWidgetItem*> outerList = samples->findItems("",Qt::MatchContains);
    for (int i = 0; i < outerList.size(); i++) {
        QList<QTreeWidgetItem*> innerList;
        for (int j = 0;j < outerList.value(i)->childCount(); j++){
            innerList.append(outerList.value(i)->child(j));
        }
        foreach (QTreeWidgetItem* item, innerList) {
            if (!item->isHidden()) {
                count++;
            }
        }
    }
    CHECK_SET_ERR(count == 1, QString("Wrong number of visible items in sample tree with 'align muscle' filter: %1").arg(count));

    //5. Press Esc.
    //Expected: the name filter is clear, all samples are shown.

    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);

    int hiddenItemsCount = 0;
    QList<QTreeWidgetItem*> newOuterList = samples->findItems("",Qt::MatchContains);
    for (int i = 0; i < newOuterList.size(); i++) {
        QList<QTreeWidgetItem*> innerList;
        for (int j = 0;j < newOuterList.value(i)->childCount(); j++){
            innerList.append(newOuterList.value(i)->child(j));
        }
        foreach (QTreeWidgetItem* item, innerList) {
            if (item->isHidden()) {
                hiddenItemsCount++;
            }
        }
    }
    CHECK_SET_ERR(hiddenItemsCount == 0, "Wrong number of visible items in sample tree");
}

GUI_TEST_CLASS_DEFINITION( test_0003 ) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Open the samples tab.
    QTabWidget* tabs = qobject_cast<QTabWidget*>(GTWidget::findWidget(os, "tabs"));
    GTTabWidget::setCurrentIndex(os,tabs,1);

    // 3. Click the "Name filter" line edit.
    QWidget *parent = GTWidget::findWidget(os, "palette");
    QLineEdit *nameFilter = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "nameFilterLineEdit", parent));
    CHECK(nameFilter, );

    // 4. Write "NGS".
    //hack. GTLineEdit can not set focus on widget. Don't know why
    GTWidget::click(os,nameFilter);
    GTKeyboardDriver::keySequence("NGS");
    GTGlobals::sleep();

    // Expected: There are two samples after filtering.
    QTreeWidget *samples;
    samples = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "samples"));
    CHECK(samples, );

    int count = 0;
    QList<QTreeWidgetItem*> outerList = samples->findItems("",Qt::MatchContains);
    for (int i = 0; i < outerList.size(); i++) {
        QList<QTreeWidgetItem*> innerList;
        for (int j = 0;j < outerList.value(i)->childCount(); j++){
            innerList.append(outerList.value(i)->child(j));
        }
        foreach (QTreeWidgetItem* item, innerList) {
            if (!item->isHidden()) {
                count++;
            }
        }
    }
    CHECK_SET_ERR(count == 22, QString("Wrong number of visible items in sample tree: %1").arg(count));
}

}

} // namespace
