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

#include "TreeViewerUtils.h"

#include <QApplication>
#include <QMessageBox>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#define IMAGE_DIR "image"

namespace U2 {

void TreeViewerUtils::saveImageDialog(const QString& filters, QString& fileName, QString& format) {
    LastUsedDirHelper lod(IMAGE_DIR);
    int i = fileName.lastIndexOf('.');
    if (i != -1) {
        fileName = fileName.left(i);
    }
    QString initialPath = lod.dir + "/" + fileName;
    fileName = U2FileDialog::getSaveFileName(nullptr, QObject::tr("Save As"), initialPath, filters, format);
    lod.url = fileName;
    if (fileName.isEmpty())
        return;

    format = format.left(3).toLower();
    if (!fileName.endsWith("." + format)) {
        fileName.append("." + format);
        if (QFile::exists(fileName)) {
            QMessageBox::StandardButtons b = QMessageBox::warning(nullptr, QObject::tr("Replace file"), QObject::tr("%1 already exists.\nDo you want to replace it?").arg(fileName), QMessageBox::Yes | QMessageBox::No);
            if (b != QMessageBox::Yes) {
                return;
            }
        }
    }
}

QFont TreeViewerUtils::getFontFromSettings(const QMap<TreeViewOption, QVariant>& settings) {
    QFont font = QApplication::font();
    QString family = settings[LABEL_FONT_FAMILY].toString();
    if (!family.isEmpty()) {
        font.setFamily(family);
    }
    font.setPointSize(settings[LABEL_FONT_SIZE].toInt());
    font.setBold(settings[LABEL_FONT_BOLD].toBool());
    font.setItalic(settings[LABEL_FONT_ITALIC].toBool());
    font.setUnderline(settings[LABEL_FONT_UNDERLINE].toBool());
    return font;
}

}  // namespace U2
