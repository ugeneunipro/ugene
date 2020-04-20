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

#include "SingleSequenceImageExportTask.h"
#include "SequenceExportSettingsWidget.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Sequence.h>

#include <U2View/ADVSingleSequenceWidget.h>

#include <U2Gui/RegionSelector.h>

#include <QDomDocument>
#include <QPrinter>
#include <QSvgGenerator>


namespace U2 {

SequenceImageExportTask::SequenceImageExportTask(QSharedPointer<ExportImagePainter> painter,
                                                 QSharedPointer<CustomExportSettings> customSettings,
                                                 const ImageExportTaskSettings& settings)
    : ImageExportTask(settings),
      painter(painter),
      customSettings(customSettings) {
    SAFE_POINT_EXT( !painter.isNull(), setError("ExportImagePainter is NULL"), );
    SAFE_POINT_EXT( !customSettings.isNull(), setError("CustomExportSettings is NULL"), );
}

void SequenceImageExportToPdfTask::run() {
    SAFE_POINT_EXT( settings.isPDFFormat(), setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("SequenceImageExportToPDFTask")), );

    QPainter p;
    QPrinter printer;
    printer.setOutputFileName(settings.fileName);

    p.setRenderHint(QPainter::Antialiasing);
    p.begin(&printer);

    painter->paint(p, customSettings.data());

    CHECK_EXT( p.end(), setError(EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

void SequenceImageExportToSvgTask::run() {
    SAFE_POINT_EXT(settings.isSVGFormat(),
               setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("SequenceImageExportToSVGTask")), );

    QPainter p;
    QSvgGenerator generator;
    generator.setFileName(settings.fileName);
    QSize size = painter->getImageSize(customSettings.data());
    generator.setSize(size);
    generator.setViewBox(QRect(0, 0, size.width(), size.height()));

    p.begin(&generator);
    painter->paint(p, customSettings.data());

    bool result = p.end();
    //fix for UGENE-76
    QDomDocument doc("svg");
    QFile file(settings.fileName);
    bool ok=file.open(QIODevice::ReadOnly);
    if (!ok && !result){
       result=false;
    }
    ok=doc.setContent(&file);
    if (!ok && !result) {
        file.close();
        result=false;
    }
    if(result){
        file.close();
        QDomNodeList radialGradients=doc.elementsByTagName("radialGradient");
        for (int i = 0; i < static_cast<int>(radialGradients.length()); i++) {
            if(radialGradients.at(i).isElement()){
                QDomElement tag=radialGradients.at(i).toElement();
                if(tag.hasAttribute("xml:id")){
                    QString id=tag.attribute("xml:id");
                    tag.removeAttribute("xml:id");
                    tag.setAttribute("id",id);
                }
            }
        }
        file.open(QIODevice::WriteOnly);
        file.write(doc.toByteArray());
        file.close();
    }
    CHECK_EXT( result, setError(EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

void SequenceImageExportToBitmapTask::run() {
    SAFE_POINT_EXT( settings.isBitmapFormat(), setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("SequenceImageExportToBitmapTask")), );

    QSize size = painter->getImageSize(customSettings.data());
    QPixmap im(size);
    im.fill(Qt::white);
    QPainter p(&im);
    painter->paint(p, customSettings.data());

    CHECK_EXT( im.save(settings.fileName, qPrintable(settings.format), settings.imageQuality), setError(EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

} // namespace
