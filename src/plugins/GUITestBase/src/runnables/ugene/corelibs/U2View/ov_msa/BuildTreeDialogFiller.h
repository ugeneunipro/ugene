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

#ifndef _U2_GT_RUNNABLES_BUILD_TREE_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_BUILD_TREE_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class BuildTreeDialogFiller : public Filler {
public:
    enum ConsensusType {
        MAJORITYEXT,
        STRICTCONSENSUS,
        MAJORITY,
        M1
    };

    BuildTreeDialogFiller(HI::GUITestOpStatus &os, const QString &saveTree = "default", int model = 0, double alpha = 0, bool displayWithMsa = false);
    BuildTreeDialogFiller(HI::GUITestOpStatus &os, int replicates = 0, const QString &saveTree = "default", int seed = 5, ConsensusType type = MAJORITYEXT, double fraction = 0.5);
    BuildTreeDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    void commonScenario();

private:
    QString saveTree;
    int model;
    int replicates;
    int seed;
    double alpha;
    double fraction;
    ConsensusType type;
    bool displayWithMsa;
};

class BuildTreeDialogFillerPhyML : public Filler {
 public:
    BuildTreeDialogFillerPhyML(HI::GUITestOpStatus &os, bool _freqOptimRadioPressed, int bootstrap = -1);
    void commonScenario();

private:
    bool freqOptimRadioPressed;
    int bootstrap;
};

}   // namespace U2

#endif // _U2_GT_RUNNABLES_BUILD_TREE_DIALOG_FILLER_H_
