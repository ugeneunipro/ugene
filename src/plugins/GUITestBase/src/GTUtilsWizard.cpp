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
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QScrollArea>
#include <QWizard>

#include <U2Core/global.h>

#include "GTUtilsWizard.h"

namespace U2 {
using namespace HI;

static void expandWizardParameterIfNeeded(HI::GUITestOpStatus& os, const QString& parameterName, QWidget* dialog) {
    auto propertyWidget = GTWidget::findWidget(os, parameterName + "_propertyWidget", dialog, {false});
    auto showHideButton = propertyWidget == nullptr ? nullptr : GTWidget::findToolButton(os, "showHideButton", propertyWidget, {false});
    if (showHideButton != nullptr && showHideButton->text() == "+") {
        GTWidget::click(os, showHideButton);
    }
}

QMap<QString, GTUtilsWizard::WizardButton> GTUtilsWizard::initButtonMap() {
    return {
        {"&Next >", Next},
        {"< &Back", Back},
        {"Apply", Apply},
        {"Run", Run},
        {"Cancel", Cancel},
        {"Defaults", Defaults},
        {"Setup", Setup},
        {"Finish", Finish},
    };
}
const QMap<QString, GTUtilsWizard::WizardButton> GTUtilsWizard::buttonMap = GTUtilsWizard::initButtonMap();

#define GT_CLASS_NAME "GTUtilsWizard"

#define GT_METHOD_NAME "setInputFiles"
void GTUtilsWizard::setInputFiles(HI::GUITestOpStatus& os, const QList<QStringList>& inputFiles) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    int i = 0;
    for (const QStringList& datasetFiles : qAsConst(inputFiles)) {
        QTabWidget* tabWidget = GTWidget::findWidgetByType<QTabWidget*>(os, dialog, "tabWidget not found");
        GTTabWidget::setCurrentIndex(os, tabWidget, i);

        QMap<QString, QStringList> dir2files;
        for (const QString& datasetFile : qAsConst(datasetFiles)) {
            QFileInfo fileInfo(datasetFile);
            dir2files[fileInfo.absoluteDir().path()] << fileInfo.fileName();
        }

        const QList<QString> dirList = dir2files.keys();
        for (const QString& dir : qAsConst(dirList)) {
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, dir, dir2files[dir]));
            QList<QWidget*> addFileButtonList = dialog->findChildren<QWidget*>("addFileButton");
            for (QWidget* addFileButton : qAsConst(addFileButtonList)) {
                if (addFileButton->isVisible()) {
                    GTWidget::click(os, addFileButton);
                    break;
                }
            }
        }
        GTThread::waitForMainThread();
        i++;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAllParameters"
void GTUtilsWizard::setAllParameters(HI::GUITestOpStatus& os, QMap<QString, QVariant> map) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QWizard* wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK(wizard, "activeModalWidget is not wizard");

    QWidget* nextButton;

    do {
        QMap<QString, QVariant>::iterator iter = map.begin();
        while (iter != map.end()) {
            const QString& parameterName = iter.key();
            auto w = GTWidget::findWidget(os, parameterName + " widget", wizard->currentPage(), {false});
            if (w != nullptr) {
                expandWizardParameterIfNeeded(os, parameterName, dialog);
                QScrollArea* area = wizard->currentPage()->findChild<QScrollArea*>();
                if (area != nullptr) {
                    area->ensureWidgetVisible(w);
                }
                setValue(os, w, iter.value());
                iter = map.erase(iter);

            } else {
                ++iter;
            }
        }
        nextButton = GTWidget::findButtonByText(os, "&Next >", wizard, {false});
        if (nextButton != nullptr && nextButton->isVisible()) {
            GTWidget::click(os, nextButton);
        }
    } while (nextButton != nullptr && nextButton->isVisible());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setParameter"
void GTUtilsWizard::setParameter(HI::GUITestOpStatus& os, const QString& parameterName, const QVariant& parameterValue) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QWizard* wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK(wizard, "activeModalWidget is not wizard");

    expandWizardParameterIfNeeded(os, parameterName, dialog);

    auto w = GTWidget::findWidget(os, parameterName + " widget", dialog);
    QScrollArea* area = wizard->currentPage()->findChild<QScrollArea*>();
    if (area != nullptr) {
        area->ensureWidgetVisible(w);
    }

    setValue(os, w, parameterValue);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getParameter"
QVariant GTUtilsWizard::getParameter(HI::GUITestOpStatus& os, const QString& parameterName) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QWizard* wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK_RESULT(wizard, "activeModalWidget is not wizard", {});

    expandWizardParameterIfNeeded(os, parameterName, dialog);

    auto w = GTWidget::findWidget(os, parameterName + " widget", dialog);

    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (combo != nullptr) {
        return QVariant(combo->currentText());
    }
    QSpinBox* spin = qobject_cast<QSpinBox*>(w);
    if (spin != nullptr) {
        return QVariant(spin->value());
    }
    QDoubleSpinBox* doubleSpin = qobject_cast<QDoubleSpinBox*>(w);
    if (doubleSpin != nullptr) {
        return QVariant(doubleSpin->value());
    }
    QLineEdit* line = qobject_cast<QLineEdit*>(w);
    if (line != nullptr) {
        return QVariant(line->text());
    }
    GT_FAIL(QString("unsupported widget class: %1").arg(w->metaObject()->className()), {});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setValue"
void GTUtilsWizard::setValue(HI::GUITestOpStatus& os, QWidget* w, QVariant value) {
    QComboBox* combo = qobject_cast<QComboBox*>(w);
    if (combo != nullptr) {
        GTComboBox::selectItemByText(os, combo, value.toString());
        return;
    }
    QSpinBox* spin = qobject_cast<QSpinBox*>(w);
    if (spin != nullptr) {
        bool ok;
        int val = value.toInt(&ok);
        GT_CHECK(ok, "spin box needs int value");
        GTSpinBox::setValue(os, spin, val, GTGlobals::UseKeyBoard);
        return;
    }
    QDoubleSpinBox* doubleSpin = qobject_cast<QDoubleSpinBox*>(w);
    if (doubleSpin != nullptr) {
        bool ok;
        int val = value.toDouble(&ok);
        GT_CHECK(ok, "double spin box needs double value");
        GTDoubleSpinbox::setValue(os, doubleSpin, val, GTGlobals::UseKeyBoard);
        return;
    }
    QLineEdit* line = qobject_cast<QLineEdit*>(w);
    if (line != nullptr) {
        GTLineEdit::setText(os, line, value.toString());
        return;
    }
    GT_FAIL(QString("unsupported widget class: %1").arg(w->metaObject()->className()), );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButton"
void GTUtilsWizard::clickButton(HI::GUITestOpStatus& os, WizardButton button) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QWidget* buttonWidget = GTWidget::findButtonByText(os, buttonMap.key(button), dialog);
    GTGlobals::sleep(500);
    GTWidget::click(os, buttonWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPageTitle"
QString GTUtilsWizard::getPageTitle(HI::GUITestOpStatus& os) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    QWizard* wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK_RESULT(wizard, "activeModalWidget is not wizard", QString());

    auto pageTitle = GTWidget::findLabel(os, "pageTitle", wizard->currentPage());
    return pageTitle->text();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
