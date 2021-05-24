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
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

const QString McaAlternativeMutationsWidget::ALTERNATIVE_MUTATIONS_THRESHOLD = "ALTERNATIVE_MUTATIONS_THRESHOLD";

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
    mutationsGroupBox->setChecked(false);
    mutationsThresholdSlider->setValue(99);

    U2OpStatus2Log os;
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(mcaObject->getEntityRef().dbiRef, os));
    CHECK_OP(os, );

    auto attributeDbi = con->dbi->getAttributeDbi();
    SAFE_POINT(attributeDbi != nullptr, "attributeDbi not found", );

    auto objectAttributes = attributeDbi->getObjectAttributes(mcaObject->getEntityRef().entityId, ALTERNATIVE_MUTATIONS_THRESHOLD, os);
    CHECK_OP(os, );
    SAFE_POINT(objectAttributes.size() == 0 || objectAttributes.size() == 1, "Unexpected objectAttributes size", );

    bool setUpFromDb = objectAttributes.size() == 1;
    if (setUpFromDb) {
        thresholdAttribute.id = objectAttributes.first();
    }
    mcaDbiObj.dbiId = mcaObject->getEntityRef().dbiRef.dbiId;
    mcaDbiObj.id = mcaObject->getEntityRef().entityId;
    mcaDbiObj.version = mcaObject->getModificationVersion();
    U2AttributeUtils::init(thresholdAttribute, mcaDbiObj, ALTERNATIVE_MUTATIONS_THRESHOLD);

    if (setUpFromDb) {
        auto thresholdIntAttribute = attributeDbi->getIntegerAttribute(thresholdAttribute.id, os);
        SAFE_POINT_OP(os, );

        if (thresholdIntAttribute.value != 0) {
            mutationsGroupBox->setChecked(true);
            mutationsThresholdSlider->setValue(thresholdIntAttribute.value);
        }
    }

    connect(mutationsGroupBox, SIGNAL(toggled(bool)), this, SLOT(sl_mutationsGroupBoxToggled(bool)));
    connect(mutationsThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(sl_mutationsThresholdValueChanged(int)));
    connect(mutationsThresholdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sl_mutationsThresholdValueChanged(int)));
    connect(updateMutationsPushButton, SIGNAL(pressed()), this, SLOT(sl_updateAlternativeMutations()));
    connect(seqArea->getEditor(), SIGNAL(si_isAbout2BeDestroyed()), mcaObject, SLOT(sl_viewIsAbout2BeDestroyed()));
}


void McaAlternativeMutationsWidget::sl_mutationsGroupBoxToggled(bool on) {
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

    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(mcaObject->getEntityRef().dbiRef, os));
    CHECK_OP(os, );

    auto attributeDbi = con->dbi->getAttributeDbi();
    SAFE_POINT(attributeDbi != nullptr, "attributeDbi not found", );

    if (!thresholdAttribute.id.isEmpty()) {
        U2AttributeUtils::removeAttribute(attributeDbi, thresholdAttribute.id, os);
        SAFE_POINT_OP(os, );
    }

    thresholdAttribute.value = mutationsGroupBox->isChecked() ? mutationsThresholdSlider->value() : 0;
    attributeDbi->createIntegerAttribute(thresholdAttribute, os);
    CHECK_OP(os, );
}

void McaAlternativeMutationsWidget::updateAlternativeMutations() {
    sl_updateAlternativeMutations();
}

}
