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

#include <utils/GTUtilsDialog.h>

#include <QApplication>

#include "PosteriorChecks.h"

namespace U2 {
namespace GUITest_posterior_checks {

POSTERIOR_CHECK_DEFINITION(post_check_0000) {
    // Check dialog waiters state
    // Stop dialogs hang checking

    GTUtilsDialog::cleanup(os);
}

POSTERIOR_CHECK_DEFINITION(post_check_0001) {
    // Check there are no modal widgets
    // Check there are no popup widgets

    QWidget *modalWidget = QApplication::activeModalWidget();
    if (NULL != modalWidget) {
        CHECK_SET_ERR(NULL == modalWidget, QString("There is a modal widget after test finish: %1").arg(modalWidget->windowTitle()));
    }

    QWidget *popupWidget = QApplication::activePopupWidget();
    CHECK_SET_ERR(NULL == popupWidget, "There is a popup widget after test finish");
}

}   // namespace GUITest_posterior_checks
}   // namespace U2
