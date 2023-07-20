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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextStream>

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

#include "Primer3DialogFiller.h"

namespace U2 {
using namespace HI;

static constexpr const int RT_PCR_DESIGN_TAB_NUMBER = 6;
static constexpr const int RESULT_ANNOTATION_SETTINGS_TAB_NUMBER = 7;

const QStringList PREFIXES = {"edit_", "checkbox_", "combobox_"};
const QStringList DOUBLE_WITH_CHECK_NAMES = {"PRIMER_PRODUCT_OPT_TM",
                                             "PRIMER_OPT_GC_PERCENT",
                                             "PRIMER_INTERNAL_OPT_GC_PERCENT"};
const QStringList DEBUG_PARAMETERS = {"SEQUENCE_ID", "SEQUENCE_TEMPLATE", "PRIMER_EXPLAIN_FLAG", "PRIMER_SECONDARY_STRUCTURE_ALIGNMENT"};
const QMap<QString, QString> LIBRARIES_PATH_AND_NAME = {
    {"drosophila.w.transposons.txt", "DROSOPHILA"},
    {"humrep_and_simple.txt", "HUMAN"},
    {"rodent_ref.txt", "RODENT"},
    {"rodrep_and_simple.txt", "RODENT_AND_SIMPLE"}};

#define GT_CLASS_NAME "GTUtilsDialog::KalignDialogFiller"
Primer3DialogFiller::Primer3DialogFiller(const Primer3Settings& settings)
    : Filler("Primer3Dialog"),
      settings(settings) {
}

void Primer3DialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (settings.resultsCount != -1) {
        auto resultsCountSpinBox = GTWidget::findSpinBox("edit_PRIMER_NUM_RETURN", dialog);
        GTSpinBox::setValue(resultsCountSpinBox, settings.resultsCount);
    }

    auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);
    if (!settings.primersGroupName.isEmpty() || !settings.primersName.isEmpty()) {
        GTTabWidget::setCurrentIndex(tabWidget, RESULT_ANNOTATION_SETTINGS_TAB_NUMBER);

        if (!settings.primersGroupName.isEmpty()) {
            GTLineEdit::setText("leGroupName", settings.primersGroupName, dialog);
        }

        if (!settings.primersName.isEmpty()) {
            GTLineEdit::setText("leAnnotationName", settings.primersName, dialog);
        }
    }

    if (settings.start != -1) {
        GTLineEdit::setText("start_edit_line", QString::number(settings.start), dialog);
    }

    if (settings.end != -1) {
        GTLineEdit::setText("end_edit_line", QString::number(settings.end), dialog);
    }
    auto leftCheckbox = GTWidget::findCheckBox("checkbox_PRIMER_PICK_LEFT_PRIMER", dialog);
    GTCheckBox::setChecked(leftCheckbox, settings.pickLeft);

    auto rightCheckbox = GTWidget::findCheckBox("checkbox_PRIMER_PICK_RIGHT_PRIMER", dialog);
    GTCheckBox::setChecked(rightCheckbox, settings.pickRight);

    auto intCheckbox = GTWidget::findCheckBox("checkbox_PRIMER_PICK_INTERNAL_OLIGO", dialog);
    GTCheckBox::setChecked(intCheckbox, settings.pickInternal);

    if (settings.rtPcrDesign) {
        GTTabWidget::setCurrentIndex(tabWidget, RT_PCR_DESIGN_TAB_NUMBER);

        auto groupBox = GTWidget::findGroupBox("spanIntronExonBox", dialog);
        GTGroupBox::setChecked(groupBox);

        if (!settings.exonRangeLine.isEmpty()) {
            GTLineEdit::setText("edit_exonRange", settings.exonRangeLine);
        }
    }

    if (!settings.filePath.isEmpty()) {
        if (!settings.loadManually) {
            auto utils = new GTFileDialogUtils(settings.filePath);
            GTUtilsDialog::waitForDialog(utils);
            GTWidget::click(GTWidget::findWidget("loadSettingsButton", dialog));
        } else {
            loadFromFileManually(dialog);
        }
    }

    if (settings.notRun) {
        GTWidget::click(GTWidget::findWidget("closeButton", dialog));
        return;
    }

    if (!settings.hasValidationErrors) {
        auto button = GTWidget::findPushButton("pickPrimersButton", dialog);
        GTWidget::click(button);
    } else {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(
            settings.continueIfError ? QMessageBox::Ok : QMessageBox::Cancel,
            settings.validationErrorsText));
        auto button = GTWidget::findPushButton("pickPrimersButton", dialog);
        GTWidget::click(button);

        for (const auto& errorWidget : qAsConst(settings.errorWidgetsNames)) {
            auto wgt = GTWidget::findWidget(errorWidget, dialog);
            auto ss = wgt->styleSheet();
            GT_CHECK(ss.contains("#ffc8c8"), QString("Incorrect color for %1").arg(errorWidget));
        }

        if (!settings.continueIfError) {
            GTWidget::click(GTWidget::findWidget("closeButton", dialog));
        }
    }
}

void Primer3DialogFiller::loadFromFileManually(QWidget* parent) {
    // Open the settings file
    QFile file(settings.filePath);
    file.open(QIODevice::ReadOnly);

    // Read the file line-by-line
    QTextStream stream(&file);
    QMap<QWidget*, Widgets> tabsAndWidgets;
    while (!stream.atEnd()) {
        // Split the line to the parameter and its value
        auto line = stream.readLine();
        CHECK_BREAK(line != "=");
        auto par = line.split('=');
        CHECK_CONTINUE(par.size() == 2);
        CHECK_CONTINUE(!DEBUG_PARAMETERS.contains(par.first()));

        // Find out a widget, which correspondes to this parameter
        QWidget* widget = nullptr;
        for (const auto& prefix : qAsConst(PREFIXES)) {
            GTGlobals::FindOptions o;
            o.failIfNotFound = false;
            widget = GTWidget::findWidget(prefix + par.first(), parent, o);
            CHECK_BREAK(widget == nullptr);
        }

        GT_CHECK(widget != nullptr, QString("Parameter %1 not found").arg(par.first()));

        // Find out a tab this widget located at
        auto widgetTab = getWidgetTab(widget);
        GT_CHECK(widgetTab != nullptr, "Tab widget isn't found");

        // Add the widget to the corresponding tab
        auto ws = tabsAndWidgets.value(widgetTab);
        if (auto s = qobject_cast<QSpinBox*>(widget)) {
            ws.spin.append({s, par.last()});
        } else if (auto c = qobject_cast<QCheckBox*>(widget)) {
            ws.check.append({c, par.last()});
        } else if (auto cb = qobject_cast<QComboBox*>(widget)) {
            ws.combo.append({cb, par.last()});
        } else if (auto d = qobject_cast<QDoubleSpinBox*>(widget)) {
            ws.doubleSpin.append({d, par.last()});
        } else if (auto l = qobject_cast<QLineEdit*>(widget)) {
            ws.line.append({l, par.last()});
        } else if (auto plain = qobject_cast<QPlainTextEdit*>(widget)) {
            ws.plainText = {plain, par.last()};
        }

        // Some widgets have a checkboxes, which should be enabled - find them
        if (DOUBLE_WITH_CHECK_NAMES.contains(par.first())) {
            auto check = qobject_cast<QCheckBox*>(GTWidget::findWidget("label_" + par.first(), widgetTab));
            ws.check.append({check, "1"});
        }

        tabsAndWidgets.insert(widgetTab, ws);
    }
    file.close();

    // Sort tabs by their order
    auto tabWidget = GTWidget::findWidget("tabWidget", parent);
    auto tw = qobject_cast<QTabWidget*>(tabWidget);
    const auto& keys = tabsAndWidgets.keys();
    QList<QPair<QWidget*, Widgets>> tabsAndWidgetsPair;
    for (auto w : qAsConst(keys)) {
        tabsAndWidgetsPair.append({w, tabsAndWidgets.value(w)});
    }
    std::sort(tabsAndWidgetsPair.begin(), tabsAndWidgetsPair.end(), [=](const QPair<QWidget*, Widgets>& first, const QPair<QWidget*, Widgets>& second) {
        return tw->indexOf(first.first) < tw->indexOf(second.first);
    });

    // Click everything
    for (auto tab : qAsConst(tabsAndWidgetsPair)) {
        auto name = GTTabWidget::getTabNameByWidget(tw, tab.first);
        GTTabWidget::clickTab(tw, name);

        const auto& widgets2click = tab.second;
        for (const auto& s : qAsConst(widgets2click.spin)) {
            bool ok = false;
            auto v = s.second.toInt(&ok);
            GT_CHECK(ok, QString("Can't cast QSpinBox value to int: %1").arg(s.second));

            GTSpinBox::setValue(s.first, v, GTGlobals::UseMethod::UseKeyBoard);
        }
        for (const auto& c : qAsConst(widgets2click.check)) {
            bool ok = false;
            auto v = c.second.toInt(&ok);
            GT_CHECK(ok, QString("Can't cast QCheckBox value to int: %1").arg(c.second));

            GTCheckBox::setChecked(c.first, bool(v));
        }
        for (const auto& c : qAsConst(widgets2click.combo)) {
            bool ok = false;
            auto v = c.second.toInt(&ok);
            if (ok) {
                GTComboBox::selectItemByIndex(c.first, v);
            } else {
                auto index = LIBRARIES_PATH_AND_NAME.value(c.second, c.second);
                GTComboBox::selectItemByText(c.first, index);
            }
        }
        for (const auto& d : qAsConst(widgets2click.doubleSpin)) {
            bool ok = false;
            auto v = d.second.toDouble(&ok);
            GT_CHECK(ok, QString("Can't cast QDoubleSpinBox value to double: %1").arg(d.second));

            // auto method = d.second.split(QRegExp("[.,]")).removeAll("0") == 2 ? GTGlobals::UseMethod::UseKey : GTGlobals::UseMethod::UseKeyBoard;
            GTDoubleSpinbox::setValue(d.first, v, GTGlobals::UseMethod::UseKeyBoard);
        }
        for (const auto& l : qAsConst(widgets2click.line)) {
            GTLineEdit::setText(l.first, l.second);
        }
        if (widgets2click.plainText.first != nullptr) {
            GTPlainTextEdit::setText(widgets2click.plainText.first, widgets2click.plainText.second);
        }
    }
}

QWidget* Primer3DialogFiller::getWidgetTab(QWidget* wt) const {
    auto parent = wt->parent();
    while (!parent->objectName().startsWith("tab_")) {
        parent = parent->parent();
    }
    return qobject_cast<QWidget*>(parent);
}

void Primer3DialogFiller::findAllChildrenWithNames(QObject* obj, QMap<QString, QObject*>& children) {
    for (QObject* o : obj->children()) {
        auto name = o->objectName();
        for (const auto& prefix : qAsConst(PREFIXES)) {
            name = name.remove(prefix);
        }
        if (!name.isEmpty()) {
            children.insert(name, o);
        }

        findAllChildrenWithNames(o, children);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
