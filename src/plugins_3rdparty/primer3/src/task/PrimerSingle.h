/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "primer3_core/libprimer3.h"

#include <QString>

#include <U2Core/U2Region.h>

namespace U2 {

/**
* This struct represents an inner UGENE representation of Primer3 single result - only one primer.
*/
struct PrimerSingle {
    PrimerSingle(oligo_type type = oligo_type::OT_LEFT);
    PrimerSingle(const primer_rec& primerRec, oligo_type type, int offset);

    /**
     * Calculates positions where, on the sequence of set length, this primer is located.
     * \param sequenceLength Length of the sequence.
     * \return Returns regions where this primer is located. If there are 2 regions,
     * primer intersects junction point.
     */
    QVector<U2Region> getSequenceRegions(int sequenceLength) const;

    int start = 0;
    int length = 0;
    double meltingTemperature = 0.0;
    double bound = 0.0;
    double gcContent = 0.0;
    double selfAny = 0.0;
    double selfEnd = 0.0;
    double templateMispriming = 0.0;
    double hairpin = 0.0;
    double endStability = 0.0;
    double quality = 0.0;
    double repeatSim = 0.0;
    QString repeatSimName;
    QString selfAnyStruct;
    QString selfEndStruct;
    oligo_type type = OT_LEFT;
};

}
