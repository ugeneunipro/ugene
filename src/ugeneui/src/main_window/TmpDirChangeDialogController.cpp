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

#include <U2Gui/U2FileDialog.h>

#include "TmpDirChangeDialogController.h"

namespace U2 {

TmpDirChangeDialogController::TmpDirChangeDialogController(QString path, QWidget* p) : QDialog(p), tmpDirPath(path) {
    setupUi(this);
    QString message = "You do not have permission to write to \"" + tmpDirPath +
                         "\" folder. Please, set the valid temp folder:";
    messageText->setText(tr(message.toLatin1()));
    tmpDirPathEdit->setText(tmpDirPath);
    tmpDirChecker = new TmpDirChecker;

    connect(browseTmpDirButton, SIGNAL(clicked()), this, SLOT(sl_changeDirButtonClicked()));
    connect(exitAppButton, SIGNAL(clicked()), this, SLOT(sl_exitAppButtonClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(sl_okButtonClicked()));
}

QString TmpDirChangeDialogController::getTmpDirPath() {
    return tmpDirPath;
}

void TmpDirChangeDialogController::sl_changeDirButtonClicked() {
    QString newPath = U2FileDialog::getExistingDirectory(parentWidget(), tr("Choose Folder"), tmpDirPath,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (newPath.isEmpty() == false)
        tmpDirPathEdit->setText(newPath);
}

void TmpDirChangeDialogController::sl_exitAppButtonClicked() {
    QDialog::reject();
}

void TmpDirChangeDialogController::sl_okButtonClicked() {
    tmpDirPath = tmpDirPathEdit->text();
    if (!tmpDirChecker->checkPath(tmpDirPath)) {
        QString message = "You do not have permission to write to \"" + tmpDirPath +
                             "\" folder. Please, set the valid temp folder:";
        messageText->setText(tr(message.toLatin1()));
    }
    else {
        QDialog::accept();
    }
}

}// namespace
