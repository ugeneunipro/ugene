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

#ifndef _U2_DNASEQUENCE_OBJECT_TESTS_H_
#define _U2_DNASEQUENCE_OBJECT_TESTS_H_

#include <U2Test/XMLTestUtils.h>

#include <QDomElement>
#include <U2Core/DNAAlphabet.h>

namespace U2 {

class Document;

class GTest_DNASequenceSize : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequenceSize, "check-sequence-size");

    ReportResult report();

    QString objContextName;

    int     seqSize;

};

class GTest_DNASequenceAlphabet : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequenceAlphabet, "check-sequence-alphabet");

    ReportResult report();

    QString objContextName;

    QString name;
    QString alphabetId;
};

class GTest_DNASequencePart : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequencePart, "check-sequence-part");

    ReportResult report();

    QString     objContextName;

    QByteArray  subseq;
    int         startPos;
};

class GTest_DNASequenceAlphabetType : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequenceAlphabetType, "check-sequence-alphabet-by-type");

    ReportResult report();

    QString         objContextName;

    DNAAlphabetType alphabetType;
};

class GTest_DNASequenceAlphabetId : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequenceAlphabetId, "check-sequence-alphabet-by-id");

    ReportResult report();

    QString objContextName;

    QString alpId;
};


class GTest_DNASequenceQualityScores : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequenceQualityScores, "check-sequence-quality-scores");

    ReportResult report();

    QString     objContextName;
    QByteArray  qualityScores;
};

class GTest_DNASequenceQualityValue : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequenceQualityValue, "check-sequence-quality-value");

    ReportResult report();

    QString objContextName;
    int     pos;
    int     expectedVal;
};

class GTest_CompareDNASequenceQualityInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareDNASequenceQualityInTwoObjects, "compare-sequence-quality-in-two-objects");

    ReportResult report();

    QString doc1CtxName;
    QString doc2CtxName;
};

class GTest_DNAcompareSequencesNamesInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAcompareSequencesNamesInTwoObjects, "compare-sequences-names-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class GTest_DNAcompareSequencesInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAcompareSequencesInTwoObjects, "compare-sequences-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class GTest_DNAcompareSequencesAlphabetsInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAcompareSequencesAlphabetsInTwoObjects, "compare-sequences-alphabets-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class GTest_DNAMulSequenceAlphabetId : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAMulSequenceAlphabetId, "check-Msequence-alphabet-by-id");

    ReportResult report();

    QString objContextName;

    QString alpId;
};

class GTest_DNAMulSequenceSize : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAMulSequenceSize, "check-Msequence-size");

    ReportResult report();

    QString objContextName;

    int     seqSize;

};

class GTest_DNAMulSequencePart : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAMulSequencePart, "check-Msequence-part");

    ReportResult report();

    QString     objContextName;

    QByteArray  subseq;
    int         startPos;
    QString     seqName;

};

class GTest_DNAMulSequenceName : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAMulSequenceName, "check-Msequence-name");

    ReportResult report();

    QString     objContextName;

    QString     seqName;

};

class GTest_DNAMulSequenceQuality : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAMulSequenceQuality, "check-Msequence-quality");

    ReportResult report();

    QString     objContextName;

    QByteArray  expectedQuality;
    int         startPos;
    QString     seqName;

};


class GTest_DNASequencInMulSequence : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNASequencInMulSequence, "check-numbers-of-sequence-in-Msequence");

    ReportResult report();

    QString objContextName;

    int     seqInMSeq;

};
class GTest_DNAcompareMulSequencesInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAcompareMulSequencesInTwoObjects, "compare-Msequences-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
    QString sortValue;
};

class GTest_DNAcompareMulSequencesNamesInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAcompareMulSequencesNamesInTwoObjects, "compare-Msequences-names-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class GTest_DNAcompareMulSequencesAlphabetIdInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNAcompareMulSequencesAlphabetIdInTwoObjects, "compare-Msequences-alphabets-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};


class DNASequenceObjectTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}//namespace
#endif
