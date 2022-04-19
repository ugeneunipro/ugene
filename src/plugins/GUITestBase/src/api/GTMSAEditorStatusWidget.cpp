/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
QWidget *GTMSAEditorStatusWidget::getStatusWidget(GUITestOpStatus &os) {
    MsaEditorWgt *editor = GTUtilsMsaEditor::getEditorUi(os);
    QWidget *mainUI = editor->getEditor()->getUI();
    return GTWidget::findExactWidget<QWidget *>(os, "msa_editor_status_bar", mainUI);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "length"
int GTMSAEditorStatusWidget::length(HI::GUITestOpStatus& os, QWidget* w) {
    auto label = GTWidget::findLabel(os, "Column", w);

    QString labelText = label->text();
    QString lengthString = labelText.section('/', -1, -1);

    bool ok = false;
    int lengthInt = lengthString.toInt(&ok);
    GT_CHECK_RESULT(ok == true, "toInt returned false", -1);

    return lengthInt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequencesCount"
int GTMSAEditorStatusWidget::getSequencesCount(HI::GUITestOpStatus& os, QWidget* w) {
    auto label = GTWidget::findLabel(os, "Line", w);

    QString labelText = label->text();
    QString countString = labelText.section('/', -1, -1);

    bool ok = false;
    int countInt = countString.toInt(&ok);
    GT_CHECK_RESULT(ok == true, "toInt returned false", -1);

    return countInt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowNumberString"
QString GTMSAEditorStatusWidget::getRowNumberString(GUITestOpStatus& os) {
    auto lineLabel = GTWidget::findLabel(os, "Line", getStatusWidget(os));

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Seq ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowsCountString"
QString GTMSAEditorStatusWidget::getRowsCountString(GUITestOpStatus& os) {
    auto lineLabel = GTWidget::findLabel(os, "Line", getStatusWidget(os));

    const QString labelText = lineLabel->text();
    return labelText.mid(QString("Seq ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnNumberString"
QString GTMSAEditorStatusWidget::getColumnNumberString(GUITestOpStatus& os) {
    auto columnLabel = GTWidget::findLabel(os, "Column", getStatusWidget(os));

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("Col ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnsCountString"
QString GTMSAEditorStatusWidget::getColumnsCountString(GUITestOpStatus& os) {
    auto columnLabel = GTWidget::findLabel(os, "Column", getStatusWidget(os));

    const QString labelText = columnLabel->text();
    return labelText.mid(QString("Col ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceUngappedPositionString"
QString GTMSAEditorStatusWidget::getSequenceUngappedPositionString(GUITestOpStatus& os) {
    auto positionLabel = GTWidget::findLabel(os, "Position", getStatusWidget(os));

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("Pos ").length() - 1).section('/', 0, 0).trimmed();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceUngappedLengthString"
QString GTMSAEditorStatusWidget::getSequenceUngappedLengthString(GUITestOpStatus& os) {
    auto positionLabel = GTWidget::findLabel(os, "Position", getStatusWidget(os));

    const QString labelText = positionLabel->text();
    return labelText.mid(QString("Pos ").length() - 1).section('/', 1, 1).trimmed();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
