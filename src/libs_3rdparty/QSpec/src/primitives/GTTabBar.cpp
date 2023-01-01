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

#include <core/CustomScenario.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QToolButton>

#include "primitives/GTTabBar.h"

namespace HI {
#define GT_CLASS_NAME "GTTabBar"

#define GT_METHOD_NAME "setCurrentIndex"
void GTTabBar::setCurrentIndex(GUITestOpStatus& os, QTabBar* tabBar, int index) {
    GT_CHECK(tabBar != nullptr, "QTabBar is null");

    // TODO: set index by mouse/keyboard
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QTabBar* tabBar, int index)
            : CustomScenario(), tabBar(tabBar), index(index) {
        }
        void run(HI::GUITestOpStatus& os) {
            if (tabBar->currentIndex() == index) {
                return;
            }
            int tabsCount = tabBar->count();
            GT_CHECK(index >= 0 && index < tabsCount, "invalid index");
            tabBar->setCurrentIndex(index);
        }
        QTabBar* tabBar;
        int index;
    };

    GTThread::runInMainThread(os, new MainThreadAction(tabBar, index));
    GTThread::waitForMainThread();

    int currIndex = tabBar->currentIndex();
    GT_CHECK(currIndex == index, "Can't set index");
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
