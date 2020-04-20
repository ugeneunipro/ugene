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
#include <QPushButton>

#include <U2Core/GUrlUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "ExportProjectDialogController.h"

namespace U2{

static QString fixProjectFile(const QString& name) {
    QString result = name;
    if (result.isEmpty()) {
        result =  "project" + PROJECTFILE_EXT;
    } else if (!result.endsWith(PROJECTFILE_EXT)) {
        result+=PROJECTFILE_EXT;
    }
    return result;
}

ExportProjectDialogController::ExportProjectDialogController(QWidget *p, const QString& defaultProjectFileName) 
: QDialog(p) 
{
    setupUi(this);
    new HelpButton(this, buttonBox, "");
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    setModal(true);
    projectFile = fixProjectFile(defaultProjectFileName);
    projectFileEdit->setText(projectFile);
    Project* proj = AppContext::getProject();
    if (proj == NULL || !proj->isItemModified() || proj->getProjectURL().isEmpty()) {
        warningLabel->setVisible(false);
    }
    connect(browseButton, SIGNAL(clicked()), this, SLOT(sl_onBrowseButton()));

    SAFE_POINT(buttonBox, "buttonBox not initialized", );
    QPushButton *b = buttonBox->button(QDialogButtonBox::Ok);
    SAFE_POINT(b, "buttonBox without OK button", );
    b->setText(tr("Export"));
}

void ExportProjectDialogController::accept(){
    QString dirPath = exportFolderEdit->text();
    projectFile = fixProjectFile(projectFileEdit->text());
    
    U2OpStatus2Log os;
    exportDir = GUrlUtils::prepareDirLocation(dirPath, os);
    if (exportDir.isEmpty()) {
        assert(os.hasError());
        QMessageBox::critical(this, this->windowTitle(), os.getError());
        return;
    }
	QDialog::accept();
}

void ExportProjectDialogController::sl_onBrowseButton(){
	LastUsedDirHelper h;
	QString folder = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), h.dir);
    if (folder.isEmpty()) {
        return;
    }
	exportFolderEdit->setText(folder);
}

}
