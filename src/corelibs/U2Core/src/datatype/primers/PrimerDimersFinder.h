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

#include <QObject>

#include <U2Core/U2Region.h>
#include <U2Core/global.h>

namespace U2 {

struct U2CORE_EXPORT DimerFinderResult {
    QString getFullReport() const;
    /**
     *
     * \return Returns short report (only GC content and base pairs number) in one line with Delta G and Base Pairs in <bold>
     */
    QString getShortBoldReport() const;
    QString getShortReport() const;

    QString dimersOverlap;
    double deltaG = 0.0;
    QString dimer;
    int baseCounts = 0;
    bool canBeFormed = false;
};

class U2CORE_EXPORT BaseDimersFinder {
public:
    BaseDimersFinder(const QByteArray& forwardPrimer, const QByteArray& reversePrimer, double energyThreshold);

    DimerFinderResult getResult() const;

    static const QMap<QByteArray, qreal> ENERGY_MAP;

protected:
    void fillResultsForCurrentIteration(const QByteArray& homologousBases, int overlapStartPos);
    virtual QString getDimersOverlapping(int dimerFormationPos) = 0;

    QByteArray forwardPrimer;
    QByteArray reversePrimer;
    QByteArray reverseComplementSequence;
    QByteArray resHomologousRegion;
    U2Region overlappingRegion;

    double energyThreshold;
    qreal maximumDeltaG;
    QString dimersOverlap;
};

class U2CORE_EXPORT SelfDimersFinder : public BaseDimersFinder {
public:
    SelfDimersFinder(const QByteArray& _forwardPattern, qreal energyThreshold = -6);

private:
    QString getDimersOverlapping(int dimerFormationPos) override;
};

class U2CORE_EXPORT HeteroDimersFinder : public BaseDimersFinder {
public:
    HeteroDimersFinder(const QByteArray& _forwardPattern, const QByteArray& reversePattern, qreal energyThreshold = -6);

private:
    QString getDimersOverlapping(int dimerFormationPos) override;
};

}  // namespace U2
