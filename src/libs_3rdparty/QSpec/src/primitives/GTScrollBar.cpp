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

#include "primitives/GTScrollBar.h"

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"

namespace HI {
#define GT_CLASS_NAME "GTScrollBar"

QScrollBar* GTScrollBar::getScrollBar(const QString& scrollBarName) {
    return GTWidget::findScrollBar(scrollBarName);
}

void GTScrollBar::pageUp(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getAreaOverSliderPosition(scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_PageUp);
            break;

        default:
            break;
    }
}

void GTScrollBar::pageDown(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getAreaUnderSliderPosition(scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_PageDown);
            break;

        default:
            break;
    }
}

void GTScrollBar::lineUp(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getUpArrowPosition(scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_Up);
            break;

        default:
            break;
    }
}

void GTScrollBar::lineDown(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    switch (useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(GTScrollBar::getDownArrowPosition(scrollbar));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_Down);
            break;

        default:
            break;
    }
}

void GTScrollBar::moveSliderWithMouseUp(QScrollBar* scrollbar, int nPix) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
    QPoint newPosition;
    if (Qt::Horizontal == scrollbar->orientation()) {
        newPosition = QPoint(QCursor::pos().x() + nPix, QCursor::pos().y());
    } else {
        newPosition = QPoint(QCursor::pos().x(), QCursor::pos().y() + nPix);
    }
    GTMouseDriver::press();
    GTMouseDriver::moveTo(newPosition);
    GTMouseDriver::release();
}

void GTScrollBar::moveSliderWithMouseDown(QScrollBar* scrollbar, int nPix) {
    GT_CHECK(scrollbar != NULL, "scrollbar is NULL");
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
    GTMouseDriver::press();
    QPoint newPosition;
    if (Qt::Horizontal == scrollbar->orientation()) {
        newPosition = QPoint(QCursor::pos().x() - nPix, QCursor::pos().y());
    } else {
        newPosition = QPoint(QCursor::pos().x(), QCursor::pos().y() - nPix);
    }
    GTMouseDriver::moveTo(newPosition);
    GTMouseDriver::release();
}

void GTScrollBar::moveSliderWithMouseToValue(QScrollBar* scrollbar, int value) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", );
    QStyleOptionSlider options = initScrollbarOptions(scrollbar);
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

    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(scrollbar->mapToGlobal(newPosition));
    GTMouseDriver::release();
}

void GTScrollBar::moveSliderWithMouseWheelUp(QScrollBar* scrollbar, int nScrolls) {
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
    GTMouseDriver::click();
    GTMouseDriver::scroll(nScrolls);
}

void GTScrollBar::moveSliderWithMouseWheelDown(QScrollBar* scrollbar, int nScrolls) {
    GTMouseDriver::moveTo(GTScrollBar::getSliderPosition(scrollbar));
    GTMouseDriver::click();
    GTMouseDriver::scroll((-1 * nScrolls));  // since scrolling down means negative value for GTMouseDriver::scroll
}

QPoint GTScrollBar::getSliderPosition(QScrollBar* scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(scrollbar);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);
    return scrollbar->mapToGlobal(sliderRect.center());
}

QPoint GTScrollBar::getUpArrowPosition(QScrollBar* scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(scrollbar);
    return scrollbar->mapToGlobal(scrollbar->rect().topLeft() + QPoint(5, 5));
}

int GTScrollBar::getValue(QScrollBar* scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", 0);
    class GetValueScenario : public CustomScenario {
    public:
        GetValueScenario(QScrollBar* _scrollBar, int& _resultValue)
            : scrollBar(_scrollBar), resultValue(_resultValue) {
        }
        void run() override {
            resultValue = scrollBar->value();
        }
        QScrollBar* scrollBar;
        int& resultValue;
    };

    int value = -1;
    GTThread::runInMainThread(new GetValueScenario(scrollbar, value));
    return value;
}

QPoint GTScrollBar::getDownArrowPosition(QScrollBar* scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(scrollbar);
    return scrollbar->mapToGlobal(scrollbar->rect().bottomRight() - QPoint(5, 5));
}

QPoint GTScrollBar::getAreaUnderSliderPosition(QScrollBar* scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);
    QRect underSliderRect;

    if (Qt::Horizontal == scrollbar->orientation()) {
        int underSliderRectWidth = grooveRect.right() - sliderRect.right();
        int underSliderRectHeight = grooveRect.height();
        underSliderRect = QRect(sliderRect.topRight() + QPoint(1, 0), QSize(underSliderRectWidth, underSliderRectHeight));
    } else {
        int underSliderRectWidth = grooveRect.width();
        int underSliderRectHeight = grooveRect.bottom() - sliderRect.bottom();
        underSliderRect = QRect(sliderRect.topRight() + QPoint(1, 1), QSize(underSliderRectWidth, underSliderRectHeight));
    }

    if (underSliderRect.contains(scrollbar->mapFromGlobal(QCursor::pos()))) {
        return QCursor::pos();
    }
    return scrollbar->mapToGlobal(underSliderRect.center());
}

QPoint GTScrollBar::getAreaOverSliderPosition(QScrollBar* scrollbar) {
    GT_CHECK_RESULT(scrollbar != NULL, "scrollbar is NULL", QPoint());
    QStyleOptionSlider options = initScrollbarOptions(scrollbar);
    QRect grooveRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarGroove);
    QRect sliderRect = scrollbar->style()->subControlRect(QStyle::CC_ScrollBar, &options, QStyle::SC_ScrollBarSlider);
    QRect overSliderRect;

    if (Qt::Horizontal == scrollbar->orientation()) {
        int overSliderRectWidth = sliderRect.left() - grooveRect.left();
        int overSliderRectHeight = grooveRect.height();
        overSliderRect = QRect(grooveRect.topLeft(), QSize(overSliderRectWidth, overSliderRectHeight));
    } else {
        int overSliderRectWidth = grooveRect.width();
        int overSliderRectHeight = sliderRect.top() - grooveRect.top();
        overSliderRect = QRect(grooveRect.topLeft(), QSize(overSliderRectWidth, overSliderRectHeight));
    }

    if (overSliderRect.contains(scrollbar->mapFromGlobal(QCursor::pos()))) {
        return QCursor::pos();
    }
    return scrollbar->mapToGlobal(overSliderRect.center() + QPoint(1, 0));
}

QStyleOptionSlider GTScrollBar::initScrollbarOptions(QScrollBar* scrollbar) {
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

#undef GT_CLASS_NAME

}  // namespace HI
// namespace
