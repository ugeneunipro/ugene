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


#include "SequencePainter.h"
#include "../view_rendering/DetViewRenderer.h"
#include "../view_rendering/PanViewRenderer.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/DetView.h>

#define MAX_ANNOTATIONS_ON_SVG_IMAGE 40000 // ~50 Mb

namespace U2 {

bool checkAnnotationsCountInRegion(SequenceObjectContext* ctx, const U2Region& region) {
    SAFE_POINT(ctx != NULL, "SequenceContext is NULL", false);
    int count = 0;
    foreach (AnnotationTableObject* table, ctx->getAnnotationObjects(true)) {
        SAFE_POINT(table != NULL, "AnnotationTableObject is NULL", false)
        count += table->getAnnotationsByRegion(region).size();
    }
    return (count < MAX_ANNOTATIONS_ON_SVG_IMAGE);
}

U2Region SequenceExportSettings::getRegion() const {
    return region;
}

SequenceExportType SequenceExportSettings::getType() const {
    return type;
}

void SequenceExportSettings::setRegion(const U2Region &r) {
    region = r;
    emit si_changed();
}

void SequenceExportSettings::setType(SequenceExportType t) {
    type = t;
    emit si_changed();
}

/************************************************************************/
/* CurrentViewPainter */
/************************************************************************/
void CurrentViewPainter::paint(QPainter &p, CustomExportSettings* /*settings*/) const {
    QPixmap screenshot = seqWidget->grab();
    p.drawPixmap(screenshot.rect(), screenshot);
}

QSize CurrentViewPainter::getImageSize(CustomExportSettings* /*settings*/) const {
    return seqWidget->size();
}

bool CurrentViewPainter::canPaintSvg(CustomExportSettings* /*settings*/, U2OpStatus &os) const {
    os.setError(tr("Warning: SVG is not supported for the currently viewed area. Please, choose another export area."));
    return false;
}

/************************************************************************/
/* ZoomedViewPainter */
/************************************************************************/
ZoomedViewPainter::ZoomedViewPainter(PanView* panView)
    : ExportImagePainter(),
      panView(panView) {
    PanViewRenderArea* ra = qobject_cast<PanViewRenderArea*>(panView->getRenderArea());
    panViewRenderer = ra->getRenderer();
}

void ZoomedViewPainter::paint(QPainter &p, CustomExportSettings* settings) const {
    SequenceExportSettings* s = qobject_cast<SequenceExportSettings*>(settings);
    SAFE_POINT(s != NULL, "Cannot cast CustomExportSettings to SequenceExportSettings", );

    panViewRenderer->drawAll(p, s->getRegion());
}

QSize ZoomedViewPainter::getImageSize(CustomExportSettings* settings) const {
    SequenceExportSettings* s = qobject_cast<SequenceExportSettings*>(settings);
    SAFE_POINT(s != NULL, "Cannot cast CustomExportSettings to SequenceExportSettings", QSize());

    return panViewRenderer->getBaseCanvasSize(s->getRegion());
}

bool ZoomedViewPainter::canPaintSvg(CustomExportSettings *settings, U2OpStatus & /*os*/) const {
    SequenceExportSettings* s = qobject_cast<SequenceExportSettings*>(settings);
    return checkAnnotationsCountInRegion(panView->getSequenceContext(), s->getRegion());
}

/************************************************************************/
/* DetailsViewPainter */
/************************************************************************/
DetailsViewPainter::DetailsViewPainter(DetView* detView)
    : ExportImagePainter() {
    detViewRenderer = detView->getDetViewRenderArea()->getRenderer();

}
void DetailsViewPainter::paint(QPainter &p, CustomExportSettings* settings) const {
    SequenceExportSettings* s = qobject_cast<SequenceExportSettings*>(settings);
    SAFE_POINT(s != NULL, "Cannot cast CustomExportSettings to SequenceExportSettings", );

    detViewRenderer->drawAll(p, detViewRenderer->getBaseCanvasSize(s->getRegion()), s->getRegion());
}

QSize DetailsViewPainter::getImageSize(CustomExportSettings* settings) const {
    SequenceExportSettings* s = qobject_cast<SequenceExportSettings*>(settings);
    SAFE_POINT(s != NULL, "Cannot cast CustomExportSettings to SequenceExportSettings", QSize());

    return detViewRenderer->getBaseCanvasSize(s->getRegion());
}

/************************************************************************/
/* SequencePainterFactory */
/************************************************************************/
QSharedPointer<ExportImagePainter> SequencePainterFactory::createPainter(ADVSingleSequenceWidget *seqWidget, SequenceExportType exportType) {
    SAFE_POINT(seqWidget != NULL, "SequenceWidget is NULL", QSharedPointer<ExportImagePainter>());

    switch (exportType) {
    case ExportCurrentView:
        return QSharedPointer<ExportImagePainter>(new CurrentViewPainter(seqWidget));
    case ExportZoomedView:
        return QSharedPointer<ExportImagePainter>(new ZoomedViewPainter(seqWidget->getPanView()));
    case ExportDetailsView:
        return QSharedPointer<ExportImagePainter>(new DetailsViewPainter(seqWidget->getDetView()));
    }
    FAIL("Invalid sequence export type", QSharedPointer<ExportImagePainter>());
}

} // namespace
