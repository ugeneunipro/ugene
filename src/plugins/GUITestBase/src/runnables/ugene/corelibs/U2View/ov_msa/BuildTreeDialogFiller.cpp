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
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QDialogButtonBox>

#include "BuildTreeDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>
#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::BuildTreeDialogFiller"

BuildTreeDialogFiller::BuildTreeDialogFiller(HI::GUITestOpStatus &os, const QString &saveTree, int model, double alpha, bool displayWithMsa) :
    Filler(os, "CreatePhyTree"),
    saveTree(saveTree),
    model(model),
    replicates(0),
    seed(0),
    alpha(alpha),
    fraction(0),
    type(MAJORITYEXT),
    displayWithMsa(displayWithMsa)
{

}

BuildTreeDialogFiller::BuildTreeDialogFiller(HI::GUITestOpStatus &os,
                                             int replicates,
                                             const QString &saveTree,
                                             int seed,
                                             BuildTreeDialogFiller::ConsensusType type,
                                             double fraction) :
    Filler(os, "CreatePhyTree"),
    saveTree(saveTree),
    model(0),
    replicates(replicates),
    seed(seed),
    alpha(0),
    fraction(fraction),
    type(type),
    displayWithMsa(false)
{

}

BuildTreeDialogFiller::BuildTreeDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario)
    : Filler(os, "CreatePhyTree", scenario),
    model(0),
    replicates(0),
    seed(0),
    alpha(0),
    fraction(0),
    type(MAJORITYEXT),
    displayWithMsa(false)
{

}

#define GT_METHOD_NAME "commonScenario"
void BuildTreeDialogFiller::commonScenario() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new LicenseAgreementDialogFiller(os));
    if(saveTree!="default"){
        QLineEdit* saveLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os,"fileNameEdit"));
        GTLineEdit::setText(os,saveLineEdit, saveTree);
    }

    if (0 != model) {
        GTComboBox::setCurrentIndex(os, GTWidget::findExactWidget<QComboBox *>(os, "cbModel", dialog), model);
    }

    if (0 != alpha) {
        GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbGamma", dialog), true);
        GTDoubleSpinbox::setValue(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, "sbAlpha", dialog), alpha, GTGlobals::UseKeyBoard);
    } else {
        GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbGamma", dialog), false);
    }

    if (0 != replicates) {
        GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);
        GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbEnableBootstrapping"), true);
        GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "sbReplicatesNumber"), replicates, GTGlobals::UseKeyBoard);
        GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "sbSeed"), seed, GTGlobals::UseKeyBoard);
        GTComboBox::setCurrentIndex(os, GTWidget::findExactWidget<QComboBox *>(os, "cbConsensusType"), type);
        if (type == M1) {
            GTDoubleSpinbox::setValue(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, "sbFraction"), fraction, GTGlobals::UseKeyBoard);
        }
    }

    if (!displayWithMsa) {
        GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 2);
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "createNewView"));
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::BuildTreeDialogFiller"

BuildTreeDialogFillerPhyML::BuildTreeDialogFillerPhyML(HI::GUITestOpStatus &os, bool _freqOptimRadioPressed, int bootstrap)
    : Filler(os, "CreatePhyTree"), freqOptimRadioPressed(_freqOptimRadioPressed), bootstrap(bootstrap)
{

}

#define GT_METHOD_NAME "commonScenario"
void BuildTreeDialogFillerPhyML::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new LicenseAgreementDialogFiller(os));

    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox*>(os, "algorithmBox", dialog), "PhyML Maximum Likelihood");

    if (freqOptimRadioPressed) {
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton*>(os, "freqOptimRadio", dialog));
    }

    if (bootstrap >= 0) {
        GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "twSettings", dialog), 1);
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "bootstrapRadioButton"));
        GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "bootstrapSpinBox"), bootstrap);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
