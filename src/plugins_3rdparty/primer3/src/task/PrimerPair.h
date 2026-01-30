/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <QSharedPointer>

namespace U2 {

struct PrimerSingle;

/**
 * This struct represents an inner UGENE representation of Primer3 pair results - left and right primers.
 */
struct PrimerPair {
public:
    PrimerPair() = default;
    PrimerPair(const primer_pair& primerPair, int offset = 0);
    PrimerPair(const PrimerPair& primerPair);
    PrimerPair& operator=(const PrimerPair& other);
    PrimerPair(PrimerPair&& other) noexcept = delete;
    PrimerPair& operator=(PrimerPair&& other) noexcept = delete;
    bool operator==(const PrimerPair& primerPair) const = delete;
    bool operator<(const PrimerPair& pair) const;

    // don't forget to change copy constructor and assignment operator when changing this!
    QSharedPointer<PrimerSingle> leftPrimer;
    QSharedPointer<PrimerSingle> rightPrimer;
    QSharedPointer<PrimerSingle> internalOligo;
    double complAny = 0.0;
    double complEnd = 0.0;
    int productSize = 0;
    double quality = 0.0;
    double tm = 0.0;
    double repeatSim = 0.0;
    QString repeatSimName;
    QString complAnyStruct;
    QString complEndStruct;
};

}
