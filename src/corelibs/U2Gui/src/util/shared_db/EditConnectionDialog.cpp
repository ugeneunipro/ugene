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

#include <U2Core/AppContext.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/Settings.h>

#include <U2Gui/HelpButton.h>

#include "util/AuthenticationWidget.h"
#include "EditConnectionDialog.h"
#include "ui_EditConnectionDialog.h"

namespace U2 {

const QString EditConnectionDialog::DEFAULT_PORT = "3306";

EditConnectionDialog::EditConnectionDialog(QWidget *parent, const QString &dbiUrl, const QString &userName, const QString &connectionName) :
    QDialog(parent),
    ui(new Ui_EditConnectionDialog)
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24749084");
    adjustSize();
    init(dbiUrl, connectionName, userName);
}

EditConnectionDialog::~EditConnectionDialog() {
    delete ui;
}

void EditConnectionDialog::setReadOnly(bool readOnly) {
    ui->leName->setDisabled(readOnly);
    ui->leHost->setDisabled(readOnly);
    ui->lePort->setDisabled(readOnly);
    ui->leDatabase->setDisabled(readOnly);
    ui->authenticationWidget->setDisabled(readOnly);
}

QString EditConnectionDialog::getName() const {
    if (!ui->leName->text().isEmpty()) {
        return ui->leName->text();
    } else {
        return getShortDbiUrl();
    }
}

QString EditConnectionDialog::getUserName() const {
    return ui->authenticationWidget->getLogin();
}

QString EditConnectionDialog::getShortDbiUrl() const {
    return U2DbiUtils::createDbiUrl(ui->leHost->text(),
                                    ui->lePort->text().toInt(),
                                    ui->leDatabase->text());
}


void EditConnectionDialog::accept() {
    if (!checkFields()) {
        return;
    }

    if (!ui->authenticationWidget->getLogin().isEmpty()) {
        saveCredentials();
    } else {
        removeCredentials();
    }

    QDialog::accept();
}

void EditConnectionDialog::init(const U2DbiId& dbiUrl, const QString& connectionName, const QString &userName) {
    initTabOrder();

    ui->leName->setText(connectionName);
    ui->lePort->setText(DEFAULT_PORT);

    if (!dbiUrl.isEmpty()) {
        QString host;
        int port = -1;
        QString dbName;

        U2DbiUtils::parseDbiUrl(dbiUrl, host,port, dbName);
        ui->leHost->setText(host);
        if (port > 0) {
            ui->lePort->setText(QString::number(port));
        }
        ui->leDatabase->setText(dbName);
    }

    const QString fullDbiUrl = U2DbiUtils::createFullDbiUrl(userName, dbiUrl);
    const QString password = AppContext::getPasswordStorage()->getEntry(fullDbiUrl);
    ui->authenticationWidget->setLogin(userName);
    ui->authenticationWidget->setPassword(password);
    ui->authenticationWidget->setRemembered(AppContext::getPasswordStorage()->isRemembered(fullDbiUrl));
}

void EditConnectionDialog::initTabOrder() {
    setTabOrder(ui->leName, ui->leHost);
    setTabOrder(ui->leHost, ui->lePort);
    setTabOrder(ui->lePort, ui->leDatabase);
    setTabOrder(ui->leDatabase, ui->authenticationWidget->leLogin);
    setTabOrder(ui->authenticationWidget->leLogin, ui->authenticationWidget->lePassword);
    setTabOrder(ui->authenticationWidget->lePassword, ui->authenticationWidget->cbRemember);
    setTabOrder(ui->authenticationWidget->cbRemember, ui->buttonBox);
}

void EditConnectionDialog::saveCredentials() const {
    const bool remember = ui->authenticationWidget->isRemembered();
    AppContext::getPasswordStorage()->addEntry(getFullDbiUrl(), ui->authenticationWidget->getPassword(), remember);
}

void EditConnectionDialog::removeCredentials() const {
    AppContext::getPasswordStorage()->removeEntry(getFullDbiUrl());
}

bool EditConnectionDialog::checkFields() {
    if (ui->leHost->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Host is not set"));
        ui->leHost->setFocus();
        return false;
    }

    if (ui->leDatabase->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Database is not set"));
        ui->leDatabase->setFocus();
        return false;
    }

    return true;
}

QString EditConnectionDialog::getFullDbiUrl() const {
    return U2DbiUtils::createFullDbiUrl(ui->authenticationWidget->getLogin(), getShortDbiUrl());
}

}   // namespace U2
