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

#include <U2Core/DocumentModel.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/DNASequenceObject.h>

#include <U2Formats/GenbankLocationParser.h>

#include "DnaStatisticsTests.h"

namespace U2 {

const QString GTest_DnaStatisticsTest::DOC_NAME_ATTR = "doc-name";
const QString GTest_DnaStatisticsTest::SEQ_NAME_ATTR = "seq-name";
const QString GTest_DnaStatisticsTest::REGIONS_ATTR = "regions";

const QString GTest_DnaStatisticsTest::EXPECTED_LENGTH = "expected-length";
const QString GTest_DnaStatisticsTest::EXPECTED_GC_CONTENT = "expected-gc-content";
const QString GTest_DnaStatisticsTest::EXPECTED_MELTING_TEMP = "expected-melting-temp";

const QString GTest_DnaStatisticsTest::EXPECTED_SS_MOLECULAR_WEIGHT = "expected-ss-melecular-weight";
const QString GTest_DnaStatisticsTest::EXPECTED_SS_EXTINCTION_COEFFICIENT = "expected-ss-extinction-coefficient";
const QString GTest_DnaStatisticsTest::EXPECTED_SS_OD260_AMOUNT_OF_SUBSTANCE = "expected-ss-od260-amount-of-substance";
const QString GTest_DnaStatisticsTest::EXPECTED_SS_OD260_MASS = "expected-ss-od260-mass";

const QString GTest_DnaStatisticsTest::EXPECTED_DS_MOLECULAR_WEIGHT = "expected-ds-melecular-weight";
const QString GTest_DnaStatisticsTest::EXPECTED_DS_EXTINCTION_COEFFICIENT = "expected-ds-extinction-coefficient";
const QString GTest_DnaStatisticsTest::EXPECTED_DS_OD260_AMOUNT_OF_SUBSTANCE = "expected-ds-od260-amount-of-substance";
const QString GTest_DnaStatisticsTest::EXPECTED_DS_OD260_MASS = "expected-ds-od260-mass";

const QString GTest_DnaStatisticsTest::EXPECTED_ISOELECTRIC_POINT = "expected-isoelectric-point";

void GTest_DnaStatisticsTest::init(XMLTestFormat *, const QDomElement& element) {
    task = nullptr;

    checkNecessaryAttributeExistence(element, DOC_NAME_ATTR);
    CHECK_OP(stateInfo, );
    docName = element.attribute(DOC_NAME_ATTR);

    checkNecessaryAttributeExistence(element, SEQ_NAME_ATTR);
    CHECK_OP(stateInfo, );
    seqName = element.attribute(SEQ_NAME_ATTR);

    checkNecessaryAttributeExistence(element, REGIONS_ATTR);
    CHECK_OP(stateInfo, );
    const QString regionsString = element.attribute(REGIONS_ATTR);
    CHECK_EXT(!regionsString.isEmpty(), emptyValue(REGIONS_ATTR), );
    U2Location location;
    Genbank::LocationParser::parseLocation(regionsString.toLatin1().constData(), regionsString.length(), location);
    regions = location->regions;
    CHECK_EXT(!regions.isEmpty(), setError(QString("Can't convert string '%1' to regions list").arg(regionsString)), );

    if (element.hasAttribute(EXPECTED_LENGTH)) {
        expectedStats.length = getInt64(element, EXPECTED_LENGTH);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_GC_CONTENT)) {
        expectedStats.gcContent = getDouble(element, EXPECTED_GC_CONTENT);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_MELTING_TEMP)) {
        expectedStats.meltingTemp = getDouble(element, EXPECTED_MELTING_TEMP);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_SS_MOLECULAR_WEIGHT)) {
        expectedStats.ssMolecularWeight = getDouble(element, EXPECTED_SS_MOLECULAR_WEIGHT);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_SS_EXTINCTION_COEFFICIENT)) {
        expectedStats.ssExtinctionCoefficient = getInt64(element, EXPECTED_SS_EXTINCTION_COEFFICIENT);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_SS_OD260_AMOUNT_OF_SUBSTANCE)) {
        expectedStats.ssOd260AmountOfSubstance = getDouble(element, EXPECTED_SS_OD260_AMOUNT_OF_SUBSTANCE);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_SS_OD260_MASS)) {
        expectedStats.ssOd260Mass = getDouble(element, EXPECTED_SS_OD260_MASS);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_DS_MOLECULAR_WEIGHT)) {
        expectedStats.dsMolecularWeight = getDouble(element, EXPECTED_DS_MOLECULAR_WEIGHT);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_DS_EXTINCTION_COEFFICIENT)) {
        expectedStats.dsExtinctionCoefficient = getInt64(element, EXPECTED_DS_EXTINCTION_COEFFICIENT);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_DS_OD260_AMOUNT_OF_SUBSTANCE)) {
        expectedStats.dsOd260AmountOfSubstance = getDouble(element, EXPECTED_DS_OD260_AMOUNT_OF_SUBSTANCE);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_DS_OD260_MASS)) {
        expectedStats.dsOd260Mass = getDouble(element, EXPECTED_DS_OD260_MASS);
        CHECK_OP(stateInfo, );
    }

    if (element.hasAttribute(EXPECTED_ISOELECTRIC_POINT)) {
        expectedStats.isoelectricPoint = getDouble(element, EXPECTED_ISOELECTRIC_POINT);
        CHECK_OP(stateInfo, );
    }
}

void GTest_DnaStatisticsTest::prepare() {
    Document *loadedDocument = getContext<Document>(this, docName);
    CHECK_EXT(nullptr != loadedDocument, setError(QString("Document not found in context: %1").arg(docName)), );

    U2SequenceObject *sequenceObject = qobject_cast<U2SequenceObject *>(loadedDocument->findGObjectByName(seqName));
    CHECK_EXT(nullptr != sequenceObject, setError(QString("Sequence object '%1' not found in document '%2'").arg(seqName).arg(docName)), );

    task = new DNAStatisticsTask(sequenceObject->getAlphabet(), sequenceObject->getEntityRef(), regions);
    addSubTask(task);
}

Task::ReportResult GTest_DnaStatisticsTest::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    const DNAStatistics result = task->getResult();
    if (0 != expectedStats.length) {
        CHECK_EXT(expectedStats.length == result.length,
                  setError(QString("Unexpected length: expected %1, got '%2'")
                           .arg(expectedStats.length).arg(result.length)), ReportResult_Finished);
    }
    if (0 != expectedStats.gcContent) {
        CHECK_EXT(qFuzzyCompare(expectedStats.gcContent + 1, result.gcContent + 1),
                  setError(QString("Unexpected GC content: expected %1, got '%2'")
                           .arg(expectedStats.gcContent).arg(result.gcContent)), ReportResult_Finished);
    }
    if (0 != expectedStats.meltingTemp) {
        CHECK_EXT(qFuzzyCompare(expectedStats.meltingTemp + 1, result.meltingTemp + 1),
                  setError(QString("Unexpected melting temperature: expected %1, got '%2'")
                           .arg(expectedStats.meltingTemp).arg(result.meltingTemp)), ReportResult_Finished);
    }
    if (0 != expectedStats.ssMolecularWeight) {
        CHECK_EXT(qFuzzyCompare(expectedStats.ssMolecularWeight + 1, result.ssMolecularWeight + 1),
                  setError(QString("Unexpected single strand molecular weight: expected %1, got '%2'")
                           .arg(expectedStats.ssMolecularWeight).arg(result.ssMolecularWeight)), ReportResult_Finished);
    }
    if (0 != expectedStats.ssExtinctionCoefficient) {
        CHECK_EXT(expectedStats.ssExtinctionCoefficient == result.ssExtinctionCoefficient,
                  setError(QString("Unexpected single strand extinction coefficient: expected %1, got '%2'")
                           .arg(expectedStats.ssExtinctionCoefficient).arg(result.ssExtinctionCoefficient)), ReportResult_Finished);
    }
    if (0 != expectedStats.ssOd260AmountOfSubstance) {
        CHECK_EXT(qFuzzyCompare(expectedStats.ssOd260AmountOfSubstance + 1, result.ssOd260AmountOfSubstance + 1),
                  setError(QString("Unexpected single strand amount of OD260: expected %1, got '%2'")
                           .arg(expectedStats.ssOd260AmountOfSubstance).arg(result.ssOd260AmountOfSubstance)), ReportResult_Finished);
    }
    if (0 != expectedStats.ssOd260Mass) {
        CHECK_EXT(qFuzzyCompare(expectedStats.ssOd260Mass + 1, result.ssOd260Mass + 1),
                  setError(QString("Unexpected single strand OD260 mass: expected %1, got '%2'")
                           .arg(expectedStats.ssOd260Mass).arg(result.ssOd260Mass)), ReportResult_Finished);
    }
    if (0 != expectedStats.dsMolecularWeight) {
        CHECK_EXT(qFuzzyCompare(expectedStats.dsMolecularWeight + 1, result.dsMolecularWeight + 1),
                  setError(QString("Unexpected double strand molecular weight: expected %1, got '%2'")
                           .arg(expectedStats.dsMolecularWeight).arg(result.dsMolecularWeight)), ReportResult_Finished);
    }
    if (0 != expectedStats.dsExtinctionCoefficient) {
        CHECK_EXT(expectedStats.dsExtinctionCoefficient == result.dsExtinctionCoefficient,
                  setError(QString("Unexpected double strand extinction coefficient: expected %1, got '%2'")
                           .arg(expectedStats.dsExtinctionCoefficient).arg(result.dsExtinctionCoefficient)), ReportResult_Finished);
    }
    if (0 != expectedStats.dsOd260AmountOfSubstance) {
        CHECK_EXT(qFuzzyCompare(expectedStats.dsOd260AmountOfSubstance + 1, result.dsOd260AmountOfSubstance + 1),
                  setError(QString("Unexpected double strand amount of OD260: expected %1, got '%2'")
                           .arg(expectedStats.dsOd260AmountOfSubstance).arg(result.dsOd260AmountOfSubstance)), ReportResult_Finished);
    }
    if (0 != expectedStats.dsOd260Mass) {
        CHECK_EXT(qFuzzyCompare(expectedStats.dsOd260Mass + 1, result.dsOd260Mass + 1),
                  setError(QString("Unexpected double strand OD260 mass: expected %1, got '%2'")
                           .arg(expectedStats.dsOd260Mass).arg(result.dsOd260Mass)), ReportResult_Finished);
    }
    if (0 != expectedStats.isoelectricPoint) {
        CHECK_EXT(qFuzzyCompare(expectedStats.isoelectricPoint + 1, result.isoelectricPoint + 1),
                  setError(QString("Unexpected isoelectric point: expected %1, got '%2'")
                           .arg(expectedStats.isoelectricPoint).arg(result.isoelectricPoint)), ReportResult_Finished);
    }
    return ReportResult_Finished;
}

QList<XMLTestFactory *> DnaStatisticsTests::createTestFactories() {
    return {GTest_DnaStatisticsTest::createFactory()};
}

} // namespace U2
