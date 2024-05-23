/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLabel.h>
#include <primitives/GTLineEdit.h>
#include "primitives/GTMenu.h"
#include <primitives/GTTabWidget.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>

#include "GTTestsCommonScenariosPrimer3.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_primer3 {

GUI_TEST_CLASS_DEFINITION(test_tab_main_all) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Main_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_tab_general_all) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_General_Settings_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_tab_advanced_all) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Advanced_Settings_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_tab_internal_all) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Internal_Oligo_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_tab_penalty_all) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Penalty_Weights_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_tab_quality_all) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Sequence_Quality_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0001.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{199, 218}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{40, 59}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{39, 58}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{610, 629}, {850, 869}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{107, 126}, {297, 316}});
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "annealing_temp.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0002.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{167, 186}, {107, 126}, {250, 269}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 3)", {{199, 218}, {167, 186}, {250, 269}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 3)", {{771, 790}, {736, 755}, {942, 961}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 3)", {{250, 269}, {107, 126}, {313, 332}});
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "boundary.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0003.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{3, 27}, {33, 57}});
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "boundary.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0004.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{3, 27}, {33, 57}});
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "check.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0005.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{31, 55}, {3, 27}, {58, 82}});
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "dmso_formamide.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0006.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{251, 270}, {199, 218}, {297, 316}});
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "end_pathology.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0007.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{22, 42}, {292, 314}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{24, 44}, {292, 314}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{22, 42}, {265, 289}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{23, 44}, {292, 314}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{22, 43}, {292, 314}});
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "first_base_index.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0008.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{2735, 2754}, {2956, 2975}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{3278, 3297}, {3549, 3568}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{3376, 3395}, {3559, 3578}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{2608, 2627}, {2735, 2754}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{2822, 2841}, {2961, 2980}});
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "gc_end.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0009.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{199, 218}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{610, 629}, {850, 869}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{107, 126}, {199, 218}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{850, 869}, {1022, 1041}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{297, 316}, {415, 434}});
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "high_gc_load_set.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0010.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{1222, 1240}, {1346, 1363}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{1221, 1239}, {1345, 1362}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{1221, 1239}, {1346, 1363}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{1220, 1238}, {1345, 1362}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{1222, 1240}, {1344, 1362}});
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "high_tm_load_set.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0011.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{1031, 1050}, {1187, 1206}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{1198, 1217}, {1346, 1365}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{582, 601}, {858, 877}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{1187, 1206}, {1346, 1365}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{582, 601}, {857, 876}});
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "human.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0012.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{233, 259}, {202, 222}, {260, 279}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 3)", {{233, 259}, {202, 222}, {260, 281}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 3)", {{233, 259}, {202, 222}, {260, 280}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 3)", {{86, 109}, {46, 65}, {112, 135}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 3)", {{86, 109}, {47, 66}, {112, 135}});
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "internal.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0013.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{31, 55}, {3, 27}, {58, 82}});
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "internal_position.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0014.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{251, 270}, {199, 218}, {297, 316}});
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "internal.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0015.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{31, 55}, {3, 27}, {58, 82}});
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "must_match.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0016.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 3)", {{270, 296}, {68, 89}, {302, 322}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 3)", {{301, 322}, {274, 298}, {510, 529}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 3)", {{882, 905}, {831, 852}, {922, 945}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 3)", {{475, 497}, {366, 391}, {498, 519}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 3)", {{68, 88}, {38, 61}, {311, 336}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 6  (0, 3)", {{555, 579}, {506, 527}, {738, 764}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 7  (0, 3)", {{301, 322}, {230, 256}, {507, 528}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 8  (0, 3)", {{56, 75}, {31, 54}, {177, 203}});
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "must_overlap_point.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0017.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{1369, 1388}, {1451, 1470}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{1460, 1479}, {1645, 1664}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{1155, 1174}, {1376, 1395}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{906, 925}, {1034, 1053}});
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "must_use.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0018.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{1, 21}, {144, 161}});
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "must_use.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0019.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{1, 21}, {144, 161}});
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "new_tasks.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0020.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("top_primers  (0, 2)", {{20, 38}, {478, 497}});
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "new_tasks.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0021.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("top_primers  (0, 2)", {{610, 629}, {1089, 1108}});
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    // Open sequence
    // Run Primer 3 with overhangs
    // Expected: overhangs marked as annotations
    GTFileDialog::openFile(testDir + "_common_data/primer3", "overhang.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0022.txt";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{15, 35}, {57, 76}});
    auto seq = GTUtilsAnnotationsTreeView::getQualifierValue("left_end_seq", "top_primers");
    auto strand = GTUtilsAnnotationsTreeView::getQualifierValue("left_end_strand", "top_primers");
    auto type = GTUtilsAnnotationsTreeView::getQualifierValue("left_end_type", "top_primers");
    CHECK_SET_ERR(seq == "TCACCCAC", QString("Expected left_end_seq: TCACCCAC, current: %1").arg(seq));
    CHECK_SET_ERR(strand == "direct", QString("Expected left_end_strand: direct, current: %1").arg(seq));
    CHECK_SET_ERR(type == "sticky", QString("Expected left_end_type: sticky, current: %1").arg(seq));

    GTUtilsAnnotationsTreeView::deleteItem("top_primers");
    seq = GTUtilsAnnotationsTreeView::getQualifierValue("right_end_seq", "top_primers");
    strand = GTUtilsAnnotationsTreeView::getQualifierValue("right_end_strand", "top_primers");
    type = GTUtilsAnnotationsTreeView::getQualifierValue("right_end_type", "top_primers");
    CHECK_SET_ERR(seq == "TAAGGATTT", QString("Expected right_end_seq: TAAGGATTT, current: %1").arg(seq));
    CHECK_SET_ERR(strand == "rev-compl", QString("Expected right_end_strand: rev-compl, current: %1").arg(seq));
    CHECK_SET_ERR(type == "sticky", QString("Expected right_end_type: sticky, current: %1").arg(seq));
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    // Open sequence
    // Set incorrect values for all possible parameters
    //
    GTFileDialog::openFile(testDir + "_common_data/primer3", "human.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0023.txt";
    settings.loadManually = false;
    settings.rtPcrDesign = true;
    settings.exonRangeLine = "qwerty";
    settings.hasValidationErrors = true;
    settings.validationErrorsText = "19 parameter(s) have an incorrect value(s), pay attention on red widgets.";
    settings.errorWidgetsNames = QStringList {"edit_SEQUENCE_PRIMER",
                                              "edit_SEQUENCE_INTERNAL_OLIGO",
                                              "edit_SEQUENCE_PRIMER_REVCOMP",
                                              "edit_SEQUENCE_OVERHANG_LEFT",
                                              "edit_SEQUENCE_OVERHANG_RIGHT",
                                              "edit_SEQUENCE_TARGET",
                                              "edit_SEQUENCE_OVERLAP_JUNCTION_LIST",
                                              "edit_SEQUENCE_EXCLUDED_REGION",
                                              "edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST",
                                              "edit_SEQUENCE_INCLUDED_REGION",
                                              "edit_PRIMER_MUST_MATCH_FIVE_PRIME",
                                              "edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME",
                                              "edit_PRIMER_MUST_MATCH_THREE_PRIME",
                                              "edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME",
                                              "edit_PRIMER_PRODUCT_SIZE_RANGE",
                                              "edit_SEQUENCE_INTERNAL_EXCLUDED_REGION",
                                              "edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST",
                                              "edit_SEQUENCE_QUALITY",
                                              "edit_exonRange"};

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "human.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0024.txt";
    settings.loadManually = false;
    settings.notRun = true;
    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");

    class Scenario : public Filler {
    public:
        Scenario()
            : Filler("Primer3Dialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::add(new GTFileDialogUtils(sandBoxDir, "test_0024.txt", GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("saveSettingsButton", dialog));
            GTWidget::click(GTWidget::findWidget("closeButton", dialog));
        }
    };

    GTUtilsDialog::add(new Scenario());
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");

    CHECK_SET_ERR(GTFile::equals(sandBoxDir + "test_0024.txt", testDir + "_common_data/primer3/input/test_0024.txt", true), "Settings are not equal");
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "human.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public Filler {
    public:
        Scenario()
            : Filler("Primer3Dialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(!GTGroupBox::getChecked("gbCheckComplementary", dialog), "\"Check complementary\" is checked, but shouldn't be");
            auto presetInfo = GTLabel::getText("lbPresetInfo", dialog);
            CHECK_SET_ERR(presetInfo.isEmpty(), "Preset info is not empty, but should be");

            GTComboBox::selectItemByText("cbPreset", dialog, "Recombinase Polymerase Amplification");
            CHECK_SET_ERR(GTGroupBox::getChecked("gbCheckComplementary", dialog), "\"Check complementary\" isn't checked, but should be");
            presetInfo = GTLabel::getText("lbPresetInfo", dialog);
            CHECK_SET_ERR(presetInfo == "Info: \"Check complementary\" has been enabled (see the \"Posterior Actions\" tab)",
                QString("Unexpected preset info: %1").arg(presetInfo));

            GTUtilsDialog::add(new GTFileDialogUtils(sandBoxDir, "test_0025_RPA.txt", GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("saveSettingsButton", dialog));
            GTWidget::click(GTWidget::findWidget("closeButton", dialog));
        }
    };

    GTUtilsDialog::add(new Scenario());
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    CHECK_SET_ERR(GTFile::equals(testDir + "_common_data/primer3/rpa_sequence.txt", sandBoxDir + "test_0025_RPA.txt", true), "RPA settings are not equal");
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    static const QString FORWAND_PRIMER("GTCTCAATCTCTTGTAACTGAATATAGATG");
    static const QString REVCOMP_PRIMER("CAAGAAAAATATGCACGGGGTCATCACTTG");

    class Scenario : public Filler {
    public:
        Scenario()
            : Filler("Primer3Dialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto editPrimerTaskCb = GTWidget::findComboBox("edit_PRIMER_TASK", dialog);
            GTComboBox::checkCurrentValue(editPrimerTaskCb, "check_primers");
            CHECK_SET_ERR(!editPrimerTaskCb->isEnabled(), "Task combo box is disabled");

            auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);
            CHECK_SET_ERR(!tabWidget->widget(Primer3DialogFiller::RT_PCR_DESIGN_TAB_NUMBER)->isEnabled(), "RT PCR tab is enabled, but shouldn't be");
            CHECK_SET_ERR(!tabWidget->widget(Primer3DialogFiller::POSTERIOR_ACTIONS_TAB_NUMBER)->isEnabled(), "Posterior actions tab is enabled, but shouldn't be");

            auto cbPickLeft = GTWidget::findCheckBox("checkbox_PRIMER_PICK_LEFT_PRIMER", dialog);
            GTCheckBox::setChecked(cbPickLeft, false);

            auto cbPickRight = GTWidget::findCheckBox("checkbox_PRIMER_PICK_RIGHT_PRIMER", dialog);
            GTCheckBox::setChecked(cbPickRight, false);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "At least one primer on the \"Main\" settings page should be enabled - this is required by the \"check_primers\" task."));

            auto pick = GTWidget::findPushButton("pickPrimersButton", dialog);
            GTWidget::click(pick);

            GTCheckBox::setChecked(cbPickLeft, true);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "The left primer on the \"Main\" settings page is enabled, but not set."));
            GTWidget::click(pick);

            GTCheckBox::setChecked(cbPickLeft, false);
            GTCheckBox::setChecked(cbPickRight, true);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "The right primer on the \"Main\" settings page is enabled, but not set."));
            GTWidget::click(pick);

            GTCheckBox::setChecked(cbPickLeft, true);
            GTLineEdit::setText("edit_SEQUENCE_PRIMER", "GTCTCAATCTCTTGTAACTGAATATAGATG", dialog);
            GTLineEdit::setText("edit_SEQUENCE_PRIMER_REVCOMP", "CAAGAAAAATATGCACGGGGTCATCACTTG", dialog);

            GTTabWidget::clickTab(tabWidget, Primer3DialogFiller::RESULT_ANNOTATION_SETTINGS_TAB_NUMBER);
            GTLineEdit::setText("outputFileLineEdit", sandBoxDir + "/result.gb", dialog);

            GTWidget::click(pick);
        }
    };

    GTUtilsDialog::add(new Scenario());
    GTMenu::clickMainMenuItem({ "Tools", "Primer", "Primer3 (no target sequence)..." });
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    static const QString MID_SEQ(140, 'N');
    static const QString EXPECTED_SEQ = FORWAND_PRIMER + MID_SEQ + "CAAGTGATGACCCCGTGCATATTTTTCTTG";

    auto sequence = GTUtilsSequenceView::getSequenceAsString();
    CHECK_SET_ERR(sequence == EXPECTED_SEQ, "Unexpected sequence");
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    GTFileDialog::openFile(testDir + "_common_data/primer3", "human.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public Filler {
    public:
        Scenario()
            : Filler("Primer3Dialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            static QStringList PRESETS = {"Default2", "qPCR", "Cloning Primers", "Annealing Temp", "Secondary Structures", "Probe"};
            for (const auto& preset : qAsConst(PRESETS)) {
                GTComboBox::selectItemByText("cbPreset", dialog, preset);
                QString presetSavedParamsFileName = "test_0025_" + preset + ".txt";
                GTUtilsDialog::add(new GTFileDialogUtils(sandBoxDir, presetSavedParamsFileName, GTFileDialogUtils::Save));
                GTWidget::click(GTWidget::findWidget("saveSettingsButton", dialog));
                CHECK_SET_ERR(GTFile::equals(testDir + "_common_data/primer3/presets/" + preset + ".txt", sandBoxDir + presetSavedParamsFileName, true), QString("%1 settings are not equal").arg(preset));
            }

            GTWidget::click(GTWidget::findWidget("closeButton", dialog));
        }
    };

    GTUtilsDialog::add(new Scenario());
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    // Open _common_data/scenarios/_regression/8094/8094.fa
    // Run the Primer3 dialog
    // Click "Pick primers"
    // Expected: "The priming sequence is too long, please, decrease the region" dialog appeared
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/8094", "8094.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public Filler {
    public:
        Scenario()
            : Filler("Primer3Dialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "The priming sequence is too long, please, decrease the region."));
            GTWidget::click(GTWidget::findWidget("pickPrimersButton", dialog));
            GTWidget::click(GTWidget::findWidget("closeButton", dialog));
        }
    };

    GTUtilsDialog::add(new Scenario());
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");


}



}  // namespace GUITest_common_scenarios_primer3
}  // namespace U2
