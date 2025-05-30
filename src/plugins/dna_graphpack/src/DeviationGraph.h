/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QAction>
#include <QBitArray>
#include <QList>
#include <QPair>

#include <U2View/GSequenceGraphView.h>

#include "DNAGraphPackPlugin.h"

namespace U2 {

class DeviationGraphFactory : public GSequenceGraphFactory {
    Q_OBJECT
public:
    enum GDeviationType { GC,
                          AT };
    DeviationGraphFactory(GDeviationType t, QObject* p);
    QList<QSharedPointer<GSequenceGraphData>> createGraphs(GSequenceGraphView* v) override;
    bool isEnabled(const U2SequenceObject* o) const override;

private:
    QPair<char, char> devPair;
};

class DeviationGraphAlgorithm : public GSequenceGraphAlgorithm {
public:
    DeviationGraphAlgorithm(const QPair<char, char>& _p);

    void calculate(QVector<float>& res, U2SequenceObject* o, qint64 window, qint64 step, U2OpStatus& os) override;

private:
    void windowStrategyWithoutMemorize(QVector<float>& res, const QByteArray& seq, qint64 startPos, qint64 window, qint64 step, qint64 nSteps, U2OpStatus& os);
    void sequenceStrategyWithMemorize(QVector<float>& res, const QByteArray& seq, const U2Region& visualRegion, qint64 window, qint64 step, U2OpStatus& os);
    QPair<int, int> matchOnStep(const QByteArray& seq, int begin, int end);

    QPair<char, char> p;
};

}  // namespace U2
