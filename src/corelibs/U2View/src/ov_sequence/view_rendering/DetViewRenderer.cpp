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

#include "DetViewRenderer.h"
#include "DetViewSingleLineRenderer.h"
#include "DetViewMultiLineRenderer.h"

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>


namespace U2 {

DetViewRenderer::DetViewRenderer(DetView* detView, SequenceObjectContext* ctx)
    : SequenceViewAnnotatedRenderer(ctx),
      detView(detView) {

}

double DetViewRenderer::getCurrentScale() const {
    return commonMetrics.charWidth;
}

qint64 DetViewRenderer::getSymbolsPerLine(const qint64 width) const {
    return width / commonMetrics.charWidth;
}

DetViewRenderer* DetViewRendererFactory::createRenderer(DetView *detView, SequenceObjectContext *ctx, bool multiLine) {
    if (multiLine) {
        return new DetViewMultiLineRenderer(detView, ctx);
    } else {
        return new DetViewSingleLineRenderer(detView, ctx);
    }
}

} // namespace
