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

#include <QDir>
#include <QPushButton>

#include <U2Core/AppContext.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "ImportCustomToolsTask.h"
#include "ImportExternalToolDialog.h"

namespace U2 {

ImportExternalToolDialog::ImportExternalToolDialog(QWidget *_parent)
    : QDialog(_parent)
{
    setupUi(this);

    new HelpButton(this, buttonBox, "24748738");

    connect(lePath, SIGNAL(textChanged(const QString &)), SLOT(sl_pathChanged()));
    connect(tbBrowse, SIGNAL(clicked()), SLOT(sl_browse()));

    sl_pathChanged();
}

void ImportExternalToolDialog::sl_browse() {
    LastUsedDirHelper lod("import external tool");
    const QString filter = DialogUtils::prepareFileFilter("UGENE external tool config file", { "xml" }, true, {});
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select configuration file to import"), lod.dir, filter);
    if (!lod.url.isEmpty()) {
        lePath->setText(QDir::toNativeSeparators(lod.url));
    }
}

void ImportExternalToolDialog::sl_pathChanged() {
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!lePath->text().isEmpty());
}

void ImportExternalToolDialog::accept() {
    AppContext::getTaskScheduler()->registerTopLevelTask(new ImportCustomToolsTask(lePath->text()));
    QDialog::accept();
}

}   // namespace U2
