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

namespace U2 {

FindPresenceOfUnwantedParametersTask::FindPresenceOfUnwantedParametersTask(const QByteArray& _sequence,
                                            const PCRPrimerDesignForDNAAssemblyTaskSettings& _settings)
    : Task("Find Presence of Unwanted Parameters Task", TaskFlags_FOSCOE),
      sequence(_sequence),
      settings(_settings) {}

void FindPresenceOfUnwantedParametersTask::run() {
    //TODO
}

bool FindPresenceOfUnwantedParametersTask::hasUnwantedParameters() const {
    //TODO
    return false;
}

const QByteArray& FindPresenceOfUnwantedParametersTask::getSequence() const {
    return sequence;
}

}