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

#ifndef _U2_DNA_FLEX_GRAPH_H_
#define _U2_DNA_FLEX_GRAPH_H_

#include <U2View/GSequenceGraphView.h>
#include <U2View/GraphMenu.h>

namespace U2 {

/**
 * Factory used to draw a DNA Flexibility graph
 */
class DNAFlexGraphFactory : public GSequenceGraphFactory {
    Q_OBJECT
public:
    DNAFlexGraphFactory(QObject*);
    QList<QSharedPointer<GSequenceGraphData>> createGraphs(GSequenceGraphView*) override;
    GSequenceGraphDrawer* getDrawer(GSequenceGraphView*) override;
    bool isEnabled(const U2SequenceObject*) const override;

private:
    /** Default size of "window" on a graph */
    static const qint64 DEFAULT_WINDOW_SIZE;

    /** Default size of "step" on a graph */
    static const int DEFAULT_WINDOW_STEP;
};

}  // namespace U2
#endif
