/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExtractSelectedAsMSADialogFiller"
#define GT_METHOD_NAME "run"
ExtractSelectedAsMSADialogFiller::ExtractSelectedAsMSADialogFiller(GUITestOpStatus &os,
                                                                   const QString &_filepath,
                                                                   const QStringList &selectedSequenceNameList,
                                                                   int _from,
                                                                   int _to,
                                                                   bool _addToProj,
                                                                   bool _invertButtonPress,
                                                                   bool _allButtonPress,
                                                                   bool _noneButtonPress,
                                                                   bool _dontCheckFilepath,
                                                                   const QString &format,
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

ExtractSelectedAsMSADialogFiller::ExtractSelectedAsMSADialogFiller(GUITestOpStatus &os, CustomScenario *c)
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
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (from) {
        QLineEdit *fromSpin = dialog->findChild<QLineEdit *>("startLineEdit");
        GT_CHECK(fromSpin != nullptr, "fromSpin is NULL")
        GTLineEdit::setText(os, fromSpin, QString::number(from));
    }

    if (to) {
        QLineEdit *toSpin = dialog->findChild<QLineEdit *>("endLineEdit");
        GT_CHECK(toSpin != nullptr, "toSpin is NULL")
        GTLineEdit::setText(os, toSpin, QString::number(to));
    }

    QLineEdit *filepathEdit = dialog->findChild<QLineEdit *>("filepathEdit");
    GT_CHECK(filepathEdit != nullptr, "filepathEdit is NULL");
    GTLineEdit::setText(os, filepathEdit, filepath, dontCheckFilepath);
    GTGlobals::sleep(300);

    if (!useDefaultSequenceSelection) {
        QWidget *noneButton = dialog->findChild<QWidget *>("noneButton");
        GT_CHECK(noneButton != nullptr, "noneButton is NULL");
        GTWidget::click(os, noneButton);
    }

    if (invertButtonPress) {
        GTGlobals::sleep(300);
        QWidget *invertButton = dialog->findChild<QWidget *>("invertButton");
        GT_CHECK(invertButton != nullptr, "invertButton is NULL");
        GTWidget::click(os, invertButton);
    }

    if (allButtonPress) {
        GTGlobals::sleep(300);
        QWidget *allButton = dialog->findChild<QWidget *>("allButton");
        GT_CHECK(allButton != nullptr, "allButton is NULL");
        GTWidget::click(os, allButton);
    }

    if (noneButtonPress) {
        GTGlobals::sleep(300);
        QWidget *noneButton = dialog->findChild<QWidget *>("noneButton");
        GT_CHECK(noneButton != nullptr, "noneButton is NULL");
        GTWidget::click(os, noneButton);
    }

    if (addToProj) {
        GTGlobals::sleep(300);
        QCheckBox *addToProjCheck = dialog->findChild<QCheckBox *>("addToProjBox");
        GT_CHECK(addToProjCheck != nullptr, "addToProjBox is NULL");
        GTCheckBox::setChecked(os, addToProjCheck, addToProj);
    }

    if (!format.isEmpty()) {
        GTGlobals::sleep(300);
        QComboBox *formatCombo = dialog->findChild<QComboBox *>("formatCombo");
        GT_CHECK(formatCombo != nullptr, "formatCombo is null");
        GTComboBox::selectItemByText(os, formatCombo, format);
    }

    if (!useDefaultSequenceSelection) {
        QTableWidget *table = dialog->findChild<QTableWidget *>("sequencesTableWidget");
        GT_CHECK(table != nullptr, "tableWidget is NULL");
        QPoint p = table->geometry().topRight();
        p.setX(p.x() - 2);
        p.setY(p.y() + 2);
        p = dialog->mapToGlobal(p);

        GTMouseDriver::moveTo(p);
        GTMouseDriver::click();
        for (int i = 0; i < table->rowCount(); i++) {
            foreach (QString s, sequenceNameList) {
                QCheckBox *box = qobject_cast<QCheckBox *>(table->cellWidget(i, 0));
                if (s == box->text()) {
                    GT_CHECK(box->isEnabled(), QString("%1 box is disabled").arg(box->text()));
                    box->setChecked(true);
                }
            }
        }
        GTGlobals::sleep();
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedSequences"
QStringList ExtractSelectedAsMSADialogFiller::getSequences(HI::GUITestOpStatus &os, bool selected) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL", QStringList());
    QStringList result;

    QTableWidget *sequencesTableWidget = GTWidget::findExactWidget<QTableWidget *>(os, "sequencesTableWidget", dialog);
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        QCheckBox *box = qobject_cast<QCheckBox *>(sequencesTableWidget->cellWidget(i, 0));
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

}    // namespace U2
