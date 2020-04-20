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

#include "GTTestsCvGeneral.h"

#include "GTUtilsCircularView.h"
#include "GTUtilsProject.h"


namespace U2 {
namespace GUITest_common_scenarios_circular_view {


GUI_TEST_CLASS_DEFINITION(general_avail_1) {
    // @@step1: Open file standard_dna_1000.fa
    // @expected: There is a CV button on the sequence toolbar, it is not toggled
    // @expected: CV is closed
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectSequence(os,
        testDir + "_common_data/alphabets", "standard_dna_1000.fa", "seq1");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step2: Click the CV button
    // @expected: CV button is toggled
    // @expected: CV is opened
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step3: Click the CV button
    // @expected: CV button it is not toggled
    // @expected: CV is closed
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_2) {
    // @@step1: Open file extended_dna_1000.fa
    // @expected: There is a CV button on the sequence toolbar, it is not toggled
    // @expected: CV is closed
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectSequence(os,
        testDir + "_common_data/alphabets", "extended_dna_1000.fa", "seq2");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step2: Click the CV button
    // @expected: CV button is toggled
    // @expected: CV is opened
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step3: Click the CV button
    // @expected: CV button it is not toggled
    // @expected: CV is closed
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_3) {
    // @@step1: Open file standard_rna_1000.fa
    // @expected: There is a CV button on the sequence toolbar, it is not toggled
    // @expected: CV is closed
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectSequence(os,
        testDir + "_common_data/alphabets", "standard_rna_1000.fa", "seq3");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step2: Click the CV button
    // @expected: CV button is toggled
    // @expected: CV is opened
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step3: Click the CV button
    // @expected: CV button it is not toggled
    // @expected: CV is closed
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_4) {
    // @@step1: Open file extended_rna_1000.fa
    // @expected: There is a CV button on the sequence toolbar, it is not toggled
    // @expected: CV is closed
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectSequence(os,
        testDir + "_common_data/alphabets", "extended_rna_1000.fa", "seq4");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step2: Click the CV button
    // @expected: CV button is toggled
    // @expected: CV is opened
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    // @@step3: Click the CV button
    // @expected: CV button it is not toggled
    // @expected: CV is closed
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_5) {
    // @@step1: Open file standard_amino_1000.fa
    // @expected: There is no CV button on the sequence toolbar
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectSequence(os,
        testDir + "_common_data/alphabets", "standard_amino_1000.fa", "seq5");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isPresent(os, seqWidget), "Unexpected state of CV button!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_6) {
    // @@step1: Open file extended_amino_1000.fa
    // @expected: There is no CV button on the sequence toolbar
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectSequence(os,
        testDir + "_common_data/alphabets", "extended_amino_1000.fa", "seq6");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isPresent(os, seqWidget), "Unexpected state of CV button!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_7) {
    // @@step1: Open file raw_alphabet_1000.fa
    // @expected: There is no CV button on the sequence toolbar
    ADVSingleSequenceWidget *seqWidget = GTUtilsProject::openFileExpectRawSequence(os,
        testDir + "_common_data/alphabets", "raw_alphabet_1000.fa", "seq7");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isPresent(os, seqWidget), "Unexpected state of CV button!");
}

GUI_TEST_CLASS_DEFINITION(general_avail_8) {
    // @@step1: Open file standard_dna_rna_amino_1000.fa
    // @expected: there are 3 sequence widgets in sequence view: seq1, seq3, seq5
    // @expected: there is CV button on the seq1 toolbar, button is not toggled
    // @expected: there is CV button on the seq3 toolbar, button is not toggled
    // @expected: there is no CV button on the seq5 toolbar
    QStringList expectedNames;
    QList<ADVSingleSequenceWidget*> seqWidgets;
    expectedNames << "seq1" << "seq3" << "seq5";

    seqWidgets = GTUtilsProject::openFileExpectSequences(os,
        testDir + "_common_data/alphabets",
        "standard_dna_rna_amino_1000.fa",
        expectedNames);
    CHECK_OP_SET_ERR(os, "Failed to open sequences!");

    ADVSingleSequenceWidget *seq1Widget = seqWidgets.at(0);
    ADVSingleSequenceWidget *seq3Widget = seqWidgets.at(1);
    ADVSingleSequenceWidget *seq5Widget = seqWidgets.at(2);

    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seq1Widget), "Unexpected state of CV button on seq1 widget!");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seq3Widget), "Unexpected state of CV button on seq3 widget!");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isPresent(os, seq5Widget), "Unexpected state of CV button on seq5 widget!");

    // @@step2: Click the CV button on seq3 widget
    // @expected: CV for seq1 widget is closed, CV button for seq1 widget is not toggled
    // @expected: CV for seq3 widget is opened, CV button for seq3 widget is not toggled
    GTUtilsCv::cvBtn::click(os, seq3Widget);

    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seq1Widget), "Unexpected state of CV button on seq1 widget!");
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seq3Widget), "Unexpected state of CV button on seq3 widget!");

    // @@step3: check CV button on sequence view
    GTUtilsCv::commonCvBtn::mustExist(os);

    // @@step4: click CV button on the sequence view toolbar
    // @expected: CV for seq1 widget is opened, CV button for seq1 widget is toggled
    // @expected: CV for seq3 widget is closed, CV button for seq1 widget is not toggled
    GTUtilsCv::commonCvBtn::click(os);
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seq1Widget), "Unexpected state of CV widget for seq1!");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seq3Widget), "Unexpected state of CV widget for seq3!");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seq1Widget), "Unexpected state of CV button on seq1 widget!");
    CHECK_SET_ERR(!GTUtilsCv::cvBtn::isChecked(os, seq3Widget), "Unexpected state of CV button on seq3 widget!");

    // @@step5: click CV button on the sequence view again
    // @expected: CV for seq1 widget is closed, CV button for seq1 widget is not toggled
    // @expected: CV for seq3 widget is opened, CV button for seq1 widget is toggled
    GTUtilsCv::commonCvBtn::click(os);
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seq1Widget), "Unexpected state of CV widget for seq1!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seq3Widget), "Unexpected state of CV widget for seq3!");
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seq1Widget), "Unexpected state of CV button on seq1 widget!");
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seq3Widget), "Unexpected state of CV button on seq3 widget!");
}


} // namespace GUITest_common_scenarios_circular_view
} // namespace U2
