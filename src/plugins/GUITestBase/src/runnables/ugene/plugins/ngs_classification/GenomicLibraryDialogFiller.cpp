/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "GenomicLibraryDialogFiller.h"
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QFileInfo>

#include "GTUtilsWorkflowDesigner.h"

namespace U2 {
using namespace HI;

GenomicLibraryDialogFiller::GenomicLibraryDialogFiller(GUITestOpStatus &os, const QStringList &_urls)
    : Filler(os, "GenomicLibraryDialog"),
      urls(_urls) {
}

#define GT_CLASS_NAME "GenomicLibraryDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void GenomicLibraryDialogFiller::commonScenario() {
    QWidget *dialog = GTWidget::getActiveModalWidget(os);

    QStringList dirUrls;
    QStringList fileUrls;
    foreach (const QString &url, urls) {
        QFileInfo fileInfo(url);
        GT_CHECK(fileInfo.exists(), QString("'%1' doesn't exist").arg(url));
        if (QFileInfo(url).isFile()) {
            fileUrls << url;
        } else if (fileInfo.isDir()) {
            dirUrls << url;
        } else {
            GT_CHECK(false, QString("An unknown entry: '%1'").arg(url));
        }
    }

    if (!fileUrls.isEmpty()) {
        GTUtilsWorkflowDesigner::setDatasetInputFiles(os, fileUrls, dialog);
    }

    if (!dirUrls.isEmpty()) {
        GTUtilsWorkflowDesigner::setDatasetInputFolders(os, dirUrls, dialog);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
