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

#include <U2Gui/ImageExportTask.h>

#include "SequenceExportSettingsWidget.h"
#include "SequencePainter.h"

namespace U2 {

class ADVSingleSequenceWidget;

/************************************************************************/
/* SequenceImageExportTask */
/************************************************************************/
class SequenceImageExportTask : public ImageExportTask {
public:
    SequenceImageExportTask(QSharedPointer<ExportImagePainter> painter,
                            QSharedPointer<CustomExportSettings> customSettings,
                            const ImageExportTaskSettings& settings);

protected:
    QSharedPointer<ExportImagePainter> painter;
    QSharedPointer<CustomExportSettings> customSettings;
};

/************************************************************************/
/* SequenceImageExportToPdfTask */
/************************************************************************/
class SequenceImageExportToPdfTask : public SequenceImageExportTask {
public:
    SequenceImageExportToPdfTask(QSharedPointer<ExportImagePainter> painter,
                                 QSharedPointer<CustomExportSettings> customSettings,
                                 const ImageExportTaskSettings& settings)
        : SequenceImageExportTask(painter,
                                  customSettings,
                                  settings) {
    }
    void run() override;
};

/************************************************************************/
/* SequenceImageExportToSvgTask */
/************************************************************************/
class SequenceImageExportToSvgTask : public SequenceImageExportTask {
public:
    SequenceImageExportToSvgTask(QSharedPointer<ExportImagePainter> painter,
                                 QSharedPointer<CustomExportSettings> customSettings,
                                 const ImageExportTaskSettings& settings)
        : SequenceImageExportTask(painter,
                                  customSettings,
                                  settings) {
    }
    void run() override;
};

/************************************************************************/
/* SequenceImageExportToBitmapTask */
/************************************************************************/
class SequenceImageExportToBitmapTask : public SequenceImageExportTask {
public:
    SequenceImageExportToBitmapTask(QSharedPointer<ExportImagePainter> painter,
                                    QSharedPointer<CustomExportSettings> customSettings,
                                    const ImageExportTaskSettings& settings)
        : SequenceImageExportTask(painter,
                                  customSettings,
                                  settings) {
    }
    void run() override;
};

}  // namespace U2
