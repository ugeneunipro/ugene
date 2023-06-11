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

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>

#include "EditConnectionDialogFiller.h"
#include "GTDatabaseConfig.h"
#include "primitives/GTAction.h"

namespace U2 {

EditConnectionDialogFiller::EditConnectionDialogFiller(const Parameters& parameters, ConnectionType type)
    : Filler("EditConnectionDialog"), parameters(parameters) {
    if (FROM_SETTINGS == type) {
        this->parameters.host = GTDatabaseConfig::host();
        this->parameters.port = QString::number(GTDatabaseConfig::port());
        this->parameters.database = GTDatabaseConfig::database();
        this->parameters.login = GTDatabaseConfig::login();
        this->parameters.password = GTDatabaseConfig::password();
    }
}

EditConnectionDialogFiller::EditConnectionDialogFiller(CustomScenario* scenario)
    : Filler("EditConnectionDialog", scenario) {
}

#define GT_CLASS_NAME "GTUtilsDialog::EditConnectionDialogFiller"
#define GT_METHOD_NAME "run"

void EditConnectionDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto lePort = GTWidget::findLineEdit("lePort", dialog);

    if (parameters.checkDefaults) {
        GT_CHECK(lePort->text() == "3306", "Wrong default port");
    } else {
        GTLineEdit::setText("leName", parameters.connectionName, dialog);
        GTLineEdit::setText("leHost", parameters.host, dialog);
        GTLineEdit::setText(lePort, parameters.port);
        GTLineEdit::setText("leDatabase", parameters.database, dialog);
        GTLineEdit::setText("leLogin", parameters.login, dialog);
        GTLineEdit::setText("lePassword", parameters.password, dialog);
        GTCheckBox::setChecked("cbRemember", parameters.rememberMe, dialog);
    }

    QString buttonName = parameters.accept ? "OK" : "Cancel";
    GTWidget::click(GTWidget::findButtonByText(buttonName, dialog));
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

AuthenticationDialogFiller::AuthenticationDialogFiller(const QString& login, const QString& password)
    : Filler("AuthenticationDialog"), login(login), password(password) {
}

#define GT_CLASS_NAME "GTUtilsDialog::AuthenticationDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void AuthenticationDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto leLogin = GTWidget::findLineEdit("leLogin", dialog);
    if (leLogin->isEnabled()) {
        GTLineEdit::setText(leLogin, login);
    }

    GTLineEdit::setText("lePassword", password, dialog);

    auto cbRemember = GTWidget::findCheckBox("cbRemember", dialog);
    GTCheckBox::setChecked(cbRemember, false);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
