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

#pragma once

#include "ImageExportTask.h"

class QWidget;

namespace U2 {

class WidgetScreenshotExportTask : public ImageExportTask {
    Q_OBJECT
public:
    WidgetScreenshotExportTask(QWidget* widget, const ImageExportTaskSettings& settings)
        : ImageExportTask(settings),
          widget(widget) {
    }
    void run() override = 0;

protected:
    QWidget* widget;
};

class WidgetScreenshotExportToSvgTask : public WidgetScreenshotExportTask {
    Q_OBJECT
public:
    WidgetScreenshotExportToSvgTask(QWidget* widget, const ImageExportTaskSettings& settings)
        : WidgetScreenshotExportTask(widget, settings) {
    }
    void run() override;
};

class WidgetScreenshotExportToPdfTask : public WidgetScreenshotExportTask {
public:
    WidgetScreenshotExportToPdfTask(QWidget* widget, const ImageExportTaskSettings& settings)
        : WidgetScreenshotExportTask(widget, settings) {
    }
    void run() override;
};

class WidgetScreenshotExportToBitmapTask : public WidgetScreenshotExportTask {
public:
    WidgetScreenshotExportToBitmapTask(QWidget* widget, const ImageExportTaskSettings& settings)
        : WidgetScreenshotExportTask(widget, settings) {
    }
    void run() override;
};

class WidgetScreenshotImageExportController : public ImageExportController {
    Q_OBJECT
public:
    WidgetScreenshotImageExportController(QWidget* widget);

    int getImageWidth() const override;
    int getImageHeight() const override;

protected:
    void initSettingsWidget() override {
    }

    Task* getExportToSvgTask(const ImageExportTaskSettings& settings) const override;
    Task* getExportToPdfTask(const ImageExportTaskSettings& settings) const override;
    Task* getExportToBitmapTask(const ImageExportTaskSettings& settings) const override;

private:
    QWidget* widget;
};

}  // namespace U2
