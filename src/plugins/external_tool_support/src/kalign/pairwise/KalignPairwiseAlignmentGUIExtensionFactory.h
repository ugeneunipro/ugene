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

#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QObject>
#include <QVariantMap>

#include <U2View/AlignmentAlgorithmGUIExtension.h>

#include "ui_KalignPairwiseAlignmentOptionsWidget.h"
#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

namespace U2 {


class Kalign3PairwiseAlignmentAlgorithm : public AlignmentAlgorithm {
public:
    Kalign3PairwiseAlignmentAlgorithm();
    bool checkAlphabet(const DNAAlphabet* alphabet) const override;
};


class KalignPairwiseAlignmentOptionsWidget : public AlignmentAlgorithmMainWidget,
                                              public Ui_KalignPairwiseAlignmentOptionsWidget {
    Q_OBJECT

public:
    KalignPairwiseAlignmentOptionsWidget(QWidget* parent, QVariantMap* s);
    ~KalignPairwiseAlignmentOptionsWidget() override;

    QVariantMap getAlignmentAlgorithmCustomSettings(bool append) final;

protected:
    void initParameters();
    void fillInnerSettings() override;
};

class KalignPairwiseAlignmentGUIExtensionFactory : public AlignmentAlgorithmGUIExtensionFactory {
    Q_OBJECT

public:
    KalignPairwiseAlignmentGUIExtensionFactory();

    AlignmentAlgorithmMainWidget* createMainWidget(QWidget* parent, QVariantMap* s) override;
};

}  // namespace U2
