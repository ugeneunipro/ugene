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

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

typedef QMap<QByteArray, qint64> DinucleotidesOccurrence;

class DNAAlphabet;
class U2SequenceObject;
class U2Region;

class U2VIEW_EXPORT DinuclOccurTask : public BackgroundTask<QMap<QByteArray, qint64>> {
public:
    DinuclOccurTask(const DNAAlphabet* alphabet, U2EntityRef seqRef, const QVector<U2Region>& regions);

    /**
     * Calculates the characters' pairs occurrence (in a different thread).
     * Assumes that the sequence consists of characters from the passed alphabet.
     */
    void run() override;

private:
    const DNAAlphabet* alphabet;
    U2EntityRef seqRef;
    QVector<U2Region> regions;

    // A region to analyze at a time
    static const qint64 REGION_TO_ANALAYZE = 1000000;
};

}  // namespace U2
