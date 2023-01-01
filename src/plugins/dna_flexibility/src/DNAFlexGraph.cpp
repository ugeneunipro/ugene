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

#include "DNAFlexGraph.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/TextUtils.h>

#include "DNAFlexGraphAlgorithm.h"

namespace U2 {

const qint64 DNAFlexGraphFactory::DEFAULT_WINDOW_SIZE = 100;

const int DNAFlexGraphFactory::DEFAULT_WINDOW_STEP = 1;

/**
 * Name of the graph (shown to a user)
 */
static QString nameByType() {
    return DNAFlexGraphFactory::tr("DNA Flexibility");
}

/**
 * Constructor of the DNA Flexibility graph
 */
DNAFlexGraphFactory::DNAFlexGraphFactory(QObject* parent)
    : GSequenceGraphFactory(nameByType(), parent) {
}

/**
 * Verifies that the sequence alphabet is standard DNA alphabet
 */
bool DNAFlexGraphFactory::isEnabled(const U2SequenceObject* sequenceObject) const {
    const DNAAlphabet* alphabet = sequenceObject->getAlphabet();
    return alphabet->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
}

/**
 * Initializes graph data
 */
QList<QSharedPointer<GSequenceGraphData>> DNAFlexGraphFactory::createGraphs(GSequenceGraphView* view) {
    assert(isEnabled(view->getSequenceObject()));
    return {QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, getGraphName(), new DNAFlexGraphAlgorithm()))};
}

/**
 * Initializes the graph drawer
 */
GSequenceGraphDrawer* DNAFlexGraphFactory::getDrawer(GSequenceGraphView* view) {
    qint64 window = qMin(DEFAULT_WINDOW_SIZE, view->getSequenceLength());
    return new GSequenceGraphDrawer(view, window, DEFAULT_WINDOW_STEP);
}

}  // namespace U2
