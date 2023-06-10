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

#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include <QLabel>
#include <QTableView>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include "GTUtilsPcr.h"
#include "system/GTFile.h"

namespace U2 {

void GTUtilsPcr::setPrimer(U2Strand::Direction direction, const QByteArray& primer) {
    auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit", primerBox(direction)));
    GTLineEdit::setText(primerEdit, primer, true);
}

void GTUtilsPcr::setMismatches(U2Strand::Direction direction, int mismatches) {
    auto mismatchesSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget("mismatchesSpinBox", primerBox(direction)));
    GTSpinBox::setValue(mismatchesSpinBox, mismatches, GTGlobals::UseKeyBoard);
}

void GTUtilsPcr::setPerfectMatch(int number) {
    auto spinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget("perfectSpinBox"));
    GTSpinBox::setValue(spinBox, number, GTGlobals::UseKeyBoard);
}

void GTUtilsPcr::setMaxProductSize(int number) {
    auto spinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget("productSizeSpinBox"));
    GTSpinBox::setValue(spinBox, number, GTGlobals::UseKeyBoard);
}

void GTUtilsPcr::setUseAmbiguousBases(bool useAmbiguousBases) {
    auto checkBox = GTWidget::findCheckBox("useAmbiguousBasesCheckBox");
    GTCheckBox::setChecked(checkBox, useAmbiguousBases);
}

QWidget* GTUtilsPcr::browseButton(U2Strand::Direction direction) {
    return GTWidget::findWidget("browseButton", primerBox(direction));
}

int GTUtilsPcr::productsCount() {
    return GTTableView::rowCount(getTable());
}

QString GTUtilsPcr::getResultRegion(int number) {
    return GTTableView::data(getTable(), number, 0);
}

QPoint GTUtilsPcr::getResultPoint(int number) {
    return GTTableView::getCellPoint(getTable(), number, 0);
}

QPoint GTUtilsPcr::getDetailsPoint() {
    auto warning = GTWidget::findWidget("detailsLinkLabel");
    QPoint result = warning->geometry().center();
    result.setX(result.x() / 2);
    return warning->parentWidget()->mapToGlobal(result);
}

QString GTUtilsPcr::getPrimerInfo(U2Strand::Direction direction) {
    auto primerInfo = GTWidget::findLabel("characteristicsLabel", GTWidget::findWidget(direction == U2Strand::Direct ? "forwardPrimerBox" : "reversePrimerBox"));
    return primerInfo->text();
}

QWidget* GTUtilsPcr::primerBox(U2Strand::Direction direction) {
    QString boxName = "forwardPrimerBox";
    if (U2Strand::Complementary == direction) {
        boxName = "reversePrimerBox";
    }
    return GTWidget::findWidget(boxName);
}

QTableView* GTUtilsPcr::getTable() {
    return GTWidget::findTableWidget("productsTable");
}

void GTUtilsPcr::clearPcrDir() {
    QString path = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/pcr";
    GTFile::removeDir(path);
}

}  // namespace U2
