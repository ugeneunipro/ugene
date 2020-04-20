/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QMessageBox>

#include <U2Gui/HelpButton.h>
#include "AuthenticationDialog.h"
#include "AuthenticationWidget.h"
#include "ui_AuthenticationDialog.h"

namespace U2 {

AuthenticationDialog::AuthenticationDialog(const QString &text, QWidget* parent) :
    QDialog(parent),
    ui(new Ui_AuthenticationDialog),
    authenticationWidget(new AuthenticationWidget)
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24749084");
    ui->mainLayout->insertWidget(1, authenticationWidget);

    if (text.isEmpty()) {
        ui->lblText->hide();
    } else {
        ui->lblText->setText(text);
    }

    adjustSize();
}

AuthenticationDialog::~AuthenticationDialog() {
    delete ui;
}

void AuthenticationDialog::setLogin(const QString& login) {
    authenticationWidget->setLogin(login);
}

void AuthenticationDialog::setPassword(const QString& password) {
    authenticationWidget->setPassword(password);
}

void AuthenticationDialog::setRemembered(bool isChecked) {
    authenticationWidget->setRemembered(isChecked);
}

QString AuthenticationDialog::getLogin() const {
    return authenticationWidget->getLogin();
}

QString AuthenticationDialog::getPassword() const {
    return authenticationWidget->getPassword();
}

bool AuthenticationDialog::isRemembered() const {
    return authenticationWidget->isRemembered();
}

void AuthenticationDialog::disableLogin() {
    authenticationWidget->leLogin->setEnabled(false);
}

void AuthenticationDialog::accept() {
    if (authenticationWidget->getLogin().isEmpty()) {
        QMessageBox::critical(this, tr("Error!"), tr("Login is not set"));
        return;
    }

    QDialog::accept();
}

} // namespace
