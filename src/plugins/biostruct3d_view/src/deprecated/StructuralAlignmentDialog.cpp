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

#include <QMessageBox>

#include <U2Algorithm/StructuralAlignmentAlgorithm.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmFactory.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <QPushButton>

#include <U2Gui/HelpButton.h>

#include "BioStruct3DSubsetEditor.h"
#include "StructuralAlignmentDialog.h"

namespace U2 {

static QList<BioStruct3DObject*> findAvailableBioStructs() {
    QList<GObject*> objs = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::BIOSTRUCTURE_3D);
    QList<BioStruct3DObject*> biostructs;
    foreach(GObject *obj, objs) {
        BioStruct3DObject *bso = qobject_cast<BioStruct3DObject*> (obj);
        assert(bso);
        biostructs << bso;
    }

    return biostructs;
}

StructuralAlignmentDialog::StructuralAlignmentDialog(const BioStruct3DObject *fixedRef/* = 0*/, int fixedRefModel/* = -1*/, QWidget *parent/* = 0*/)
    : QDialog(parent), task(0) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748812");
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));

    StructuralAlignmentAlgorithmRegistry *reg = AppContext::getStructuralAlignmentAlgorithmRegistry();
    foreach(const QString &id, reg->getFactoriesIds()) {
        algorithmCombo->addItem(id, qVariantFromValue(id));
    }

    QList<BioStruct3DObject*> biostructs = findAvailableBioStructs();
    ref = new BioStruct3DSubsetEditor(biostructs, fixedRef, fixedRefModel);
    mob = new BioStruct3DSubsetEditor(biostructs);

    if (fixedRef) {
        ref->setBiostructDisabled();
    }
    if (fixedRefModel != -1) {
        ref->setModelDisabled();
    }

    QVBoxLayout *refBox = new QVBoxLayout();
    refBox->addWidget(ref);
    refGroup->setLayout(refBox);

    QVBoxLayout *altBox = new QVBoxLayout();
    altBox->addWidget(mob);
    altGroup->setLayout(altBox);

    updateGeometry();
}

void StructuralAlignmentDialog::accept() {
    if (algorithmCombo->count() < 1) {
        return;
    }

    QString msg, err;
    err = ref->validate();
    if (!err.isEmpty()) {
        msg += QString("Reference: ") + err + "\n";
    }
    err = mob->validate();
    if (!err.isEmpty()) {
        msg += QString("Mobile: ") + err + "\n";
    }
    if (!msg.isEmpty()) {
        QMessageBox::warning(this, "Error", msg);
        return;
    }

    BioStruct3DReference refSubset(ref->getSubset());
    BioStruct3DReference mobSubset(mob->getSubset());

    // Since we unable to change mob structure we clone the GObject
    // TODO: clone live-range?
    U2OpStatus2Log os;
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    BioStruct3DObject *mobClone = qobject_cast<BioStruct3DObject*> (mobSubset.obj->clone(dbiRef, os));
    mobSubset.obj = mobClone;

    StructuralAlignmentTaskSettings settings(refSubset, mobSubset);

    StructuralAlignmentAlgorithmRegistry *reg = AppContext::getStructuralAlignmentAlgorithmRegistry();

    QString algorithmId = algorithmCombo->itemData(algorithmCombo->currentIndex()).value<QString>();
    StructuralAlignmentAlgorithm *algorithm = reg->createStructuralAlignmentAlgorithm(algorithmId);

    err = algorithm->validate(settings);
    if (!err.isEmpty()) {
        msg = QString("%1 validate failed: %2").arg(algorithmId).arg(err);
        QMessageBox::warning(this, "Error", msg);
        return;
    }

    task = new StructuralAlignmentTask(algorithm, settings);

    QDialog::accept();
}

int StructuralAlignmentDialog::execIfAlgorithmAvailable() {
    StructuralAlignmentAlgorithmRegistry *reg = AppContext::getStructuralAlignmentAlgorithmRegistry();
    if (reg->getFactoriesIds().isEmpty()) {
        QMessageBox::warning(this, "Error", "No available algorithms, make sure that appropriate plugin loaded (for ex. PTools)");
        return Rejected;
    } else {
        return exec();
    }
}

}   // namespace U2
