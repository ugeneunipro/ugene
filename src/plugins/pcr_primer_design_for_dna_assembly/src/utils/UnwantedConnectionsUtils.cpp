/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "UnwantedConnectionsUtils.h"

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/Log.h>
#include <U2Core/PrimerStatistics.h>

#include "PCRPrimerDesignForDNAAssemblyPlugin.h"

namespace U2 {

using ExclusionCriteria = OptionalParametersToExclude::ExclusionCriteria;

bool UnwantedConnectionsUtils::isUnwantedSelfDimer(const QByteArray& forwardSequence,
                                                   double unwantedDeltaG,
                                                   double unwantedMeltingTemperature,
                                                   int unwantedDimerLength) {
    QString unused;
    return isUnwantedSelfDimer(forwardSequence,
                               ExclusionCriteria::All,
                               {unwantedDeltaG, unwantedMeltingTemperature, unwantedDimerLength},
                               unused);
}

bool UnwantedConnectionsUtils::isUnwantedSelfDimer(const QByteArray& forwardSequence,
                                                   const ExclusionCriteria& criterion,
                                                   const OptionalParametersToExclude& optionalThresholds,
                                                   QString& report) {
    PrimerStatisticsCalculator calc(forwardSequence, PrimerStatisticsCalculator::Direction::DoesntMatter);
    report = PCRPrimerDesignForDNAAssemblyPlugin::tr("<b>Self-dimer:</b><br>");
    return areUnwantedParametersPresentedInDimersInfo(calc.getDimersInfo(), criterion, optionalThresholds, report);
}

bool UnwantedConnectionsUtils::isUnwantedSelfDimer(const QByteArray& forwardSequence,
                                                   const ExclusionCriteria& criterion,
                                                   const OptionalParametersToExclude& optionalThresholds) {
    QString unused;
    return isUnwantedSelfDimer(forwardSequence, criterion, optionalThresholds, unused);
}

bool UnwantedConnectionsUtils::isUnwantedHeteroDimer(const QByteArray& forwardSequence,
                                                     const QByteArray& reverseSequence,
                                                     const ExclusionCriteria& criterion,
                                                     const OptionalParametersToExclude& optionalThresholds) {
    QString unused;
    return isUnwantedHeteroDimer(forwardSequence, reverseSequence, criterion, optionalThresholds, unused);
}

bool UnwantedConnectionsUtils::isUnwantedHeteroDimer(const QByteArray& forwardSequence,
                                                     const QByteArray& reverseSequence,
                                                     const ExclusionCriteria& criterion,
                                                     const OptionalParametersToExclude& optionalThresholds,
                                                     QString& report) {
    PrimersPairStatistics calc(forwardSequence, reverseSequence);
    report = PCRPrimerDesignForDNAAssemblyPlugin::tr("<b>Hetero-dimer:</b><br>");
    return areUnwantedParametersPresentedInDimersInfo(calc.getDimersInfo(), criterion, optionalThresholds, report);
}

bool UnwantedConnectionsUtils::areUnwantedParametersPresentedInDimersInfo(const DimerFinderResult& dimersInfo,
                                                                          const ExclusionCriteria& criterion,
                                                                          const OptionalParametersToExclude& thresholds,
                                                                          QString& report) {
    if (dimersInfo.dimersOverlap.isEmpty() || !thresholds.isAnyParameterSet()) {
        return false;
    }

    QList<bool> conditionResult;
    if (thresholds.gibbsFreeEnergy.canConvert<double>()) {
        conditionResult += dimersInfo.deltaG <= thresholds.gibbsFreeEnergy.toDouble();
    }
    if (thresholds.meltingPoint.canConvert<double>()) {
        QByteArray dimer = dimersInfo.dimer.toLocal8Bit();
        double dimerMeltingTemp = PrimerStatistics::getMeltingTemperature(dimer);
        double reverseDimerMeltingTemp =
            PrimerStatistics::getMeltingTemperature(DNASequenceUtils::reverseComplement(dimer));
        double unwantedMeltingTemperature = thresholds.meltingPoint.toDouble();

        conditionResult += unwantedMeltingTemperature <= dimerMeltingTemp &&
                           unwantedMeltingTemperature <= reverseDimerMeltingTemp;
    }
    if (thresholds.complementLength.canConvert<int>()) {
        int dimerLength = dimersInfo.dimer.length();
        conditionResult += thresholds.complementLength.toInt() <= dimerLength;
    }

    int numberOfUnwanted = conditionResult.count(true);
    bool isUnwantedParameter = criterion == ExclusionCriteria::Any ? numberOfUnwanted > 0
                                                                   : numberOfUnwanted == conditionResult.size();
    report += PCRPrimerDesignForDNAAssemblyPlugin::tr(dimersInfo.getReportWithMeltingTemp().toLocal8Bit());
    if (isUnwantedParameter) {
        algoLog.details(report);
    }

    return isUnwantedParameter;
}

}  // namespace U2
