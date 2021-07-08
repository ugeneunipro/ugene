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

#include "FindPresenceOfUnwantedParametersTask.h"

#include <utils/UnwantedConnectionsUtils.h>

namespace U2 {

FindPresenceOfUnwantedParametersTask::FindPresenceOfUnwantedParametersTask(const QByteArray& _sequence,
                                            const PCRPrimerDesignForDNAAssemblyTaskSettings& _settings)
    : Task("Find Presence of Unwanted Parameters Task", TaskFlags_FOSCOE),
      sequence(_sequence),
      settings(_settings) {}

void FindPresenceOfUnwantedParametersTask::run() {
    if (settings.bachbone5Length < 0 || settings.bachbone3Length < 0) {
        setError("Backbone length must be greater than 0");
        return;
    }
    if (sequence.length() < settings.bachbone5Length) {
        stateInfo.addWarning("Sequence length is less than backbone5Length, the entire sequence is used");
    }
    if (sequence.length() < settings.bachbone3Length) {
        stateInfo.addWarning("Sequence length is less than backbone3Length, the entire sequence is used");
    }
    QByteArray forward = sequence.left(settings.bachbone5Length);
    QByteArray reverse = sequence.right(settings.bachbone3Length);
    QString report;

    UnwantedConnectionsUtils::isUnwantedSelfDimer(forward, settings.gibbsFreeEnergyExclude,
        settings.meltingPointExclude, settings.complementLengthExclude, report);
    if (!report.isEmpty()) {
        unwantedStructures = "<strong>5' homodimers:</strong><br><br>";
        unwantedStructures += report;
        unwantedStructures += "<br>";
        report.clear();
    }

    UnwantedConnectionsUtils::isUnwantedSelfDimer(reverse, settings.gibbsFreeEnergyExclude,
        settings.meltingPointExclude, settings.complementLengthExclude, report);
    if (!report.isEmpty()) {
        unwantedStructures += "<strong>3' homodimers:</strong><br><br>";
        unwantedStructures += report;
        unwantedStructures += "<br>";
        report.clear();
    }

    UnwantedConnectionsUtils::isUnwantedHeteroDimer(forward, reverse, settings.gibbsFreeEnergyExclude,
        settings.meltingPointExclude, settings.complementLengthExclude, report);
    if (!report.isEmpty()) {
        unwantedStructures += "<strong>Heterodimers:</strong><br><br>";
        unwantedStructures += report;
    }
}

bool FindPresenceOfUnwantedParametersTask::hasUnwantedParameters() const {
    return !unwantedStructures.isEmpty();
}

const QByteArray& FindPresenceOfUnwantedParametersTask::getSequence() const {
    return sequence;
}

const QString &FindPresenceOfUnwantedParametersTask::getUnwantedStructures() const {
    return unwantedStructures;
}
}
