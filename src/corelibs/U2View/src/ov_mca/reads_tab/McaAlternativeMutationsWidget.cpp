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

#include "McaAlternativeMutationsWidget.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/McaDbiUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "../McaEditorSequenceArea.h"
#include "../McaEditorStatusBar.h"

namespace U2 {

const QString McaAlternativeMutationsWidget::ALTERNATIVE_MUTATIONS_CHECKED = "ALTERNATIVE_MUTATIONS_CHECKED";
const QString McaAlternativeMutationsWidget::ALTERNATIVE_MUTATIONS_THRESHOLD = "ALTERNATIVE_MUTATIONS_THRESHOLD";

McaAlternativeMutationsWidget::McaAlternativeMutationsWidget(QWidget* parent)
    : QWidget(parent) {
    setupUi(this);
}

void McaAlternativeMutationsWidget::init(MultipleAlignmentObject* _maObject,
                                         MaEditorSequenceArea* _seqArea,
                                         MaEditorStatusBar* _statusBar) {
    SAFE_POINT(_seqArea != nullptr, "MaConsensusModeWidget can not be initialized: MaEditorSequenceArea is nullptr", );
    SAFE_POINT(_maObject != nullptr, "MaConsensusModeWidget can not be initialized: MultipleAlignmentObject is nullptr", );
    SAFE_POINT(_statusBar != nullptr, "MaConsensusModeWidget can not be initialized: MaEditorStatusBar is nullptr", );

    seqArea = qobject_cast<McaEditorSequenceArea*>(_seqArea);
    SAFE_POINT(seqArea != nullptr, "MaConsensusModeWidget can not be initialized: McaEditorSequenceArea is nullptr", );

    mcaObject = qobject_cast<MultipleChromatogramAlignmentObject*>(_maObject);
    SAFE_POINT(mcaObject != nullptr, "MaConsensusModeWidget can not be initialized: MultipleChromatogramAlignmentObject is nullptr", );

    statusBar = qobject_cast<McaEditorStatusBar*>(_statusBar);
    SAFE_POINT(mcaObject != nullptr, "MaConsensusModeWidget can not be initialized: McaEditorStatusBar is nullptr", );

    mutationsGroupBox->setChecked(false);
    mutationsThresholdSlider->setValue(99);

    connect(mutationsGroupBox, SIGNAL(toggled(bool)), this, SLOT(sl_updateAlternativeMutations()));
    connect(updateMutationsPushButton, SIGNAL(pressed()), this, SLOT(sl_updateAlternativeMutations()));
    connect(mcaObject->getDocument(), SIGNAL(si_lockedStateChanged()), this, SLOT(sl_updateLockState()));
}

void McaAlternativeMutationsWidget::sl_updateAlternativeMutations() {
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(mcaObject->getEntityRef(), os);
    Q_UNUSED(userModStep);

    mcaObject->updateAlternativeMutations(mutationsGroupBox->isChecked(), mutationsThresholdSlider->value(), os);
    CHECK_OP(os, );

    updateDb(os);
    CHECK_OP(os, );
}

const QString McaAlternativeMutationsWidget::getAlternativeMutationsCheckedId() {
    return ALTERNATIVE_MUTATIONS_CHECKED;
}

void McaAlternativeMutationsWidget::sl_updateLockState() {
    mutationsGroupBox->setDisabled(mcaObject->getDocument()->isStateLocked());
}

void McaAlternativeMutationsWidget::showEvent(QShowEvent* e) {
    updateValuesFromDb();
    QWidget::showEvent(e);
}

void McaAlternativeMutationsWidget::updateValuesFromDb() {
    U2OpStatus2Log os;
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(mcaObject->getEntityRef().dbiRef, os));
    CHECK_OP(os, );

    auto attributeDbi = con->dbi->getAttributeDbi();
    SAFE_POINT(attributeDbi != nullptr, "attributeDbi not found", );

    auto initAttribute = [&](U2IntegerAttribute& attribute, const QString& name) {
        auto objectAttributes = attributeDbi->getObjectAttributes(mcaObject->getEntityRef().entityId, name, os);
        CHECK_OP(os, );
        SAFE_POINT(objectAttributes.size() == 0 || objectAttributes.size() == 1, QString("Unexpected %1 objectAttributes size").arg(name), );

        bool setUpFromDb = objectAttributes.size() == 1;
        if (setUpFromDb) {
            attribute.id = objectAttributes.first();
        }
        mcaDbiObj.dbiId = mcaObject->getEntityRef().dbiRef.dbiId;
        mcaDbiObj.id = mcaObject->getEntityRef().entityId;
        mcaDbiObj.version = mcaObject->getModificationVersion();
        U2AttributeUtils::init(attribute, mcaDbiObj, name);
    };

    initAttribute(checkedStateAttribute, ALTERNATIVE_MUTATIONS_CHECKED);
    CHECK_OP(os, );

    initAttribute(thresholdAttribute, ALTERNATIVE_MUTATIONS_THRESHOLD);
    CHECK_OP(os, );

    auto avaliableAttributeNames = attributeDbi->getAvailableAttributeNames(os);
    CHECK_OP(os, );

    bool setUpFromDb = avaliableAttributeNames.contains(ALTERNATIVE_MUTATIONS_CHECKED);
    if (setUpFromDb) {
        auto checkedIntAttribute = attributeDbi->getIntegerAttribute(checkedStateAttribute.id, os);
        CHECK_OP(os, );

        auto thresholdIntAttribute = attributeDbi->getIntegerAttribute(thresholdAttribute.id, os);
        CHECK_OP(os, );

        mutationsThresholdSlider->setValue(thresholdIntAttribute.value);
        mutationsGroupBox->setChecked((bool)checkedIntAttribute.value);
    }
}

void McaAlternativeMutationsWidget::updateDb(U2OpStatus& os) {
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(mcaObject->getEntityRef().dbiRef, os));
    CHECK_OP(os, );

    auto attributeDbi = con->dbi->getAttributeDbi();
    SAFE_POINT(attributeDbi != nullptr, "attributeDbi not found", );

    auto updateAttribute = [&](U2IntegerAttribute& attribute, int value) {
        if (!attribute.id.isEmpty()) {
            U2AttributeUtils::removeAttribute(attributeDbi, attribute.id, os);
            CHECK_OP(os, );
        }

        attribute.value = value;
        attributeDbi->createIntegerAttribute(attribute, os);
        CHECK_OP(os, );
    };

    updateAttribute(checkedStateAttribute, (int)mutationsGroupBox->isChecked());
    CHECK_OP(os, );

    updateAttribute(thresholdAttribute, mutationsThresholdSlider->value());
    CHECK_OP(os, );

    statusBar->setMutationStatus(mutationsGroupBox->isChecked());
}

}  // namespace U2
