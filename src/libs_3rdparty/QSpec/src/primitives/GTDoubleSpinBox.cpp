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
#include "primitives/GTDoubleSpinBox.h"
#include "primitives/GTWidget.h"

#include <utils/GTThread.h>

namespace HI {

#define GT_CLASS_NAME "GTDoubleSpinBox"

#define GT_METHOD_NAME "getValue"
int GTDoubleSpinbox::getValue(GUITestOpStatus &os, QDoubleSpinBox *spinBox) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(spinBox != NULL, "spinBox is NULL", -1);
    return spinBox->value();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getValue"
int GTDoubleSpinbox::getValue(GUITestOpStatus &os, const QString &spinBoxName, QWidget *parent) {
    return GTDoubleSpinbox::getValue(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, spinBoxName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setValue"
void GTDoubleSpinbox::setValue(GUITestOpStatus& os, QDoubleSpinBox *spinBox, double v, GTGlobals::UseMethod useMethod) {
    GT_CHECK(spinBox != NULL, "spinBox is NULL");
    if(spinBox->value() == v){
        return;
    }

    QPoint arrowPos;
    QRect spinBoxRect;

    GT_CHECK(spinBox->isEnabled(), "SpinBox is not enabled");

    if (spinBox->value() != v) {
        switch(useMethod) {
        case GTGlobals::UseMouse:
            spinBoxRect = spinBox->rect();
            if (v > spinBox->value()) {
                arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() / 4); // -5 it's needed that area under cursor was clickable
            } else {
                arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() * 3 / 4);
            }

            GTMouseDriver::moveTo(spinBox->mapToGlobal(arrowPos));
            while (QString().setNum(spinBox->value()) != QString().setNum(v)) {
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

            GTWidget::setFocus(os, spinBox);
            while (QString().setNum(spinBox->value()) != QString().setNum(v)) {
                GTKeyboardDriver::keyClick( key);
                GTGlobals::sleep(100);
            }
            break;
        }
        case GTGlobals::UseKeyBoard:
            QString s = QString::number(v);
            GTWidget::setFocus(os, spinBox);

            GTGlobals::sleep(100);
            GTKeyboardDriver::keyClick( Qt::Key_Up);
            GTGlobals::sleep(100);
            GTKeyboardDriver::keyClick( Qt::Key_Down);
            GTKeyboardDriver::keySequence(s);
            GTGlobals::sleep(100);

        }
    }

    GTThread::waitForMainThread();
    double currIndex = spinBox->value();
    GT_CHECK(QString().setNum(currIndex) == QString().setNum(v), "Expected: " + QString().setNum(v) + " Found: " + QString().setNum(currIndex));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setValue"
void GTDoubleSpinbox::setValue(GUITestOpStatus &os, const QString &spinBoxName, int v, GTGlobals::UseMethod useMethod, QWidget *parent) {
    GTDoubleSpinbox::setValue(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, spinBoxName, parent), v, useMethod);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setValue"
void GTDoubleSpinbox::setValue(GUITestOpStatus &os, const QString &spinBoxName, int v, QWidget *parent) {
    GTDoubleSpinbox::setValue(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, spinBoxName, parent), v);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkLimits"
void GTDoubleSpinbox::checkLimits(GUITestOpStatus &os, QDoubleSpinBox *spinBox, double min, double max){
    GT_CHECK(spinBox != NULL, "spinbox is NULL");
    int actualMin = spinBox->minimum();
    int actualMax = spinBox->maximum();
    GT_CHECK(actualMin == min, QString("wrong minimum. Expected: %1, actual: 2").arg(min).arg(actualMin));
    GT_CHECK(actualMax == max, QString("wrong maximum. Expected: %1, actual: 2").arg(max).arg(actualMax));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkLimits"
void GTDoubleSpinbox::checkLimits(GUITestOpStatus &os, const QString &spinBoxName, int min, int max, QWidget *parent){
    GTDoubleSpinbox::checkLimits(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, spinBoxName, parent), min, max);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
