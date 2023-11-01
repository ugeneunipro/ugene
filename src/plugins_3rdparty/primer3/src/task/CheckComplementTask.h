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

#include <U2Core/Task.h>

#include "Primer3TaskSettings.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/PrimerDimersFinder.h>

#include <QList>
#include <QPointer>

namespace U2 {

class PrimerSingle;
class PrimerPair;

/**
 * This class represents a task, which does additional Primer3 calculations -
 * filtering Primer3 primer pairs depending on self- and hetero-dimers
 * they produce.
 */
class CheckComplementTask : public Task {
    Q_OBJECT
public:
    CheckComplementTask(const CheckComplementSettings& settings, const QList<QSharedPointer<PrimerPair>>& results, U2SequenceObject* seqObj);

    void run() override;
    QString generateReport() const override;
    ReportResult report() override;

    /**
     * \return Returns a list of PrimerPair pointers
     */
    QList<QSharedPointer<PrimerPair>> getFilteredPrimers() const;

private:
    QByteArray getPrimerSequence(QSharedPointer<PrimerSingle> primer) const;

    bool isBasePairNumberBad(const DimerFinderResult& dimer) const;
    bool isGcContentBad(const DimerFinderResult& dimer) const;
    QString getBasePairsCountString(const DimerFinderResult& dimer) const;
    QString getGcContentString(const DimerFinderResult& dimer) const;
    static int getGcPairsCount(const QString& dimer);
    static double getGcContent(const QString& dimer);

    const CheckComplementSettings& settings;
    QList<QSharedPointer<PrimerPair>> results;
    QPointer<U2SequenceObject> seqObj;

    struct PrimerPairData {
        QSharedPointer<PrimerPair> primerPair;
        QString leftPrimerSequence;
        QString rightPrimerSequence;
        DimerFinderResult leftPrimerSelfDimer;
        DimerFinderResult rightPrimerSelfDimer;
        DimerFinderResult heteroDimer;
        bool filtered = false;
    };

    QList<PrimerPairData> primers;

    static constexpr int MINIMUN_G_AND_C_NUMBER_FOR_BAD_DIMER = 2;
    static constexpr const char* CORRECT_PRIMER_PAIR_COLOR = "lightgreen";
    static constexpr const char* FILTERED_PRIMER_PAIR_COLOR = "lightsalmon";
};


}
