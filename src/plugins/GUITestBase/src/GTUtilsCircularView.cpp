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

#include "GTUtilsCircularView.h"

#include <primitives/GTAction.h>
#include <GTGlobals.h>
#include <primitives/GTWidget.h>

#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSingleSequenceWidget.h>


namespace U2{
const QString GTUtilsCv::actionName = "CircularViewAction";

#define GT_CLASS_NAME "GTUtilsCv"


//////////////////////////////////////////////////////////////////////////
// CV common test utils
//////////////////////////////////////////////////////////////////////////
#define GT_METHOD_NAME "cvBtn::isPresent"
bool GTUtilsCv::isCvPresent(HI::GUITestOpStatus &os, ADVSingleSequenceWidget *seqWidget) {
    CHECK_SET_ERR_RESULT(NULL != seqWidget, "NULL sequence widget!", false);

    QString cvWidgetName = "CV_" + seqWidget->objectName();
    QWidget *cvWidget = GTWidget::findWidget(os, cvWidgetName, NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR_RESULT(!os.isCoR(), "Error getting CV widget!", false);

    return NULL != cvWidget;
}
#undef GT_METHOD_NAME


//////////////////////////////////////////////////////////////////////////
// GTUtilsCv::cvBtn
//////////////////////////////////////////////////////////////////////////
#define GT_METHOD_NAME "cvBtn::isPresent"
bool GTUtilsCv::cvBtn::isPresent(HI::GUITestOpStatus &os, ADVSingleSequenceWidget *seqWidget) {
    QAbstractButton *cvButton = getCvButton(os, seqWidget, false);
    CHECK_OP_SET_ERR_RESULT(os, "Error getting CV button!", false);

    return NULL != cvButton;
}
#undef GT_METHOD_NAME


#define GT_METHOD_NAME "cvBtn::isChecked"
bool GTUtilsCv::cvBtn::isChecked(HI::GUITestOpStatus &os, ADVSingleSequenceWidget *seqWidget) {
    QAbstractButton *cvButton = getCvButton(os, seqWidget, true /* CV button must exist */ );

    CHECK_OP_SET_ERR_RESULT(os, "Error getting CV button!", false);
    SAFE_POINT(NULL != cvButton, "cvButton is NULL!", false);

    CHECK_SET_ERR_RESULT(cvButton->isCheckable(), "CV button is not checkable!", false);

    return cvButton->isChecked();
}
#undef GT_METHOD_NAME


#define GT_METHOD_NAME "cvBtn::click"
void GTUtilsCv::cvBtn::click(HI::GUITestOpStatus &os, ADVSingleSequenceWidget* seqWidget) {
    QAbstractButton *cvButton = getCvButton(os, seqWidget, true /* CV button must exist */ );

    CHECK_OP_SET_ERR(os, "Error getting CV button!");
    SAFE_POINT(NULL != cvButton, "cvButton is NULL!",);

    GTWidget::click(os, cvButton);
    CHECK_OP_SET_ERR(os, "Error clicking CV button!");
}
#undef GT_METHOD_NAME


//////////////////////////////////////////////////////////////////////////
// GTUtilsCv::commonCvBtn
//////////////////////////////////////////////////////////////////////////
#define GT_METHOD_NAME "commonCvBtn::mustExist"
void GTUtilsCv::commonCvBtn::mustExist(HI::GUITestOpStatus &os){
    GTWidget::findWidget(os, "globalToggleViewAction_widget");
    CHECK_OP_SET_ERR(os, "Error getting global CV button!");
}
#undef GT_METHOD_NAME


#define GT_METHOD_NAME "commonCvBtn::click"
void GTUtilsCv::commonCvBtn::click(HI::GUITestOpStatus& os){
    QWidget* button = GTWidget::findWidget(os, "globalToggleViewAction_widget");
    CHECK_OP_SET_ERR(os, "Error getting global CV button!");

    if(!button->isVisible()){
        QWidget* ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "mwtoolbar_activemdi"), GTGlobals::FindOptions(false));
        if(ext_button != NULL){
            GTWidget::click(os, ext_button);
        }
        GTGlobals::sleep(500);
    }
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME


////////////////////////////////////////////////////////////////////////
// Helper methods
////////////////////////////////////////////////////////////////////////

#define GT_METHOD_NAME "GTUtilsCv::getCvButton"
QAbstractButton * GTUtilsCv::getCvButton(HI::GUITestOpStatus& os, ADVSingleSequenceWidget* seqWidget, bool setFailedIfNotFound) {
    GT_CHECK_RESULT(NULL != seqWidget, "NULL sequence widget!", NULL)

    QAbstractButton* cvButton = GTAction::button(os, actionName, seqWidget, GTGlobals::FindOptions(setFailedIfNotFound));
    return cvButton;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}

