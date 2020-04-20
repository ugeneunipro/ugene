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
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>
#include <QPushButton>

#include "GetSequenceByIdDialog.h"

namespace U2 {

GetSequenceByIdDialog::GetSequenceByIdDialog(QWidget *w): QDialog(w) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748945");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(toolButton, SIGNAL(clicked()), SLOT(sl_saveFilenameButtonClicked()));
    QString defaultPath = AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath();
    directoryEdit->setText(defaultPath);
    dir = defaultPath;
}

const QString DOWNLOAD_REMOTE_FILE_DOMAIN = "DownloadRemoteFileDialog";

void GetSequenceByIdDialog::sl_saveFilenameButtonClicked() {
    LastUsedDirHelper lod(DOWNLOAD_REMOTE_FILE_DOMAIN);
    QString dirName = U2FileDialog::getExistingDirectory(this, tr("Select folder to save"), lod.dir);
    if(!dirName.isEmpty()) {
        directoryEdit->setText(dirName);
        dir = dirName;
    }
}

void GetSequenceByIdDialog::accept() {
    if(dir.isEmpty()) {
        return;
    }

    QDir downloadDir(dir);
    if (!downloadDir.exists()) {
        if (QMessageBox::Yes == QMessageBox::question(this,
            windowTitle(), tr("Folder doesn't exist. Do you want to create it?"),
            QMessageBox::Yes, QMessageBox::No))
        {
            downloadDir.mkpath(dir);
        } else {
            return;
        }
    }

    addToProject = addBox->isChecked();
    QDialog::accept();
}

}
