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

#include "DialogUtils.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QWizard>

#include <U2Core/DocumentModel.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/Task.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

void WizardUtils::setWizardMinimumSize(QWizard* wizard, const QSize& minimumSize) {
    QSize bestSize = minimumSize;
    foreach (int pageId, wizard->pageIds()) {
        QWizardPage* page = wizard->page(pageId);
        page->adjustSize();
        bestSize = bestSize.expandedTo(page->size());
    }
    wizard->setMinimumSize(bestSize);
    wizard->adjustSize();
}

/********************************
 * FileLineEdit
 ********************************/
void FileLineEdit::sl_onBrowse() {
    LastUsedDirHelper lod(type);

    QFileDialog::Options options = 0;
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        options |= QFileDialog::DontUseNativeDialog;
    }

    QString name;
    if (multi) {
        QStringList lst = U2FileDialog::getOpenFileNames(nullptr, tr("Select file(s)"), lod.dir, FileFilter, nullptr, options);
        name = lst.join(";");
        if (!lst.isEmpty()) {
            lod.url = lst.first();
        }
    } else {
        lod.url = name = U2FileDialog::getOpenFileName(nullptr, tr("Select file(s)"), lod.dir, FileFilter, nullptr, options);
    }
    if (!name.isEmpty()) {
        setText(name);
    }
    setFocus();
}

}  // namespace U2
