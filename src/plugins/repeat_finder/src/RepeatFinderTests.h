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

#ifndef _U2_REPEAT_FINDER_TESTS_H_
#define _U2_REPEAT_FINDER_TESTS_H_

#include "RFBase.h"

#include <U2Core/U2Region.h>

#include <U2Test/XMLTestUtils.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNASequence.h>
#include <U2Core/AppContext.h>
#include <U2Algorithm/BitsTable.h>
#include <U2Algorithm/RepeatFinderSettings.h>


#include <QDomElement>


namespace U2 {

class FindRepeatsTask;
class TandemFinder;

class GTest_FindSingleSequenceRepeatsTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_FindSingleSequenceRepeatsTask, "find-repeats", TaskFlags_FOSCOE);

    void prepare();
    void run();
    U2Region parseRegion(const QString& n, const QDomElement& el);

    QString     seq, seq2;
    RFAlgorithm alg;

    U2Region     region;

    int         w, c, minD, maxD;
    bool        inverted;
    bool        reflect;
    bool        filterNested;
    bool        filterUnique;
    RepeatsFilterAlgorithm	filter;
    QStringList excludeList;

    QString     resultFile;

};

class GTest_FindTandemRepeatsTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_FindTandemRepeatsTask, "find-tandems", TaskFlags_FOSCOE);

    void prepare();
    void run();
    U2Region parseRegion(const QString& n, const QDomElement& el);

    U2Region         region;

    char*           string;
    int             minD, maxD;
    int             minSize, maxSize, repeatCount;
    bool            inverted;
    bool            reflect;
    bool            filterNested;
    bool            filterUnique;
    RepeatsFilterAlgorithm	filter;
    DNASequence*    seqObj;
    QString         sequence;
    QString         results;

};

class GTest_FindRealTandemRepeatsTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_FindRealTandemRepeatsTask, "find-tandems-real", TaskFlags_FOSCOE);

    void prepare();
    void run();
    U2Region parseRegion(const QString& n, const QDomElement& el);

    U2Region         region;

    char*           string;
    int             minD, maxD;
    int             minSize, repeatCount;
    bool            inverted;
    bool            reflect;
    bool            filterNested;
    bool            filterUnique;
    RepeatsFilterAlgorithm	filter;
    DNASequence*    seqObj;
    QString         sequence;
    QString         results;

};

class SArrayIndex;
class SArrayBasedFindTask;

class GTest_SArrayBasedFindTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_SArrayBasedFindTask, "sarray-based-find", TaskFlags_FOSCOE);

    void prepare();
    void run();
    void cleanup();

    QString                 seqObjName;
    QString                 query;
    bool                    useBitMask;
    BitsTable               bt;
    int                     nMismatches;
    DNASequence*            seqObj;
    QByteArray              wholeSeq;
    SArrayIndex*            index;
    SArrayBasedFindTask*    findTask;
    QList<int>              expectedResults;
};

class RepeatFinderTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};


} //namespace
#endif

