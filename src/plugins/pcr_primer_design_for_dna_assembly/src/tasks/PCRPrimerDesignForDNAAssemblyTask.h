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

#ifndef _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_H_
#define _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_H_

#include <U2Core/Task.h>

#include "PCRPrimerDesignForDNAAssemblyTaskSettings.h"

namespace U2 {

class LoadDocumentTask;
class FindPresenceOfUnwantedParametersTask;
class FindUnwantedIslandsTask;

class PCRPrimerDesignForDNAAssemblyTask : public Task {
    Q_OBJECT
public:
    PCRPrimerDesignForDNAAssemblyTask(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings, const QByteArray& sequence);

    void prepare() override;

    void run() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    QString generateReport() const override;

    QList<U2Region> getResults() const;
    QByteArray getBackboneSequence() const;
    const PCRPrimerDesignForDNAAssemblyTaskSettings& getSettings() const;

    static const QStringList FRAGMENT_INDEX_TO_NAME;

private:
    enum class UserPrimer {
        Forward,
        Reverse
    };
    // Sequences for finding unwanted connections with user primers.
    enum class SeqToSearchInThem {
        ForwardUser,
        ReverseUser,
        Sequence,
        RevComplSeq,
        OtherSeq
    };

    QList<QByteArray> extractLoadedSequences(LoadDocumentTask* task);
    void findB1ReversePrimer(const QByteArray& b1ForwardCandidatePrimerSequence);
    enum class SecondaryPrimer {
        B2,
        B3
    };
    void findSecondaryForwardReversePrimers(SecondaryPrimer type);
    void findSecondaryReversePrimer(SecondaryPrimer type, const QByteArray& forwardCandidatePrimerSequence);
    bool areMetlingTempAndDeltaGood(const QByteArray& primer) const;
    bool hasUnwantedConnections(const QByteArray& primer) const;
    void updatePrimerRegion(int& primerEnd, int& primerLength) const;

    QString regionToString(const U2Region& region, bool isComplement) const;

    /**
     * Check user primers: if they aren't specified, write to log, otherwise find all unwanted connections and add them
     * to @userPrimersUnwantedConnections. Includes homodimers, heterodimers.
     */
    void saveUnwantedConnectionsReports();
    /**
     * Helper method for @saveUnwantedConnectionsReports. Find all unwanted connections in user primer and add them to
     * @userPrimersUnwantedConnections.
     */
    void saveUnwantedConnections(const QByteArray& primer, UserPrimer primerType);
    /**
     * Helper methods for @generateReport. Return html with result sequences for primer pair.
     * Called when there are no unwanted connections.
     */
    QString getPairReport(U2Region forward, U2Region reverse, const QString& primerName) const;
    QString getPairReportForUserPrimers() const;
    /**
     * Return html report of unwanted connections in user primers (summary table and each connection).
     */
    QString getUserPrimersUnwantedConnectionsReport() const;


    PCRPrimerDesignForDNAAssemblyTaskSettings settings;
    QByteArray sequence;
    QByteArray reverseComplementSequence;

    LoadDocumentTask* loadBackboneSequence = nullptr;
    LoadDocumentTask* loadOtherSequencesInPcr = nullptr;
    FindPresenceOfUnwantedParametersTask* checkBackboneSequence = nullptr;
    FindUnwantedIslandsTask* findUnwantedIslands = nullptr;
    FindUnwantedIslandsTask* findUnwantedIslandsReverseComplement = nullptr;

    QList<QByteArray> backboneSequencesCandidates;
    QList<QByteArray> otherSequencesInPcr;
    QByteArray backboneSequence;
    QList<U2Region> regionsBetweenIslandsForward;
    QList<U2Region> regionsBetweenIslandsReverse;
    // User primer and sequence analyzed for unwanted connections -> Reports of unwanted connections.
    QMap<QPair<UserPrimer, SeqToSearchInThem>, QStringList> userPrimersUnwantedConnections;

    //Results
    U2Region aForward;
    U2Region aReverse;
    U2Region b1Forward;
    U2Region b1Reverse;
    U2Region b2Forward;
    U2Region b2Reverse;
    U2Region b3Forward;
    U2Region b3Reverse;

    static constexpr int MINIMUM_LENGTH_BETWEEN_ISLANDS = 30;
    static constexpr int SECOND_PRIMER_OFFSET = 4;
};

}

#endif // _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_H_
