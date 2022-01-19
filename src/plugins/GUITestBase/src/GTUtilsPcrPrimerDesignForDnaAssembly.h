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

#include <U2Core/U2Range.h>
#include <U2Core/U2Type.h>

#include "GTGlobals.h"
#include "base_dialogs/DefaultDialogFiller.h"

namespace U2 {
using namespace HI;

// Methods for testing PCR Primer Design tab. Before calling all methods, open the tab by calling openTab.
class GTUtilsPcrPrimerDesign {
public:
    // Buttons "Add sequence to a user primer's end".
    enum class UserPrimer {
        Forward5,
        Forward3,
        Reverse5,
        Reverse3
    };
    // Primer search areas for insert.
    enum class AreaType {
        Left,
        Right
    };
    // Number of mouse clicks on resulting sequence.
    enum class ClickType {
        Single = 1,
        Double
    };
    // For paired spinboxes with correspond names.
    struct ParametersOfPrimingSequences {
        U2Range<int> gibbsFreeEnergy {-40, -30},  // Min -999999999, max seq_len.
            meltingPoint {57, 65},  // Min 0, max seq_len.
            overlapLength {18, 25};  // Min 0, max seq_len.
    };
    // For paired spinboxes with correspond names.
    struct ParametersToExcludeInWholePrimers {
        int gibbsFreeEnergy = -7,  // Min -999999999, max 0.
            meltingPoint = 20,  // Min 0, max 999999999.
            motifLen = 3;  // Min 0, max 999999999.
    };
    // Groupbox "Insert to backbone bearings" setting.
    struct InsertToBackboneBearings {
        enum class InsertTo {
            Backbone5,
            Backbone3
        };
        InsertTo insertTo = InsertTo::Backbone5;
        int backbone5Len = 0,  // Min 0, max 99.
            backbone3Len = 0;  // Min 0, max 99.
    };
    // Primer search areas for insert.
    struct SearchArea {
        SearchArea(const U2Range<int> &r, bool selectManually = false);

        U2Range<int> region;  // Min 1, max seq_len.
        bool selectManually = false;
    };

    // Filler for "The unwanted structures have been found in the following backbone sequence candidate".
    class BadBackboneFiller : public DefaultDialogFiller {
    public:
        enum class Button {
            No,
            Yes
        };
        BadBackboneFiller(GUITestOpStatus &os, const Button &btn);
        BadBackboneFiller(GUITestOpStatus &os, CustomScenario *scenario);
    };

    static void openTab(GUITestOpStatus &os);

    // Sets forward (if direction=Direct) or reverse (if direction=Complementary) user primer.
    static void setUserPrimer(GUITestOpStatus &os, const QString &primer, const U2Strand::Direction &direction);

    // Sets filter.
    static void filterGeneratedSequences(GUITestOpStatus &os, const QString &filter);

    /**
     * Clicks once generated sequence with a specific sequence number in generated sequence table. Fails, if
     * the table is empty. The indices are numbered starting from 0. Checks that the number is correct before clicking.
     * The number must be non-negative and less than the number of rows in the table.
     */
    static void selectGeneratedSequence(GUITestOpStatus &os, int num);

    /**
     * Searches the table of generated sequences for given sequence and clicks it. Search parameters are configured by
     * options (depth is ignored).
     */
    static void selectGeneratedSequence(GUITestOpStatus &os, const QString &sequence,
                                        const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    // Clicks Find reverse-complement button from the 'Generate sequence' widget.
    static void findReverseComplement(GUITestOpStatus &os);

    // Clicks one of 4 buttons that add a sequence fragment to the user primer.
    static void addToUserPrimer(GUITestOpStatus &os, const UserPrimer &userPrimer);

    // Sets Parameters of priming sequences.
    static void setParametersOfPrimingSequences(GUITestOpStatus &os, const ParametersOfPrimingSequences &params);

    // Sets Parameters to exclude in whole primers.
    static void setParametersToExcludeInWholePrimers(GUITestOpStatus &os,
                                                     const ParametersToExcludeInWholePrimers &params);

    // Sets settings for backbone bearings (doesn't set backbone path).
    static void configureInsertToBackboneBearings(GUITestOpStatus &os, const InsertToBackboneBearings &params);

    /**
     * Sets a range in two spinboxes for left (if areaType=Left) or right (if areaType=Right) primer search area for
     * insert. If params.selectManually==true, clicks the "Select manually" button, brings up the "Region Selection"
     * dialog for the first sequence, sets params.region as the region in that dialog, then clicks the "Select manually"
     * button again.
     */
    static void setSearchArea(GUITestOpStatus &os, const SearchArea &params, const AreaType &areaType);

    /**
     * Sets path to the backbone sequence. Fills in line editor if useButton=false, otherwise uses the file selection
     * dialog.
     */
    static void setBackbone(GUITestOpStatus &os, const QString &path, bool useButton = false);

    /**
     * Sets path to other sequences in PCR reaction. Fills in line editor if useButton=false, otherwise uses the file
     * selection dialog.
     */
    static void setOtherSequences(GUITestOpStatus &os, const QString &path, bool useButton = false);

    /**
     * Clicks single (if clickType=Single) or double (if clickType=Double) on a resulting sequence with a given sequence
     * number in result table. Fails, if the table is empty. The indices are numbered starting from 0. Checks that
     * the number is correct before clicking. The number must be non-negative and less than the number of rows in
     * the table.
     */
    static void clickInResultsTable(GUITestOpStatus &os, int num, const ClickType &clickType = ClickType::Single);

    /**
     * Checks that the (num+1)th row of the result table contains a given fragment with a given region. Fails, if
     * the table is empty. The indices are numbered starting from 0. Checks that the number is correct before clicking.
     * The number must be non-negative and less than the number of rows in the table.
     */
    static void checkEntryInResultsTable(GUITestOpStatus &os, int num, const QString &expectedFragment,
                                         const U2Range<int> &expectedRegion);

    // Clicks "Start" button. Launches the PCR Primer Design For DNA Assembly Task with the current parameters.
    static void clickStart(GUITestOpStatus &os);
};

}  // namespace U2

#endif  // _U2_GUI_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_UTILS_H_
