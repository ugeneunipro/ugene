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

#include <QWidget>

#include <U2Core/U2SafePoints.h>

#include "ImageExportTask.h"

namespace U2 {

const QString ImageExportTaskSettings::SVG_FORMAT = "SVG";
const QString ImageExportTaskSettings::PS_FORMAT = "PS";
const QString ImageExportTaskSettings::PDF_FORMAT = "PDF";

ImageExportTaskSettings::ImageExportTaskSettings(const QString &fileName,
                                                 const QString &format,
                                                 const QSize &size,
                                                 const int quality,
                                                 const int dpi)
    : fileName(fileName),
      format(format),
      imageSize(size),
      imageQuality(quality),
      imageDpi(dpi)
{
}

bool ImageExportTaskSettings::isBitmapFormat() const {
    return !(isSVGFormat() || isPDFFormat());
}

bool ImageExportTaskSettings::isSVGFormat() const {
    return (SVG_FORMAT == format);
}

bool ImageExportTaskSettings::isPDFFormat() const {
    return (PS_FORMAT == format || PDF_FORMAT == format);
}

ImageExportTask::ImageExportTask(const ImageExportTaskSettings &settings)
    : Task(tr("Image export task"), TaskFlag_RunInMainThread), settings(settings) {
    WRONG_FORMAT_MESSAGE = tr("Format %1 is not supported by %2.");
    EXPORT_FAIL_MESSAGE = tr("Failed to export image to %1.");
}

Task::ReportResult ImageExportTask::report() {
    ioLog.info(tr("Done!"));
    return Task::ReportResult_Finished;
}

ImageExportController::ImageExportController(const ExportImageFormatPolicy& fPolicy)
    : settingsWidget(NULL),
      formatPolicy(fPolicy)
{

}

Task * ImageExportController::getTaskInstance(const ImageExportTaskSettings &settings) const {
    if (settings.isSVGFormat()) {
        SAFE_POINT(isSvgSupported(), tr("SVG format is not supported"), NULL);
        return getExportToSvgTask(settings);
    }
    if (settings.isPDFFormat()) {
        SAFE_POINT(isPdfSupported(), tr("PS/PDF format is not supported"), NULL);
        return getExportToPdfTask(settings);
    }
    SAFE_POINT(isRasterFormatsEnabled(), tr("Raster formats are disabled"), NULL);
    return getExportToBitmapTask(settings);
}

QWidget * ImageExportController::getSettingsWidget() {
    if (settingsWidget == NULL) {
        initSettingsWidget();
    }
    return settingsWidget;
}

} // namespace
