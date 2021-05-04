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

#include "McaAlternativeMutationsWidget.h"

#include "../McaEditorSequenceArea.h"

#include <U2Core/McaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include "U2Core/U2Mod.h"
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

McaAlternativeMutationsWidget::McaAlternativeMutationsWidget(QWidget* parent)
    : QWidget(parent) {
    setupUi(this);

}

void McaAlternativeMutationsWidget::init(MultipleAlignmentObject* _maObject, MaEditorSequenceArea* _seqArea) {
    SAFE_POINT(_seqArea != nullptr, "MaConsensusModeWidget can not be initialized: MaEditorSequenceArea is nullptr", );
    SAFE_POINT(_maObject != nullptr, "MaConsensusModeWidget can not be initialized: MultipleAlignmentObject is nullptr", );

    seqArea = qobject_cast<McaEditorSequenceArea*>(_seqArea);
    SAFE_POINT(seqArea != nullptr, "MaConsensusModeWidget can not be initialized: McaEditorSequenceArea is nullptr", );

    mcaObject = qobject_cast<MultipleChromatogramAlignmentObject*>(_maObject);
    SAFE_POINT(mcaObject != nullptr, "MaConsensusModeWidget can not be initialized: MultipleChromatogramAlignmentObject is nullptr", );

    const auto& tabSettings = seqArea->getMcaReadsTabSettings();
    mutationsGroupBox->setChecked(tabSettings.showAlternativeMutations);
    mutationsThresholdSlider->setValue(tabSettings.threshold);

    connect(mutationsGroupBox, SIGNAL(toggled(bool)), this, SLOT(sl_mutationsGroupBoxToggeled(bool)));
    connect(mutationsThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(sl_mutationsThresholdValueChanged(int)));
    connect(mutationsThresholdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sl_mutationsThresholdValueChanged(int)));
    connect(updateMutationsPushButton, SIGNAL(pressed()), this, SLOT(sl_updateAlternativeMutations()));
    connect(seqArea->getEditor(), SIGNAL(si_isAbout2BeDestroyed()), mcaObject, SLOT(sl_viewIsAbout2BeDestroyed()));
}


void McaAlternativeMutationsWidget::sl_mutationsGroupBoxToggeled(bool on) {
    auto tabSettings = seqArea->getMcaReadsTabSettings();
    tabSettings.showAlternativeMutations = on;
    seqArea->setMcaReadsTabSettings(tabSettings);
    updateAlternativeMutations();
}

void McaAlternativeMutationsWidget::sl_mutationsThresholdValueChanged(int newValue) {
    auto tabSettings = seqArea->getMcaReadsTabSettings();
    tabSettings.threshold = newValue;
    seqArea->setMcaReadsTabSettings(tabSettings);
}

void McaAlternativeMutationsWidget::sl_updateAlternativeMutations() {
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(mcaObject->getEntityRef(), os);
    Q_UNUSED(userModStep);

    const auto& tabSettings = seqArea->getMcaReadsTabSettings();
    mcaObject->updateAlternativeMutations(tabSettings, os);
    SAFE_POINT_OP(os, );
}

void McaAlternativeMutationsWidget::updateAlternativeMutations() {
    sl_updateAlternativeMutations();
}

}
