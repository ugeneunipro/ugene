/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_DISTANCE_ALGORITHM_H_
#define _U2_MSA_DISTANCE_ALGORITHM_H_

#include <QMutex>
#include <QVarLengthArray>
#include <QVector>

#include <U2Core/AppResources.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>

namespace U2 {

class DNAAlphabet;
class MSADistanceAlgorithm;
class MSADistanceMatrix;

enum DistanceAlgorithmFlag {
    DistanceAlgorithmFlag_Nucleic = 1 << 0,
    DistanceAlgorithmFlag_Amino = 1 << 1,
    DistanceAlgorithmFlag_Raw = 1 << 2,
    DistanceAlgorithmFlag_ExcludeGaps = 1 << 3
};

typedef QFlags<DistanceAlgorithmFlag> DistanceAlgorithmFlags;
#define DistanceAlgorithmFlags_AllAlphabets (DistanceAlgorithmFlags(DistanceAlgorithmFlag_Nucleic) | DistanceAlgorithmFlag_Amino | DistanceAlgorithmFlag_Raw)
#define DistanceAlgorithmFlags_NuclAmino    (DistanceAlgorithmFlags(DistanceAlgorithmFlag_Nucleic) | DistanceAlgorithmFlag_Amino)

class U2ALGORITHM_EXPORT MSADistanceAlgorithmFactory : public QObject {
    Q_OBJECT
public:
    MSADistanceAlgorithmFactory(const QString& algoId, DistanceAlgorithmFlags flags, QObject* p = NULL);

    virtual MSADistanceAlgorithm* createAlgorithm(const MultipleSequenceAlignment& ma, QObject* parent = NULL) = 0;

    QString getId() const {return algorithmId;}

    DistanceAlgorithmFlags getFlags() const {return flags;}

    void setFlag(DistanceAlgorithmFlag flag);
    void resetFlag(DistanceAlgorithmFlag flag);

    virtual QString getDescription() const = 0;

    virtual QString getName() const = 0;

    // utility method
    static DistanceAlgorithmFlags getAphabetFlags(const DNAAlphabet* al);

protected:
    QString                 algorithmId;
    DistanceAlgorithmFlags  flags;

};

typedef QVarLengthArray<QVarLengthArray<int> > varLengthMatrix;

class U2ALGORITHM_EXPORT MSADistanceMatrix {
    friend class MSADistanceAlgorithm;
private:
    MSADistanceMatrix();
    MSADistanceMatrix(const MultipleSequenceAlignment& ma, bool _excludeGaps, bool _usePercents);

public:
    bool isEmpty(){ return table.isEmpty(); }
    int getSimilarity(int row1, int row2) const;
    int getSimilarity(int row1, int row2, bool _usePercents) const;
    void setPercentSimilarity(bool _usePercents) { usePercents = _usePercents; }
    bool isPercentSimilarity() { return usePercents; }

protected:
    varLengthMatrix                             table;
    bool                                        usePercents;
    bool                                        excludeGaps;
    QVector<int>                                seqsUngappedLenghts;
    int                                         alignmentLength;
};

class U2ALGORITHM_EXPORT MSADistanceAlgorithm : public Task {
    Q_OBJECT

public:
    MSADistanceAlgorithm(MSADistanceAlgorithmFactory* factory, const MultipleSequenceAlignment& ma);

    int getSimilarity(int row1, int row2, bool usePercents);

    const MSADistanceMatrix& getMatrix() const;

    virtual QString getDescription() const {return factory->getDescription();}

    virtual QString getName() const {return factory->getName();}

    QString getId() const {return factory->getId();}

    bool isSimilarityMeasure() const {return isSimilarity;}

    void setExcludeGaps(bool _excludeGaps);

    MSADistanceAlgorithmFactory* getFactory() const {return factory;}

    bool getExcludeGapsFlag() const {return excludeGaps;}

    void setDistanceValue(int row1, int row2, int distance);

private:
    MSADistanceMatrix            distanceMatrix;
    MSADistanceAlgorithmFactory* factory;
    MemoryLocker                 memoryLocker;

protected:
    virtual void fillTable();
    virtual int calculateSimilarity(int , int ){return 0;}
    MultipleSequenceAlignment                   ma;
    mutable QMutex                              lock;
    bool                                        excludeGaps;
    bool                                        isSimilarity;
};

}//namespace

#endif
