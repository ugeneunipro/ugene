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

#include "TrimmomaticDialogFiller.h"

#include <QApplication>
#include <QToolButton>
#include <QVBoxLayout>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTUtilsWorkflowDesigner.h"
#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTComboBox.h"
#include "primitives/GTDoubleSpinBox.h"
#include "primitives/GTGroupBox.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTListWidget.h"
#include "primitives/GTMenu.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTTableView.h"
#include "primitives/GTWidget.h"

namespace U2 {

using namespace HI;

#define GT_CLASS_NAME "TrimmomaticDialogFiller"

const QMap<TrimmomaticDialogFiller::TrimmomaticSteps, QString> TrimmomaticDialogFiller::STEPS = {
    {TrimmomaticDialogFiller::TrimmomaticSteps::AVGQUAL, "AVGQUAL"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::CROP, "CROP"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::HEADCROP, "HEADCROP"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::ILLUMINACLIP, "ILLUMINACLIP"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::LEADING, "LEADING"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::MAXINFO, "MAXINFO"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::MINLEN, "MINLEN"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::SLIDINGWINDOW, "SLIDINGWINDOW"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::TOPHRED33, "TOPHRED33"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::TOPHRED64, "TOPHRED64"},
    {TrimmomaticDialogFiller::TrimmomaticSteps::TRAILING, "TRAILING"}};

const QMap<TrimmomaticDialogFiller::TrimmomaticValues, QString> TrimmomaticDialogFiller::VALUES = {
    {TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold, "sbQualityThreshold"},
    {TrimmomaticDialogFiller::TrimmomaticValues::Length, "leLength"},
    {TrimmomaticDialogFiller::TrimmomaticValues::AdapterSequence, "fileName"},
    {TrimmomaticDialogFiller::TrimmomaticValues::SeedMismatches, "mismatches"},
    {TrimmomaticDialogFiller::TrimmomaticValues::PalindromeClipThreshold, "palindromeThreshold"},
    {TrimmomaticDialogFiller::TrimmomaticValues::SimpleClipThreshold, "simpleThreshold"},
    {TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings, "groupBox"},
    {TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength, "minLengthSpin"},
    {TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads, "keepBothCombo"},
    {TrimmomaticDialogFiller::TrimmomaticValues::TargetLength, "sbTargetLength"},
    {TrimmomaticDialogFiller::TrimmomaticValues::Strictness, "dsbStrictness"},
    {TrimmomaticDialogFiller::TrimmomaticValues::WindowSize, "leWindowSize"}};

void TrimmomaticDialogFiller::openDialog(HI::GUITestOpStatus& os, WorkflowProcessItem* trimmomaticElement) {
    GTUtilsWorkflowDesigner::click(os, trimmomaticElement);
    auto table = GTWidget::findTableView(os, "table");
    GTMouseDriver::moveTo(GTTableView::getCellPoint(os, table, 1, 1));
    GTMouseDriver::click();
    GTGlobals::sleep();
    GTWidget::click(os, GTWidget::findWidget(os, "trimmomaticPropertyToolButton", table));
    GTGlobals::sleep(500);
}

TrimmomaticDialogFiller::TrimmomaticDialogFiller(HI::GUITestOpStatus& os, const QList<QPair<TrimmomaticSteps, QMap<TrimmomaticValues, QVariant>>>& _addValues)
    : Filler(os, "TrimmomaticPropertyDialog"), addValues(_addValues) {
    a = U2::TrimmomaticDialogFiller::Action::AddSteps;
}

TrimmomaticDialogFiller::TrimmomaticDialogFiller(HI::GUITestOpStatus& os, const QList<QPair<QPair<TrimmomaticSteps, int>, TrimmomaticDirection>>& _moveValues)
    : Filler(os, "TrimmomaticPropertyDialog"), moveValues(_moveValues) {
    a = U2::TrimmomaticDialogFiller::Action::MoveSteps;
}

TrimmomaticDialogFiller::TrimmomaticDialogFiller(HI::GUITestOpStatus& os, const QList<QPair<TrimmomaticSteps, int>>& _removeValues)
    : Filler(os, "TrimmomaticPropertyDialog"), removeValues(_removeValues) {
    a = U2::TrimmomaticDialogFiller::Action::RemoveSteps;
}

TrimmomaticDialogFiller::TrimmomaticDialogFiller(HI::GUITestOpStatus& os, CustomScenario* customScenario)
    : Filler(os, "TrimmomaticPropertyDialog", customScenario) {
    a = U2::TrimmomaticDialogFiller::Action::AddSteps;
}

#define GT_METHOD_NAME "run"
void TrimmomaticDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    switch (a) {
        case U2::TrimmomaticDialogFiller::Action::AddSteps:
            addSteps();
            break;
        case U2::TrimmomaticDialogFiller::Action::MoveSteps:
            moveSteps();
            break;
        case U2::TrimmomaticDialogFiller::Action::RemoveSteps:
            removeSteps();
            break;
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

void TrimmomaticDialogFiller::addSteps() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    for (auto step : qAsConst(addValues)) {
        auto stepString = STEPS.value(step.first);
        GT_CHECK(!stepString.isEmpty(), "Step not found");

        GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
        QMenu* menu = qobject_cast<QMenu*>(GTWidget::findWidget(os, "stepsMenu"));
        GTMenu::clickMenuItemByName(os, menu, {stepString});
        GTKeyboardDriver::keyClick(Qt::Key_Escape);
        GTGlobals::sleep(500);

        auto settings = step.second;
        auto keys = settings.keys();
        for (int i = 0; i < keys.size(); i++) {
            const auto& set = keys[i];
            auto widgetName = VALUES.value(set);
            GT_CHECK(!widgetName.isEmpty(), "widgetName not found");

            auto widget = GTWidget::findWidget(os, "widgetStepSettings", dialog);

            auto l = qobject_cast<QVBoxLayout*>(widget->layout());
            GT_CHECK(l != nullptr, "layout not found");

            auto parentWidget = l->itemAt(l->count() - 1)->widget();
            GT_CHECK(parentWidget != nullptr, "parentWidget not found");

            switch (set) {
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::QualityThreshold:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::SeedMismatches:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::PalindromeClipThreshold:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::SimpleClipThreshold:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::TargetLength:
                    GTSpinBox::setValue(os, widgetName, settings[set].toInt(), parentWidget);
                    break;
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::Strictness:
                    GTDoubleSpinbox::setValue(os, widgetName, settings[set].toDouble(), HI::GTGlobals::UseKey, parentWidget);
                    break;
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::Length:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::AdapterSequence:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::WindowSize:
                    GTLineEdit::setText(os, widgetName, settings[set].toString(), parentWidget);
                    break;
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength:
                case U2::TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads:
                    GTWidget::click(os, GTWidget::findPushButton(os, "pushButton"));
                    GTGlobals::sleep(200);
                    QWidget* addSettingsDialog = GTWidget::getActiveModalWidget(os);

                    if (keys.contains(U2::TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings)) {
                        auto provide = settings[U2::TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings];
                        keys.removeOne(U2::TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings);
                        bool provideBool = provide.toBool();
                        if (!provideBool) {
                            GTUtilsDialog::clickButtonBox(os, addSettingsDialog, QDialogButtonBox::Ok);
                        }

                        GTGroupBox::setChecked(os, VALUES[U2::TrimmomaticDialogFiller::TrimmomaticValues::ProvideOptionalSettings], provideBool, addSettingsDialog);

                        if (keys.contains(U2::TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength)) {
                            auto minLength = settings[U2::TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength];
                            keys.removeOne(U2::TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength);
                            auto minNameValue = minLength.toInt();
                            GTSpinBox::setValue(os, VALUES[U2::TrimmomaticDialogFiller::TrimmomaticValues::MinAdapterLength], minNameValue, addSettingsDialog);
                        }

                        if (keys.contains(U2::TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads)) {
                            auto keepBoth = settings[U2::TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads];
                            keys.removeOne(U2::TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads);
                            auto keepBothValue = keepBoth.toString();
                            GTComboBox::selectItemByText(os, VALUES[U2::TrimmomaticDialogFiller::TrimmomaticValues::KeepBothReads], addSettingsDialog, keepBothValue);
                        }
                    }

                    GTUtilsDialog::clickButtonBox(os, addSettingsDialog, QDialogButtonBox::Ok);
                    break;
            }
        }
    }
}

void TrimmomaticDialogFiller::moveSteps() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    for (auto step : qAsConst(moveValues)) {
        auto stepString = STEPS.value(step.first.first);
        GT_CHECK(!stepString.isEmpty(), "Step not found");

        auto listSteps = GTWidget::findListWidget(os, "listSteps", dialog);

        GTListWidget::click(os, listSteps, stepString, Qt::LeftButton, step.first.second);

        QString buttonName;
        switch (step.second) {
            case U2::TrimmomaticDialogFiller::TrimmomaticDirection::Up:
                buttonName = "buttonUp";
                break;
            case U2::TrimmomaticDialogFiller::TrimmomaticDirection::Down:
                buttonName = "buttonDown";
                break;
        }
        GTWidget::click(os, GTWidget::findToolButton(os, buttonName, dialog));
    }
}

void TrimmomaticDialogFiller::removeSteps() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    for (auto step : qAsConst(removeValues)) {
        auto stepString = STEPS.value(step.first);
        GT_CHECK(!stepString.isEmpty(), "Step not found");

        auto listSteps = GTWidget::findListWidget(os, "listSteps", dialog);

        GTListWidget::click(os, listSteps, stepString, Qt::LeftButton, step.second);

        GTWidget::click(os, GTWidget::findToolButton(os, "buttonRemove", dialog));
    }
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
