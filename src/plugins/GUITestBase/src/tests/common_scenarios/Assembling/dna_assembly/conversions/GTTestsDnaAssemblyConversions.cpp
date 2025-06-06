/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/MessageBoxFiller.h>

#include <U2Core/U2IdTypes.h>
#include <U2Core/U2SafePoints.h>

#include "GTTestsDnaAssemblyConversions.h"
#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "system/GTFile.h"

namespace U2 {
namespace GUITest_dna_assembly_conversions {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTLogTracer lt;
    AlignShortReadsFiller::Parameters parameters(
        testDir + "_common_data/e_coli/",
        "NC_008253.gb",
        testDir + "_common_data/e_coli/",
        "e_coli_1000.gff",
        AlignShortReadsFiller::Parameters::Bowtie2);

    auto alignShortReadsFiller = new AlignShortReadsFiller(&parameters);

    GTUtilsDialog::add(alignShortReadsFiller);

    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));

    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "output.ugenedb"));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    GTFile::checkFileExists(testDir + "_common_data/e_coli/NC_008253.gb.fa");
    GTFile::checkFileExists(testDir + "_common_data/e_coli/e_coli_1000.gff.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTLogTracer lt;
    AlignShortReadsFiller::Parameters parameters(
        testDir + "_common_data/e_coli/",
        "NC_008253.gff",
        testDir + "_common_data/e_coli/",
        "e_coli_1000.fastq",
        AlignShortReadsFiller::Parameters::Bowtie);

    auto alignShortReadsFiller = new AlignShortReadsFiller(&parameters);

    GTUtilsDialog::add(alignShortReadsFiller);

    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));

    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "GUITest_dna_assembly_conversions/test_0002.ugenedb"));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    GTFile::checkFileExists(testDir + "_common_data/e_coli/NC_008253.gff.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTLogTracer lt;
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/e_coli/", "NC_008253.gb", testDir + "_common_data/e_coli/", "e_coli_1000.gff");

    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "result.ugenedb"));

    // UGENE can hang up here
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    GTFile::checkFileExists(testDir + "_common_data/e_coli/e_coli_1000.gff.fa");
}

}  // namespace GUITest_dna_assembly_conversions
}  // namespace U2
