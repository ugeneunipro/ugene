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

#ifndef _U2_NEIGHBOR_JOIN_WIDGET_H_
#define _U2_NEIGHBOR_JOIN_WIDGET_H_

#include <U2View/CreatePhyTreeWidget.h>

#include "ui_NeighborJoinWidget.h"

namespace U2 {

class ConsensusModelTypes {
public:
    static QString M1;
    static QString Strict;
    static QString MajorityRuleExt;
    static QString MajorityRule;
    static QList<QString> getConsensusModelTypes();
};

class NeighborJoinWidget : public CreatePhyTreeWidget, public Ui_NeighborJoinWidget {
    Q_OBJECT
public:
    NeighborJoinWidget(const MultipleSequenceAlignment &ma, QWidget *parent = NULL);

    void fillSettings(CreatePhyTreeSettings &settings);
    void storeSettings();
    void restoreDefault();
    bool checkMemoryEstimation(QString &msg, const MultipleSequenceAlignment &msa, const CreatePhyTreeSettings &settings);
    bool checkSettings(QString &msg, const CreatePhyTreeSettings &settings);

private slots:
    void sl_onMatrixModelChanged(const QString &matrixModelName);
    void sl_onConsensusTypeChanged(const QString &consensusTypeName);

private:
    void init(const MultipleSequenceAlignment &ma);
    void connectSignals();
    static int getRandomSeed();
    static bool checkSeed(int seed);
};

}   // namespace U2

#endif // _U2_NEIGHBOR_JOIN_WIDGET_H_
