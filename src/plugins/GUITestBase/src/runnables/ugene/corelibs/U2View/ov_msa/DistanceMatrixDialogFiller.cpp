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

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>

#include <GTGlobals.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include "DistanceMatrixDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DistanceMatrixDialogFiller"

DistanceMatrixDialogFiller::DistanceMatrixDialogFiller(HI::GUITestOpStatus &os, bool _hamming, bool _counts, bool _excludeGaps) :
    Filler(os,"DistanceMatrixMSAProfileDialog"),
    hamming(_hamming),
    counts(_counts),
    excludeGaps(_excludeGaps),
    saveToFile(false),
    format(HTML)
{

}

DistanceMatrixDialogFiller::DistanceMatrixDialogFiller(HI::GUITestOpStatus &os, DistanceMatrixDialogFiller::SaveFormat _format, QString _path) :
    Filler(os,"DistanceMatrixMSAProfileDialog"),
    hamming(true),
    counts(true),
    excludeGaps(true),
    saveToFile(true),
    format(_format),
    path(QDir::toNativeSeparators(_path))
{

}

DistanceMatrixDialogFiller::DistanceMatrixDialogFiller(HI::GUITestOpStatus &os, CustomScenario *c)
    : Filler(os, "DistanceMatrixMSAProfileDialog", c),
      hamming(false),
      counts(false),
      excludeGaps(false),
      saveToFile(false),
      format(NONE)
{

}

#define GT_METHOD_NAME "run"
void DistanceMatrixDialogFiller::commonScenario(){

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog!=NULL, "dialog not found");


    QComboBox* algoCombo = dialog->findChild<QComboBox*>("algoCombo");
    if(hamming){
        GTComboBox::setCurrentIndex(os,algoCombo,0);
    }
    else{
        GTComboBox::setCurrentIndex(os,algoCombo,1);
    }

    if(counts){
        QRadioButton* countsRB = dialog->findChild<QRadioButton*>("countsRB");
        GTRadioButton::click(os,countsRB);
    }
    else{
        QRadioButton* percentsRB = dialog->findChild<QRadioButton*>("percentsRB");
        GTRadioButton::click(os,percentsRB);
    }

    if (format != NONE) {
        if (saveToFile) {
            GTGroupBox::setChecked(os, "saveBox", dialog);

            QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileEdit");
            GTLineEdit::setText(os, fileEdit, QDir::toNativeSeparators(path));

            if (format == HTML) {
                QRadioButton* htmlRB = dialog->findChild<QRadioButton*>("htmlRB");
                GTRadioButton::click(os, htmlRB);
            } else {
                QRadioButton* csvRB = dialog->findChild<QRadioButton*>("csvRB");
                GTRadioButton::click(os, csvRB);
            }
        }
    }
    QCheckBox* checkBox = dialog->findChild<QCheckBox*>("checkBox");
    GTCheckBox::setChecked(os,checkBox,excludeGaps);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}
