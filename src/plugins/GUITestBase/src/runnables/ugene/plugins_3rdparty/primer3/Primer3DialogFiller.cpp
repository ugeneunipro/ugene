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

#include <U2Core/global.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include "Primer3DialogFiller.h"

namespace U2 {
using namespace HI;

const QStringList Primer3DialogFiller::PREFIXES = { "edit_", "checkbox_", "combobox_" };
const QStringList Primer3DialogFiller::DOUBLE_WITH_CHECK_NAMES = { "PRIMER_PRODUCT_OPT_TM", 
                                                                   "PRIMER_OPT_GC_PERCENT", 
                                                                   "PRIMER_INTERNAL_OPT_GC_PERCENT" };
const QStringList Primer3DialogFiller::DEBUG_PARAMETERS = { "SEQUENCE_ID", "SEQUENCE_TEMPLATE", "PRIMER_EXPLAIN_FLAG", "PRIMER_SECONDARY_STRUCTURE_ALIGNMENT"};
const QMap<QString, QString> Primer3DialogFiller::LIBRARIES_PATH_AND_NAME = { 
                                                {"drosophila.w.transposons.txt", "DROSOPHILA"}, 
                                                {"humrep_and_simple.txt", "HUMAN"}, 
                                                {"rodent_ref.txt", "RODENT"}, 
                                                {"rodrep_and_simple.txt", "RODENT_AND_SIMPLE"}};

#define GT_CLASS_NAME "GTUtilsDialog::KalignDialogFiller"
Primer3DialogFiller::Primer3DialogFiller(HI::GUITestOpStatus& os, const Primer3Settings& settings)
    : Filler(os, "Primer3Dialog"),
      settings(settings) {
}

#define GT_METHOD_NAME "run"
void Primer3DialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (settings.resultsCount != -1) {
        auto resultsCountSpinBox = GTWidget::findSpinBox(os, "edit_PRIMER_NUM_RETURN", dialog);
        GTSpinBox::setValue(os, resultsCountSpinBox, settings.resultsCount);
    }

    auto tabWidget = GTWidget::findTabWidget(os, "tabWidget", dialog);
    if (!settings.primersGroupName.isEmpty() || !settings.primersName.isEmpty()) {
        GTTabWidget::setCurrentIndex(os, tabWidget, 7);

        if (!settings.primersGroupName.isEmpty()) {
            GTLineEdit::setText(os, "leGroupName", settings.primersGroupName, dialog);
        }

        if (!settings.primersName.isEmpty()) {
            GTLineEdit::setText(os, "leAnnotationName", settings.primersName, dialog);
        }
    }

    if (settings.start != -1) {
        GTLineEdit::setText(os, "start_edit_line", QString::number(settings.start), dialog);
    }

    if (settings.end != -1) {
        GTLineEdit::setText(os, "end_edit_line", QString::number(settings.end), dialog);
    }
    auto leftCheckbox = GTWidget::findCheckBox(os, "checkbox_PRIMER_PICK_LEFT_PRIMER", dialog);
    GTCheckBox::setChecked(os, leftCheckbox, settings.pickLeft);

    auto rightCheckbox = GTWidget::findCheckBox(os, "checkbox_PRIMER_PICK_RIGHT_PRIMER", dialog);
    GTCheckBox::setChecked(os, rightCheckbox, settings.pickRight);

    auto intCheckbox = GTWidget::findCheckBox(os, "checkbox_PRIMER_PICK_INTERNAL_OLIGO", dialog);
    GTCheckBox::setChecked(os, intCheckbox, settings.pickInternal);

    if (settings.rtPcrDesign) {
        GTTabWidget::setCurrentIndex(os, tabWidget, 6);

        auto groupBox = GTWidget::findGroupBox(os, "spanIntronExonBox", dialog);
        GTGroupBox::setChecked(os, groupBox);
    }

    if (!settings.filePath.isEmpty()) {
        if (!settings.loadManually) {
            auto utils = new GTFileDialogUtils(os, settings.filePath);
            GTUtilsDialog::waitForDialog(os, utils);
            GTWidget::click(os, GTWidget::findWidget(os, "loadSettingsButton", dialog));
        } else {
            loadFromFileManually(dialog);
        }
    }

    if (settings.notRun) {
        GTKeyboardDriver::keyClick(Qt::Key_Escape);
        return;
    }

    if (!settings.shortRegion) {
        auto button = GTWidget::findPushButton(os, "pickPrimersButton", dialog);
        GTWidget::click(os, button);
    } else {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
        auto button = GTWidget::findPushButton(os, "pickPrimersButton", dialog);
        GTWidget::click(os, button);
        if (isOsMac()) {
            dialog->close();
        } else {
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "loadFromFileManually"
void Primer3DialogFiller::loadFromFileManually(QWidget* parent) {
    QFile file(settings.filePath);
    file.open(QIODevice::ReadOnly);

    QTextStream stream(&file);
    QMap<QWidget*, Widgets> tabsAndWidgets;
    while (!stream.atEnd()) {
        auto line = stream.readLine();
        CHECK_BREAK(line != "=");
        auto par = line.split('=');
        CHECK_CONTINUE(par.size() == 2);
        CHECK_CONTINUE(!DEBUG_PARAMETERS.contains(par.first()));

        QWidget* widget = nullptr;
        for (const auto& prefix : qAsConst(PREFIXES)) {
            GTGlobals::FindOptions o;
            o.failIfNotFound = false;
            widget = GTWidget::findWidget(os, prefix + par.first(), parent, o);
            CHECK_BREAK(widget == nullptr);
        }

        GT_CHECK(widget != nullptr, QString("Parameter %1 not found").arg(par.first()));

        auto widgetTab = getWidgetTab(widget);
        auto ws = tabsAndWidgets.value(widgetTab);

        if (qobject_cast<QSpinBox*>(widget) != nullptr) {
            auto s = qobject_cast<QSpinBox*>(widget);
            ws.spin.append({ s, par.last() });
        } else if (qobject_cast<QCheckBox*>(widget) != nullptr) {
            auto c = qobject_cast<QCheckBox*>(widget);
            ws.check.append({ c, par.last() });
        } else if (qobject_cast<QComboBox*>(widget) != nullptr) {
            auto c = qobject_cast<QComboBox*>(widget);
            ws.combo.append({ c, par.last() });
        } else if (qobject_cast<QDoubleSpinBox*>(widget) != nullptr) {
            auto d = qobject_cast<QDoubleSpinBox*>(widget);
            ws.doubleSpin.append({ d, par.last() });
        } else if (qobject_cast<QLineEdit*>(widget) != nullptr) {
            auto l = qobject_cast<QLineEdit*>(widget);
            ws.line.append({ l, par.last() });
        } else if (qobject_cast<QPlainTextEdit*>(widget) != nullptr) {
            auto plain = qobject_cast<QPlainTextEdit*>(widget);
            ws.plainText = { plain, par.last() };
        }

        if (DOUBLE_WITH_CHECK_NAMES.contains(par.first())) {
            auto check = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "label_" + par.first(), widgetTab));
            ws.check.append({ check, "1" });
        }

        tabsAndWidgets.insert(widgetTab, ws);
    }
    file.close();


    auto tabWidget = GTWidget::findWidget(os, "tabWidget", parent);
    auto tw = qobject_cast<QTabWidget*>(tabWidget);
    const auto& keys = tabsAndWidgets.keys();
    QList<QPair<QWidget*, Widgets>> tabsAndWidgetsPair;
    for (auto w : qAsConst(keys)) {
        tabsAndWidgetsPair.append({ w, tabsAndWidgets.value(w) });
    }
    std::sort(tabsAndWidgetsPair.begin(), tabsAndWidgetsPair.end(), 
        [=](const QPair<QWidget*, Widgets>& first, const QPair<QWidget*, Widgets>& second) {
        return tw->indexOf(first.first) < tw->indexOf(second.first);
    });


    for (auto tab : qAsConst(tabsAndWidgetsPair)) {
        auto name = GTTabWidget::getTabNameByWidget(os, tw, tab.first);
        GTTabWidget::clickTab(os, tw, name);

        const auto& widgets2click = tab.second;
        for (const auto& s : widgets2click.spin) {
            bool ok = false;
            auto v = s.second.toInt(&ok);
            GT_CHECK(ok, QString("Can't cast QSpinBox value to int: %1").arg(s.second));

            GTSpinBox::setValue(os, s.first, v, GTGlobals::UseMethod::UseKeyBoard);
        }
        for (const auto& c : widgets2click.check) {
            bool ok = false;
            auto v = c.second.toInt(&ok);
            GT_CHECK(ok, QString("Can't cast QCheckBox value to int: %1").arg(c.second));

            GTCheckBox::setChecked(os, c.first, bool(v));
        }
        for (const auto& c : widgets2click.combo) {
            bool ok = false;
            auto v = c.second.toInt(&ok);
            if (ok) {
                GTComboBox::selectItemByIndex(os, c.first, v);
            } else {
                auto index = LIBRARIES_PATH_AND_NAME.value(c.second, c.second);
                GTComboBox::selectItemByText(os, c.first, index);
            }
        }
        for (const auto& d : widgets2click.doubleSpin) {
            bool ok = false;
            auto v = d.second.toDouble(&ok);
            GT_CHECK(ok, QString("Can't cast QDoubleSpinBox value to double: %1").arg(d.second));

            //auto method = d.second.split(QRegExp("[.,]")).removeAll("0") == 2 ? GTGlobals::UseMethod::UseKey : GTGlobals::UseMethod::UseKeyBoard;
            GTDoubleSpinbox::setValue(os, d.first, v, GTGlobals::UseMethod::UseKeyBoard);
        }
        for (const auto& l : widgets2click.line) {
            GTLineEdit::setText(os, l.first, l.second);
        }
        if (widgets2click.plainText.first != nullptr) {
            GTPlainTextEdit::setPlainText(os, widgets2click.plainText.first, widgets2click.plainText.second);
        }
    }
}
#undef GT_METHOD_NAME

QWidget* Primer3DialogFiller::getWidgetTab(QWidget* wt) const {
    auto parent = wt->parent();
    while (!parent->objectName().startsWith("tab_")) {
        parent = parent->parent();
    }
    return qobject_cast<QWidget*>(parent);
}

void Primer3DialogFiller::findAllChildrenWithNames(QObject* obj, QMap<QString, QObject*>& children) {
        for(QObject * o : obj->children()) {
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
