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

#include "GTUtilsGeneCut.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsTaskTreeView.h"

#include <U2Core/L10n.h>

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include <QStackedWidget>
#include <QTextBrowser>

#include <functional>

namespace U2 {

const QMap<GTUtilsGeneCut::Steps, QString> GTUtilsGeneCut::STEP_ENUM_2_STRING = {
        {GTUtilsGeneCut::Steps::OptimizeCodonContext, "Optimize codon context"},
        {GTUtilsGeneCut::Steps::ExcludeRestrictionSites, "Exclude restrictions and splicing sites"},
        {GTUtilsGeneCut::Steps::LongFragmentsAssembly, "Long fragments assembly"},
        {GTUtilsGeneCut::Steps::OligonucleotidesAssembly, "Oligonucleotides assembly"}};

const QMap<GTUtilsGeneCut::Status, QString> GTUtilsGeneCut::STATUS_ENUM_2_STRING = {
        {GTUtilsGeneCut::Status::Completed, "Completed"},
        {GTUtilsGeneCut::Status::CompletedWithError, "Completed with error"},
        {GTUtilsGeneCut::Status::Interrupted, "Interrupted"} };


void GTUtilsGeneCut::login(HI::GUITestOpStatus& os, const QString& email, const QString& password, bool showPassword, bool rememberMe) {
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::GeneCut);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "leEmail"), email);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "lePasword"), password);
    GTCheckBox::setChecked(os, "cbShowPass", showPassword);
    GTCheckBox::setChecked(os, "cbRememberMe", rememberMe);
    GTWidget::click(os, GTWidget::findPushButton(os, "pbLogin"));
    auto mainStacKWidget = qobject_cast<QStackedWidget*>(GTWidget::findWidget(os, "stackedWidget"));
    CHECK_SET_ERR(mainStacKWidget != nullptr, L10N::nullPointerError("QStackedWidget"));

    while (mainStacKWidget->currentIndex() != GENECUT_MAIN_FORM_INDEX) {
        GTGlobals::sleep(500);
    }
}

void GTUtilsGeneCut::resetPassword(HI::GUITestOpStatus& os, const QString& email) {
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::GeneCut);
    GTWidget::click(os, GTWidget::findPushButton(os, "pbForgot"));
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "leResetPassword"), email);
    auto pbReset = GTWidget::findPushButton(os, "pbReset");
    GTWidget::click(os, pbReset);
    while (!pbReset->isEnabled()) {
        GTGlobals::sleep(500);
    }
}

void GTUtilsGeneCut::createNewUser(HI::GUITestOpStatus& os,
                                   const QString& email,
                                   const QString& password,
                                   const QString& passwordConfirm,
                                   const QString& firstName,
                                   const QString& secondName) {
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::GeneCut);
    GTWidget::click(os, GTWidget::findPushButton(os, "pbRegister"));
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "leEmailNew"), email);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "lePasswordNew"), password);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "lePasswordConformationNew"), passwordConfirm);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "leFirstName"), firstName);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "leLastName"), secondName);
    auto pbRegisterNew = GTWidget::findPushButton(os, "pbRegisterNew");
    GTWidget::click(os, pbRegisterNew);
    while (!pbRegisterNew->isEnabled()) {
        GTGlobals::sleep(500);
    }
}

void GTUtilsGeneCut::selectResultByIndex(HI::GUITestOpStatus& os, int index, bool fetchResults) {
    auto mainStacKWidget = qobject_cast<QStackedWidget*>(GTWidget::findWidget(os, "stackedWidget"));
    CHECK_SET_ERR(mainStacKWidget != nullptr, L10N::nullPointerError("QStackedWidget"));
    CHECK_SET_ERR(mainStacKWidget->currentIndex() == GENECUT_MAIN_FORM_INDEX, "Login should be called before");

    if (fetchResults) {
        auto pbFetchResults = GTWidget::findPushButton(os, "pbFetchResults", mainStacKWidget);
        GTWidget::click(os, pbFetchResults);
        while (!pbFetchResults->isEnabled()) {
            GTGlobals::sleep(500);
        }
    }
    GTTableView::click(os, GTWidget::findTableWidget(os, "twResults", mainStacKWidget), index, 0);
}

void GTUtilsGeneCut::checkResultInfo(HI::GUITestOpStatus& os, const QString& inputFileName, const QList<Steps>& steps, Status status) {
    auto twResults = GTWidget::findTableWidget(os, "twResults");
    CHECK_SET_ERR(twResults->rowCount() != 0, "selectResultByIndex should be called before");

    auto currentStatus = twResults->item(twResults->currentRow(), 1)->text();
    CHECK_SET_ERR(currentStatus == STATUS_ENUM_2_STRING.value(status), QString("Expected status: %1").arg(STATUS_ENUM_2_STRING.value(status)));

    auto tbShortDescription = GTWidget::findTextBrowser(os, "tbShortDescription");
    auto text = tbShortDescription->toPlainText();
    CHECK_SET_ERR(!text.isEmpty(), "tbShortDescription is empty");
    CHECK_SET_ERR(text.contains(inputFileName), QString("Expected input file name: %1").arg(inputFileName));
    for (auto step : qAsConst(steps)) {
        CHECK_SET_ERR(text.contains(STEP_ENUM_2_STRING.value(step)), QString("Expected step: %1").arg(STEP_ENUM_2_STRING.value(step)));
    }
}

void GTUtilsGeneCut::compareFiles(HI::GUITestOpStatus& os, FileType fileType, const QString& seqenceFilePath, bool sequencesOnly) {
    auto twResults = GTWidget::findTableWidget(os, "twResults");
    CHECK_SET_ERR(twResults->rowCount() != 0, "selectResultByIndex should be called before");

    QPushButton* getFile = nullptr;
    switch (fileType)     {
    case U2::GTUtilsGeneCut::FileType::Input:
        getFile = GTWidget::findPushButton(os, "pbGetInputSequence");
        break;
    case U2::GTUtilsGeneCut::FileType::Result:
        getFile = GTWidget::findPushButton(os, "pbGetResultSequence");
        break;
    }

    GTWidget::click(os, getFile);
    while (!getFile->isEnabled()) {
        GTGlobals::sleep(500);
    }
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto inputSequence = GTUtilsSequenceView::getSequenceAsString(os);
    auto inputSequenceName = GTUtilsSequenceView::getSeqName(os);
    GTFileDialog::openFile(os, seqenceFilePath);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    auto expectedInputSequence = GTUtilsSequenceView::getSequenceAsString(os);
    auto expectedInputSequenceName = GTUtilsSequenceView::getSeqName(os);
    if (!sequencesOnly) {
        CHECK_SET_ERR(inputSequenceName == expectedInputSequenceName, QString("Input \"%1\" and expected input \"%2\" sequence names are different")
            .arg(inputSequenceName).arg(expectedInputSequenceName));
    }
    CHECK_SET_ERR(inputSequence == expectedInputSequence, QString("Input \"%1\" and expected input \"%2\" sequences are different")
        .arg(inputSequence).arg(expectedInputSequence));

}

}