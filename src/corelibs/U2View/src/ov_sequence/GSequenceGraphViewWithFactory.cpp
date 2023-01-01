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

#include "GSequenceGraphViewWithFactory.h"

#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/GraphUtils.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

namespace U2 {

GSequenceGraphDrawer* GSequenceGraphFactory::getDrawer(GSequenceGraphView* v) {
    qint64 seqLen = v->getSequenceLength();
    // By default, we have max window = 500: it is normal for DNA regions to have some meaningful content in this range.
    qint64 window = qBound((qint64)40, GraphUtils::pickRoundedNumberBelow(seqLen / 300), (qint64)500);
    qint64 step = window / 2;
    return new GSequenceGraphDrawer(v, window, step);
}

/**
 * Constructor of a graph view with factory
 */
GSequenceGraphViewWithFactory::GSequenceGraphViewWithFactory(
    ADVSingleSequenceWidget* sequenceWidget,
    GSequenceGraphFactory* _factory)
    : GSequenceGraphView(
          sequenceWidget,
          sequenceWidget->getSequenceContext(),
          sequenceWidget->getPanGSLView(),
          _factory->getGraphName()),
      factory(_factory) {
    setObjectName("GSequenceGraphViewWithFactory");
}

}  // namespace U2
