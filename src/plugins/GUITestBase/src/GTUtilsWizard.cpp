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
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QDir>
#include <QFileInfo>
#include <QScrollArea>
#include <QWizard>

#include <U2Core/global.h>

#include "GTUtilsWizard.h"

namespace U2 {
using namespace HI;

static void expandWizardParameterIfNeeded(const QString& parameterName, QWidget* dialog) {
    auto propertyWidget = GTWidget::findWidget(parameterName + "_propertyWidget", dialog, {false});
    auto showHideButton = propertyWidget == nullptr ? nullptr : GTWidget::findToolButton("showHideButton", propertyWidget, {false});
    if (showHideButton != nullptr && showHideButton->text() == "+") {
        GTWidget::click(showHideButton);
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
void GTUtilsWizard::setInputFiles(const QList<QStringList>& inputFiles) {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    int i = 0;
    for (const QStringList& datasetFiles : qAsConst(inputFiles)) {
        QTabWidget* tabWidget = GTWidget::findWidgetByType<QTabWidget*>(dialog, "tabWidget not found");
        GTTabWidget::setCurrentIndex(tabWidget, i);

        QMap<QString, QStringList> dir2files;
        for (const QString& datasetFile : qAsConst(datasetFiles)) {
            QFileInfo fileInfo(datasetFile);
            dir2files[fileInfo.absoluteDir().path()] << fileInfo.fileName();
        }

        const QList<QString> dirList = dir2files.keys();
        for (const QString& dir : qAsConst(dirList)) {
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(dir, dir2files[dir]));
            QList<QWidget*> addFileButtonList = dialog->findChildren<QWidget*>("addFileButton");
            for (QWidget* addFileButton : qAsConst(addFileButtonList)) {
                if (addFileButton->isVisible()) {
                    GTWidget::click(addFileButton);
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
void GTUtilsWizard::setAllParameters(QMap<QString, QVariant> map) {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK(wizard, "activeModalWidget is not wizard");

    QWidget* nextButton;

    do {
        QMap<QString, QVariant>::iterator iter = map.begin();
        while (iter != map.end()) {
            const QString& parameterName = iter.key();
            auto w = GTWidget::findWidget(parameterName + " widget", wizard->currentPage(), {false});
            if (w != nullptr) {
                expandWizardParameterIfNeeded(parameterName, dialog);
                QScrollArea* area = wizard->currentPage()->findChild<QScrollArea*>();
                if (area != nullptr) {
                    area->ensureWidgetVisible(w);
                }
                setValue(w, iter.value());
                iter = map.erase(iter);

            } else {
                ++iter;
            }
        }
        nextButton = GTWidget::findButtonByText("&Next >", wizard, {false});
        if (nextButton != nullptr && nextButton->isVisible()) {
            GTWidget::click(nextButton);
        }
    } while (nextButton != nullptr && nextButton->isVisible());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setParameter"
void GTUtilsWizard::setParameter(const QString& parameterName, const QVariant& parameterValue) {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK(wizard, "activeModalWidget is not wizard");

    expandWizardParameterIfNeeded(parameterName, dialog);

    auto w = GTWidget::findWidget(parameterName + " widget", dialog);
    QScrollArea* area = wizard->currentPage()->findChild<QScrollArea*>();
    if (area != nullptr) {
        area->ensureWidgetVisible(w);
    }

    setValue(w, parameterValue);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getParameter"
QVariant GTUtilsWizard::getParameter(const QString& parameterName) {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK_RESULT(wizard, "activeModalWidget is not wizard", {});

    expandWizardParameterIfNeeded(parameterName, dialog);

    auto w = GTWidget::findWidget(parameterName + " widget", dialog);

    auto combo = qobject_cast<QComboBox*>(w);
    if (combo != nullptr) {
        return QVariant(combo->currentText());
    }
    auto spin = qobject_cast<QSpinBox*>(w);
    if (spin != nullptr) {
        return QVariant(spin->value());
    }
    auto doubleSpin = qobject_cast<QDoubleSpinBox*>(w);
    if (doubleSpin != nullptr) {
        return QVariant(doubleSpin->value());
    }
    auto line = qobject_cast<QLineEdit*>(w);
    if (line != nullptr) {
        return QVariant(line->text());
    }
    GT_FAIL(QString("unsupported widget class: %1").arg(w->metaObject()->className()), {});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setValue"
void GTUtilsWizard::setValue(QWidget* w, QVariant value) {
    auto combo = qobject_cast<QComboBox*>(w);
    if (combo != nullptr) {
        GTComboBox::selectItemByText(combo, value.toString());
        return;
    }
    auto spin = qobject_cast<QSpinBox*>(w);
    if (spin != nullptr) {
        bool ok;
        int val = value.toInt(&ok);
        GT_CHECK(ok, "spin box needs int value");
        GTSpinBox::setValue(spin, val, GTGlobals::UseKeyBoard);
        return;
    }
    auto doubleSpin = qobject_cast<QDoubleSpinBox*>(w);
    if (doubleSpin != nullptr) {
        bool ok;
        int val = value.toDouble(&ok);
        GT_CHECK(ok, "double spin box needs double value");
        GTDoubleSpinbox::setValue(doubleSpin, val, GTGlobals::UseKeyBoard);
        return;
    }
    auto line = qobject_cast<QLineEdit*>(w);
    if (line != nullptr) {
        GTLineEdit::setText(line, value.toString());
        return;
    }
    GT_FAIL(QString("unsupported widget class: %1").arg(w->metaObject()->className()), );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButton"
void GTUtilsWizard::clickButton(WizardButton button) {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    QWidget* buttonWidget = GTWidget::findButtonByText(buttonMap.key(button), dialog);
    GTWidget::click(buttonWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPageTitle"
QString GTUtilsWizard::getPageTitle() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto wizard = qobject_cast<QWizard*>(dialog);
    GT_CHECK_RESULT(wizard, "activeModalWidget is not wizard", QString());

    auto pageTitle = GTWidget::findLabel("pageTitle", wizard->currentPage());
    return pageTitle->text();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
