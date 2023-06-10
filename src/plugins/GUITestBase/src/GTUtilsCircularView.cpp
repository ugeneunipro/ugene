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

#include "GTUtilsCircularView.h"
#include <GTGlobals.h>
#include <primitives/GTAction.h>
#include <primitives/GTWidget.h>

#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSingleSequenceWidget.h>

namespace U2 {
const QString GTUtilsCv::actionName = "CircularViewAction";

#define GT_CLASS_NAME "GTUtilsCv"

//////////////////////////////////////////////////////////////////////////
// CV common test utils
//////////////////////////////////////////////////////////////////////////
#define GT_METHOD_NAME "cvBtn::isPresent"
bool GTUtilsCv::isCvPresent(ADVSingleSequenceWidget* seqWidget) {
    CHECK_SET_ERR_RESULT(seqWidget != nullptr, "NULL sequence widget!", false);

    QString cvWidgetName = "CV_" + seqWidget->objectName();
    auto cvWidget = GTWidget::findWidget(cvWidgetName, nullptr, {false});
    return cvWidget != nullptr;
}
#undef GT_METHOD_NAME

//////////////////////////////////////////////////////////////////////////
// GTUtilsCv::cvBtn
//////////////////////////////////////////////////////////////////////////
#define GT_METHOD_NAME "cvBtn::isPresent"
bool GTUtilsCv::cvBtn::isPresent(ADVSingleSequenceWidget* seqWidget) {
    QAbstractButton* cvButton = getCvButton(seqWidget, false);
    return cvButton != nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "cvBtn::isChecked"
bool GTUtilsCv::cvBtn::isChecked(ADVSingleSequenceWidget* seqWidget) {
    QAbstractButton* cvButton = getCvButton(seqWidget, true /* CV button must exist */);

    SAFE_POINT(cvButton != nullptr, "cvButton is NULL!", false);

    CHECK_SET_ERR_RESULT(cvButton->isCheckable(), "CV button is not checkable!", false);

    return cvButton->isChecked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "cvBtn::click"
void GTUtilsCv::cvBtn::click(ADVSingleSequenceWidget* seqWidget) {
    QAbstractButton* cvButton = getCvButton(seqWidget, true /* CV button must exist */);
    SAFE_POINT(nullptr != cvButton, "cvButton is NULL!", );
    GTWidget::click(cvButton);
}
#undef GT_METHOD_NAME

//////////////////////////////////////////////////////////////////////////
// GTUtilsCv::commonCvBtn
//////////////////////////////////////////////////////////////////////////
#define GT_METHOD_NAME "commonCvBtn::mustExist"
void GTUtilsCv::commonCvBtn::mustExist() {
    GTWidget::findWidget("globalToggleViewAction_widget");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "commonCvBtn::click"
void GTUtilsCv::commonCvBtn::click() {
    auto button = GTWidget::findWidget("globalToggleViewAction_widget");

    if (!button->isVisible()) {
        auto ext_button = GTWidget::findWidget("qt_toolbar_ext_button", GTWidget::findWidget("mwtoolbar_activemdi"), {false});
        if (ext_button != nullptr) {
            GTWidget::click(ext_button);
        }
        GTGlobals::sleep(500);
    }
    GTWidget::click(button);
}
#undef GT_METHOD_NAME

////////////////////////////////////////////////////////////////////////
// Helper methods
////////////////////////////////////////////////////////////////////////

#define GT_METHOD_NAME "GTUtilsCv::getCvButton"
QAbstractButton* GTUtilsCv::getCvButton(ADVSingleSequenceWidget* seqWidget, bool setFailedIfNotFound) {
    GT_CHECK_RESULT(nullptr != seqWidget, "NULL sequence widget!", nullptr)

    QAbstractButton* cvButton = GTAction::button(actionName, seqWidget, GTGlobals::FindOptions(setFailedIfNotFound));
    return cvButton;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
