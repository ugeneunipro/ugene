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


#include <QFile>

#include <U2Core/AppContext.h>

#include "LicenseDialog.h"
#include "ui_LicenseDialog.h"


namespace U2 {

LicenseDialog::LicenseDialog(Plugin *_plugin, QWidget *parent) :
    QDialog(parent), ui(new Ui_LicenseDialog), plugin(_plugin)
{
    ui->setupUi(this);
    //Opening license file
    QFile licenseFile(plugin->getLicensePath().getURLString());
    if (!licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->licenseTextBrowser->setText(tr("License file not found."));
    }else{
        ui->licenseTextBrowser->setText(QString(licenseFile.readAll()));
        licenseFile.close();
    }

    connect(ui->acceptButton,SIGNAL(clicked()),SLOT(sl_accept()));
}

LicenseDialog::~LicenseDialog()
{
    delete ui;
}

void LicenseDialog::sl_accept(){
    AppContext::getPluginSupport()->setLicenseAccepted(plugin);
    accept();
}
}//namespace
