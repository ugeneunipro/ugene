/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "primitives/GTSpinBox.h"
#include <utils/GTThread.h>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"
#include "utils//GTKeyboardUtils.h"

namespace HI {

#define GT_CLASS_NAME "GTSpinBox"

int GTSpinBox::getValue(QSpinBox* spinBox) {
    GT_CHECK_RESULT(spinBox != nullptr, "spinBox is NULL", -1);
    return spinBox->value();
}

int GTSpinBox::getValue(const QString& spinBoxName, QWidget* parent) {
    return GTSpinBox::getValue(GTWidget::findSpinBox(spinBoxName, parent));
}

void GTSpinBox::setValue(QSpinBox* spinBox, int v, GTGlobals::UseMethod useMethod) {
    GT_CHECK(spinBox != nullptr, "spinBox is NULL");
    qDebug("GTSpinBox::setValue %d", v);
    if (getValue(spinBox) == v) {
        return;
    }
    GT_CHECK(v <= spinBox->maximum(), QString("value for this spinbox cannot be more then %1").arg(spinBox->maximum()));
    GT_CHECK(v >= spinBox->minimum(), QString("value for this spinbox cannot be less then %1").arg(spinBox->minimum()));

    QPoint arrowPos;
    QRect spinBoxRect;

    GT_CHECK(spinBox->isEnabled(), "SpinBox is not enabled");

    if (spinBox->value() != v) {
        switch (useMethod) {
            case GTGlobals::UseMouse:
                spinBoxRect = spinBox->rect();
                if (v > spinBox->value()) {
                    arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() / 4);  // -5 it's needed that area under cursor was clickable
                } else {
                    arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() * 3 / 4);
                }

                GTMouseDriver::moveTo(spinBox->mapToGlobal(arrowPos));
                while (spinBox->value() != v) {
                    GTMouseDriver::click();
                    GTGlobals::sleep(100);
                }
                break;

            case GTGlobals::UseKey: {
                Qt::Key key;
                if (v > spinBox->value()) {
                    key = Qt::Key_Up;
                } else {
                    key = Qt::Key_Down;
                }

                GTWidget::setFocus(spinBox);
                while (spinBox->value() != v) {
                    GTKeyboardDriver::keyClick(key);
                    GTGlobals::sleep(100);
                }
                break;
            }
            case GTGlobals::UseKeyBoard:
                QString s = QString::number(v);
                GTWidget::setFocus(spinBox);
                GTGlobals::sleep(100);
                GTKeyboardUtils::selectAll();
                GTGlobals::sleep(100);
                GTKeyboardDriver::keyClick(Qt::Key_Backspace);
                GTGlobals::sleep(100);
                GTKeyboardDriver::keySequence(s);
                GTGlobals::sleep(100);
        }
    }
    GTThread::waitForMainThread();
    int currIndex = spinBox->value();
    GT_CHECK(currIndex == v, QString("Can't set index. Expected: %1 actual: %2").arg(v).arg(currIndex));
    GTGlobals::sleep(100);
}

void GTSpinBox::setValue(const QString& spinBoxName, int v, GTGlobals::UseMethod useMethod, QWidget* parent) {
    GTSpinBox::setValue(GTWidget::findSpinBox(spinBoxName, parent), v, useMethod);
}

void GTSpinBox::setValue(const QString& spinBoxName, int v, QWidget* parent) {
    GTSpinBox::setValue(GTWidget::findSpinBox(spinBoxName, parent), v);
}

void GTSpinBox::checkLimits(QSpinBox* spinBox, int min, int max) {
    GT_CHECK(spinBox != NULL, "spinbox is NULL");
    int actualMin = spinBox->minimum();
    int actualMax = spinBox->maximum();
    GT_CHECK(actualMin == min, QString("wrong minimum. Expected: %1, actual: %2").arg(min).arg(actualMin));
    GT_CHECK(actualMax == max, QString("wrong maximum. Expected: %1, actual: %2").arg(max).arg(actualMax));
}

void GTSpinBox::checkLimits(const QString& spinBoxName, int min, int max, QWidget* parent) {
    GTSpinBox::checkLimits(GTWidget::findSpinBox(spinBoxName, parent), min, max);
}

#undef GT_CLASS_NAME

}  // namespace HI
