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

#include "BioStruct3DGLImageExportTask.h"
#include "BioStruct3DGLWidget.h"
#include "gl2ps/gl2ps.h"
#include <U2Core/U2SafePoints.h>


namespace U2 {

void BioStruct3DImageExportToSVGTask::run() {
    SAFE_POINT_EXT( settings.isSVGFormat(),
                    setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("BioStruct3DImageExportToSVGTask")), );

    int opt = GL2PS_NONE;
    glWidget->writeImage2DToFile(GL2PS_SVG, opt, 2, qPrintable(settings.fileName));
    //TODO: need check on error
}

void BioStruct3DImageExportToPDFTask::run() {
    SAFE_POINT_EXT( settings.isPDFFormat(),
                    setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("BioStruct3DImageExportToPDFTask")), );

    int opt = GL2PS_NONE;

    if (settings.format == "ps"){
        glWidget->writeImage2DToFile(GL2PS_PS, opt, 2, qPrintable(settings.fileName));
        return; //TODO: need check on error
    } else if (settings.format == "pdf"){
        glWidget->writeImage2DToFile(GL2PS_PDF, opt, 2, qPrintable(settings.fileName));
        return; //TODO: need check on error
    }
    setError(EXPORT_FAIL_MESSAGE.arg(settings.fileName));
}

void BioStruct3DImageExportToBitmapTask::run() {
    SAFE_POINT_EXT( settings.isBitmapFormat(),
                    setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("BioStruct3DImageExportToBitmapTask")), );

    glWidget->setImageRenderingMode(true);
    QPixmap image = glWidget->grab().scaled( settings.imageSize, Qt::KeepAspectRatio);
    glWidget->setImageRenderingMode(false);

    CHECK_EXT( image.save(settings.fileName, qPrintable(settings.format), settings.imageQuality),
               setError(EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

int BioStruct3DImageExportController::getImageWidth() const {
    return glWidget->width();
}

int BioStruct3DImageExportController::getImageHeight() const {
    return glWidget->height();
}

} // namespace

