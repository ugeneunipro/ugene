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


#ifndef _U2_SEQUENCE_PAINTER_H_
#define _U2_SEQUENCE_PAINTER_H_

#include <QPainter>
#include <U2Core/U2Region.h>

#include "ExportImagePainter.h"

namespace U2 {

class ADVSingleSequenceWidget;
class DetView;
class DetViewRenderer;
class PanView;
class PanViewRenderer;

enum SequenceExportType {
    ExportCurrentView,
    ExportZoomedView,
    ExportDetailsView
};

class SequenceExportSettings : public CustomExportSettings {
    Q_OBJECT
public:
    SequenceExportSettings(qint64 seqLen, SequenceExportType type)
        : region(U2Region(0, seqLen)),
          type(type) {}

    U2Region            getRegion() const;
    SequenceExportType  getType() const;

    void setRegion(const U2Region& r);
    void setType(SequenceExportType t);

private:
    U2Region            region;
    SequenceExportType  type;
};


/************************************************************************/
/* CurrentViewPainter */
/************************************************************************/
class CurrentViewPainter : public ExportImagePainter {
    Q_OBJECT
public:
    CurrentViewPainter(ADVSingleSequenceWidget* seqWidget)
        : ExportImagePainter(),
          seqWidget(seqWidget) {}

    void    paint(QPainter &p, CustomExportSettings* settings) const;
    QSize   getImageSize(CustomExportSettings* settings) const;
    bool    canPaintSvg(CustomExportSettings* settings, U2OpStatus &os) const;

private:
    ADVSingleSequenceWidget*    seqWidget;
};

/************************************************************************/
/* ZoomedViewPainter */
/************************************************************************/
class ZoomedViewPainter : public ExportImagePainter {
public:
    ZoomedViewPainter(PanView* panView);

    void    paint(QPainter &p, CustomExportSettings* settings) const;
    QSize   getImageSize(CustomExportSettings* settings) const;
    bool    canPaintSvg(CustomExportSettings* settings, U2OpStatus &os) const;

private:
    PanView*    panView;
    PanViewRenderer*    panViewRenderer;
};

/************************************************************************/
/* DetailsViewPainter */
/************************************************************************/
class DetailsViewPainter : public ExportImagePainter {
public:
    DetailsViewPainter(DetView* detView);

    void    paint(QPainter &p, CustomExportSettings* settings) const;
    QSize   getImageSize(CustomExportSettings* settings) const;
    bool    canPaintSvg(CustomExportSettings* /*settings*/, U2OpStatus &/*os*/) const { return true; }

private:
    DetViewRenderer*    detViewRenderer;
};

/************************************************************************/
/* SequencePainterFactory */
/************************************************************************/
class SequencePainterFactory {
public:
    static QSharedPointer<ExportImagePainter> createPainter(ADVSingleSequenceWidget* seqWidget, SequenceExportType exportType);
};

} // namespace

#endif // _U2_SEQUENCE_PAINTER_H_
