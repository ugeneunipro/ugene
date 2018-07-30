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

#include <QCheckBox>
#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include "DotPlotImageExportTask.h"
#include "DotPlotWidget.h"

namespace U2 {

void DotPlotImageExportToBitmapTask::run() {
    SAFE_POINT_EXT(settings.isBitmapFormat(),
                   setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("DotPlotImageExportToBitmapTask")), );
    QImage im(settings.imageSize, QImage::Format_RGB32);
    int dpm = settings.imageDpi / 0.0254;
    im.setDotsPerMeterX( dpm );
    im.setDotsPerMeterY( dpm );
    im.fill(Qt::white);
    QPainter painter(&im);

    int defaultDpm = 0;
    {
        QImage tmp(10, 10, QImage::Format_RGB32);
        tmp.fill(Qt::white);
        defaultDpm = tmp.dotsPerMeterX();
    }
    SAFE_POINT(dpm != 0, tr("Incorrect DPI paramter"), );
    float fontScale = (float)defaultDpm / dpm;
    dotplotWidget->drawAll(painter, settings.imageSize, fontScale, dpExportSettings);

    CHECK_EXT( im.save(settings.fileName, qPrintable(settings.format), settings.imageQuality), setError(EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
    SAFE_POINT_EXT( settings.isBitmapFormat(), setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("CircularViewImageExportToBitmapTask")), );
}

DotPlotImageExportController::DotPlotImageExportController(DotPlotWidget *wgt)
    : ImageExportController(),
      dotplotWidget(wgt)
{
    SAFE_POINT( dotplotWidget != NULL, tr("Dotplot widget is NULL"), );
    shortDescription = tr("Dotplot");
    initSettingsWidget();
}

int DotPlotImageExportController::getImageWidth() const {
    return dotplotWidget->width();
}

int DotPlotImageExportController::getImageHeight() const {
    return dotplotWidget->height();
}

void DotPlotImageExportController::initSettingsWidget() {
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->setContentsMargins(0, 0, 0, 0);

    includeAreaSelection = new QCheckBox(tr("Include area selection"));
    includeRepeatSelection = new QCheckBox(tr("Include repeat selection"));

    includeAreaSelection->setObjectName("include_area_selection");
    includeRepeatSelection->setObjectName("include_repeat_selection");

    includeAreaSelection->setChecked(false);
    includeRepeatSelection->setChecked(false);

    includeAreaSelection->setEnabled(dotplotWidget->hasSelectedArea());
    includeRepeatSelection->setEnabled(dotplotWidget->nearestRepeat != NULL);

    layout->addWidget(includeAreaSelection);
    layout->addWidget(includeRepeatSelection);

    settingsWidget = new QWidget();
    settingsWidget->setLayout(layout);
}

Task* DotPlotImageExportController::getExportToBitmapTask(const ImageExportTaskSettings &settings) const {
    DotPlotImageExportSettings dpSettings(includeAreaSelection->isChecked(),
                                          includeRepeatSelection->isChecked());
    return new DotPlotImageExportToBitmapTask(dotplotWidget, dpSettings, settings);
}

} // namespace
