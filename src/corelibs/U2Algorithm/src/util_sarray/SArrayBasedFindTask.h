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

#include <QMutex>

#include <U2Algorithm/BitsTable.h>

#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class SArrayIndex;

#define MAX_PERCENTAGE 100

struct U2ALGORITHM_EXPORT SArrayBasedSearchSettings {
    SArrayBasedSearchSettings()
        : query(""), useBitMask(false), unknownChar('\0'),
          bitMask(nullptr), ptMismatches(0), nMismatches(0), absMismatches(true) {
    }
    QByteArray query;
    bool useBitMask;
    char unknownChar;
    quint32 bitMaskCharBitsNum;
    const quint32* bitMask;
    int ptMismatches;
    int nMismatches;
    bool absMismatches;
};

class U2ALGORITHM_EXPORT SArrayBasedFindTask : public Task {
    Q_OBJECT
public:
    SArrayBasedFindTask(SArrayIndex* i, const SArrayBasedSearchSettings& s, bool onlyFirstMatch = false);
    void run() override;
    void cleanup() override;
    const QList<int>& getResults() const {
        return results;
    }
    const QByteArray& getQuery() const {
        return config->query;
    }

private:
    void runSearch();
    void runSearchWithMismatches();
    SArrayIndex* index;
    SArrayBasedSearchSettings* config;
    QList<int> results;
    QMutex lock;
    bool onlyFirstMatch;
};

}  // namespace U2
