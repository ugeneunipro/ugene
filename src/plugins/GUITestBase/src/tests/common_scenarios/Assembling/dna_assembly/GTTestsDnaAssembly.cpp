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

#include "GTTestsDnaAssembly.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include <base_dialogs/MessageBoxFiller.h>
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"

#include <U2Gui/ToolsMenu.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {
namespace GUITest_dna_assembly {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    AlignShortReadsFiller::Parameters parameters(
        dataDir + "samples/FASTA/",
        "human_T1.fa",
        testDir + "_common_data/scenarios/dna_assembly/",
        "shread.fa");

    AlignShortReadsFiller *alignShortReadsFiller = new AlignShortReadsFiller(os, &parameters);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, alignShortReadsFiller);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    CHECK_OP(os, );

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    CHECK_OP(os, );

    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // UGENE Genome Aligner and gz input reference
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/e_coli/",
                                                 "NC_008253.fa.gz",
                                                 testDir + "_common_data/e_coli/",
                                                 "e_coli_1000.fastq");

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "GUITest_dna_assembly_test_0002.ugenedb"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Bowtie and gz input reference
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/e_coli/",
                                                 "NC_008253.fa.gz",
                                                 testDir + "_common_data/fastq/",
                                                 "short_sample.fastq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "GUITest_dna_assembly_test_0003.ugenedb"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // BWA and gz input reference
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/e_coli/",
                                                 "NC_008253.fa.gz",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bwa);

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "GUITest_dna_assembly_test_0004.ugenedb"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // BWA-MEM and gz input reference
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/bowtie2/",
                                                 "lambda_virus.fa.gz",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::BwaMem);

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "GUITest_dna_assembly_test_0005.ugenedb", "", "", false, 200000));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // BWA_SW and gz input reference
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/bowtie2/",
                                                 "lambda_virus.fa.gz",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_2.fq",
                                                 AlignShortReadsFiller::Parameters::BwaSw);

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "GUITest_dna_assembly_test_0006.ugenedb", "", "", false, 200000));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}


} // GUITest_dna_assembly
} // U2
