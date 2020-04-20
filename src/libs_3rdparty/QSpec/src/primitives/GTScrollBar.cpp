/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTScrollBar.h"
#include "primitives/GTWidget.h"

namespace HI {
#define GT_CLASS_NAME "GTScrollBar"

#define GT_METHOD_NAME "getScrollBar"
QScrollBar *GTScrollBar::getScrollBar(GUITestOpStatus &os, const QString &scrollBarName) {
    return GTWidget::findExactWidget<QScrollBar *>(os, scrollBarName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pageUp"
void GTScrollBar::pageUp(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getAreaOverSliderPosition(os, scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick( Qt::Key_PageUp);
            break;

        default:
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pageDown"
void GTScrollBar::pageDown(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getAreaUnderSliderPosition(os, scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick( Qt::Key_PageDown);
            break;

        default:
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "lineUp"
void GTScrollBar::lineUp(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getUpArrowPosition(os, scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick( Qt::Key_Up);
            break;

        default:
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "lineDown"
void GTScrollBar::lineDown(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getDownArrowPosition(os, scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick( Qt::Key_Down);
            break;

        default:
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveSliderWithMouseUp"
void GTScrollBar::moveSliderWithMouseUp(GUITestOpStatus &os, QScrollBar *scrollbar, int nPix) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
    QPoint newPosition;
    if (Qt::Horizontal == scrollbar->orientation()) {
        newPosition = QPoint(QCursor::pos().x() + nPix, QCursor::pos().y());
    }
    else {
        newPosition = QPoint(QCursor::pos().x(), QCursor::pos().y() + nPix);
    }
    GTMouseDriver::press();
    GTMouseDriver::moveTo(newPosition);
    GTMouseDriver::release();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveSliderWithMouseDown"
void GTScrollBar::moveSliderWithMouseDown(GUITestOpStatus &os, QScrollBar *scrollbar, int nPix) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
    GTMouseDriver::press();
    QPoint newPosition;
    if (Qt::Horizontal == scrollbar->orientation()) {
        newPosition = QPoint(QCursor::pos().x() - nPix , QCursor::pos().y());
    }
    else {
        newPosition = QPoint(QCursor::pos().x(), QCursor::pos().y() - nPix);
    }
    GTMouseDriver::moveTo(newPosition);
    GTMouseDriver::release();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveSliderWithMouseToValue"
void GTScrollBar::moveSliderWithMouseToValue(GUITestOpStatus &os, QScrollBar *scrollbar, int value) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", );
    QStyleOptionSlider options = initScrollbarOptions(os, scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);

    value = qBound(0, value, scrollbar->maximum());
    QPoint newPosition;
    switch (scrollbar->orientation()) {
    case Qt::Horizontal: {
        int newPositionX = qBound((sliderRect.width() - 1) / 2,
                                  (sliderRect.width() - 1) / 2 + (int)((double)value * (grooveRect.width() - sliderRect.width()) / scrollbar->maximum()),
                                  grooveRect.width() - sliderRect.width() / 2);
        newPosition = QPoint(grooveRect.x() + newPositionX, grooveRect.height() / 2);
        break;
    }
    case Qt::Vertical: {
        int newPositionY = qBound((sliderRect.height() - 1) / 2,
                                  (sliderRect.height() - 1) / 2 + (int)((double)value * (grooveRect.height() - sliderRect.height()) / scrollbar->maximum()),
                                  grooveRect.height() - sliderRect.height() / 2);
        newPosition = QPoint(grooveRect.width() / 2, grooveRect.y() + newPositionY);
        break;
    }
    }

    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(scrollbar->mapToGlobal(newPosition));
    GTMouseDriver::release();
}
#undef GT_METHOD_NAME

void GTScrollBar::moveSliderWithMouseWheelUp(GUITestOpStatus &os, QScrollBar *scrollbar, int nScrolls) {
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
    GTMouseDriver::click();
    GTMouseDriver::scroll(nScrolls);
}

void GTScrollBar::moveSliderWithMouseWheelDown(GUITestOpStatus &os, QScrollBar *scrollbar, int nScrolls) {
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(os, scrollbar));
    GTMouseDriver::click();
    GTMouseDriver::scroll((-1 * nScrolls)); //since scrolling down means negative value for GTMouseDriver::scroll
}

#define GT_METHOD_NAME "getSliderPosition"
QPoint GTScrollBar::getSliderPosition(GUITestOpStatus &os, QScrollBar *scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(os, scrollbar);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);
    return scrollbar->mapToGlobal(sliderRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getUpArrowPosition"
QPoint GTScrollBar::getUpArrowPosition(GUITestOpStatus &os, QScrollBar *scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(os, scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);

    return scrollbar->mapToGlobal(scrollbar->rect().topLeft() + QPoint(5, 5));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDownArrowPosition"
QPoint GTScrollBar::getDownArrowPosition(GUITestOpStatus &os, QScrollBar *scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(os, scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);

    return scrollbar->mapToGlobal(scrollbar->rect().bottomRight() - QPoint(5, 5));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAreaUnderSliderPosition"
QPoint GTScrollBar::getAreaUnderSliderPosition(GUITestOpStatus &os, QScrollBar *scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(os, scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);
    QRect underSliderRect;

    if (Qt::Horizontal == scrollbar->orientation()) {
        int underSliderRectWidth = grooveRect.right() - sliderRect.right();
        int underSliderRectHeight = grooveRect.height();
        underSliderRect = QRect(sliderRect.topRight() + QPoint(1, 0), QSize(underSliderRectWidth, underSliderRectHeight));
    }
    else {
        int underSliderRectWidth = grooveRect.width();
        int underSliderRectHeight = grooveRect.bottom() - sliderRect.bottom();
        underSliderRect = QRect(sliderRect.topRight() + QPoint(1, 1), QSize(underSliderRectWidth, underSliderRectHeight));
    }

    if (underSliderRect.contains(scrollbar->mapFromGlobal(QCursor::pos()))) {
        return QCursor::pos();
    }
    return scrollbar->mapToGlobal(underSliderRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAreaOverSliderPosition"
QPoint GTScrollBar::getAreaOverSliderPosition(GUITestOpStatus &os, QScrollBar *scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(os, scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);
    QRect overSliderRect;

    if (Qt::Horizontal == scrollbar->orientation()) {
        int overSliderRectWidth = sliderRect.left() - grooveRect.left();
        int overSliderRectHeight = grooveRect.height();
        overSliderRect = QRect(grooveRect.topLeft(), QSize(overSliderRectWidth, overSliderRectHeight));
    }
    else {
        int overSliderRectWidth = grooveRect.width();
        int overSliderRectHeight = sliderRect.top() - grooveRect.top();
        overSliderRect = QRect(grooveRect.topLeft(), QSize(overSliderRectWidth, overSliderRectHeight));
    }

    if (overSliderRect.contains(scrollbar->mapFromGlobal(QCursor::pos()))) {
        return QCursor::pos();
    }
    return scrollbar->mapToGlobal(overSliderRect.center() + QPoint(1,0));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "initScrollbarOptions"
QStyleOptionSlider GTScrollBar::initScrollbarOptions(GUITestOpStatus &os, QScrollBar *scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QStyleOptionSlider());
    QStyleOptionSlider options;
    options.initFrom(scrollbar);
    options.sliderPosition = scrollbar->sliderPosition();
    options.maximum = scrollbar->maximum();
    options.minimum = scrollbar->minimum();
    options.singleStep = scrollbar->singleStep();
    options.pageStep = scrollbar->pageStep();
    options.orientation = scrollbar->orientation();
    options.sliderValue = options.sliderPosition;
    options.upsideDown = false;
    options.state = QStyle::State_Sunken | QStyle::State_Enabled;

    return options;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
// namespace
