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

#include "base_dialogs/ColorDialogFiller.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QSpinBox>
#include <QWidget>

namespace HI{

#define GT_CLASS_NAME "GTUtilsDialog::ColorDialog filler"
#define GT_METHOD_NAME "commonScenario"
void ColorDialogFiller::commonScenario() {
    GTGlobals::sleep();
#ifdef Q_OS_LINUX
    setWithQt = true;
#endif
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog!=NULL, "dialog is NULL");

    if(setWithQt){
        class Scenario : public CustomScenario {
        public:
            Scenario(QColorDialog* _d, QColor _c): d(_d), c(_c) {}
            void run(GUITestOpStatus &os) {
                Q_UNUSED(os);
                d->setCurrentColor(c);
                GTGlobals::sleep();
                d->accept();
            }
        private:
            QColorDialog* d;
            QColor c;
        };

        QColorDialog* d = qobject_cast<QColorDialog*>(dialog);
        GTThread::runInMainThread(os, new Scenario(d, QColor(r, g, b)));
        GTThread::waitForMainThread();
        GTGlobals::sleep(500);
        return;
    }

    QList<QSpinBox*> spinList = dialog->findChildren<QSpinBox*>();
    GTSpinBox::setValue(os,spinList.at(3),r,GTGlobals::UseKeyBoard);
    GTSpinBox::setValue(os,spinList.at(4),g,GTGlobals::UseKeyBoard);
    GTSpinBox::setValue(os,spinList.at(5),b,GTGlobals::UseKeyBoard);

    GTGlobals::sleep();
#ifdef Q_OS_MAC
    GTWidget::click(os, GTWidget::findButtonByText(os, "OK",dialog));
#else
    GTKeyboardDriver::keyClick( Qt::Key_Enter);
#endif
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}
