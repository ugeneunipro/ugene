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

#include "SnpEffDatabaseDialogFiller.h"

#include "primitives/GTLineEdit.h"
#include "primitives/GTTableView.h"
#include "primitives/GTWidget.h"

#include <drivers/GTMouseDriver.h>

#include <QApplication>
#include <QTableView>

//#include <U2

namespace U2 {
using namespace HI;

SnpEffDatabaseDialogFiller::SnpEffDatabaseDialogFiller(GUITestOpStatus &os, const QString &dbName, bool dbShouldBeFound)
    : Filler(os, "SnpEffDatabaseDialog"),
      dbName(dbName),
      dbShouldBeFound(dbShouldBeFound) {

}

#define GT_CLASS_NAME "SnpEffDatabaseDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void SnpEffDatabaseDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit* lineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "lineEdit", dialog);
    GT_CHECK(lineEdit, "lineEdit is NULL");
    GTLineEdit::setText(os, lineEdit, dbName, false, true);
    GTGlobals::sleep();

    QTableView* table = dynamic_cast<QTableView*>(GTWidget::findWidget(os, "tableView"));
    GT_CHECK(table, "tableView is NULL");

    QAbstractItemModel* model = table->model();
    GT_CHECK(model, "model is NULL");

    int rowCount = GTTableView::rowCount(os, table);
    int row = -1;
    for (int i = 0; i < rowCount; i++) {
        QModelIndex idx = model->index(i, 0);
        if (model->data(idx).toString() == dbName) {
            row = i;
            break;
        }
    }

    if (dbShouldBeFound) {
        GT_CHECK(row != -1, QString("Genome %1 is not found in the table").arg(dbName));

        GTMouseDriver::moveTo(GTTableView::getCellPoint(os, table, row, 0));
        GTMouseDriver::click();

        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
    } else {
        GT_CHECK(row == -1, QString("Genome %1 is unexpectedly found").arg(dbName));
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
    }
}

} // namespace U2
