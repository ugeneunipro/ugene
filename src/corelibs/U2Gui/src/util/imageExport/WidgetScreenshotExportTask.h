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

#ifndef _U2_WIDGET_SCREENSHOT_EXPORT_TASK_H_
#define _U2_WIDGET_SCREENSHOT_EXPORT_TASK_H_

#include "ImageExportTask.h"

class QWidget;

namespace U2 {

class WidgetScreenshotExportTask : public ImageExportTask {
    Q_OBJECT
public:
    WidgetScreenshotExportTask(QWidget *widget, const ImageExportTaskSettings &settings)
        : ImageExportTask(settings),
          widget(widget) {}
    virtual void run() = 0;
protected:
    QWidget *widget;
};

class WidgetScreenshotExportToSvgTask : public WidgetScreenshotExportTask {
    Q_OBJECT
public:
    WidgetScreenshotExportToSvgTask(QWidget *widget, const ImageExportTaskSettings &settings)
        : WidgetScreenshotExportTask(widget, settings) {}
    void run();
};

class WidgetScreenshotExportToPdfTask : public WidgetScreenshotExportTask {
public:
    WidgetScreenshotExportToPdfTask(QWidget *widget, const ImageExportTaskSettings &settings)
        : WidgetScreenshotExportTask(widget, settings) {}
    void run();
};

class WidgetScreenshotExportToBitmapTask : public WidgetScreenshotExportTask {
public:
    WidgetScreenshotExportToBitmapTask(QWidget *widget, const ImageExportTaskSettings &settings)
        : WidgetScreenshotExportTask(widget, settings) {}
    void run();
};

class WidgetScreenshotImageExportController : public ImageExportController {
    Q_OBJECT
public:
    WidgetScreenshotImageExportController(QWidget *widget);

    int getImageWidth() const;
    int getImageHeight() const;

protected:
    void initSettingsWidget() {}

    Task* getExportToSvgTask(const ImageExportTaskSettings &settings) const;
    Task* getExportToPdfTask(const ImageExportTaskSettings &settings) const;
    Task* getExportToBitmapTask(const ImageExportTaskSettings &settings) const;

private:
    QWidget *widget;
};

} // namespace

#endif // _U2_WIDGET_SCREENSHOT_EXPORT_TASK_H_
