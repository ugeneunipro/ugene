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

#include "primitives/GTMenu.h"
#include "system/GTFile.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/BuildIndexDialogFiller.h"

#include "GTTestsBowtie2.h"

#include <U2Gui/ToolsMenu.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {
namespace GUITest_Bowtie2 {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //1. {Tools -> Align to reference -> build index}
    //2. Fill the dialog:
    //  {Align short reads method} : Bowtie2
    //  {Reference sequence} : _common_data/fasta/human_T1_cutted.fa
    //  {Index file name} : _tmp/bowtie2/human_T1_cutted
    // And click Start.
    GTUtilsDialog::waitForDialog(os, new BuildIndexDialogFiller(os,
                                            testDir + "_common_data/fasta/",
                                            "human_T1_cutted.fa",
                                            "Bowtie2",
                                            false,
                                            testDir + "_common_data/scenarios/sandbox/",
                                            "human_T1_cutted"));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Build index for reads mapping...");

    // Expected state: there are six files as result:
    //human_T1_cutted.1.bt2, human_T1_cutted.2.bt2, human_T1_cutted.3.bt2, human_T1_cutted.4.bt2,
    //human_T1_cutted.rev.1.bt2, human_T1_cutted.rev.2.bt2
    QStringList indexList;
    for (int i = 0; i < 4; i++) {
        indexList << testDir + "_common_data/scenarios/sandbox/human_T1_cutted." + QString::number(i + 1) + ".bt2";
    }
    indexList << testDir + "_common_data/scenarios/sandbox/human_T1_cutted.rev.1.bt2";
    indexList << testDir + "_common_data/scenarios/sandbox/human_T1_cutted.rev.2.bt2";

    GTGlobals::sleep(500);
    for (int i = 0; i < indexList.size(); i++){
        CHECK_SET_ERR(GTFile::check(os, indexList[i]), "Index file " + indexList[i] + " is missing!");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0002){
//Align short reads with bowtie2
//no prebuilt index, default settings
//The settings defined here could be used as default in other tests

//1. {Tools -> Align to reference -> Align short reads}

//2. Fill the dialog:
//    {Mapping tool} : Bowtie2
//    {Reference sequence} : _common_data/fasta/human_T1_cutted.fa
//    {Result file name} : _tmp/bowtie2/human_T1_cutted.sam
//    {Library} : single-end
//    {Prebuilt index} : unchecked
//    {Short reads} : _common_data/fasta/shuffled.fa

//Parameters:
//    {Mode} : --end-to-end
//    {Number of mismatches} : 0
//    {Seed length (--L)} : unchecked (20)
//    {Add columns to allow gaps (--dpad)} : unchecked (15)
//    {Disallow gaps (--gbar)} : unchecked (4)
//    {Seed (--seed)} : unchecked (0)
//    {Threads} : 4 (depends on CPU cores)

//Flags:
//    {No unpaired alignments (--no-mixed)} : unchecked
//    {No discordant alignments (--no-discordant)} : unchecked
//    {No forward orientation (--nofw)} : unchecked
//    {No reverse-complement orientation (--norc)} : unchecked
//    {No overlapping mates (--no-overlap)} : unchecked
//    {No mates containing one another (--no-contain)} : unchecked

//And click Start.
//Expected state: an "Import SAM file" dialog appears. The incoming assembly has name "human_T1" and contains 3 reads.
    GTFile::copy(os, testDir + "_common_data/bowtie2/index/human_T1_cutted.fa", testDir + "_common_data/scenarios/sandbox/human_T1_cutted.fa");
    CHECK_OP(os, );

    AlignShortReadsFiller::Bowtie2Parameters bowtie2Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                               "human_T1_cutted.fa",
                                                               testDir + "_common_data/fasta/",
                                                               "shuffled.fa");
    // Parameters
    AlignShortReadsFiller* alignShortReadsFiller = new AlignShortReadsFiller(os, &bowtie2Parameters);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, alignShortReadsFiller);
    CHECK_OP(os, );

    ImportBAMFileFiller* importBAMFileFiller = new ImportBAMFileFiller(os);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, importBAMFileFiller);
    CHECK_OP(os, );

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    CHECK_OP(os, );
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
//    File will be copied to the sandbox
//    1. {Tools -> Align to reference -> Align short reads}
//    2. Fill the dialog:
//        {Mapping tool} : Bowtie2
//        {Reference sequence} : _common_data/bowtie2/index/human_T1_cutted.fa
//        {Result file name} : _tmp/bowtie2/human_T1_cutted.sam (in current test result path is default)
//        {Library} : single-end
//        {Prebuilt index} : unchecked
//        {Short reads} : _common_data/fasta/shuffled.fa

//    Parameters:
//        {Mode} : --end-to-end
//        {Number of mismatches} : 0
//        {Seed length (--L)} : checked (20)
//        {Add columns to allow gaps (--dpad)} : checked (15)
//        {Disallow gaps (--gbar)} : checked (4)
//        {Seed (--seed)} : checked (0)
//        {Threads} : 4 (depends on CPU cores)

//    Flags:
//        {No unpaired alignments (--no-mixed)} : checked
//        {No discordant alignments (--no-discordant)} : checked
//        {No forward orientation (--nofw)} : checked
//        {No reverse-complement orientation (--norc)} : checked
//        {No overlapping mates (--no-overlap)} : checked
//        {No mates containing one another (--no-contain)} : checked
    GTFile::copy(os, testDir + "_common_data/bowtie2/index/human_T1_cutted.fa", testDir + "_common_data/scenarios/sandbox/human_T1_cutted.fa");
    CHECK_OP(os, );

    AlignShortReadsFiller::Bowtie2Parameters bowtie2Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                               "human_T1_cutted.fa",
                                                               testDir + "_common_data/fasta/",
                                                               "shuffled.fa");

    // Parameters
    bowtie2Parameters.seedLengthCheckBox = true;
    bowtie2Parameters.addColumnsToAllowGapsCheckBox = true;
    bowtie2Parameters.disallowGapsCheckBox = true;
    bowtie2Parameters.seedCheckBox = true;

    // Flags
    bowtie2Parameters.noUnpairedAlignments = true;
    bowtie2Parameters.noDiscordantAlignments = true;
    bowtie2Parameters.noForwardOrientation = true;
    bowtie2Parameters.noReverseComplementOrientation = true;
    bowtie2Parameters.noOverlappingMates = true;
    bowtie2Parameters.noMatesContainingOneAnother = true;

    AlignShortReadsFiller* alignShortReadsFiller = new AlignShortReadsFiller(os, &bowtie2Parameters);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, alignShortReadsFiller);
    CHECK_OP(os, );

    ImportBAMFileFiller* importBAMFileFiller = new ImportBAMFileFiller(os);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, importBAMFileFiller);
    CHECK_OP(os, );

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    CHECK_OP(os, );
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
//    File will be copied to the sandbox
//    1. {Tools -> Align to reference -> Align short reads}
//    2. Fill the dialog:
//        {Mapping tool} : Bowtie2
//        {Reference sequence} : _common_data/bowtie2/index/human_T1_cutted.fa
//        {Library} : single-end
//        {Prebuilt index} : unchecked
//        {Short reads} : _common_data/fasta/shuffled.fa

//    Parameters:
//        {Mode} : --local
//        {Number of mismatches} : 1
//        {Seed length (--L)} : checked (24)
//        {Add columns to allow gaps (--dpad)} : checked (13)
//        {Disallow gaps (--gbar)} : checked (5)
//        {Seed (--seed)} : checked (23)
//        {Threads} : 3 (depends on CPU cores)

//    Flags:
//        {No unpaired alignments (--no-mixed)} : checked
//        {No discordant alignments (--no-discordant)} : unchecked
//        {No forward orientation (--nofw)} : unchecked
//        {No reverse-complement orientation (--norc)} : checked
//        {No overlapping mates (--no-overlap)} : checked
//        {No mates containing one another (--no-contain)} : unchecked

    GTFile::copy(os, testDir + "_common_data/bowtie2/index/human_T1_cutted.fa",
                 testDir + "_common_data/scenarios/sandbox/human_T1_cutted.fa");
    CHECK_OP(os, );

    AlignShortReadsFiller::Bowtie2Parameters bowtie2Parameters(testDir + "_common_data/bowtie2/index",
                                                               "human_T1_cutted.fa",
                                                               testDir + "_common_data/fasta",
                                                               "shuffled.fa");

    // Parameters
    bowtie2Parameters.mode = AlignShortReadsFiller::Bowtie2Parameters::Local;
    bowtie2Parameters.numberOfMismatches = 1;
    bowtie2Parameters.seedLengthCheckBox = true;
    bowtie2Parameters.seedLength = 24;
    bowtie2Parameters.addColumnsToAllowGapsCheckBox = true;
    bowtie2Parameters.addColumnsToAllowGaps = 13;
    bowtie2Parameters.disallowGapsCheckBox = true;
    bowtie2Parameters.disallowGaps = 5;
    bowtie2Parameters.seedCheckBox = true;
    bowtie2Parameters.seed = 23;
    bowtie2Parameters.threads = 3;

    // Flags
    bowtie2Parameters.noUnpairedAlignments = true;
    bowtie2Parameters.noDiscordantAlignments = false;
    bowtie2Parameters.noForwardOrientation = false;
    bowtie2Parameters.noReverseComplementOrientation = true;
    bowtie2Parameters.noOverlappingMates = false;
    bowtie2Parameters.noMatesContainingOneAnother = false;

    AlignShortReadsFiller* alignShortReadsFiller = new AlignShortReadsFiller(os, &bowtie2Parameters);
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, alignShortReadsFiller);
    CHECK_OP(os, );

    ImportBAMFileFiller* importBAMFileFiller = new ImportBAMFileFiller(os,
                    testDir + "_common_data/scenarios/sandbox/human_T1_cutted.sam.ugenedb");
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, importBAMFileFiller);
    CHECK_OP(os, );

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    CHECK_OP(os, );
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    AlignShortReadsFiller::Bowtie2Parameters bowtie2Parameters(testDir + "_common_data/scenarios/assembly/bowtie2/index/",
                                                               "e_coli_1000.1.bt2",
                                                               testDir + "_common_data/scenarios/assembly/bowtie2/",
                                                               "e_coli_1000.fq");
    bowtie2Parameters.prebuiltIndex = true;
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &bowtie2Parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    AlignShortReadsFiller::Bowtie2Parameters parameters(testDir + "_common_data/bowtie2/",
                                                 "lambda_virus.fa.gz",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_1.fq");
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

} // GUITest_Bowtie2
} // U2
