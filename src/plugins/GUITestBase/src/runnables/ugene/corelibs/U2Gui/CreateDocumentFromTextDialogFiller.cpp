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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>

#include <U2Core/Log.h>

#include "CreateDocumentFromTextDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::createDocumentFiller"
CreateDocumentFiller::CreateDocumentFiller(
    const QString& _pasteDataHere,
    bool _customSettings = false,
    documentAlphabet _alphabet = StandardDNA,
    bool _skipUnknownSymbols = true,
    bool _replaceUnknownSymbols = false,
    const QString _symbol = "",
    const QString& _documentLocation = QString(),
    documentFormat _format = FASTA,
    const QString& _sequenceName = QString(),
    GTGlobals::UseMethod method)
    : Filler("CreateDocumentFromTextDialog"),
      customSettings(_customSettings),
      alphabet(_alphabet),
      skipUnknownSymbols(_skipUnknownSymbols),
      replaceUnknownSymbols(_replaceUnknownSymbols),
      symbol(_symbol),
      format(_format),
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

CreateDocumentFiller::CreateDocumentFiller(CustomScenario* scenario)
    : Filler("CreateDocumentFromTextDialog", scenario),
      customSettings(false),
      alphabet(StandardDNA),
      skipUnknownSymbols(false),
      replaceUnknownSymbols(false),
      format(FASTA),
      useMethod(GTGlobals::UseMouse) {
}

void CreateDocumentFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto plainText = GTWidget::findPlainTextEdit("sequenceEdit", dialog);
    GTPlainTextEdit::setText(plainText, pasteDataHere);
    GTGlobals::sleep();

    if (customSettings) {
        GTGroupBox::setChecked(GTWidget::findGroupBox("groupBox", dialog), true);
        if (skipUnknownSymbols) {
            GTRadioButton::click("skipRB", dialog);
        } else if (replaceUnknownSymbols) {
            GTRadioButton::click("replaceRB", dialog);
            GTLineEdit::setText("symbolToReplaceEdit", symbol, dialog);
        } else {
            GT_FAIL("Unsupported state", );  // replace skipUnknownSymbols and replaceUnknownSymbols variables with enum
        }

        auto alphabetComboBox = GTWidget::findComboBox("alphabetBox", dialog);
        int alphabetIndex = alphabetComboBox->findText(comboBoxAlphabetItems[alphabet]);
        GT_CHECK(alphabetIndex != -1, QString("item \"%1\" in combobox not found").arg(comboBoxAlphabetItems[alphabet]));

        GTComboBox::selectItemByIndex(alphabetComboBox, alphabetIndex, useMethod);
    }

    GTLineEdit::setText("filepathEdit", documentLocation, dialog);

    auto comboBox = GTWidget::findComboBox("formatBox", dialog);

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    GTComboBox::selectItemByIndex(comboBox, index, useMethod);

    GTLineEdit::setText("nameEdit", sequenceName, dialog);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
    GTThread::waitForMainThread();
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::cancelCreateDocumentFiller"
CancelCreateDocumentFiller::CancelCreateDocumentFiller(const QString& _pasteDataHere, bool _customSettings = false, documentAlphabet _alphabet = StandardDNA, bool _skipUnknownSymbols = true, bool _replaceUnknownSymbols = false, const QString _symbol = "", const QString& _documentLocation = QString(), documentFormat _format = FASTA, const QString& _sequenceName = QString(), GTGlobals::UseMethod method)
    : Filler("CreateDocumentFromTextDialog"), customSettings(_customSettings), alphabet(_alphabet), skipUnknownSymbols(_skipUnknownSymbols), replaceUnknownSymbols(_replaceUnknownSymbols),
      symbol(_symbol), format(_format), useMethod(method) {
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

void CancelCreateDocumentFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto plainText = GTWidget::findPlainTextEdit("sequenceEdit", dialog);
    GTPlainTextEdit::setText(plainText, pasteDataHere);

    if (customSettings) {
        GTGroupBox::setChecked(GTWidget::findGroupBox("groupBox", dialog), true);

        if (skipUnknownSymbols) {
            GTRadioButton::click("skipRB", dialog);
        } else if (replaceUnknownSymbols) {
            GTRadioButton::click("replaceRB", dialog);
            GTLineEdit::setText("symbolToReplaceEdit", symbol, dialog);
        } else {
            GT_FAIL("Unsupported state", );
        }

        auto alphabetComboBox = GTWidget::findComboBox("alphabetBox", dialog);
        int alphabetIndex = alphabetComboBox->findText(comboBoxAlphabetItems[alphabet]);
        GT_CHECK(alphabetIndex != -1, QString("item \"%1\" in combobox not found").arg(comboBoxAlphabetItems[alphabet]));

        GTComboBox::selectItemByIndex(alphabetComboBox, alphabetIndex, useMethod);
    }

    GTLineEdit::setText("filepathEdit", documentLocation, dialog);

    auto comboBox = GTWidget::findComboBox("formatBox", dialog);

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    GTComboBox::selectItemByIndex(comboBox, index, useMethod);

    GTLineEdit::setText("nameEdit", sequenceName, dialog);

    GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
}

#undef GT_CLASS_NAME

}  // namespace U2
