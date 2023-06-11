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

#include "BuildTreeDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::BuildTreeDialogFiller"

BuildTreeDialogFiller::BuildTreeDialogFiller(const QString& saveTree, int model, double alpha, bool displayWithMsa)
    : Filler("CreatePhyTree"),
      saveTree(saveTree),
      model(model),
      replicates(0),
      seed(0),
      alpha(alpha),
      fraction(0),
      type(MAJORITYEXT),
      displayWithMsa(displayWithMsa) {
}

BuildTreeDialogFiller::BuildTreeDialogFiller(
    int replicates,
    const QString& saveTree,
    int seed,
    BuildTreeDialogFiller::ConsensusType type,
    double fraction)
    : Filler("CreatePhyTree"),
      saveTree(saveTree),
      model(0),
      replicates(replicates),
      seed(seed),
      alpha(0),
      fraction(fraction),
      type(type),
      displayWithMsa(false) {
}

BuildTreeDialogFiller::BuildTreeDialogFiller(CustomScenario* scenario)
    : Filler("CreatePhyTree", scenario),
      model(0),
      replicates(0),
      seed(0),
      alpha(0),
      fraction(0),
      type(MAJORITYEXT),
      displayWithMsa(false) {
}

#define GT_METHOD_NAME "commonScenario"
void BuildTreeDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (saveTree != "default") {
        GTLineEdit::setText("fileNameEdit", saveTree);
    }

    if (model != 0) {
        GTComboBox::selectItemByIndex(GTWidget::findComboBox("cbModel", dialog), model);
    }

    if (alpha != 0) {
        GTCheckBox::setChecked(GTWidget::findCheckBox("chbGamma", dialog), true);
        GTDoubleSpinbox::setValue(GTWidget::findDoubleSpinBox("sbAlpha", dialog), alpha, GTGlobals::UseKeyBoard);
    } else {
        GTCheckBox::setChecked(GTWidget::findCheckBox("chbGamma", dialog), false);
    }

    if (replicates != 0) {
        GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);
        GTCheckBox::setChecked(GTWidget::findCheckBox("chbEnableBootstrapping"), true);
        GTSpinBox::setValue(GTWidget::findSpinBox("sbReplicatesNumber"), replicates, GTGlobals::UseKeyBoard);
        GTSpinBox::setValue(GTWidget::findSpinBox("sbSeed"), seed, GTGlobals::UseKeyBoard);
        GTComboBox::selectItemByIndex(GTWidget::findComboBox("cbConsensusType"), type);
        if (type == M1) {
            GTDoubleSpinbox::setValue(GTWidget::findDoubleSpinBox("sbFraction"), fraction, GTGlobals::UseKeyBoard);
        }
    }

    if (!displayWithMsa) {
        GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 2);
        GTRadioButton::click(GTWidget::findRadioButton("createNewView"));
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::BuildTreeDialogFiller"

BuildTreeDialogFillerPhyML::BuildTreeDialogFillerPhyML(bool _freqOptimRadioPressed, int bootstrap)
    : Filler("CreatePhyTree"), freqOptimRadioPressed(_freqOptimRadioPressed), bootstrap(bootstrap) {
}

#define GT_METHOD_NAME "commonScenario"
void BuildTreeDialogFillerPhyML::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmBox", dialog), "PhyML Maximum Likelihood");

    if (freqOptimRadioPressed) {
        GTRadioButton::click(GTWidget::findRadioButton("freqOptimRadio", dialog));
    }

    if (bootstrap >= 0) {
        GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("twSettings", dialog), 1);
        GTRadioButton::click(GTWidget::findRadioButton("bootstrapRadioButton"));
        GTSpinBox::setValue(GTWidget::findSpinBox("bootstrapSpinBox"), bootstrap);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
