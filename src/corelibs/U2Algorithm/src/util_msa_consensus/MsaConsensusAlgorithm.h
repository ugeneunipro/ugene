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

#include <U2Core/Msa.h>
#include <U2Core/U2Region.h>

namespace U2 {

class MsaConsensusAlgorithm;
class DNAAlphabet;

enum ConsensusAlgorithmFlag {
    ConsensusAlgorithmFlag_Nucleic = 1 << 0,
    ConsensusAlgorithmFlag_Amino = 1 << 1,
    ConsensusAlgorithmFlag_Raw = 1 << 2,
    ConsensusAlgorithmFlag_SupportThreshold = 1 << 3,
    ConsensusAlgorithmFlag_AvailableForChromatogram = 1 << 4
};

Q_DECLARE_FLAGS(ConsensusAlgorithmFlags, ConsensusAlgorithmFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(ConsensusAlgorithmFlags)

#define ConsensusAlgorithmFlags_AllAlphabets (ConsensusAlgorithmFlags(ConsensusAlgorithmFlag_Nucleic) | ConsensusAlgorithmFlag_Amino | ConsensusAlgorithmFlag_Raw)
#define ConsensusAlgorithmFlags_NuclAmino (ConsensusAlgorithmFlags(ConsensusAlgorithmFlag_Nucleic) | ConsensusAlgorithmFlag_Amino)

class U2ALGORITHM_EXPORT MsaConsensusAlgorithmFactory : public QObject {
    Q_OBJECT
public:
    MsaConsensusAlgorithmFactory(const QString& algoId, ConsensusAlgorithmFlags flags);

    virtual MsaConsensusAlgorithm* createAlgorithm(const Msa& ma, bool ignoreTrailingLeadingGaps) = 0;

    QString getId() const;

    ConsensusAlgorithmFlags getFlags() const;

    QString getDescription() const;

    QString getName() const;

    bool supportsThreshold() const;

    int getMinThreshold() const;

    int getMaxThreshold() const;

    int getDefaultThreshold() const;

    QString getThresholdSuffix() const;

    bool isSequenceLikeResult() const;

    static ConsensusAlgorithmFlags getAlphabetFlags(const DNAAlphabet* al);

protected:
    QString algorithmId;
    ConsensusAlgorithmFlags flags;

    QString name;
    QString description;
    int minThreshold = 0;
    int maxThreshold = 0;
    int defaultThreshold = 0;
    QString thresholdSuffix = "";
    bool isSequenceLikeResultFlag = false;
};

class U2ALGORITHM_EXPORT MsaConsensusAlgorithm : public QObject {
    Q_OBJECT
public:
    MsaConsensusAlgorithm(MsaConsensusAlgorithmFactory* factory, bool ignoreTrailingLeadingGaps);
    MsaConsensusAlgorithm(const MsaConsensusAlgorithm& algorithm);

    /**
        Returns consensus char and score for the given row
        Score is a number: [0, num] sequences. Usually is means count of the char in the row
        Note that consensus character may be out of the to MSA alphabet symbols range
    */
    virtual char getConsensusCharAndScore(const Msa& ma, int column, int& score) const;

    virtual char getConsensusChar(const Msa& ma, int column) const = 0;

    virtual void reinitializeData(const Msa&) {}

    virtual MsaConsensusAlgorithm* clone() const = 0;

    QString getDescription() const;

    QString getName() const;

    void setThreshold(int val);

    int getThreshold() const;

    bool supportsThreshold() const;

    int getMinThreshold() const;

    int getMaxThreshold() const;

    int getDefaultThreshold() const;

    QString getThresholdSuffix() const;

    QString getId() const;

    MsaConsensusAlgorithmFactory* getFactory() const;

    static const char INVALID_CONS_CHAR;

signals:
    void si_thresholdChanged(int);

protected:
    /** Returns row indexes where `pos` is inside core area (not a part of the leading or trailing gap). */
    static QVector<int> pickRowsWithCharInCoreArea(const Msa& ma, int pos);

    /**
     * Returns rows that should be used to calculate the consensus.
     * If `ignoreTrailingAndLeadingGaps` is false always returns an empty list and consensus must be calculated using all rows.
     * If `ignoreTrailingAndLeadingGaps` is true and an empty list is returned - there are no rows to calculate the consensus.
     */
    QVector<int> pickRowsToUseInConsensus(const Msa& ma, int pos) const;

protected:
    MsaConsensusAlgorithmFactory* factory = nullptr;
    int threshold = 0;

    /**
     * TODO: this mode is used only for MCA.
     *
     * There is a better solution in this case: wrap an original MSA algorithm with
     * an MCA wrapper and remove this field from the original MSA algorithm.
     */
    bool ignoreTrailingAndLeadingGaps = false;

private:
    /**
     * Stub of the empty vector. Used to avoid new vector creation when only an empty value is needed.
     * In this case it is beneficial (from the performance POV) to create a shared copy of `emptyRowIdxStub`.
     */
    const QVector<int> emptyRowIdxStub;
};

}  // namespace U2
