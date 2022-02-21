/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <QMessageBox>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#define IMAGE_DIR "image"

namespace U2 {

QFont* TreeViewerUtils::font = nullptr;
const char* TreeViewerUtils::IMAGE_FILTERS =
    "BMP - Windows Bitmap (*.bmp);;"
    "GIF - Graphic Interchange Format (*.gif);;"
    "JPG/JPEG format (*.jpg);;"
    "PBM - Portable Bitmap (*.pbm);;"
    "PNG - Portable Network Graphics (*.png);;"
    "PPM - Portable Pixmap (*.ppm);;"
    "TIFF - Tagged Image File format (*.tif);;"
    "XBM - X11 Bitmap (*.xbm);;"
    "XPM - X11 Pixmap (*.xpm)";

void TreeViewerUtils::saveImageDialog(const QString& filters, QString& fileName, QString& format) {
    LastUsedDirHelper lod(IMAGE_DIR);
    int i = fileName.lastIndexOf('.');
    if (i != -1) {
        fileName = fileName.left(i);
    }
    QString initialPath = lod.dir + "/" + fileName;
    fileName = U2FileDialog::getSaveFileName(nullptr, QObject::tr("Save As"), initialPath, filters, &format);
    lod.url = fileName;
    if (fileName.isEmpty())
        return;

    format = format.left(3).toLower();
    if (!fileName.endsWith("." + format)) {
        fileName.append("." + format);
        if (QFile::exists(fileName)) {
            QMessageBox::StandardButtons b = QMessageBox::warning(0, QObject::tr("Replace file"), QObject::tr("%1 already exists.\nDo you want to replace it?").arg(fileName), QMessageBox::Yes | QMessageBox::No);
            if (QMessageBox::Yes != b) {
                return;
            }
        }
    }
}

const QFont& TreeViewerUtils::getFont() {
    if (font == nullptr) {
        font = new QFont();
        font->setPointSize(8);
    }
    return *font;
}

}  // namespace U2
