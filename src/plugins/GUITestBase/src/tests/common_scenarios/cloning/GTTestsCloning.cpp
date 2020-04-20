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

#include "GTTestsCloning.h"

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "utils/GTUtilsDialog.h"

#include <base_dialogs/GTFileDialog.h>
#include "primitives/GTMenu.h"
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"

#include <U2Gui/ToolsMenu.h>

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

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "SacII"));
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Cloning" << "Digest into fragments...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* fr1 = GTUtilsAnnotationsTreeView::findItem(os, "Fragment 1");
    CHECK_SET_ERR(fr1 != NULL, "Fragment 1 annotation not found");

    QTreeWidgetItem* fr2 = GTUtilsAnnotationsTreeView::findItem(os, "Fragment 2");
    CHECK_SET_ERR(fr2 != NULL, "Fragment 2 annotation not found");

    GTTreeWidget::expand(os, fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_seq", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_strand", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_term", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_type", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_seq", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_strand", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_term", fr1);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_type", fr1);

    GTTreeWidget::expand(os, fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_seq", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_strand", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_term", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "left_end_type", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_seq", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_strand", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_term", fr2);
    GTUtilsAnnotationsTreeView::findItem(os, "right_end_type", fr2);
}

} // namespace GUITest_common_scenarios_cloning

} //namespace U2
