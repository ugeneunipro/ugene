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

#include "SingleSequenceImageExportController.h"
#include "SingleSequenceImageExportTask.h"
#include "SequenceExportSettingsWidget.h"
#include "SequencePainter.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSingleSequenceWidget.h>

namespace U2 {

/************************************************************************/
/* SingleSequenceImageExportController */
/************************************************************************/
SingleSequenceImageExportController::SingleSequenceImageExportController(ADVSingleSequenceWidget* seqWidget)
    : ImageExportController(ExportImageFormatPolicy(EnableRasterFormats | SupportSvg)),
      sequenceWidget(seqWidget),
      seqSettingsWidget(NULL)
{
    SAFE_POINT( seqWidget != NULL, tr("Sequence Widget is NULL"), );
    shortDescription = tr("Sequence");

    U2SequenceObject* seqObject = sequenceWidget->getSequenceObject();
    SAFE_POINT( seqObject != NULL, tr("Sequence Object is NULL"), );
    customExportSettings = QSharedPointer<SequenceExportSettings>(new SequenceExportSettings(seqObject->getSequenceLength(), ExportCurrentView));
    connect(customExportSettings.data(), SIGNAL(si_changed()), SLOT(sl_customSettingsChanged()));

    initSettingsWidget();

    currentPainter = SequencePainterFactory::createPainter(sequenceWidget, qobject_cast<SequenceExportSettings*>(customExportSettings)->getType());
}

void SingleSequenceImageExportController::initSettingsWidget() {
    U2SequenceObject* seqObject = sequenceWidget->getSequenceObject();
    SAFE_POINT( seqObject != NULL, tr("Sequence Object is NULL"), );

    settingsWidget = new SequenceExportSettingsWidget(seqObject, customExportSettings, sequenceWidget->getSequenceSelection());
}


Task* SingleSequenceImageExportController::getExportToPdfTask(const ImageExportTaskSettings &s) const {
    return new SequenceImageExportToPdfTask(currentPainter,
                                            customExportSettings, s);
}

Task* SingleSequenceImageExportController::getExportToSvgTask(const ImageExportTaskSettings &s) const {
    return new SequenceImageExportToSvgTask(currentPainter,
                                            customExportSettings,s);
}

Task* SingleSequenceImageExportController::getExportToBitmapTask(const ImageExportTaskSettings &s) const {
    return new SequenceImageExportToBitmapTask(currentPainter,
                                               customExportSettings, s);
}

void SingleSequenceImageExportController::sl_onFormatChanged(const QString &f) {
    format = f;
    checkExportSettings();
}

void SingleSequenceImageExportController::sl_customSettingsChanged() {
    checkExportSettings();
}

void SingleSequenceImageExportController::checkExportSettings() {
    currentPainter.clear();
    currentPainter = SequencePainterFactory::createPainter(sequenceWidget, qobject_cast<SequenceExportSettings*>(customExportSettings)->getType());

    QSize size = currentPainter->getImageSize(customExportSettings.data());
    if (size.width() > IMAGE_SIZE_LIMIT || size.height() > IMAGE_SIZE_LIMIT) {
        disableMessage = tr("Warning: selected region is too big to be exported.");
        emit si_disableExport(true);
        emit si_showMessage(disableMessage);
        return;
    }

    if (qobject_cast<SequenceExportSettings*>(customExportSettings)->getType() == ExportZoomedView && size.width() < 5) {
        disableMessage = tr("Warning: selected region is too small. Try to Zoom In.");
        emit si_disableExport(true);
        emit si_showMessage(disableMessage);
        return;
    }

    U2OpStatusImpl os;
    if (format.contains("svg", Qt::CaseInsensitive) && !currentPainter->canPaintSvg(customExportSettings.data(), os)) {
        disableMessage = tr("Warning: there are too many objects to be exported.");
        if (os.hasError()) {
            disableMessage = os.getError();
        }
        emit si_disableExport(true);
        emit si_showMessage(disableMessage);
        return;
    }

    emit si_disableExport(false);
    emit si_showMessage("");
}


} // namespace
