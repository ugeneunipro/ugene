/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "GTTestsCloning.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <U2Gui/ToolsMenu.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsNotifications.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/EditFragmentDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_common_scenarios_cloning {

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //    1. Open file (samples/FASTA/human_T1.gb)
    //    2. Perform search for restriction site 'SacII'
    //    3. Use {Tools->Cloning->Digest Sequence Into Fragments}
    //    4. Select found enzymes and press 'OK'
    //    Expected state: there are the following qualifiers in the founded fragment annotaions
    //        left_end_seq,
    //        left_end_term,
    //        left_end_type,
    //        left_end_strand,
    //        right_end_seq,
    //        right_end_term,
    //        right_end_type,
    //        right_end_strand

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"SacII"}));
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller());
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* fr1 = GTUtilsAnnotationsTreeView::findItem("Fragment 1");
    QTreeWidgetItem* fr2 = GTUtilsAnnotationsTreeView::findItem("Fragment 2");

    GTTreeWidget::expand(fr1);
    GTUtilsAnnotationsTreeView::findItem("left_end_seq", fr1);
    GTUtilsAnnotationsTreeView::findItem("left_end_strand", fr1);
    GTUtilsAnnotationsTreeView::findItem("left_end_term", fr1);
    GTUtilsAnnotationsTreeView::findItem("left_end_type", fr1);
    GTUtilsAnnotationsTreeView::findItem("right_end_seq", fr1);
    GTUtilsAnnotationsTreeView::findItem("right_end_strand", fr1);
    GTUtilsAnnotationsTreeView::findItem("right_end_term", fr1);
    GTUtilsAnnotationsTreeView::findItem("right_end_type", fr1);

    GTTreeWidget::expand(fr2);
    GTUtilsAnnotationsTreeView::findItem("left_end_seq", fr2);
    GTUtilsAnnotationsTreeView::findItem("left_end_strand", fr2);
    GTUtilsAnnotationsTreeView::findItem("left_end_term", fr2);
    GTUtilsAnnotationsTreeView::findItem("left_end_type", fr2);
    GTUtilsAnnotationsTreeView::findItem("right_end_seq", fr2);
    GTUtilsAnnotationsTreeView::findItem("right_end_strand", fr2);
    GTUtilsAnnotationsTreeView::findItem("right_end_term", fr2);
    GTUtilsAnnotationsTreeView::findItem("right_end_type", fr2);
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // Open _common_data/cloning/murine_fragments.gb
    // Click "Tools" -> "Cloning" -> "Construct molecule..."
    // Click on the "Fragment 1"
    // Expected: fragment right end is red and does not fit to the left end of the fragment below
    // Click "Adjust right end"
    // Expected: fragment right end has been chaned and now fits to the corresponding left end of the fragment below
    // Click on the "Fragment 2"
    // Expected: fragment left end is red and does not fit to the right end of the fragment above
    // Click "Adjust left end"
    // Expected: fragment left end has been chaned and now fits to the corresponding right end of the fragment above
    // Click "OK"
    // The new product has been created, no error notifications
    GTFileDialog::openFile(testDir + "_common_data/cloning/", "murine_fragments.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddAllFragments, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickAdjustRight, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment 2");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickAdjustLeft, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(actions));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Construct molecule..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::checkNoVisibleNotifications();
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Open _common_data/cloning/murine_fragments.gb
    // Click "Tools" -> "Cloning" -> "Construct molecule..."
    // Click on the "Fragment 3"
    // Click on the "Fragment 1"
    // Check "Make circular"
    // Select "Fragment 3"
    // Expected: Fragment 3 left end is red and does not fit to the right end of the Fragment 1
    // Click "Adjust left end"
    // Expected: Fragment 3 left end has been chaned and now fits to the corresponding right end of the Fragment 1
    // Click "OK"
    // The new product has been created, no error notifications
    GTFileDialog::openFile(testDir + "_common_data/cloning/", "murine_fragments.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddFragment, "Fragment 3");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::CheckMakeCircular, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::SelectAddedFragment, "Fragment 3");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickAdjustLeft, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(actions));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Construct molecule..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::checkNoVisibleNotifications();
}


}  // namespace GUITest_common_scenarios_cloning

}  // namespace U2
