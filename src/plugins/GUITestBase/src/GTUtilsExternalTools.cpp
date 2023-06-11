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

#include "GTUtilsExternalTools.h"
#include <primitives/GTWidget.h>

#include <QApplication>

#include <U2Gui/MainWindow.h>

#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

class RemoveToolScenario : public CustomScenario {
public:
    RemoveToolScenario(const QString& toolName)
        : CustomScenario(), toolName(toolName) {
    }

    void run() {
        AppSettingsDialogFiller::clearToolPath(toolName);

        QWidget* dialog = GTWidget::getActiveModalWidget();
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
    }

private:
    const QString toolName;
};

class CheckValidationScenario : public CustomScenario {
public:
    CheckValidationScenario(const QString& toolName)
        : CustomScenario(), toolName(toolName) {
    }

    void run() {
        bool isValid = AppSettingsDialogFiller::isExternalToolValid(toolName);
        CHECK_SET_ERR(isValid, QString("External Tool %1 is not valid, byu should be").arg(toolName));

        QWidget* dialog = GTWidget::getActiveModalWidget();
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
    }

private:
    const QString toolName;
};

class SetToolUrlScenario : public CustomScenario {
public:
    SetToolUrlScenario(const QString& toolName, const QString& url)
        : CustomScenario(), toolName(toolName), url(url) {
    }

    void run() {
        AppSettingsDialogFiller::setExternalToolPath(toolName, url);

        QWidget* dialog = GTWidget::getActiveModalWidget();
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
    }

private:
    const QString toolName;
    const QString url;
};

#define GT_CLASS_NAME "GTUtilsExternalTools"

#define GT_METHOD_NAME "removeTool"
void GTUtilsExternalTools::removeTool(const QString& toolName) {
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new RemoveToolScenario(toolName)));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isValid"
void GTUtilsExternalTools::checkValidation(const QString& toolName) {
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CheckValidationScenario(toolName)));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setToolUrl"
void GTUtilsExternalTools::setToolUrl(const QString& toolName, const QString& url) {
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new SetToolUrlScenario(toolName, url)));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
