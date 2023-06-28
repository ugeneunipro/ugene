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

#include <U2Algorithm/TmCalculator.h>
#include <U2Algorithm/FindAlgorithmTask.h>

#include <U2Core/GObjectReference.h>
#include <U2Core/Task.h>

namespace U2 {

struct InSilicoPcrTaskSettings {
    InSilicoPcrTaskSettings() = default;

    QByteArray sequence;
    GObjectReference sequenceObject;
    bool isCircular = false;
    QByteArray forwardPrimer;
    QByteArray reversePrimer;
    uint forwardMismatches = 0;
    uint reverseMismatches = 0;
    uint maxProductSize = 0;
    uint perfectMatch = 0;
    bool useAmbiguousBases = true;
    QString sequenceName;
    QSharedPointer<TmCalculator> temperatureCalculator;

    static constexpr qint64 MAX_SEQUENCE_LENGTH = 500 * 1024 * 1024;  // 500 Mb;
};

struct InSilicoPcrProduct {
    InSilicoPcrProduct();

    bool operator==(const InSilicoPcrProduct& anotherProduct) const;
    bool isValid() const;

    /* Region within the original sequence */
    U2Region region;
    /* Annealing temperature */
    double ta;
    /* Primer sequence */
    QByteArray forwardPrimer;
    /* Primer sequence */
    QByteArray reversePrimer;
    /* The length of found primer region within the original sequence */
    int forwardPrimerMatchLength;
    /* The length of found primer region within the original sequence */
    int reversePrimerMatchLength;
    /**/
    QByteArray forwardPrimerLedge;
    /**/
    QByteArray reversePrimerLedge;
};

class InSilicoPcrTask : public Task {
    Q_OBJECT
public:
    InSilicoPcrTask(InSilicoPcrTaskSettings* settings);
    ~InSilicoPcrTask();

    // Task
    void prepare();
    void run();
    QString generateReport() const;

    const QList<InSilicoPcrProduct>& getResults() const;
    const InSilicoPcrTaskSettings* getSettings() const;

private:
    struct PrimerBind {
        PrimerBind();

        QByteArray primer;
        uint mismatches;
        U2Region region;
        int ledge;
    };
    PrimerBind getPrimerBind(const FindAlgorithmResult& forward, const FindAlgorithmResult& reverse, U2Strand::Direction direction) const;

    qint64 getProductSize(const PrimerBind& leftBind, const PrimerBind& rightBind) const;
    FindAlgorithmTaskSettings getFindPatternSettings(U2Strand::Direction direction);
    bool isCorrectProductSize(qint64 productSize, qint64 minPrimerSize) const;
    bool isProductAcceptable(const PrimerBind& leftBind, const PrimerBind& rightBind, const U2Region& product) const;
    bool checkPerfectMatch(const PrimerBind& bind, U2Strand::Direction direction) const;
    QByteArray getSequence(const U2Region& region, U2Strand::Direction direction) const;
    void createAndAddResult(const PrimerBind& leftPrimer, const U2Region& product, const PrimerBind& rightPrimer, U2Strand::Direction direction);
    bool updateSequenceByPrimers(const PrimerBind& leftPrimer, const PrimerBind& rightPrimer, QByteArray& productSequence) const;
    void updateSequenceByPrimer(const PrimerBind& primer, QByteArray& productSequence) const;

private:
    // it takes ~6 minutes to process matrix 50x50 results with default algorithm settings in release build
    static constexpr int MAX_RESULTS_FOR_PRIMERS_PER_STRAND = 50;

    const InSilicoPcrTaskSettings* settings = nullptr;
    FindAlgorithmTask* forwardSearch;
    FindAlgorithmTask* reverseSearch;
    QList<InSilicoPcrProduct> results;
    int minProductSize;
};

}  // namespace U2

Q_DECLARE_METATYPE(U2::InSilicoPcrProduct)
