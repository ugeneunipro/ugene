/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "GTTestsGeneCut.h"

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsGeneCut.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTWidget.h>

#include <U2Core/L10n.h>

namespace U2 {
namespace GUITest_common_scenarios_genecut {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Try to reset password
    // Use fake email to avoid confirmation letter
    GTUtilsGeneCut::resetPassword(os, "fake@email.com");
    auto lbResetStatus = qobject_cast<QLabel*>(GTWidget::findWidget(os, "lbResetStatus"));
    CHECK_SET_ERR(lbResetStatus != nullptr, L10N::nullPointerError("QLabel"));

    // Expected: error, because there is no account with such email
    CHECK_SET_ERR(lbResetStatus->text().startsWith("Error"), QString("lbResetStatus has incoorect text: %1").arg(lbResetStatus->text()));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Try to create a new user
    // Use diffenerent passwords
    GTUtilsGeneCut::createNewUser(os, "genecut@unipro.ru", "password", "another_password");
    auto lbRegisterWarning = qobject_cast<QLabel*>(GTWidget::findWidget(os, "lbRegisterWarning"));
    CHECK_SET_ERR(lbRegisterWarning != nullptr, L10N::nullPointerError("QLabel"));

    // Expected: error, because passords do not match
    CHECK_SET_ERR(lbRegisterWarning->text() == "Error: passwords do not match", QString("lbResetStatus has incoorect text: %1").arg(lbRegisterWarning->text()));
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Try to create a new user
    // Use emeil wich exists
    GTUtilsGeneCut::createNewUser(os);
    auto lbRegisterWarning = qobject_cast<QLabel*>(GTWidget::findWidget(os, "lbRegisterWarning"));
    CHECK_SET_ERR(lbRegisterWarning != nullptr, L10N::nullPointerError("QLabel"));

    // Expected: error, because user already exists
    CHECK_SET_ERR(lbRegisterWarning->text().contains("already exist"), QString("lbResetStatus has incoorect text: %1").arg(lbRegisterWarning->text()));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 9
    GTUtilsGeneCut::selectResultByIndex(os, 8);

    // Expected: input file gfp.fa, OptimizeCodonContext was chosen, completed
    GTUtilsGeneCut::checkResultInfo(os, "gfp.fa", { GTUtilsGeneCut::Steps::OptimizeCodonContext });

    // Compare the result file with _common_data/genecut/output/gfp_optimized.fa
    GTUtilsGeneCut::compareFiles(os, GTUtilsGeneCut::FileType::Result, testDir + "_common_data/genecut/output/gfp_optimized.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 10
    GTUtilsGeneCut::selectResultByIndex(os, 9);

    // Expected: input file AMINO.fa, OptimizeCodonContext was chosen, completed
    GTUtilsGeneCut::checkResultInfo(os, "AMINO.fa", { GTUtilsGeneCut::Steps::OptimizeCodonContext });

    // Expected: can't compare input and output
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbCompare")->isEnabled(), "pbCompare should be disabled");

    // Compare the result file with _common_data/genecut/output/AMINO263_optimized.fa
    GTUtilsGeneCut::compareFiles(os, GTUtilsGeneCut::FileType::Result, testDir + "_common_data/genecut/output/AMINO263_optimized.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 11
    GTUtilsGeneCut::selectResultByIndex(os, 10);

    // Expected: input file gfp_cut.fa, OptimizeCodonContext was chosen, completed with errors
    GTUtilsGeneCut::checkResultInfo(os, "gfp_cut.fa", { GTUtilsGeneCut::Steps::OptimizeCodonContext }, GTUtilsGeneCut::Status::CompletedWithError);

    // Expected: no result sequence
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbGetResultSequence")->isEnabled(), "pbCompare should be disabled");

    // Expected: can't compare input and output
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbCompare")->isEnabled(), "pbCompare should be disabled");

    // Compare the input file with _common_data/genecut/output/AMINO.fa
    GTUtilsGeneCut::compareFiles(os, GTUtilsGeneCut::FileType::Input, testDir + "_common_data/genecut/input/gfp_cut.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 1
    GTUtilsGeneCut::selectResultByIndex(os, 0);

    // Expected: input file gfp.fa, ExcludeRestrictionSites was chosen, completed
    GTUtilsGeneCut::checkResultInfo(os, "gfp.fa", { GTUtilsGeneCut::Steps::ExcludeRestrictionSites });

    // Compare the result file with _common_data/genecut/output/gfp_sites_excluded.fa
    GTUtilsGeneCut::compareFiles(os, GTUtilsGeneCut::FileType::Result, testDir + "_common_data/genecut/output/gfp_sites_excluded.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 2
    GTUtilsGeneCut::selectResultByIndex(os, 1);

    // Expected: input file gfp.fa, ExcludeRestrictionSites was chosen, completed
    GTUtilsGeneCut::checkResultInfo(os, "gfp.fa", { GTUtilsGeneCut::Steps::ExcludeRestrictionSites });

    // Compare the result file with the input file
    GTUtilsGeneCut::compareFiles(os, GTUtilsGeneCut::FileType::Result, testDir + "_common_data/genecut/input/gfp.fa", true);
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 3
    GTUtilsGeneCut::selectResultByIndex(os, 2);

    // Expected: input file gfp.fa, OligonucleotidesAssembly was chosen, completed
    GTUtilsGeneCut::checkResultInfo(os, "gfp.fa", { GTUtilsGeneCut::Steps::OligonucleotidesAssembly });

    // Load the result sequence with oligonucleotides
    GTWidget::click(os, GTWidget::findPushButton(os, "pbGetResultSequence"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check for Long Oligonucleotides assembly
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Oligonucleotides assembly  (0, 21)",
        { {1, 38}, {19, 75}, {56, 110}, {93, 152}, {128, 186},  {19, 75},
          {169, 226}, {209, 268}, {252, 311}, {284, 340}, {323, 381},
          {354, 413}, {387, 445}, {420, 474}, {453, 512}, {486, 538},
          {520, 578}, {565, 617}, {600, 658}, {641, 691}, {674, 717} });
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 4
    GTUtilsGeneCut::selectResultByIndex(os, 3);

    // Expected: input file AMINO.fa, OptimizeCodonContext and OligonucleotidesAssembly were chosen, completed with errors
    GTUtilsGeneCut::checkResultInfo(os, "AMINO.fa", { GTUtilsGeneCut::Steps::OptimizeCodonContext, GTUtilsGeneCut::Steps::OligonucleotidesAssembly }, GTUtilsGeneCut::Status::CompletedWithError);

    // Expected: no result sequence
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbGetResultSequence")->isEnabled(), "pbCompare should be disabled");

    // Expected: can't compare input and output
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbCompare")->isEnabled(), "pbCompare should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 5
    GTUtilsGeneCut::selectResultByIndex(os, 4);

    // Expected: input file HLP-hF8-N6M-pA.fa, OptimizeCodonContext and OligonucleotidesAssembly were chosen, Interrupted
    GTUtilsGeneCut::checkResultInfo(os, "HLP-hF8-N6M-pA.fa",
        { GTUtilsGeneCut::Steps::OptimizeCodonContext,
         GTUtilsGeneCut::Steps::ExcludeRestrictionSites,
         GTUtilsGeneCut::Steps::LongFragmentsAssembly,
         GTUtilsGeneCut::Steps::OligonucleotidesAssembly }, GTUtilsGeneCut::Status::Interrupted);

    // Expected: no result sequence
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbGetResultSequence")->isEnabled(), "pbCompare should be disabled");

    // Expected: can't compare input and output
    CHECK_SET_ERR(!GTWidget::findPushButton(os, "pbCompare")->isEnabled(), "pbCompare should be disabled");

}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // Open _common_data/fasta/human_T1_cutted.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Login
    GTUtilsGeneCut::login(os);

    // Fetch results and select 8
    GTUtilsGeneCut::selectResultByIndex(os, 7);

    // Expected: input file gfp.fa, OligonucleotidesAssembly was chosen, completed
    GTUtilsGeneCut::checkResultInfo(os, "gfp.fa",
        { GTUtilsGeneCut::Steps::OptimizeCodonContext,
          GTUtilsGeneCut::Steps::ExcludeRestrictionSites,
          GTUtilsGeneCut::Steps::LongFragmentsAssembly,
          GTUtilsGeneCut::Steps::OligonucleotidesAssembly });


    // Load the result sequence with oligonucleotides
    GTWidget::click(os, GTWidget::findPushButton(os, "pbGetResultSequence"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check for Long fragments assembly
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Long fragments assembly  (0, 2)", { {1, 717} });
}


}

}
