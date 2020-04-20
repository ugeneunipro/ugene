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

#ifndef _U2_BIO_STRUCT_3DGL_IMAGE_EXPORT_TASK_H_
#define _U2_BIO_STRUCT_3DGL_IMAGE_EXPORT_TASK_H_

#include <U2Gui/ImageExportTask.h>

namespace U2 {

class BioStruct3DGLWidget;

class BioStruct3DImageExportTask : public ImageExportTask {
public:
    BioStruct3DImageExportTask(BioStruct3DGLWidget* widget, const ImageExportTaskSettings &settings)
        : ImageExportTask(settings),
          glWidget(widget) {}
    virtual void run() = 0;
protected:
    BioStruct3DGLWidget* glWidget;
};

class BioStruct3DImageExportToSVGTask : public BioStruct3DImageExportTask {
public:
    BioStruct3DImageExportToSVGTask(BioStruct3DGLWidget* widget, const ImageExportTaskSettings &settings)
        : BioStruct3DImageExportTask(widget, settings) {}
    void run();
};

class BioStruct3DImageExportToPDFTask : public BioStruct3DImageExportTask {
public:
    BioStruct3DImageExportToPDFTask(BioStruct3DGLWidget* widget, const ImageExportTaskSettings &settings)
        : BioStruct3DImageExportTask(widget, settings) {}
    void run();
};

class BioStruct3DImageExportToBitmapTask : public BioStruct3DImageExportTask {
public:
    BioStruct3DImageExportToBitmapTask(BioStruct3DGLWidget* widget, const ImageExportTaskSettings &settings)
        : BioStruct3DImageExportTask(widget, settings) {}
    void run();
};

class BioStruct3DImageExportController : public ImageExportController {
public:
    BioStruct3DImageExportController(BioStruct3DGLWidget* widget)
        : ImageExportController(ExportImageFormatPolicy_SupportAll),
          glWidget(widget) {}

    int getImageWidth() const;
    int getImageHeight() const;

protected:
    void initSettingsWidget() {}

    Task* getExportToSvgTask(const ImageExportTaskSettings &settings) const {
        return new BioStruct3DImageExportToSVGTask(glWidget, settings);
    }
    Task* getExportToPdfTask(const ImageExportTaskSettings &settings) const {
        return new BioStruct3DImageExportToPDFTask(glWidget, settings);
    }
    Task* getExportToBitmapTask(const ImageExportTaskSettings &settings) const {
        return new BioStruct3DImageExportToBitmapTask(glWidget, settings);
    }

private:
    BioStruct3DGLWidget* glWidget;
};

} // namespace

#endif // _U2_BIO_STRUCT_3DGL_IMAGE_EXPORT_TASK_H_
