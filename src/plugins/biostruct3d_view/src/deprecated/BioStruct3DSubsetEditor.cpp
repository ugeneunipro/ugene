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

#include "BioStruct3DSubsetEditor.h"

#include <U2Algorithm/StructuralAlignmentAlgorithm.h>
#include <U2Core/BioStruct3DObject.h>

namespace U2 {

static const QString ALL_CHAINS("All chains");

BioStruct3DSubsetEditor::BioStruct3DSubsetEditor(const QList<BioStruct3DObject*> &biostructs, const BioStruct3DObject *selected /* = 0*/, int selectedModel /* = -1*/,  QWidget *parent /* = 0*/)
        : QWidget(parent)
{
    setupUi(this);

    foreach (BioStruct3DObject *bs, biostructs) {
        objectCombo->addItem(bs->getGObjectName(), qVariantFromValue((void*)bs));
    }

    if (selected) {
        int idx = objectCombo->findData(qVariantFromValue((void*)selected));
        assert(idx != -1 && "Selected object must present in biostructs");
        objectCombo->setCurrentIndex(idx);
    }

    fillChainCombo();
    fillRegionEdit();

    fillModelCombo();

    if (selectedModel != -1) {
        int idx = modelCombo->findData(qVariantFromValue(selectedModel));
        assert(idx != -1 && "Selected model must present in biostruct");
        modelCombo->setCurrentIndex(idx);
    }

    connect(objectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_onBiostructChanged(int)));
    connect(chainCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_onChainChanged(int)));
}

void BioStruct3DSubsetEditor::fillChainCombo() {
    BioStruct3DObject *bso = static_cast<BioStruct3DObject*>( objectCombo->itemData(objectCombo->currentIndex()).value<void*>() );
    chainCombo->clear();
    chainCombo->addItem(ALL_CHAINS);

    foreach (const int chainIndex, bso->getBioStruct3D().moleculeMap.keys()) {
        char chainId = bso->getBioStruct3D().getChainIdByIndex(chainIndex);
        if (chainId > 0) {
            chainCombo->addItem(QString(chainId), qVariantFromValue(chainIndex));
        } else {
            chainCombo->addItem(QString::number(chainIndex), qVariantFromValue(chainIndex));
        }
    }
}

void BioStruct3DSubsetEditor::fillModelCombo() {
    BioStruct3DObject *bso = static_cast<BioStruct3DObject*>( objectCombo->itemData(objectCombo->currentIndex()).value<void*>() );
    modelCombo->clear();
    foreach (const int modelId, bso->getBioStruct3D().modelMap.keys()) {
        modelCombo->addItem(QString::number(modelId), qVariantFromValue(modelId));
    }
}

void BioStruct3DSubsetEditor::fillRegionEdit() {
    if (chainCombo->currentText() != ALL_CHAINS) {
        BioStruct3DObject *bso = static_cast<BioStruct3DObject*>( objectCombo->itemData(objectCombo->currentIndex()).value<void*>() );
        int chainId = chainCombo->itemData(chainCombo->currentIndex()).value<int>();
        int length = bso->getBioStruct3D().moleculeMap.value(chainId)->residueMap.size();

        setRegion(U2Region(0, length));
        regionEdit->setEnabled(true);
    }
    else {
        regionEdit->setText("");
        regionEdit->setDisabled(true);
    }
}

void BioStruct3DSubsetEditor::sl_onBiostructChanged(int) {
    fillModelCombo();
    fillChainCombo();
    fillRegionEdit();
}

void BioStruct3DSubsetEditor::sl_onChainChanged(int idx) {
    if (idx != -1) {
        // reject chainCombo->clear() event from fillChainCombo() method
        fillRegionEdit();
    }
}

BioStruct3DReference BioStruct3DSubsetEditor::getSubset() {
    assert(validate().isEmpty() && "validate first!");

    BioStruct3DObject *obj = static_cast<BioStruct3DObject*> ( objectCombo->itemData(objectCombo->currentIndex()).value<void*>() );
    int modelId = modelCombo->itemData(modelCombo->currentIndex()).value<int>();

    if (chainCombo->currentText() == ALL_CHAINS) {
        QList<int> chains = obj->getBioStruct3D().moleculeMap.keys();
        return BioStruct3DReference(obj, chains, modelId);
    }
    else {
        int chainId = chainCombo->itemData(chainCombo->currentIndex()).value<int>();
        U2Region chainRegion = getRegion();
        return BioStruct3DReference(obj, chainId, chainRegion, modelId);
    }
}

QString BioStruct3DSubsetEditor::validate() const {
    if (regionEdit->isEnabled()) {
        QString regionText = regionEdit->text();
        if (!QRegExp("[0-9]*\\.\\.[0-9]*").exactMatch(regionText)) {
            return QString("invalid region spec %1").arg(regionText);
        }

        BioStruct3DObject *bso = static_cast<BioStruct3DObject*>( objectCombo->itemData(objectCombo->currentIndex()).value<void*>() );
        int chainId = chainCombo->itemData(chainCombo->currentIndex()).value<int>();
        int length = bso->getBioStruct3D().moleculeMap.value(chainId)->residueMap.size();

        U2Region region = getRegion();

        if (region.startPos < 0 || region.startPos >= region.endPos() || region.endPos() > length) {
            return QString("region values out of range %1..%2 (%3..%4)").arg(region.startPos + 1).arg(region.endPos()).arg(1).arg(length);
        }
    }

    return QString("");
}

U2Region BioStruct3DSubsetEditor::getRegion() const {
    U2Region ret;

    QString text = regionEdit->text();
    QStringList broken = text.split("..", QString::SkipEmptyParts);
    if (broken.size() != 2) return ret;

    bool ok = false;
    int start = broken.at(0).toInt(&ok);
    if (!ok) return ret;

    int end = broken.at(1).toInt(&ok);
    if (!ok) return ret;

    ret = U2Region(start - 1, end - start + 1);
    return ret;
}

void BioStruct3DSubsetEditor::setRegion(const U2Region &region) {
    QString text = QString("%1..%2").arg(region.startPos + 1).arg(region.endPos());
    regionEdit->setText(text);
}

void BioStruct3DSubsetEditor::setBiostructDisabled() {
    objectCombo->setDisabled(true);
}

void BioStruct3DSubsetEditor::setModelDisabled() {
    modelCombo->setDisabled(true);
}

}   // namespace U2
