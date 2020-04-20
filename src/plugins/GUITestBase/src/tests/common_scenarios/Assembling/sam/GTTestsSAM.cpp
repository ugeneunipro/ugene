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

#include "utils/GTUtilsDialog.h"
#include "GTTestsSAM.h"
#include "primitives/GTMenu.h"
#include "primitives/GTAction.h"
#include <primitives/GTWidget.h>
#include "GTGlobals.h"
#include "GTUtilsLog.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ConvertAssemblyToSAMDialogFiller.h"
#include "GTUtilsTaskTreeView.h"



namespace U2 {
namespace GUITest_SAM {

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 1. Open "samples/Assembly/chrM.sam".
    //    Expected:
    //      1) The import dialog is shown.
    //      2) There are no table with the sequences check boxes in the dialog.
    //      3) The reference line edit, button and warning are shown.
    // 2. Click the reference sequence browse button. Choose "samples/PDB/1CF7.PDB".
    //    Expected: the warning is not shown.
    // 3. Click "Import".
    //    Expected: the import task is started and finished with the error that PDB reference is not supported.
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, "", dataDir + "samples/PDB", "1CF7.PDB"));
    GTFileDialog::openFile(os, dataDir + "samples/Assembly", "chrM.sam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.hasError() == true, "There is no error message in log");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Open "_common_data/sam/out.sam".
    // Expected: 1) The import dialog is shown.
    // 2) There are no table with the sequences check boxes in the dialog.
    // 3) The reference line edit, button and warning are shown.
    // 2. Click the reference sequence browse button. Choose "_common_data/bam/small.bam.sorted.bam.bai".
    // Expected: the warning is not shown.
    // 3. Click "Import".
    // Expected: the import task is started and finished with the error that reference format is unknown.
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, "", testDir + "_common_data/bam/", "small.bam.sorted.bam.bai"));
    GTFileDialog::openFile(os, testDir + "_common_data/sam/", "out.sam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.hasError() == true, "There is no error message in log");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // 1. Open "samples/Assembly/chrM.sam".
    //    Expected:
    //      1) The import dialog is shown.
    //      2) There are no table with the sequences check boxes in the dialog.
    //      3) The reference line edit, button and warning are shown.
    // 2. Click the reference sequence browse button. Choose "samples/Assembly/chrM.fa".
    //    Expected: the warning is not shown.
    // 3. Click "Import".
    //    1) Expected: the import task is started and finished successfully.
    //    2) Assembly browser is opened with the imported assembly.
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, "", dataDir + "samples/Assembly", "chrM.fa"));
    GTFileDialog::openFile(os, dataDir + "samples/Assembly", "chrM.sam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!l.hasError(), "Open 'samples/Assembly/chrM.sam' finished with error");
}

} // GUITest_SAM
} // U2
