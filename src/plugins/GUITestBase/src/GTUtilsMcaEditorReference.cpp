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

#include <GTGlobals.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTScrollBar.h>

#include <U2Core/U2SafePoints.h>

#include <U2View/BaseWidthController.h>
#include <U2View/McaEditorReferenceArea.h>
#include <U2View/McaEditorSequenceArea.h>
#include <U2View/ScrollController.h>

#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorReference.h"

using namespace HI;
namespace U2 {

#define GT_CLASS_NAME "GTUtilsMcaEditorReference"

#define GT_METHOD_NAME "clickToPosition"
void GTUtilsMcaEditorReference::clickToPosition(int position) {
    McaEditorReferenceArea* referenceArea = GTUtilsMcaEditor::getReferenceArea();
    McaEditorWgt* mcaEditorWgt = GTUtilsMcaEditor::getEditorUi();
    GT_CHECK(mcaEditorWgt->getSequenceArea()->isInRange(QPoint(position, 0)), QString("Position %1 is out of range").arg(position));

    scrollToPosition(position);

    const QPoint positionCenter(mcaEditorWgt->getBaseWidthController()->getBaseScreenCenter(position),
                                referenceArea->height() / 2);
    GT_CHECK(referenceArea->rect().contains(positionCenter, false), QString("Position %1 is not visible").arg(position));

    GTMouseDriver::moveTo(referenceArea->mapToGlobal(positionCenter));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToPosition"
void GTUtilsMcaEditorReference::scrollToPosition(int position) {
    const int scrollBarValue = GTUtilsMcaEditor::getEditorUi()->getBaseWidthController()->getBaseGlobalRange(position).center() -
                               GTUtilsMcaEditor::getEditorUi()->getSequenceArea()->width() / 2;
    CHECK(!GTUtilsMcaEditor::getReferenceArea()->getVisibleRange().contains(position), );
    GTScrollBar::moveSliderWithMouseToValue(GTUtilsMcaEditor::getHorizontalScrollBar(),
                                            scrollBarValue);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
