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

#include "MaConsensusModeWidget.h"

#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/MSAEditorConsensusArea.h>

#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorWgt.h"

namespace U2 {

MaConsensusModeWidget::MaConsensusModeWidget(QWidget* parent)
    : QWidget(parent),
      consArea(nullptr),
      maObject(nullptr) {
    setupUi(this);
}

void MaConsensusModeWidget::reInit(MultipleAlignmentObject* _maObject, MaEditorConsensusArea* _consArea) {
    SAFE_POINT(_maObject != nullptr, "MaConsensusModeWidget can not be initialized: MultipleAlignmentObject is NULL", );
    SAFE_POINT(_consArea != nullptr, "MaConsensusModeWidget can not be initialized: MaEditorConsensusArea is NULL", );

    consArea = _consArea;
    maObject = _maObject;

    initConsensusTypeCombo();

    connect(this, SIGNAL(si_algorithmChanged(QString)), consArea, SLOT(sl_changeConsensusAlgorithm(QString)));
    connect(this, SIGNAL(si_thresholdChanged(int)), consArea, SLOT(sl_changeConsensusThreshold(int)));

    connect(consArea, &MaEditorConsensusArea::si_consensusAlgorithmChanged,
            this, &MaConsensusModeWidget::sl_algorithmChanged);
    connect(consArea, &MaEditorConsensusArea::si_consensusThresholdChanged,
            this, &MaConsensusModeWidget::sl_thresholdChanged);
}

void MaConsensusModeWidget::init(MultipleAlignmentObject* _maObject, MaEditorConsensusArea* _consArea) {
    SAFE_POINT(_maObject != nullptr, "MaConsensusModeWidget can not be initialized: MultipleAlignmentObject is NULL", );
    SAFE_POINT(_consArea != nullptr, "MaConsensusModeWidget can not be initialized: MaEditorConsensusArea is NULL", );

    consArea = _consArea;
    maObject = _maObject;

    initConsensusTypeCombo();

    connect(consensusType, SIGNAL(currentIndexChanged(int)), SLOT(sl_algorithmSelectionChanged(int)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)), SLOT(sl_thresholdSliderChanged(int)));
    connect(thresholdSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_thresholdSpinBoxChanged(int)));
    connect(thresholdResetButton, SIGNAL(clicked(bool)), SLOT(sl_thresholdResetClicked(bool)));

    connect(this, SIGNAL(si_algorithmChanged(QString)), consArea, SLOT(sl_changeConsensusAlgorithm(QString)));
    connect(this, SIGNAL(si_thresholdChanged(int)), consArea, SLOT(sl_changeConsensusThreshold(int)));

    connect(consArea,
            SIGNAL(si_consensusAlgorithmChanged(QString)),
            SLOT(sl_algorithmChanged(QString)));
    connect(consArea,
            SIGNAL(si_consensusThresholdChanged(int)),
            SLOT(sl_thresholdChanged(int)));
}

void MaConsensusModeWidget::updateState() {
    SAFE_POINT(consArea != nullptr, "MaConsensusModeWidget is not initialized", );

    const MSAConsensusAlgorithm* algo = consArea->getConsensusAlgorithm();
    updateThresholdState(algo->supportsThreshold(),
                         algo->getMinThreshold(),
                         algo->getMaxThreshold(),
                         algo->getThreshold());
    consensusType->setToolTip(algo->getDescription());
}

void MaConsensusModeWidget::updateThresholdState(bool enable, int minVal, int maxVal, int value) {
    if (false == enable) {
        minVal = 0;
        maxVal = 0;
        value = 0;
    }

    thresholdLabel->setEnabled(enable);
    thresholdSlider->setEnabled(enable);
    thresholdSpinBox->setEnabled(enable);
    thresholdResetButton->setEnabled(enable);

    thresholdSlider->setRange(minVal, maxVal);
    thresholdSpinBox->setRange(minVal, maxVal);

    thresholdSpinBox->setValue(qBound(minVal, value, maxVal));
    thresholdSlider->setValue(qBound(minVal, value, maxVal));
}

void MaConsensusModeWidget::sl_algorithmChanged(const QString& algoId) {
    GCounter::increment("Consensus type changed", consArea->getEditorWgt()->getEditor()->getFactoryId());
    // Update state for the current algorithm
    SAFE_POINT(maObject != nullptr, "MaConsensusModeWidget is not initialized", );

    const DNAAlphabet* alphabet = maObject->getAlphabet();
    if (curAlphabetId != alphabet->getId()) {
        disconnect(consensusType, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_algorithmSelectionChanged(int)));
        consensusType->clear();
        initConsensusTypeCombo();
        connect(consensusType, SIGNAL(currentIndexChanged(int)), SLOT(sl_algorithmSelectionChanged(int)));
    } else {
        consensusType->setCurrentIndex(consensusType->findData(algoId));
        updateState();
    }
}

void MaConsensusModeWidget::sl_algorithmSelectionChanged(int index) {
    // TODO:ichebyki Workaround for index < 0 (SAFE_POINT removed)
    if (index >= 0) {
        QString selectedAlgorithmId = consensusType->itemData(index).toString();
        updateState();
        emit si_algorithmChanged(selectedAlgorithmId);
    } else {
        // Incorrect consensus algorithm index is detected
    }
}

void MaConsensusModeWidget::sl_thresholdSliderChanged(int value) {
    GCounter::increment("Consensus threshold changed", consArea->getEditorWgt()->getEditor()->getFactoryId());
    thresholdSpinBox->disconnect(this);
    thresholdSpinBox->setValue(value);
    connect(thresholdSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_thresholdSpinBoxChanged(int)));
    emit si_thresholdChanged(value);
}

void MaConsensusModeWidget::sl_thresholdSpinBoxChanged(int value) {
    thresholdSlider->disconnect(this);
    thresholdSlider->setValue(value);
    connect(thresholdSlider, SIGNAL(valueChanged(int)), SLOT(sl_thresholdSliderChanged(int)));
    emit si_thresholdChanged(value);
}

void MaConsensusModeWidget::sl_thresholdResetClicked(bool newState) {
    Q_UNUSED(newState);
    MSAConsensusAlgorithmRegistry* reg = AppContext::getMSAConsensusAlgorithmRegistry();
    MSAConsensusAlgorithmFactory* factory = reg->getAlgorithmFactory(consensusType->itemData(consensusType->currentIndex()).toString());
    SAFE_POINT(nullptr != factory, "Consensus alorithm factory is NULL", );
    sl_thresholdChanged(factory->getDefaultThreshold());
}

void MaConsensusModeWidget::sl_thresholdChanged(int value) {
    thresholdSpinBox->setValue(value);  // Slider updates automatically
}

void MaConsensusModeWidget::initConsensusTypeCombo() {
    MSAConsensusAlgorithmRegistry* reg = AppContext::getMSAConsensusAlgorithmRegistry();
    SAFE_POINT(nullptr != reg, "Consensus algorithm registry is NULL.", );

    const DNAAlphabet* alphabet = maObject->getAlphabet();
    curAlphabetId = alphabet->getId();
    ConsensusAlgorithmFlags flags = MSAConsensusAlgorithmFactory::getAphabetFlags(alphabet);
    if (qobject_cast<MultipleChromatogramAlignmentObject*>(maObject) != nullptr) {
        flags |= ConsensusAlgorithmFlag_AvailableForChromatogram;
    }
    QList<MSAConsensusAlgorithmFactory*> algos = reg->getAlgorithmFactories(flags);
    consensusType->clear();
    foreach (const MSAConsensusAlgorithmFactory* algo, algos) {
        consensusType->addItem(algo->getName(), algo->getId());
    }
    QString currentAlgorithmName = consArea->getConsensusAlgorithm()->getName();
    consensusType->setCurrentIndex(consensusType->findText(currentAlgorithmName));

    // Update state for the current algorithm
    updateState();
}

}  // namespace U2
