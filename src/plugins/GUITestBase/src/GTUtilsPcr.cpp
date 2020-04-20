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

#include <QLabel>
#include <QTableView>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include "system/GTFile.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include "GTUtilsPcr.h"

namespace U2 {

void GTUtilsPcr::setPrimer(HI::GUITestOpStatus &os, U2Strand::Direction direction, const QByteArray &primer) {
    QLineEdit *primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget(os, "primerEdit", primerBox(os, direction)));
    GTLineEdit::setText(os, primerEdit, primer, true);
}

void GTUtilsPcr::setMismatches(HI::GUITestOpStatus &os, U2Strand::Direction direction, int mismatches) {
    QSpinBox *mismatchesSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget(os, "mismatchesSpinBox", primerBox(os, direction)));
    GTSpinBox::setValue(os, mismatchesSpinBox, mismatches, GTGlobals::UseKeyBoard);
}

void GTUtilsPcr::setPerfectMatch(HI::GUITestOpStatus &os, int number) {
    QSpinBox *spinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget(os, "perfectSpinBox"));
    GTSpinBox::setValue(os, spinBox, number, GTGlobals::UseKeyBoard);
}

void GTUtilsPcr::setMaxProductSize(HI::GUITestOpStatus &os, int number) {
    QSpinBox *spinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget(os, "productSizeSpinBox"));
    GTSpinBox::setValue(os, spinBox, number, GTGlobals::UseKeyBoard);
}

QWidget * GTUtilsPcr::browseButton(HI::GUITestOpStatus &os, U2Strand::Direction direction) {
    return GTWidget::findWidget(os, "browseButton", primerBox(os, direction));
}

int GTUtilsPcr::productsCount(HI::GUITestOpStatus &os) {
    return GTTableView::rowCount(os, table(os));
}

QString GTUtilsPcr::getResultRegion(HI::GUITestOpStatus &os, int number) {
    return GTTableView::data(os, table(os), number, 0);
}

QPoint GTUtilsPcr::getResultPoint(HI::GUITestOpStatus &os, int number) {
    return GTTableView::getCellPoint(os, table(os), number, 0);
}

QPoint GTUtilsPcr::getDetailsPoint(HI::GUITestOpStatus &os) {
    QWidget *warning = GTWidget::findWidget(os, "detailsLinkLabel");
    QPoint result = warning->geometry().center();
    result.setX(result.x()/2);
    return warning->parentWidget()->mapToGlobal(result);
}

QString GTUtilsPcr::getPrimerInfo(GUITestOpStatus &os, U2Strand::Direction direction) {
    QLabel* primerInfo = GTWidget::findExactWidget<QLabel*>(os, "characteristicsLabel",
                                                            GTWidget::findWidget(os, direction == U2Strand::Direct ? "forwardPrimerBox" : "reversePrimerBox"));
    CHECK_SET_ERR_RESULT(primerInfo != NULL, "Cannot find primer info label", QString());
    return primerInfo->text();
}

QWidget * GTUtilsPcr::primerBox(HI::GUITestOpStatus &os, U2Strand::Direction direction) {
    QString boxName = "forwardPrimerBox";
    if (U2Strand::Complementary == direction) {
        boxName = "reversePrimerBox";
    }
    return GTWidget::findWidget(os, boxName);
}

QTableView * GTUtilsPcr::table(HI::GUITestOpStatus &os) {
    return dynamic_cast<QTableView*>(GTWidget::findWidget(os, "productsTable"));
}

void GTUtilsPcr::clearPcrDir(HI::GUITestOpStatus &os){
    Q_UNUSED(os);
    QString path = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/pcr";
    GTFile::removeDir(path);
}

} // U2
