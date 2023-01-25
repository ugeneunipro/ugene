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

#include "ExtractSelectedAsMSADialogFiller.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QTableWidget>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExtractSelectedAsMSADialogFiller"
#define GT_METHOD_NAME "run"
ExtractSelectedAsMSADialogFiller::ExtractSelectedAsMSADialogFiller(GUITestOpStatus& os,
                                                                   const QString& _filepath,
                                                                   const QStringList& selectedSequenceNameList,
                                                                   int _from,
                                                                   int _to,
                                                                   bool _addToProj,
                                                                   bool _invertButtonPress,
                                                                   bool _allButtonPress,
                                                                   bool _noneButtonPress,
                                                                   bool _dontCheckFilepath,
                                                                   const QString& format,
                                                                   bool useDefaultSequenceSelectionFlag)
    : Filler(os, "CreateSubalignmentDialog"),
      filepath(_filepath),
      format(format),
      sequenceNameList(selectedSequenceNameList),
      from(_from),
      to(_to),
      addToProj(_addToProj),
      invertButtonPress(_invertButtonPress),
      allButtonPress(_allButtonPress),
      noneButtonPress(_noneButtonPress),
      dontCheckFilepath(_dontCheckFilepath),
      useDefaultSequenceSelection(useDefaultSequenceSelectionFlag) {
}

ExtractSelectedAsMSADialogFiller::ExtractSelectedAsMSADialogFiller(GUITestOpStatus& os, CustomScenario* c)
    : Filler(os, "CreateSubalignmentDialog", c),
      from(0),
      to(0),
      addToProj(false),
      invertButtonPress(false),
      allButtonPress(false),
      noneButtonPress(false),
      dontCheckFilepath(false),
      useDefaultSequenceSelection(false) {
}

void ExtractSelectedAsMSADialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (from) {
        GTLineEdit::setText(os, "startLineEdit", QString::number(from), dialog);
    }

    if (to) {
        GTLineEdit::setText(os, "endLineEdit", QString::number(to), dialog);
    }

    GTLineEdit::setText(os, "filepathEdit", filepath, dialog, dontCheckFilepath);

    if (!useDefaultSequenceSelection) {
        GTWidget::click(os, GTWidget::findWidget(os, "noneButton", dialog));
    }

    if (invertButtonPress) {
        GTWidget::click(os, GTWidget::findWidget(os, "invertButton", dialog));
    }

    if (allButtonPress) {
        GTWidget::click(os, GTWidget::findWidget(os, "allButton", dialog));
    }

    if (noneButtonPress) {
        GTWidget::click(os, GTWidget::findWidget(os, "noneButton", dialog));
    }

    if (addToProj) {
        GTCheckBox::setChecked(os, "addToProjBox", addToProj, dialog);
    }

    if (!format.isEmpty()) {
        GTComboBox::selectItemByText(os, "formatCombo", dialog, format);
    }

    if (!useDefaultSequenceSelection) {
        auto table = GTWidget::findTableWidget(os, "sequencesTableWidget", dialog);
        QPoint p = table->geometry().topRight();
        p.setX(p.x() - 2);
        p.setY(p.y() + 2);
        p = dialog->mapToGlobal(p);

        GTMouseDriver::moveTo(p);
        GTMouseDriver::click();
        for (int i = 0; i < table->rowCount(); i++) {
            for (const QString& sequenceName : qAsConst(sequenceNameList)) {
                auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
                GT_CHECK(box != nullptr, "Not a QCheckBox cell");
                if (sequenceName == box->text()) {
                    GT_CHECK(box->isEnabled(), QString("%1 box is disabled").arg(box->text()));
                    GTWidget::scrollToIndex(os, table, table->model()->index(i, 0));
                    GTCheckBox::setChecked(os, box, true);
                }
            }
        }
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedSequences"
QStringList ExtractSelectedAsMSADialogFiller::getSequences(HI::GUITestOpStatus& os, bool selected) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QStringList result;

    auto sequencesTableWidget = GTWidget::findTableWidget(os, "sequencesTableWidget", dialog);
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        QCheckBox* box = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        GT_CHECK_RESULT(box != nullptr, "cell widget is not checkbox", QStringList());
        if (box->isChecked() == selected) {
            result << box->text();
        }
    }

    return result;
}
#undef GT_METHOD_NAME

void ExtractSelectedAsMSADialogFiller::setUseDefaultSequenceSelection(bool flag) {
    useDefaultSequenceSelection = flag;
}

#undef GT_CLASS_NAME

}  // namespace U2
