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

#include "GTUtilsPrimerLibrary.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include <QTableView>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "GTUtilsMdi.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/plugins/pcr/AddPrimerDialogFiller.h"
#include "utils/GTKeyboardUtils.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsPrimerLibrary"

QWidget* GTUtilsPrimerLibrary::openLibrary() {
    GTMenu::clickMainMenuItem({"Tools", "Primer", "Primer library"});
    return GTUtilsMdi::activeWindow();
}

void GTUtilsPrimerLibrary::clickButton(Button button) {
    GTWidget::click(getButton(button));
}

QAbstractButton* GTUtilsPrimerLibrary::getButton(Button button) {
    QDialogButtonBox* box = GTUtilsDialog::buttonBox(GTWidget::findWidget("PrimerLibraryWidget"));
    switch (button) {
        case Add:
            return box->buttons()[1];
        case Edit:
            return box->buttons()[2];
        case Close:
            return box->button(QDialogButtonBox::Close);
        case Remove:
            return box->buttons()[3];
        case Import:
            return box->buttons()[4];
        case Export:
            return box->buttons()[5];
        case Temperature:
            return box->buttons()[6];
        default:
            return nullptr;
    }
}

int GTUtilsPrimerLibrary::librarySize() {
    return GTTableView::rowCount(table());
}

QString GTUtilsPrimerLibrary::getPrimerSequence(int number) {
    return GTTableView::data(table(), number, 4);
}

#define GT_METHOD_NAME "getPrimerSequence"
QString GTUtilsPrimerLibrary::getPrimerSequence(const QString& name) {
    for (int i = 0; i < GTTableView::rowCount(table()); i++) {
        if (name == GTTableView::data(table(), i, 0)) {
            return getPrimerSequence(i);
        }
    }
    GT_CHECK_RESULT(false, QString("Primer with name '%1' not found").arg(name), "");
}
#undef GT_METHOD_NAME

QPoint GTUtilsPrimerLibrary::getPrimerPoint(int number) {
    return GTTableView::getCellPoint(table(), number, 0);
}

void GTUtilsPrimerLibrary::clickPrimer(int number) {
    GTMouseDriver::moveTo(getPrimerPoint(number));
    GTMouseDriver::click();
}

void GTUtilsPrimerLibrary::clearLibrary() {
    int size = librarySize();
    CHECK(size > 0, );
    GTWidget::click(table());
    selectAll();
    clickButton(Remove);
}

void GTUtilsPrimerLibrary::addPrimer(const QString& name, const QString& data) {
    AddPrimerDialogFiller::Parameters parameters;
    parameters.name = name;
    parameters.primer = data;
    GTUtilsDialog::waitForDialog(new AddPrimerDialogFiller(parameters));
    clickButton(Add);
    GTGlobals::sleep(50);
}

#define GT_METHOD_NAME "selectPrimers"
void GTUtilsPrimerLibrary::selectPrimers(const QList<int>& numbers) {
    const int size = librarySize();

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (int number, numbers) {
        GT_CHECK(number < size, "Primer number is out of range");
        GTUtilsPrimerLibrary::clickPrimer(number);
    }
    GTKeyboardDriver::keyClick(Qt::Key_Control);
}
#undef GT_METHOD_NAME

void GTUtilsPrimerLibrary::selectAll() {
    GTWidget::click(table());
    GTKeyboardUtils::selectAll();
}

QTableView* GTUtilsPrimerLibrary::table() {
    return dynamic_cast<QTableView*>(GTWidget::findWidget("primerTable"));
}

#undef GT_CLASS_NAME

}  // namespace U2
