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

#ifndef _U2_PRIMER_DIMERS_FINDER_H_
#define _U2_PRIMER_DIMERS_FINDER_H_

#include <QObject>
#include <U2Core/U2Region.h>

namespace U2 {

class DimerFinderResult {
public:
    DimerFinderResult() : deltaG(0.0), baseCounts(0), canBeFormed(false) {}

    QString getFullReport() const;
    QString getShortReport() const;

    QString dimersOverlap;
    double  deltaG;
    int     baseCounts;
    bool    canBeFormed;
};

class BaseDimersFinder {
public:
    BaseDimersFinder(const QByteArray &forwardPrimer, const QByteArray &reversePrimer, double energyThreshold);

    DimerFinderResult getResult() const;

protected:
    void fillResultsForCurrentIteration(const QByteArray& homologousBases, int overlapStartPos);
    virtual QString getDimersOverlapping(int dimerFormationPos) = 0;

protected:
    static QMap<QByteArray, qreal> energyMap;

    QByteArray  forwardPrimer;
    QByteArray  reversePrimer;
    QByteArray  reverseComplementSequence;
    QByteArray  resHomologousRegion;
    U2Region    overlappingRegion;

    double      energyThreshold;
    qreal       maximumDeltaG;
    QString     dimersOverlap;
};

class SelfDimersFinder : public BaseDimersFinder {
public:
    SelfDimersFinder(const QByteArray &_forwardPattern, const qreal energyThreshold  = -6);

private:
    QString getDimersOverlapping(int dimerFormationPos);
};

class HeteroDimersFinder : public BaseDimersFinder {
public:
    HeteroDimersFinder(const QByteArray &_forwardPattern, const QByteArray &reversePattern, const qreal energyThreshold  = -6);

private:
    QString getDimersOverlapping(int dimerFormationPos);
};

} // U2

#endif // _U2_PRIMER_STATISTICS_H_
