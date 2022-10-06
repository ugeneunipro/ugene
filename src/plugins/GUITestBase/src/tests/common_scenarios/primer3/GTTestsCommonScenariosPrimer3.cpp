/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <primitives/GTAction.h>
//#include <primitives/GTComboBox.h>
//#include <primitives/GTLineEdit.h>
//#include <primitives/GTTabWidget.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>

//#include <QApplication>

#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "GTTestsCommonScenariosPrimer3.h"
#include "GTUtilsAnnotationsTreeView.h"
//#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"

namespace U2 {
namespace GUITest_common_scenarios_primer3 {

GUI_TEST_CLASS_DEFINITION(test_tab_main_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Main_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_tab_general_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_General_Settings_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_tab_advanced_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Advanced_Settings_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

}

GUI_TEST_CLASS_DEFINITION(test_tab_internal_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Internal_Oligo_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

}

GUI_TEST_CLASS_DEFINITION(test_tab_penalty_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Penalty_Weights_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

}

GUI_TEST_CLASS_DEFINITION(test_tab_quality_all) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/tab_Sequence_Quality_all_changed.txt";
    settings.notRun = true;

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0001.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {199, 218}, {297, 316} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {40, 59}, {297, 316} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {39, 58}, {297, 316} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {610, 629}, {850, 869} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {107, 126}, {297, 316} });
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "annealing_temp.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0002.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {167, 186}, {107, 126}, {250, 269} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 3)", { {199, 218}, {167, 186}, {250, 269} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 3)", { {771, 790}, {736, 755}, {942, 961} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 3)", { {250, 269}, {107, 126}, {313, 332} });
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "boundary.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0003.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {3, 27}, {33, 57} });
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "boundary.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0004.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {3, 27}, {33, 57} });
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "check.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0005.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {31, 55}, {3, 27}, {58, 82} });
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "dmso_formamide.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0006.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {251, 270}, {199, 218}, {297, 316} });
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "end_pathology.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0007.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {22, 42}, {292, 314} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {24, 44}, {292, 314} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {22, 42}, {265, 289} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {23, 44}, {292, 314} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {22, 43}, {292, 314} });
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "first_base_index.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0008.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {2735, 2754}, {2956, 2975} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {3278, 3297}, {3549, 3568} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {3376, 3395}, {3559, 3578} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {2608, 2627}, {2735, 2754} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {2822, 2841}, {2961, 2980} });
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "gc_end.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0009.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {199, 218}, {297, 316} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {610, 629}, {850, 869} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {107, 126}, {199, 218} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {850, 869}, {1022, 1041} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {297, 316}, {415, 434} });
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "high_gc_load_set.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0010.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {1222, 1240}, {1346, 1363} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {1221, 1239}, {1345, 1362} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {1221, 1239}, {1346, 1363} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {1220, 1238}, {1345, 1362} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {1222, 1240}, {1344, 1362} });
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "high_tm_load_set.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0011.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {1031, 1050}, {1187, 1206} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {1198, 1217}, {1346, 1365} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {582, 601}, {858, 877} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {1187, 1206}, {1346, 1365} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {582, 601}, {857, 876} });
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "human.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0012.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {233, 259}, {202, 222}, {260, 279} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 3)", { {233, 259}, {202, 222}, {260, 281} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 3)", { {233, 259}, {202, 222}, {260, 280} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 3)", { {86, 109}, {46, 65}, {112, 135} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 3)", { {86, 109}, {47, 66}, {112, 135} });
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "internal.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0013.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {31, 55}, {3, 27}, {58, 82} });
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "internal_position.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0014.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {251, 270}, {199, 218}, {297, 316} });
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "internal.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0015.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {31, 55}, {3, 27}, {58, 82} });
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "must_match.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0016.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 3)", { {270, 296}, {68, 89}, {302, 322} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 3)", { {301, 322}, {274, 298}, {510, 529} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 3)", { {882, 905}, {831, 852}, {922, 945} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 3)", { {475, 497}, {366, 391}, {498, 519} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 3)", { {68, 88}, {38, 61}, {311, 336} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 6  (0, 3)", { {555, 579}, {506, 527}, {738, 764} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 7  (0, 3)", { {301, 322}, {230, 256}, {507, 528} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 8  (0, 3)", { {56, 75}, {31, 54}, {177, 203} });

}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "must_overlap_point.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0017.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {1369, 1388}, {1451, 1470} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {1460, 1479}, {1645, 1664} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {1155, 1174}, {1376, 1395} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {906, 925}, {1034, 1053} });
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "must_use.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0018.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {1, 21}, {144, 161} });
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "must_use.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0019.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {1, 21}, {144, 161} });
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "new_tasks.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0020.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "top_primers  (0, 2)", { {20, 38}, {478, 497} });
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    GTFileDialog::openFile(os, testDir + "_common_data/primer3", "new_tasks.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/primer3/input/test_0021.txt";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "top_primers  (0, 2)", { {610, 629}, {1089, 1108} });
}


}  // namespace GUITest_common_scenarios_phyml
}  // namespace U2
