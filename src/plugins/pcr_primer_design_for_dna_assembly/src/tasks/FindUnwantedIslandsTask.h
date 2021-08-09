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

#ifndef _U2_FIND_UNWANTED_ISLANDS_TASK_H_
#define _U2_FIND_UNWANTED_ISLANDS_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/U2Type.h>
#include <U2Core/U2Region.h>

namespace U2 {

class FindUnwantedIslandsTask : public Task {
    Q_OBJECT
public:
    FindUnwantedIslandsTask(const U2Region& searchArea, int possibleOverlap, const QByteArray& sequence, bool isComplement);

    void run() override;

    const QList<U2Region>& getRegionBetweenIslands() const;

private:
    bool hasUnwantedConnections(const U2Region& region) const;
    QString regionToString(const U2Region& region) const;

    U2Region searchArea;
    int possibleOverlap = 0;
    QByteArray sequence;
    bool isComplement = false;

    QList<U2Region> regionsBetweenIslands;

    static constexpr int ISLAND_LENGTH = 8;
    static constexpr int NUCLEOTIDE_PAIR_LENGTH = 2;

    static constexpr double UNWANTED_MELTING_TEMPERATURE = 16;
    static constexpr double UNWANTED_DELTA_G = -7;
    static constexpr int UNWANTED_MAX_LENGTH = 4;
};


}

#endif
