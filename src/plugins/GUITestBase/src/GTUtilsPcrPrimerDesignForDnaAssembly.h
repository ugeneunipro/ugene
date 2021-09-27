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

#ifndef _U2_GUI_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_UTILS_H_
#define _U2_GUI_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_UTILS_H_

#include "GTGlobals.h"

#include <U2Core/U2Range.h>
#include <U2Core/U2Type.h>

class QLineEdit;
class QSpinBox;
class QTableWidget;

namespace U2 {
using namespace HI;

// Methods for testing PCR Primer Design tab.
class GTUtilsPcrPrimerDesignForDnaAssembly {
public:
    enum class UserPrimerEndButton {
        Forward5,
        Forward3,
        Reverse5,
        Reverse3
    };
    struct ParamsOfPrimingSeqs {
        U2Range<int> gibbsFreeEnergy{ -40, -30 };
        U2Range<int> meltingPoint   {  57,  65 };
        U2Range<int> overlapLength  {  18,  25 };
    };
    struct ParamsToExcludeInWholePrimers {
        int gibbsFreeEnergy = -7;
        int meltingPoint = 20;
        int motifLen = 3;
    };
    struct InsertToBackboneBearings {
        bool insertTo5 = true;
        int  backbone5Len = 0;
        int  backbone3Len = 0;
    };
    static void setUserPrimer(GUITestOpStatus& os, const QString& primer,
                              U2Strand::Direction direction = U2Strand::Direct);
    static void filterGeneratedSequences(GUITestOpStatus& os, const QString& filter);
    static void selectGeneratedSequence(GUITestOpStatus& os, int i);
    static void selectGeneratedSequence(GUITestOpStatus& os, const QString& sequence,
        const GTGlobals::FindOptions& options = GTGlobals::FindOptions());
    static void findReverseComplement(GUITestOpStatus& os);
    static void addUserPrimerEnd(GUITestOpStatus& os, UserPrimerEndButton buttonType);
    static void setParamsOfPrimingSeqs(GUITestOpStatus& os, const ParamsOfPrimingSeqs& params);
    static void setParamsToExcludeInWholePrimers(GUITestOpStatus& os, ParamsToExcludeInWholePrimers params);
    static void setInsertToBackbone(GUITestOpStatus& os, InsertToBackboneBearings params);
    static void setPrimerSearchArea(GUITestOpStatus& os, bool isLeft, U2Range<int> area, bool selectManually = false);
    static void openBackbone(GUITestOpStatus& os, const QString& path, bool useDialog = false);
    static void openOtherSequences(GUITestOpStatus& os, const QString& path, bool useDialog = false);
    static void start(GUITestOpStatus& os);
    static void clickResultTable(GUITestOpStatus& os, int i, bool isDoubleClick = false);
    static void checkResultTable(GUITestOpStatus& os, int i, U2Range<int> expected);

    static QLineEdit* getUserPrimer(GUITestOpStatus& os, U2Strand::Direction direction);
    static QTableWidget* getGeneratedSequenceTableWgt(GUITestOpStatus& os, const QWidget* tab = nullptr);
    static QPair<QSpinBox*, QSpinBox*> getAreaSpinboxes(GUITestOpStatus& os, bool isLeft);
    static QTableWidget* getResultTableWgt(GUITestOpStatus& os, const QWidget* tab = nullptr);
private:
    static QWidget* openTabAndReturnIt(GUITestOpStatus& os);
    static QWidget* getGenerateSequenceWgt(GUITestOpStatus& os, const QWidget* tab);
    static QWidget* getAreasForPrimingSearchWgt(GUITestOpStatus& os, const QWidget* tab);
    static void scrollToWidget(GUITestOpStatus& os, const QWidget* scrollTo, const QWidget* tab);
    static void selectGeneratedSequence(GUITestOpStatus& os, int i, QTableWidget* table, const QWidget* tab);
};

}  // namespace U2

#endif  // _U2_GUI_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_UTILS_H_
