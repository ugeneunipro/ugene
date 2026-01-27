/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <utils/GTUtilsDialog.h>

#include <QApplication>

#include "GTUtilsTaskTreeView.h"
#include "PosteriorChecks.h"

namespace U2 {
namespace GUITest_posterior_checks {

POSTERIOR_CHECK_DEFINITION(post_check_0000) {
    GT_LOG("wait all active waiters to resolve");
    // All unprocessed handlers must be resolved.
    GTUtilsDialog::checkNoActiveWaiters();
    GT_LOG("wait all active waiters to resolve DONE");

    GT_LOG("wait all tasks finished");
    // All active tasks must finish.
    // Allow only short-lived tasks left by the test. Otherwise, wait in the test.
    GTUtilsTaskTreeView::waitTaskFinished(20000);
    GT_LOG("wait all tasks finished DONE");

    // Check there are no modal widgets
    // Check there are no popup widgets
    QWidget* modalWidget = QApplication::activeModalWidget();
    if (modalWidget != nullptr) {
        GT_FAIL("There is a modal widget after test finish: " + modalWidget->windowTitle(), );
    }

    QWidget* popupWidget = QApplication::activePopupWidget();
    if (popupWidget != nullptr) {
        GT_FAIL("There is a popup widget after test finish: " + popupWidget->objectName(), );
    }
}

}  // namespace GUITest_posterior_checks
}  // namespace U2
