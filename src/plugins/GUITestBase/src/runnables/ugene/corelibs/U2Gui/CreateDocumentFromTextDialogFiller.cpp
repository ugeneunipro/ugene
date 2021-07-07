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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>

#include <U2Core/Log.h>

#include "CreateDocumentFromTextDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::createDocumentFiller"
CreateDocumentFiller::CreateDocumentFiller(HI::GUITestOpStatus &_os,
                                           const QString &_pasteDataHere,
                                           bool _customSettings = false,
                                           documentAlphabet _alphabet = StandardDNA,
                                           bool _skipUnknownSymbols = true,
                                           bool _replaceUnknownSymbols = false,
                                           const QString _symbol = "",
                                           const QString &_documentLocation = QString(),
                                           documentFormat _format = FASTA,
                                           const QString &_sequenceName = QString(),
                                           bool saveFile = false,
                                           GTGlobals::UseMethod method)
    : Filler(_os, "CreateDocumentFromTextDialog"),
      customSettings(_customSettings),
      alphabet(_alphabet),
      skipUnknownSymbols(_skipUnknownSymbols),
      replaceUnknownSymbols(_replaceUnknownSymbols),
      symbol(_symbol),
      format(_format),
      saveFile(saveFile),
      useMethod(method) {
    sequenceName = _sequenceName;
    pasteDataHere = _pasteDataHere;
    documentLocation = GTFileDialog::toAbsoluteNativePath(_documentLocation);
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxAlphabetItems[StandardDNA] = "Standard DNA";
    comboBoxAlphabetItems[StandardRNA] = "Standard RNA";
    comboBoxAlphabetItems[ExtendedDNA] = "Extended DNA";
    comboBoxAlphabetItems[ExtendedRNA] = "Extended RNA";
    comboBoxAlphabetItems[StandardAmino] = "Standard amino";
    comboBoxAlphabetItems[AllSymbols] = "Raw";
}

CreateDocumentFiller::CreateDocumentFiller(HI::GUITestOpStatus &os, CustomScenario *scenario)
    : Filler(os, "CreateDocumentFromTextDialog", scenario),
      customSettings(false),
      alphabet(StandardDNA),
      skipUnknownSymbols(false),
      replaceUnknownSymbols(false),
      format(FASTA),
      saveFile(false),
      useMethod(GTGlobals::UseMouse) {
}

#define GT_METHOD_NAME "commonScenario"
void CreateDocumentFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != nullptr, "dialog not found");

    QPlainTextEdit *plainText = dialog->findChild<QPlainTextEdit *>("sequenceEdit");
    GT_CHECK(plainText != nullptr, "plain text not found");
    GTPlainTextEdit::setPlainText(os, plainText, pasteDataHere);
    GTGlobals::sleep();

    if (customSettings) {
        QGroupBox *customSettingsCheckBox = qobject_cast<QGroupBox *>(GTWidget::findWidget(os, "groupBox", dialog));
        GTGroupBox::setChecked(os, customSettingsCheckBox, true);

        QComboBox *alphabetComboBox = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "alphabetBox", dialog));
        GT_CHECK(alphabetComboBox != nullptr, "ComboBox not found");

        if (skipUnknownSymbols) {
            QRadioButton *skipUnknownSymbols = qobject_cast<QRadioButton *>(GTWidget::findWidget(os, "skipRB", dialog));
            skipUnknownSymbols->setChecked(true);
        } else if (replaceUnknownSymbols) {
            QRadioButton *replaceUnknownSymbols = qobject_cast<QRadioButton *>(GTWidget::findWidget(os, "replaceRB", dialog));
            replaceUnknownSymbols->setChecked(true);

            QLineEdit *lineEdit = dialog->findChild<QLineEdit *>("symbolToReplaceEdit");
            GT_CHECK(lineEdit != nullptr, "line edit not found");
            GTLineEdit::setText(os, lineEdit, symbol);
        } else {
            assert(false);    // replase skipUnknownSymbols and replaceUnknownSymbols variables with enum
        }

        int alphabetIndex = alphabetComboBox->findText(comboBoxAlphabetItems[alphabet]);
        GT_CHECK(alphabetIndex != -1, QString("item \"%1\" in combobox not found").arg(comboBoxAlphabetItems[alphabet]));

        GTComboBox::selectItemByIndex(os, alphabetComboBox, alphabetIndex, useMethod);
    }

    QLineEdit *lineEdit = dialog->findChild<QLineEdit *>("filepathEdit");
    GT_CHECK(lineEdit != nullptr, "line edit not found");
    GTLineEdit::setText(os, lineEdit, documentLocation);

    QComboBox *comboBox = dialog->findChild<QComboBox *>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    GTComboBox::selectItemByIndex(os, comboBox, index, useMethod);

    QLineEdit *lineEditName = dialog->findChild<QLineEdit *>("nameEdit");
    GT_CHECK(lineEditName != nullptr, "line edit not found");
    GTLineEdit::setText(os, lineEditName, sequenceName);

    if (saveFile) {
        QCheckBox *saveFileCheckBox = qobject_cast<QCheckBox *>(GTWidget::findWidget(os, "saveImmediatelyBox", dialog));
        GTCheckBox::setChecked(os, saveFileCheckBox);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
    GTThread::waitForMainThread();
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::cancelCreateDocumentFiller"
CancelCreateDocumentFiller::CancelCreateDocumentFiller(HI::GUITestOpStatus &_os, const QString &_pasteDataHere, bool _customSettings = false, documentAlphabet _alphabet = StandardDNA, bool _skipUnknownSymbols = true, bool _replaceUnknownSymbols = false, const QString _symbol = "", const QString &_documentLocation = QString(), documentFormat _format = FASTA, const QString &_sequenceName = QString(), bool saveFile = false, GTGlobals::UseMethod method)
    : Filler(_os, "CreateDocumentFromTextDialog"), customSettings(_customSettings), alphabet(_alphabet), skipUnknownSymbols(_skipUnknownSymbols), replaceUnknownSymbols(_replaceUnknownSymbols),
      symbol(_symbol), format(_format), saveFile(saveFile), useMethod(method) {
    sequenceName = _sequenceName;
    pasteDataHere = _pasteDataHere;
    documentLocation = GTFileDialog::toAbsoluteNativePath(_documentLocation);
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxAlphabetItems[StandardDNA] = "Standard DNA";
    comboBoxAlphabetItems[StandardRNA] = "Standard RNA";
    comboBoxAlphabetItems[ExtendedDNA] = "Extended DNA";
    comboBoxAlphabetItems[ExtendedRNA] = "Extended RNA";
    comboBoxAlphabetItems[StandardAmino] = "Standard amino";
    comboBoxAlphabetItems[AllSymbols] = "All symbols";
}

#define GT_METHOD_NAME "commonScenario"
void CancelCreateDocumentFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != nullptr, "dialog not found");

    QPlainTextEdit *plainText = dialog->findChild<QPlainTextEdit *>("sequenceEdit");
    GT_CHECK(plainText != nullptr, "plain text not found");
    GTPlainTextEdit::setPlainText(os, plainText, pasteDataHere);

    if (customSettings) {
        QGroupBox *customSettingsCheckBox = qobject_cast<QGroupBox *>(GTWidget::findWidget(os, "groupBox", dialog));
        customSettingsCheckBox->setChecked(true);

        QComboBox *alphabetComboBox = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "alphabetBox", dialog));
        GT_CHECK(alphabetComboBox != nullptr, "ComboBox not found");

        if (skipUnknownSymbols) {
            QRadioButton *skipUnknownSymbols = qobject_cast<QRadioButton *>(GTWidget::findWidget(os, "skipRB", dialog));
            skipUnknownSymbols->setChecked(true);
        } else if (replaceUnknownSymbols) {
            QRadioButton *replaceUnknownSymbols = qobject_cast<QRadioButton *>(GTWidget::findWidget(os, "replaceRB", dialog));
            replaceUnknownSymbols->setChecked(true);

            QLineEdit *lineEdit = dialog->findChild<QLineEdit *>("symbolToReplaceEdit");
            GT_CHECK(lineEdit != nullptr, "line edit not found");
            GTLineEdit::setText(os, lineEdit, symbol);
        } else {
            assert(false);    // replase skipUnknownSymbols and replaceUnknownSymbols variables with enum
        }

        int alphabetIndex = alphabetComboBox->findText(comboBoxAlphabetItems[alphabet]);
        GT_CHECK(alphabetIndex != -1, QString("item \"%1\" in combobox not found").arg(comboBoxAlphabetItems[alphabet]));

        GTComboBox::selectItemByIndex(os, alphabetComboBox, alphabetIndex, useMethod);
    }

    QLineEdit *lineEdit = dialog->findChild<QLineEdit *>("filepathEdit");
    GT_CHECK(lineEdit != nullptr, "line edit not found");
    GTLineEdit::setText(os, lineEdit, documentLocation);

    QComboBox *comboBox = dialog->findChild<QComboBox *>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    GTComboBox::selectItemByIndex(os, comboBox, index, useMethod);

    QLineEdit *lineEditName = dialog->findChild<QLineEdit *>("nameEdit");
    GT_CHECK(lineEditName != nullptr, "line edit not found");
    GTLineEdit::setText(os, lineEditName, sequenceName);

    if (saveFile) {
        QCheckBox *saveFileCheckBox = qobject_cast<QCheckBox *>(GTWidget::findWidget(os, "saveImmediatelyBox", dialog));
        GTCheckBox::setChecked(os, saveFileCheckBox);
    }

    QDialogButtonBox *box = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != nullptr, "buttonBox is NULL");
    QPushButton *button = box->button(QDialogButtonBox::Cancel);
    GT_CHECK(button != nullptr, "cancel button is NULL");
    GTWidget::click(os, button);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}    // namespace U2
