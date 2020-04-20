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

#ifndef _U2_SINGLE_SEQUENCE_IMAGE_EXPORT_CONTROLLER_H_
#define _U2_SINGLE_SEQUENCE_IMAGE_EXPORT_CONTROLLER_H_

#include <QSharedPointer>

#include <U2Core/U2Region.h>

#include <U2Gui/ImageExportTask.h>

#include "ExportImagePainter.h"

namespace U2 {

class ADVSingleSequenceWidget;
class SequenceExportSettingsWidget;

/************************************************************************/
/* SingleSequenceImageExportController */
/************************************************************************/
class SingleSequenceImageExportController : public ImageExportController {
    Q_OBJECT
public:
    SingleSequenceImageExportController(ADVSingleSequenceWidget* sequenceWidget);

protected:
    void initSettingsWidget();

    Task* getExportToSvgTask(const ImageExportTaskSettings &imageSettings) const;
    Task* getExportToPdfTask(const ImageExportTaskSettings &imageSettings) const;
    Task* getExportToBitmapTask(const ImageExportTaskSettings &imageSettings) const;

public slots:
    void sl_onFormatChanged(const QString& format);
    void sl_customSettingsChanged();

private:
    void checkExportSettings();
    ADVSingleSequenceWidget *               sequenceWidget;
    SequenceExportSettingsWidget *          seqSettingsWidget;
    QString                                 format;

    QSharedPointer<ExportImagePainter>      currentPainter;
    QSharedPointer<CustomExportSettings>    customExportSettings;
};


} // namespace

#endif // _U2_SINGLE_SEQUENCE_IMAGE_EXPORT_CONTROLLER_H_
