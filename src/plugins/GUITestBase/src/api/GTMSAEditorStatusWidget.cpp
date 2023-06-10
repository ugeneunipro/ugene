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

#include <primitives/GTWidget.h>

#include <U2View/MSAEditor.h>
#include <U2View/MsaEditorWgt.h>

#include "GTMSAEditorStatusWidget.h"
#include "GTUtilsMsaEditor.h"

namespace U2 {

#define GT_CLASS_NAME "GTMSAEditorStatusWidget"

#define GT_METHOD_NAME "getStatusWidget"
QWidget* GTMSAEditorStatusWidget::getStatusWidget() {
    MsaEditorWgt* editor = GTUtilsMsaEditor::getEditorUi();
    QWidget* mainUI = editor->getEditor()->getUI();
    return GTWidget::findExactWidget<QWidget*>("msa_editor_status_bar", mainUI);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "length"
int GTMSAEditorStatusWidget::length(QWidget* w) {
    auto label = GTWidget::findLabel("Column", w);

    QString labelText = label->text();
    QString lengthString = labelText.section('/', -1, -1);

    bool ok = false;
    int lengthInt = lengthString.toInt(&ok);
    GT_CHECK_RESULT(ok == true, "toInt returned false", -1);

    return lengthInt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequencesCount"
int GTMSAEditorStatusWidget::getSequencesCount(QWidget* w) {
    auto label = GTWidget::findLabel("Line", w);

    QString labelText = label->text();
    QString countString = labelText.section('/', -1, -1);

    bool ok = false;
    int countInt = countString.toInt(&ok);
    GT_CHECK_RESULT(ok == true, "toInt returned false", -1);

    return countInt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowNumberString"
QString GTMSAEditorStatusWidget::getRowNumberString() {
    auto lineLabel = GTWidget::findLabel("Line", getStatusWidget());

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Seq ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowsCountString"
QString GTMSAEditorStatusWidget::getRowsCountString() {
    auto lineLabel = GTWidget::findLabel("Line", getStatusWidget());

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Seq ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnNumberString"
QString GTMSAEditorStatusWidget::getColumnNumberString() {
    auto columnLabel = GTWidget::findLabel("Column", getStatusWidget());

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("Col ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnsCountString"
QString GTMSAEditorStatusWidget::getColumnsCountString() {
    auto columnLabel = GTWidget::findLabel("Column", getStatusWidget());

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("Col ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceUngappedPositionString"
QString GTMSAEditorStatusWidget::getSequenceUngappedPositionString() {
    auto positionLabel = GTWidget::findLabel("Position", getStatusWidget());

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("Pos ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceUngappedLengthString"
QString GTMSAEditorStatusWidget::getSequenceUngappedLengthString() {
    auto positionLabel = GTWidget::findLabel("Position", getStatusWidget());

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("Pos ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
