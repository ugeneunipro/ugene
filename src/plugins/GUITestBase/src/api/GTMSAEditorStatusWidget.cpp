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

#include <primitives/GTWidget.h>

#include <U2View/MsaEditorWgt.h>

#include "GTMSAEditorStatusWidget.h"
#include "GTUtilsMsaEditor.h"

namespace U2 {

#define GT_CLASS_NAME "GTMSAEditorStatusWidget"

#define GT_METHOD_NAME "getStatusWidget"
QWidget *GTMSAEditorStatusWidget::getStatusWidget(GUITestOpStatus &os) {
    QWidget *editor = GTUtilsMsaEditor::getEditorUi(os);
    return GTWidget::findExactWidget<QWidget *>(os, "msa_editor_status_bar", editor);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "length"
int GTMSAEditorStatusWidget::length(HI::GUITestOpStatus& os, QWidget* w) {
    QLabel* label = qobject_cast<QLabel*>(GTWidget::findWidget(os, "Column", w));
    GT_CHECK_RESULT(label != NULL, "label is NULL", -1);

    QString labelText = label->text();
    QString lengthString = labelText.section('/', -1, -1);

    bool ok = false;
    int lengthInt = lengthString.toInt(&ok);
    GT_CHECK_RESULT(ok == true, "toInt returned false", -1);

    return lengthInt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequencesCount"
int GTMSAEditorStatusWidget::getSequencesCount(HI::GUITestOpStatus &os, QWidget *w) {
    QLabel* label = GTWidget::findExactWidget<QLabel *>(os, "Line", w);
    GT_CHECK_RESULT(label != NULL, "label is NULL", -1);

    QString labelText = label->text();
    QString countString = labelText.section('/', -1, -1);

    bool ok = false;
    int countInt = countString.toInt(&ok);
    GT_CHECK_RESULT(ok == true, "toInt returned false", -1);

    return countInt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowNumberString"
QString GTMSAEditorStatusWidget::getRowNumberString(GUITestOpStatus &os) {
    QLabel *lineLabel = GTWidget::findExactWidget<QLabel *>(os, "Line", getStatusWidget(os));
    GT_CHECK_RESULT(lineLabel != NULL, "Line label is NULL", "-1");

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Seq ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowsCountString"
QString GTMSAEditorStatusWidget::getRowsCountString(GUITestOpStatus &os) {
    QLabel *lineLabel = GTWidget::findExactWidget<QLabel *>(os, "Line", getStatusWidget(os));
    GT_CHECK_RESULT(lineLabel != NULL, "Line label is NULL", "-1");

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Seq ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnNumberString"
QString GTMSAEditorStatusWidget::getColumnNumberString(GUITestOpStatus &os) {
    QLabel *columnLabel = GTWidget::findExactWidget<QLabel *>(os, "Column", getStatusWidget(os));
    GT_CHECK_RESULT(columnLabel != NULL, "Column label is NULL", "-1");

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("Col ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnsCountString"
QString GTMSAEditorStatusWidget::getColumnsCountString(GUITestOpStatus &os) {
    QLabel *columnLabel = GTWidget::findExactWidget<QLabel *>(os, "Column", getStatusWidget(os));
    GT_CHECK_RESULT(columnLabel != NULL, "Column label is NULL", "-1");

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("Col ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceUngappedPositionString"
QString GTMSAEditorStatusWidget::getSequenceUngappedPositionString(GUITestOpStatus &os) {
    QLabel *positionLabel = GTWidget::findExactWidget<QLabel *>(os, "Position", getStatusWidget(os));
    GT_CHECK_RESULT(positionLabel != NULL, "Position label is NULL", "-1");

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("Pos ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceUngappedLengthString"
QString GTMSAEditorStatusWidget::getSequenceUngappedLengthString(GUITestOpStatus &os) {
    QLabel *positionLabel = GTWidget::findExactWidget<QLabel *>(os, "Position", getStatusWidget(os));
    GT_CHECK_RESULT(positionLabel != NULL, "Position label is NULL", "-1");

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("Pos ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
