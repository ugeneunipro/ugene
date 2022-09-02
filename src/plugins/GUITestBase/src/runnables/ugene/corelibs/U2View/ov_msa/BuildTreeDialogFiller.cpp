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

BuildTreeDialogFiller::BuildTreeDialogFiller(HI::GUITestOpStatus& os, const QString& saveTree, int model, double alpha, bool displayWithMsa)
    : Filler(os, "CreatePhyTree"),
      saveTree(saveTree),
      model(model),
      replicates(0),
      seed(0),
      alpha(alpha),
      fraction(0),
      type(MAJORITYEXT),
      displayWithMsa(displayWithMsa) {
}

BuildTreeDialogFiller::BuildTreeDialogFiller(HI::GUITestOpStatus& os,
                                             int replicates,
                                             const QString& saveTree,
                                             int seed,
                                             BuildTreeDialogFiller::ConsensusType type,
                                             double fraction)
    : Filler(os, "CreatePhyTree"),
      saveTree(saveTree),
      model(0),
      replicates(replicates),
      seed(seed),
      alpha(0),
      fraction(fraction),
      type(type),
      displayWithMsa(false) {
}

BuildTreeDialogFiller::BuildTreeDialogFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "CreatePhyTree", scenario),
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
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (saveTree != "default") {
        GTLineEdit::setText(os, "fileNameEdit", saveTree);
    }

    if (model != 0) {
        GTComboBox::selectItemByIndex(os, GTWidget::findComboBox(os, "cbModel", dialog), model);
    }

    if (alpha != 0) {
        GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbGamma", dialog), true);
        GTDoubleSpinbox::setValue(os, GTWidget::findDoubleSpinBox(os, "sbAlpha", dialog), alpha, GTGlobals::UseKeyBoard);
    } else {
        GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbGamma", dialog), false);
    }

    if (replicates != 0) {
        GTTabWidget::setCurrentIndex(os, GTWidget::findTabWidget(os, "tabWidget", dialog), 1);
        GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbEnableBootstrapping"), true);
        GTSpinBox::setValue(os, GTWidget::findSpinBox(os, "sbReplicatesNumber"), replicates, GTGlobals::UseKeyBoard);
        GTSpinBox::setValue(os, GTWidget::findSpinBox(os, "sbSeed"), seed, GTGlobals::UseKeyBoard);
        GTComboBox::selectItemByIndex(os, GTWidget::findComboBox(os, "cbConsensusType"), type);
        if (type == M1) {
            GTDoubleSpinbox::setValue(os, GTWidget::findDoubleSpinBox(os, "sbFraction"), fraction, GTGlobals::UseKeyBoard);
        }
    }

    if (!displayWithMsa) {
        GTTabWidget::setCurrentIndex(os, GTWidget::findTabWidget(os, "tabWidget", dialog), 2);
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "createNewView"));
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::BuildTreeDialogFiller"

BuildTreeDialogFillerPhyML::BuildTreeDialogFillerPhyML(HI::GUITestOpStatus& os, bool _freqOptimRadioPressed, int bootstrap)
    : Filler(os, "CreatePhyTree"), freqOptimRadioPressed(_freqOptimRadioPressed), bootstrap(bootstrap) {
}

#define GT_METHOD_NAME "commonScenario"
void BuildTreeDialogFillerPhyML::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "algorithmBox", dialog), "PhyML Maximum Likelihood");

    if (freqOptimRadioPressed) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "freqOptimRadio", dialog));
    }

    if (bootstrap >= 0) {
        GTTabWidget::setCurrentIndex(os, GTWidget::findTabWidget(os, "twSettings", dialog), 1);
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "bootstrapRadioButton"));
        GTSpinBox::setValue(os, GTWidget::findSpinBox(os, "bootstrapSpinBox"), bootstrap);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
