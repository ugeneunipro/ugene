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
#include <base_dialogs/GTFileDialog.h>
#include <functional>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include <QStackedWidget>
#include <QTextBrowser>

#include <U2Core/L10n.h>

#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

const QMap<GTUtilsGeneCut::Steps, QString> GTUtilsGeneCut::STEP_ENUM_2_STRING = {
    {GTUtilsGeneCut::Steps::OptimizeCodonContext, "Optimize codon context"},
    {GTUtilsGeneCut::Steps::ExcludeRestrictionSites, "Exclude restrictions and splicing sites"},
    {GTUtilsGeneCut::Steps::LongFragmentsAssembly, "Long fragments assembly"},
    {GTUtilsGeneCut::Steps::OligonucleotidesAssembly, "Oligonucleotides assembly"}};

const QMap<GTUtilsGeneCut::Status, QString> GTUtilsGeneCut::STATUS_ENUM_2_STRING = {
    {GTUtilsGeneCut::Status::Completed, "Completed"},
    {GTUtilsGeneCut::Status::CompletedWithError, "Completed with error"},
    {GTUtilsGeneCut::Status::Interrupted, "Interrupted"}};

void GTUtilsGeneCut::login(const QString& email, const QString& password, bool showPassword, bool rememberMe) {
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Tabs::GeneCut);
    GTLineEdit::setText(GTWidget::findLineEdit("leEmail"), email);
    GTLineEdit::setText(GTWidget::findLineEdit("lePasword"), password);
    GTCheckBox::setChecked("cbShowPass", showPassword);
    GTCheckBox::setChecked("cbRememberMe", rememberMe);
    GTWidget::click(GTWidget::findPushButton("pbLogin"));
    auto mainStacKWidget = qobject_cast<QStackedWidget*>(GTWidget::findWidget("stackedWidget"));
    CHECK_SET_ERR(mainStacKWidget != nullptr, L10N::nullPointerError("QStackedWidget"));

    while (mainStacKWidget->currentIndex() != GENECUT_MAIN_FORM_INDEX) {
        GTGlobals::sleep(500);
    }
}

void GTUtilsGeneCut::resetPassword(const QString& email) {
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Tabs::GeneCut);
    GTWidget::click(GTWidget::findPushButton("pbForgot"));
    GTLineEdit::setText(GTWidget::findLineEdit("leResetPassword"), email);
    auto pbReset = GTWidget::findPushButton("pbReset");
    GTWidget::click(pbReset);
    while (!pbReset->isEnabled()) {
        GTGlobals::sleep(500);
    }
}

void GTUtilsGeneCut::createNewUser(
    const QString& email,
    const QString& password,
    const QString& passwordConfirm,
    const QString& firstName,
    const QString& secondName) {
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Tabs::GeneCut);
    GTWidget::click(GTWidget::findPushButton("pbRegister"));
    GTLineEdit::setText(GTWidget::findLineEdit("leEmailNew"), email);
    GTLineEdit::setText(GTWidget::findLineEdit("lePasswordNew"), password);
    GTLineEdit::setText(GTWidget::findLineEdit("lePasswordConformationNew"), passwordConfirm);
    GTLineEdit::setText(GTWidget::findLineEdit("leFirstName"), firstName);
    GTLineEdit::setText(GTWidget::findLineEdit("leLastName"), secondName);
    auto pbRegisterNew = GTWidget::findPushButton("pbRegisterNew");
    GTWidget::click(pbRegisterNew);
    while (!pbRegisterNew->isEnabled()) {
        GTGlobals::sleep(500);
    }
}

void GTUtilsGeneCut::selectResultByIndex(int index, bool fetchResults) {
    auto mainStacKWidget = qobject_cast<QStackedWidget*>(GTWidget::findWidget("stackedWidget"));
    CHECK_SET_ERR(mainStacKWidget != nullptr, L10N::nullPointerError("QStackedWidget"));
    CHECK_SET_ERR(mainStacKWidget->currentIndex() == GENECUT_MAIN_FORM_INDEX, "Login should be called before");

    if (fetchResults) {
        auto pbFetchResults = GTWidget::findPushButton("pbFetchResults", mainStacKWidget);
        GTWidget::click(pbFetchResults);
        while (!pbFetchResults->isEnabled()) {
            GTGlobals::sleep(500);
        }
    }
    GTTableView::click(GTWidget::findTableWidget("twResults", mainStacKWidget), index, 0);
}

void GTUtilsGeneCut::checkResultInfo(const QString& inputFileName, const QList<Steps>& steps, Status status) {
    auto twResults = GTWidget::findTableWidget("twResults");
    CHECK_SET_ERR(twResults->rowCount() != 0, "selectResultByIndex should be called before");

    auto currentStatus = twResults->item(twResults->currentRow(), 1)->text();
    CHECK_SET_ERR(currentStatus == STATUS_ENUM_2_STRING.value(status), QString("Expected status: %1").arg(STATUS_ENUM_2_STRING.value(status)));

    auto tbShortDescription = GTWidget::findTextBrowser("tbShortDescription");
    auto text = tbShortDescription->toPlainText();
    CHECK_SET_ERR(!text.isEmpty(), "tbShortDescription is empty");
    CHECK_SET_ERR(text.contains(inputFileName), QString("Expected input file name: %1").arg(inputFileName));
    for (auto step : qAsConst(steps)) {
        CHECK_SET_ERR(text.contains(STEP_ENUM_2_STRING.value(step)), QString("Expected step: %1").arg(STEP_ENUM_2_STRING.value(step)));
    }
}

void GTUtilsGeneCut::compareFiles(FileType fileType, const QString& seqenceFilePath, bool sequencesOnly) {
    auto twResults = GTWidget::findTableWidget("twResults");
    CHECK_SET_ERR(twResults->rowCount() != 0, "selectResultByIndex should be called before");

    QPushButton* getFile = nullptr;
    switch (fileType) {
        case U2::GTUtilsGeneCut::FileType::Input:
            getFile = GTWidget::findPushButton("pbGetInputSequence");
            break;
        case U2::GTUtilsGeneCut::FileType::Result:
            getFile = GTWidget::findPushButton("pbGetResultSequence");
            break;
    }

    GTWidget::click(getFile);
    while (!getFile->isEnabled()) {
        GTGlobals::sleep(500);
    }
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    auto inputSequence = GTUtilsSequenceView::getSequenceAsString();
    auto inputSequenceName = GTUtilsSequenceView::getSeqName();
    GTFileDialog::openFile(seqenceFilePath);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    auto expectedInputSequence = GTUtilsSequenceView::getSequenceAsString();
    auto expectedInputSequenceName = GTUtilsSequenceView::getSeqName();
    if (!sequencesOnly) {
        CHECK_SET_ERR(inputSequenceName == expectedInputSequenceName, QString("Input \"%1\" and expected input \"%2\" sequence names are different").arg(inputSequenceName).arg(expectedInputSequenceName));
    }
    CHECK_SET_ERR(inputSequence == expectedInputSequence, QString("Input \"%1\" and expected input \"%2\" sequences are different").arg(inputSequence).arg(expectedInputSequence));
}

}  // namespace U2