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

#pragma once

#include <QObject>
#include <QSharedPointer>

#include <U2Algorithm/TmCalculator.h>

#include <U2Core/global.h>

#include "PrimerDimersFinder.h"

namespace U2 {

class U2CORE_EXPORT PrimerStatistics : public QObject {
    Q_OBJECT
public:
    static QString checkPcrPrimersPair(const QByteArray& forward, const QByteArray& reverse, const QSharedPointer<TmCalculator>& temperatureCalculator, bool& isCriticalError);

    static bool validate(const QByteArray& primer);
    static bool validatePrimerLength(const QByteArray& primer);
    static bool validate(QString primer);

    static QString getDoubleStringValue(double value);

private:
    /** Minimum supported primer length. A length below this value may result to a memory and results count overflow. */
    static constexpr int MINIMUM_PRIMER_LENGTH = 15;

    /** Maximum supported primer length. A length above this value is found as non-meaningful for a primers search. */
    static constexpr int MAXIMUM_PRIMER_LENGTH = 200;
};

class U2CORE_EXPORT PrimerStatisticsCalculator {
public:
    enum Direction { Forward,
                     Reverse,
                     DoesntMatter };
    PrimerStatisticsCalculator(const QByteArray& sequence, const QSharedPointer<TmCalculator>& temperatureCalculator, Direction direction = DoesntMatter, const qreal energyThreshold = -6);

    double getGC() const;
    double getTm() const;
    int getGCClamp() const;
    int getRuns() const;
    const DimerFinderResult& getDimersInfo() const;

    QString getFirstError() const;
    /* Returns the error occurred during initialization (construction) */
    QString getInitializationError() const;

    bool isValidGC(QString& error) const;
    bool isValidTm(QString& error) const;
    bool isValidGCClamp(QString& error) const;
    bool isValidRuns(QString& error) const;
    bool isSelfDimer(QString& error) const;

    static const double GC_BOTTOM;
    static const double GC_TOP;
    static const double TM_BOTTOM;
    static const double TM_TOP;
    static const int CLAMP_BOTTOM;
    static const int RUNS_TOP;
    static const double DIMERS_ENERGY_THRESHOLD;

private:

    QString getMessage(const QString& error) const;

private:
    DimerFinderResult dimersInfo;
    const QByteArray sequence;
    QSharedPointer<TmCalculator> temperatureCalculator;
    Direction direction;
    qreal energyThreshold = 0.0;
    int nA;
    int nC;
    int nG;
    int nT;
    int maxRun;

    QString initializationError;
};

class U2CORE_EXPORT PrimersPairStatistics {
public:
    PrimersPairStatistics(const QByteArray& forward, const QByteArray& reverse, const QSharedPointer<TmCalculator>& temperatureCalculator);
    ~PrimersPairStatistics() = default;
    Q_DISABLE_COPY_MOVE(PrimersPairStatistics);

    QString getFirstError() const;

    QString generateReport() const;
    /* Returns the error occurred during initialization (construction) */
    QString getInitializationError() const;

    static const QString TmString;
    static QString toString(double value);

    const PrimerStatisticsCalculator& getForwardCalculator() {
        return forward;
    }
    const PrimerStatisticsCalculator& getReverseCalculator() {
        return reverse;
    }

    const DimerFinderResult& getDimersInfo() const;

    bool isHeteroDimers() {
        return dimersInfo.canBeFormed;
    }

private:
    void addDimersToReport(QString& report) const;

private:
    DimerFinderResult dimersInfo;
    PrimerStatisticsCalculator forward;
    PrimerStatisticsCalculator reverse;

    QString initializationError;
};

}  // namespace U2
