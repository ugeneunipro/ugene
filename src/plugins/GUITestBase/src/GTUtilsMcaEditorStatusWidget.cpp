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

#include <primitives/GTWidget.h>

#include <U2View/McaEditor.h>

#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorStatusWidget.h"

using namespace HI;

namespace U2 {

#define GT_CLASS_NAME "GTUtilsMcaEditorStatusWidget"

QWidget* GTUtilsMcaEditorStatusWidget::getStatusWidget() {
    QWidget* editor = GTUtilsMcaEditor::getEditorUi();
    return GTWidget::findWidget("mca_editor_status_bar", editor);
}

QString GTUtilsMcaEditorStatusWidget::getRowNumberString() {
    auto lineLabel = GTWidget::findLabel("Line", getStatusWidget());

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Ln ").length() - 1).section('/', 0, 0).trimmed();
}

int GTUtilsMcaEditorStatusWidget::getRowNumber() {
    const QString rowNumberString = getRowNumberString();

    bool ok = false;
    const int rowNumber = rowNumberString.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Row number cannot be converted to int: %1").arg(rowNumberString), -1);

    return rowNumber;
}

QString GTUtilsMcaEditorStatusWidget::getRowsCountString() {
    auto lineLabel = GTWidget::findLabel("Line", getStatusWidget());

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Ln ").length() - 1).section('/', 1, 1).trimmed();
}

int GTUtilsMcaEditorStatusWidget::getRowsCount() {
    const QString rowsCountString = getRowsCountString();

    bool ok = false;
    const int rowsCount = rowsCountString.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Rows count cannot be converted to int: %1").arg(rowsCountString), -1);

    return rowsCount;
}

QString GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString() {
    auto columnLabel = GTWidget::findLabel("Column", getStatusWidget());

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("RefPos ").length() - 1).section('/', 0, 0).trimmed();
}

int GTUtilsMcaEditorStatusWidget::getReferenceUngappedPosition() {
    const QString referencePositionString = getReferenceUngappedPositionString();

    bool ok = false;
    const int referencePosition = referencePositionString.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Reference position cannot be converted to int: %1").arg(referencePositionString), -1);

    return referencePosition;
}

QString GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString() {
    auto columnLabel = GTWidget::findLabel("Column", getStatusWidget());

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("RefPos ").length() - 1).section('/', 1, 1).trimmed();
}

int GTUtilsMcaEditorStatusWidget::getReferenceUngappedLength() {
    const QString referenceLengthString = getReferenceUngappedLengthString();

    bool ok = false;
    const int referenceLength = referenceLengthString.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Reference length cannot be converted to int: %1").arg(referenceLengthString), -1);

    return referenceLength;
}

bool GTUtilsMcaEditorStatusWidget::isGapInReference() {
    return "gap" == getReferenceUngappedPositionString();
}

QString GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString() {
    auto positionLabel = GTWidget::findLabel("Position", getStatusWidget());

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("ReadPos ").length() - 1).section('/', 0, 0).trimmed();
}

int GTUtilsMcaEditorStatusWidget::getReadUngappedPosition() {
    const QString readPositionString = getReadUngappedPositionString();

    bool ok = false;
    const int readPosition = readPositionString.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Read position cannot be converted to int: %1").arg(readPositionString), -1);

    return readPosition;
}

QString GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString() {
    auto positionLabel = GTWidget::findLabel("Position", getStatusWidget());

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("ReadPos ").length() - 1).section('/', 1, 1).trimmed();
}

int GTUtilsMcaEditorStatusWidget::getReadUngappedLength() {
    const QString readPositionString = getReadUngappedLengthString();

    bool ok = false;
    const int readPosition = readPositionString.toInt(&ok);
    GT_CHECK_RESULT(ok, QString("Read position cannot be converted to int: %1").arg(readPositionString), -1);

    return readPosition;
}

bool GTUtilsMcaEditorStatusWidget::isGapInRead() {
    return "gap" == getReadUngappedPositionString();
}

#undef GT_CLASS_NAME

}  // namespace U2
