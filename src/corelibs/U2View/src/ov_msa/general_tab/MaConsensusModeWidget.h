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

#include "ui_MaConsensusModeWidget.h"

namespace U2 {

class MaEditorConsensusArea;
class MsaObject;

class MaConsensusModeWidget : public QWidget, public Ui_MaConsensusModeWidget {
    Q_OBJECT
public:
    MaConsensusModeWidget(QWidget* parent = nullptr);
    void init(MsaObject* maObject, MaEditorConsensusArea* consArea);
    void reInit(MsaObject* maObject, MaEditorConsensusArea* consArea);

    void updateState();
    void updateThresholdState(bool enable, int minVal = 0, int maxVal = 0, int value = 0);

signals:
    void si_algorithmChanged(const QString& algoId);
    void si_thresholdChanged(int val);

public slots:
    void sl_algorithmChanged(const QString& algoId);
    void sl_algorithmSelectionChanged(int index);
    void sl_thresholdSliderChanged(int value);
    void sl_thresholdSpinBoxChanged(int value);
    void sl_thresholdResetClicked(bool newState);
    void sl_thresholdChanged(int value);

private:
    void initConsensusTypeCombo();

    MaEditorConsensusArea* consArea;
    MsaObject* maObject;
    QString curAlphabetId;
};

}  // namespace U2
