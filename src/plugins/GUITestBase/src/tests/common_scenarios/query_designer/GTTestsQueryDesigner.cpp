/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "GTTestsQueryDesigner.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include <QGraphicsItem>

#include <U2Gui/ToolsMenu.h>

#include "GTGlobals.h"
#include "GTUtilsQueryDesigner.h"

namespace U2 {

namespace GUITest_common_scenarios_query_designer {
using namespace HI;

void test1(QString s = "") {
    // Bug: QD: Crash while resizing and deleting elements (0002402)
    // 1. Open Query Designer
    GTUtilsQueryDesigner::openQueryDesigner();

    QString array[] = {"CDD", "Base Content", "HMM2", "ORF"};
    QPoint p;
    // 2. Add any algorithm to the scheme
    for (int i = 0; i < 4; i++) {
        // 3. Reduce any elements size by dragging its right corner as far to the left as possible
        //   (or a bit less than as far as possible)
        if (s == "arr") {
            GTUtilsQueryDesigner::addAlgorithm(array[i]);

            GTMouseDriver::moveTo(GTUtilsQueryDesigner::getItemCenter(array[i]));
            p = GTMouseDriver::getMousePosition();
            p.setX(GTUtilsQueryDesigner::getItemRight(array[i]));
        } else {
            GTUtilsQueryDesigner::addAlgorithm(s);

            GTMouseDriver::moveTo(GTUtilsQueryDesigner::getItemCenter(s));
            p = GTMouseDriver::getMousePosition();
            p.setX(GTUtilsQueryDesigner::getItemRight(s));
        }
        GTMouseDriver::moveTo(p);

        GTMouseDriver::press();
        if (s == "arr") {
            p.setX(GTUtilsQueryDesigner::getItemLeft(array[i]) + 100);
        } else {
            p.setX(GTUtilsQueryDesigner::getItemLeft(s) + 100);
        }
        GTMouseDriver::moveTo(p);

        GTMouseDriver::release();

        // 4. Select the resized element and press <Del>
        if (s == "arr") {
            GTMouseDriver::moveTo(GTUtilsQueryDesigner::getItemCenter(array[i]));
        } else {
            GTMouseDriver::moveTo(GTUtilsQueryDesigner::getItemCenter(s));
        }
        GTMouseDriver::click();
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
        // check no elements on scene
        auto sceneView = GTWidget::findGraphicsView("sceneView");
        QList<QGraphicsItem*> items = sceneView->items();
        CHECK_SET_ERR(items.size() == 2, "Delete shortcut is not working");  // 2 - is equal empty scene
    }
    // 5. repeat from step 2 (do 4 iterations)
    // Expected state: UGENE not crashes.
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    test1("Pattern");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    test1("CDD");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    test1("arr");
}

}  // namespace GUITest_common_scenarios_query_designer
}  // namespace U2
