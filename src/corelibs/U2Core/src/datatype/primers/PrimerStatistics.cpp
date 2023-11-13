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

#include "PrimerStatistics.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include "PrimerValidator.h"

namespace U2 {

QString PrimerStatistics::checkPcrPrimersPair(const QByteArray& forward, const QByteArray& reverse, const QSharedPointer<TmCalculator>& temperatureCalculator, bool& isCriticalError) {
    QString message;
    isCriticalError = false;
    bool forwardIsValid = validate(forward);
    bool reverseIsValid = validate(reverse);
    if (!forwardIsValid && !reverseIsValid) {
        message = tr("The primers contain a character from the Extended DNA alphabet.");
    } else if (!forwardIsValid) {
        message = tr("The forward primer contains a character from the Extended DNA alphabet.");
    } else if (!reverseIsValid) {
        message = tr("The reverse primer contains a character from the Extended DNA alphabet.");
    } else if (!validatePrimerLength(forward)) {
        message = tr("The forward primer length should be between %1 and %2 bp.").arg(QString::number(MINIMUM_PRIMER_LENGTH)).arg(QString::number(MAXIMUM_PRIMER_LENGTH));
    } else if (!validatePrimerLength(reverse)) {
        message = tr("The reverse primer length should be between %1 and %2 bp.").arg(QString::number(MINIMUM_PRIMER_LENGTH)).arg(QString::number(MAXIMUM_PRIMER_LENGTH));
    }
    if (!message.isEmpty()) {
        message += tr(" Unable to calculate primer statistics.");
        isCriticalError = true;
        return message;
    }

    PrimersPairStatistics calc(forward, reverse, temperatureCalculator);
    message = calc.getFirstError();
    return message;
}

bool PrimerStatistics::validate(const QByteArray& primer) {
    return validate(QString(primer));
}

bool PrimerStatistics::validate(QString primer) {
    PrimerValidator pv(nullptr, false);
    int pos = 0;
    return pv.validate(primer, pos) == QValidator::Acceptable;
}

bool PrimerStatistics::validatePrimerLength(const QByteArray& primer) {
    return primer.size() >= MINIMUM_PRIMER_LENGTH && primer.size() <= MAXIMUM_PRIMER_LENGTH;
}

QString PrimerStatistics::getDoubleStringValue(double value) {
    QString result = QString::number(value, 'f', 2);
    result.remove(QRegExp("\\.?0+$"));
    return result;
}

/************************************************************************/
/* PrimerStatisticsCalculator */
/************************************************************************/
const double PrimerStatisticsCalculator::GC_BOTTOM = 50.0;
const double PrimerStatisticsCalculator::GC_TOP = 60.0;
const double PrimerStatisticsCalculator::TM_BOTTOM = 55.0;
const double PrimerStatisticsCalculator::TM_TOP = 80.0;
const int PrimerStatisticsCalculator::CLAMP_BOTTOM = 1;
const int PrimerStatisticsCalculator::RUNS_TOP = 4;
const double PrimerStatisticsCalculator::DIMERS_ENERGY_THRESHOLD = -6.0;

PrimerStatisticsCalculator::PrimerStatisticsCalculator(const QByteArray& _sequence,
                                                       const QSharedPointer<TmCalculator>& _temperatureCalculator,
                                                       Direction _direction,
                                                       const qreal _energyThreshold)
    : sequence(_sequence), 
      temperatureCalculator(_temperatureCalculator), 
      direction(_direction), 
      energyThreshold(_energyThreshold),
      nA(0), nC(0), nG(0), nT(0), maxRun(0) {
    SAFE_POINT(temperatureCalculator != nullptr, "PrimerStatisticsCalculator:temperatureCalculator is null" ,)
    CHECK(!sequence.isEmpty(), );

    int currentRun = 0;
    char prevC = sequence[0];
    foreach (const char c, sequence) {
        switch (c) {
            case 'A':
                nA++;
                break;
            case 'C':
                nC++;
                break;
            case 'G':
                nG++;
                break;
            case 'T':
                nT++;
                break;
            case 'N':
                break;
            default:
                initializationError = PrimerStatistics::tr("Unexpected symbol: %1").arg(c);
                return;
        }
        if (prevC == c) {
            currentRun++;
        } else {
            if (currentRun > maxRun) {
                maxRun = currentRun;
            }
            currentRun = 1;
            prevC = c;
        }
    }
    if (currentRun > maxRun) {
        maxRun = currentRun;
    }

    HeteroDimersFinder dimersFinder(sequence, sequence, energyThreshold);
    dimersInfo = dimersFinder.getResult();
}

double PrimerStatisticsCalculator::getGC() const {
    if (sequence.isEmpty()) {
        return 0;
    }
    return 100.0 * (nG + nC) / double(sequence.length());
}

double PrimerStatisticsCalculator::getTm() const {
    return temperatureCalculator->getMeltingTemperature(sequence);
}

int PrimerStatisticsCalculator::getGCClamp() const {
    QString right = sequence.right(5);
    int result = 0;
    foreach (char c, right.toLocal8Bit()) {
        if ('C' == c || 'G' == c) {
            result++;
        }
    }
    return result;
}

int PrimerStatisticsCalculator::getRuns() const {
    return maxRun;
}

const DimerFinderResult& PrimerStatisticsCalculator::getDimersInfo() const {
    return dimersInfo;
}

QString PrimerStatisticsCalculator::getFirstError() const {
    QString result;

    // GC
    if (!isValidGC(result)) {
        return result;
    }

    // Tm
    if (!isValidTm(result)) {
        return result;
    }

    // GC clamp
    if (!isValidGCClamp(result)) {
        return result;
    }

    // Runs
    if (!isValidRuns(result)) {
        return result;
    }

    // Self dimers
    if (!isSelfDimer(result)) {
        return result;
    }

    return result;
}

QString PrimerStatisticsCalculator::getInitializationError() const {
    return initializationError;
}

bool PrimerStatisticsCalculator::isValidGC(QString& error) const {
    double value = getGC();
    CHECK_EXT(value >= GC_BOTTOM, error = getMessage(PrimerStatistics::tr("low GC-content")), false);
    CHECK_EXT(value <= GC_TOP, error = getMessage(PrimerStatistics::tr("high GC-content")), false);
    return true;
}

bool PrimerStatisticsCalculator::isValidTm(QString& error) const {
    double value = getTm();
    CHECK_EXT(value >= TM_BOTTOM, error = getMessage(PrimerStatistics::tr("low melting temperature")), false);
    CHECK_EXT(value <= TM_TOP, error = getMessage(PrimerStatistics::tr("high melting temperature")), false);
    return true;
}

bool PrimerStatisticsCalculator::isValidGCClamp(QString& error) const {
    int value = getGCClamp();
    CHECK_EXT(value >= CLAMP_BOTTOM, error = getMessage(PrimerStatistics::tr("low GC clamp")), false);
    return true;
}

bool PrimerStatisticsCalculator::isValidRuns(QString& error) const {
    int value = getRuns();
    CHECK_EXT(value <= RUNS_TOP, error = getMessage(PrimerStatistics::tr("high base runs value")), false);
    return true;
}

QString PrimerStatisticsCalculator::getMessage(const QString& error) const {
    switch (direction) {
        case Forward:
            return PrimerStatistics::tr("forward primer has %1.").arg(error);
        case Reverse:
            return PrimerStatistics::tr("reverse primer has %1.").arg(error);
        default:
            return error;
    }
}

bool PrimerStatisticsCalculator::isSelfDimer(QString& error) const {
    CHECK_EXT(!dimersInfo.canBeFormed, error = "<br>Self-dimer can be formed:<br>" + dimersInfo.getShortReport(), false);
    return true;
}

/************************************************************************/
/* PrimersPairStatistics */
/************************************************************************/
const QString PrimersPairStatistics::TmString = "Tm" + QString::fromLatin1(" (\x00B0") + "C)";

namespace {
const QString GC_RANGE = QString("%1-%2").arg(PrimerStatisticsCalculator::GC_BOTTOM).arg(PrimerStatisticsCalculator::GC_TOP);
const QString TM_RANGE = QString("%1-%2").arg(PrimerStatisticsCalculator::TM_BOTTOM).arg(PrimerStatisticsCalculator::TM_TOP);
const QString DIMERS_RANGE = QString("&Delta;G &gt;=%1 kcal/mol").arg(PrimerStatisticsCalculator::DIMERS_ENERGY_THRESHOLD);
}  // namespace

PrimersPairStatistics::PrimersPairStatistics(const QByteArray& _forward, const QByteArray& _reverse, const QSharedPointer<TmCalculator>& temperatureCalculator)
    : forward(_forward, temperatureCalculator, PrimerStatisticsCalculator::Forward),
      reverse(_reverse, temperatureCalculator, PrimerStatisticsCalculator::Reverse) {
    initializationError = forward.getInitializationError().isEmpty() ? reverse.getInitializationError() : forward.getInitializationError();
    if (!initializationError.isEmpty()) {
        return;
    }
    HeteroDimersFinder dimersFinder(_forward, _reverse);
    dimersInfo = dimersFinder.getResult();
}

QString PrimersPairStatistics::getFirstError() const {
    QString result = forward.getFirstError();

    if (!result.isEmpty()) {
        return result;
    }

    result = reverse.getFirstError();
    if (!result.isEmpty()) {
        return result;
    }

    if (dimersInfo.canBeFormed) {
        return dimersInfo.getShortReport();
    }

    return "";
}

#define CREATE_COLUMN(name, width, center) \
    result += QString("<th width=\"%1%\"/><p %2><strong>%3</strong></p></th>").arg(width).arg((center) ? "align=\"center\"" : "align=\"left\"").arg(name);

#define CREATE_CELL(value, good, center) \
    result += QString("<td %1 %2>%3</td>").arg((good) ? "" : " style=\"color: red;\"").arg((center) ? " align=\"center\"" : "").arg(value);

#define CREATE_ROW(criteria, range, value1, value2, good1, good2) \
    result += "<tr>"; \
    CREATE_CELL(criteria, true, false); \
    CREATE_CELL(range, true, false); \
    CREATE_CELL(value1, good1, true); \
    CREATE_CELL(value2, good2, true); \
    result += "</tr>";

QString PrimersPairStatistics::generateReport() const {
    QString result;
    result += "<table>";
    result += "<tr>";
    CREATE_COLUMN(PrimerStatistics::tr("Criteria"), 30, false);
    CREATE_COLUMN(PrimerStatistics::tr("Valid Values"), 30, false);
    CREATE_COLUMN(PrimerStatistics::tr("Forward"), 20, true);
    CREATE_COLUMN(PrimerStatistics::tr("Reverse"), 20, true);
    result += "</tr>";

    QString e;
    CREATE_ROW("GC (%)", GC_RANGE, toString(forward.getGC()), toString(reverse.getGC()), forward.isValidGC(e), reverse.isValidGC(e));
    CREATE_ROW(TmString, TM_RANGE, toString(forward.getTm()), toString(reverse.getTm()), forward.isValidTm(e), reverse.isValidTm(e));
    const QString clumpRange = PrimerStatistics::tr("&gt;=%1 G or C at 3' end").arg(PrimerStatisticsCalculator::CLAMP_BOTTOM);
    CREATE_ROW(PrimerStatistics::tr("GC Clamp"), clumpRange, QString::number(forward.getGCClamp()), QString::number(reverse.getGCClamp()), forward.isValidGCClamp(e), reverse.isValidGCClamp(e));
    const QString mononuclLength = PrimerStatistics::tr("&lt;=%1 mononucleotide repeat length").arg(PrimerStatisticsCalculator::RUNS_TOP);
    CREATE_ROW(PrimerStatistics::tr("Poly-X"), mononuclLength, QString::number(forward.getRuns()), QString::number(reverse.getRuns()), forward.isValidRuns(e), reverse.isValidRuns(e));
    result += "</table>";
    addDimersToReport(result);
    return result;
}

QString PrimersPairStatistics::getInitializationError() const {
    return initializationError;
}

const DimerFinderResult& U2::PrimersPairStatistics::getDimersInfo() const {
    return dimersInfo;
}

void PrimersPairStatistics::addDimersToReport(QString& report) const {
    if (forward.getDimersInfo().canBeFormed || reverse.getDimersInfo().canBeFormed) {
        report += QString("<h4>%1: </h4>").arg(PrimerStatistics::tr("Self-dimers"));
        if (forward.getDimersInfo().canBeFormed) {
            report += "<p>" + forward.getDimersInfo().getFullReport() + "</p>";
        }
        if (reverse.getDimersInfo().canBeFormed) {
            report += "<p>" + reverse.getDimersInfo().getFullReport() + "</p>";
        }
    }
    if (dimersInfo.canBeFormed) {
        report += QString("<h4>%1: </h4>").arg(PrimerStatistics::tr("Hetero-dimers"));
        report += "<p>" + dimersInfo.getFullReport() + "</p>";
    }
}

QString PrimersPairStatistics::toString(double value) {
    QString result = QString::number(value, 'f', 2);
    result.remove(QRegExp("\\.?0+$"));
    return result;
}

}  // namespace U2
