/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "DnaStatisticsTests.h"

#include <atomic>
#include <cmath>
#include <future>
#include <thread>
#include <utility>
#include <vector>

#include <U2Algorithm/RoughTmCalculatorCmdFactory.h>

#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/GenbankLocationParser.h>

namespace U2 {

const QString GTest_DnaStatisticsTest::DOC_NAME_ATTR = "doc-name";
const QString GTest_DnaStatisticsTest::SEQ_NAME_ATTR = "seq-name";
const QString GTest_DnaStatisticsTest::SEQ_ATTR = "seq";
const QString GTest_DnaStatisticsTest::REGIONS_ATTR = "regions";
const QString GTest_DnaStatisticsTest::TEMPERATURE_CALCULATION_ATTR = "temperature-calculation";

const QString GTest_DnaStatisticsTest::EXPECTED_LENGTH = "expected-length";
const QString GTest_DnaStatisticsTest::EXPECTED_GC_CONTENT = "expected-gc-content";
const QString GTest_DnaStatisticsTest::EXPECTED_MELTING_TEMP = "expected-melting-temp";

const QString GTest_DnaStatisticsTest::EXPECTED_SS_MOLECULAR_WEIGHT = "expected-ss-molecular-weight";
const QString GTest_DnaStatisticsTest::EXPECTED_SS_EXTINCTION_COEFFICIENT = "expected-ss-extinction-coefficient";
const QString GTest_DnaStatisticsTest::EXPECTED_SS_OD260_AMOUNT_OF_SUBSTANCE = "expected-ss-od260-amount-of-substance";
const QString GTest_DnaStatisticsTest::EXPECTED_SS_OD260_MASS = "expected-ss-od260-mass";

const QString GTest_DnaStatisticsTest::EXPECTED_DS_MOLECULAR_WEIGHT = "expected-ds-molecular-weight";
const QString GTest_DnaStatisticsTest::EXPECTED_DS_EXTINCTION_COEFFICIENT = "expected-ds-extinction-coefficient";
const QString GTest_DnaStatisticsTest::EXPECTED_DS_OD260_AMOUNT_OF_SUBSTANCE = "expected-ds-od260-amount-of-substance";
const QString GTest_DnaStatisticsTest::EXPECTED_DS_OD260_MASS = "expected-ds-od260-mass";

const QString GTest_DnaStatisticsTest::EXPECTED_ISOELECTRIC_POINT = "expected-isoelectric-point";

void GTest_DnaStatisticsTest::init(XMLTestFormat*, const QDomElement& element) {
    task = nullptr;

    if (element.hasAttribute(DOC_NAME_ATTR) && element.hasAttribute(SEQ_NAME_ATTR)) {
        CHECK_OP(stateInfo, );
        docName = element.attribute(DOC_NAME_ATTR);

        CHECK_OP(stateInfo, );
        seqName = element.attribute(SEQ_NAME_ATTR);
    } else if (element.hasAttribute(SEQ_ATTR)) {
        CHECK_OP(stateInfo, );
        seq = element.attribute(SEQ_ATTR);
        seqName = getTaskName();
    } else {
        setError("No sequence");
        return;
    }

    if (element.hasAttribute(REGIONS_ATTR)) {
        const QString regionsString = element.attribute(REGIONS_ATTR);
        CHECK_EXT(!regionsString.isEmpty(), emptyValue(REGIONS_ATTR), );

        U2Location location;
        Genbank::LocationParser::parseLocation(regionsString.toLatin1().constData(), regionsString.length(), location);
        regions = location->regions;
        CHECK_EXT(!regions.isEmpty(), setError(QString("Can't convert string '%1' to regions list").arg(regionsString)), );
    }

    {
        checkNecessaryAttributeExistence(element, TEMPERATURE_CALCULATION_ATTR);
        CHECK_OP(stateInfo, );

        const QString temperatureCalculationSting = element.attribute(TEMPERATURE_CALCULATION_ATTR);
        CHECK_EXT(!temperatureCalculationSting.isEmpty(), emptyValue(TEMPERATURE_CALCULATION_ATTR), );

        auto settingsStringList = temperatureCalculationSting.split(";");
        QVariantMap resultMap;
        for (const auto& setting : qAsConst(settingsStringList)) {
            auto keyAndValue = setting.split("=");
            CHECK_EXT(keyAndValue.size() == 2, setError(QString("Incorrect value %1").arg(setting)), );

            resultMap.insert(keyAndValue.first(), keyAndValue.last());
        }
        CHECK_EXT(!resultMap.isEmpty(), setError("No temperature settings"), );
        CHECK_EXT(resultMap.keys().contains(TmCalculator::KEY_ID), setError("No ID were set"), );

        temperatureCalculator = AppContext::getTmCalculatorRegistry()->createTmCalculator(resultMap);
        CHECK_EXT(temperatureCalculator != nullptr, setError("Can't set temperature settings: " + resultMap.value("id").toString()), );
    }

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
    U2SequenceObject* sequenceObject = nullptr;
    if (!docName.isEmpty()) {
        auto loadedDocument = getContext<Document>(this, docName);
        CHECK_EXT(loadedDocument != nullptr, setError(QString("Document not found in context: %1").arg(docName)), );

        sequenceObject = qobject_cast<U2SequenceObject*>(loadedDocument->findGObjectByName(seqName));
        CHECK_EXT(sequenceObject != nullptr, setError(QString("Sequence object '%1' not found in document '%2'").arg(seqName).arg(docName)), );
    } else if (!seq.isEmpty()) {
        U2OpStatus2Log os;
        auto sessionDbi = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
        CHECK_OP_EXT(os, setError(os.getError()), );

        DNASequence dnaSequence(seqName, seq.toLocal8Bit(), AppContext::getDNAAlphabetRegistry()->findById(U2::BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()));
        const U2::U2EntityRef sequenceRef = U2SequenceUtils::import(os, sessionDbi, dnaSequence, dnaSequence.alphabet->getId());
        CHECK_OP_EXT(os, setError(os.getError()), );

        sequenceObject = new U2SequenceObject(dnaSequence.getName(), sequenceRef);
    }

    if (regions.isEmpty()) {
        U2Region reg(0, sequenceObject->getSequenceLength());
        regions << reg;
    }

    task = new DNAStatisticsTask(sequenceObject->getAlphabet(), sequenceObject->getEntityRef(), regions, temperatureCalculator);
    addSubTask(task);
}

bool checkDoubleProperty(double value, double expectedValue) {
    return qAbs(value - expectedValue) <= qMax(qAbs(value / 1000), 0.005);
}

Task::ReportResult GTest_DnaStatisticsTest::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    const DNAStatistics result = task->getResult();
    if (0 != expectedStats.length) {
        CHECK_EXT(expectedStats.length == result.length,
                  setError(QString("Unexpected length: expected %1, got '%2'")
                               .arg(expectedStats.length)
                               .arg(result.length)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.gcContent) {
        CHECK_EXT(checkDoubleProperty(expectedStats.gcContent, result.gcContent),
                  setError(QString("Unexpected GC content: expected %1, got '%2'")
                               .arg(expectedStats.gcContent)
                               .arg(result.gcContent)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.meltingTemp) {
        CHECK_EXT(checkDoubleProperty(expectedStats.meltingTemp, result.meltingTemp),
                  setError(QString("Unexpected melting temperature: expected %1, got '%2'")
                               .arg(expectedStats.meltingTemp)
                               .arg(result.meltingTemp)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.ssMolecularWeight) {
        CHECK_EXT(checkDoubleProperty(expectedStats.ssMolecularWeight, result.ssMolecularWeight),
                  setError(QString("Unexpected single strand molecular weight: expected %1, got '%2'")
                               .arg(expectedStats.ssMolecularWeight)
                               .arg(result.ssMolecularWeight)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.ssExtinctionCoefficient) {
        CHECK_EXT(expectedStats.ssExtinctionCoefficient == result.ssExtinctionCoefficient,
                  setError(QString("Unexpected single strand extinction coefficient: expected %1, got '%2'")
                               .arg(expectedStats.ssExtinctionCoefficient)
                               .arg(result.ssExtinctionCoefficient)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.ssOd260AmountOfSubstance) {
        CHECK_EXT(checkDoubleProperty(expectedStats.ssOd260AmountOfSubstance, result.ssOd260AmountOfSubstance),
                  setError(QString("Unexpected single strand amount of OD260: expected %1, got '%2'")
                               .arg(expectedStats.ssOd260AmountOfSubstance)
                               .arg(result.ssOd260AmountOfSubstance)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.ssOd260Mass) {
        CHECK_EXT(checkDoubleProperty(expectedStats.ssOd260Mass, result.ssOd260Mass),
                  setError(QString("Unexpected single strand OD260 mass: expected %1, got '%2'")
                               .arg(expectedStats.ssOd260Mass)
                               .arg(result.ssOd260Mass)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.dsMolecularWeight) {
        CHECK_EXT(checkDoubleProperty(expectedStats.dsMolecularWeight, result.dsMolecularWeight),
                  setError(QString("Unexpected double strand molecular weight: expected %1, got '%2'")
                               .arg(expectedStats.dsMolecularWeight)
                               .arg(result.dsMolecularWeight)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.dsExtinctionCoefficient) {
        CHECK_EXT(expectedStats.dsExtinctionCoefficient == result.dsExtinctionCoefficient,
                  setError(QString("Unexpected double strand extinction coefficient: expected %1, got '%2'")
                               .arg(expectedStats.dsExtinctionCoefficient)
                               .arg(result.dsExtinctionCoefficient)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.dsOd260AmountOfSubstance) {
        CHECK_EXT(checkDoubleProperty(expectedStats.dsOd260AmountOfSubstance, result.dsOd260AmountOfSubstance),
                  setError(QString("Unexpected double strand amount of OD260: expected %1, got '%2'")
                               .arg(expectedStats.dsOd260AmountOfSubstance)
                               .arg(result.dsOd260AmountOfSubstance)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.dsOd260Mass) {
        CHECK_EXT(checkDoubleProperty(expectedStats.dsOd260Mass, result.dsOd260Mass),
                  setError(QString("Unexpected double strand OD260 mass: expected %1, got '%2'")
                               .arg(expectedStats.dsOd260Mass)
                               .arg(result.dsOd260Mass)),
                  ReportResult_Finished);
    }
    if (0 != expectedStats.isoelectricPoint) {
        CHECK_EXT(checkDoubleProperty(expectedStats.isoelectricPoint, result.isoelectricPoint),
                  setError(QString("Unexpected isoelectric point: expected %1, got '%2'")
                               .arg(expectedStats.isoelectricPoint)
                               .arg(result.isoelectricPoint)),
                  ReportResult_Finished);
    }
    return ReportResult_Finished;
}

namespace {

class CacheProbeTmCalculator : public TmCalculator {
public:
    explicit CacheProbeTmCalculator(int offset = 0) : TmCalculator({{"offset", offset}}) {
    }

    std::atomic<int> calls{0};

protected:
    double calculateMeltingTemperature(const QByteArray& sequence) override {
        ++calls;
        return sequence.isEmpty() ? INVALID_TM : sequence.size() + sequence.count('G') + settings.value("offset").toInt();
    }
};

QString testTemperatureCache() {
    const QByteArray primer("ACGTCAGTACGATCGTACGA");
    CacheProbeTmCalculator calculator;
    const double temperature = calculator.getMeltingTemperature(primer);
    if (calculator.getMeltingTemperature(primer) != temperature || calculator.calls != 1) {
        return "Repeated melting-temperature calculation was not cached";
    }
    CacheProbeTmCalculator differentSettings(10);
    if (differentSettings.getMeltingTemperature(primer) != temperature + 10 || differentSettings.calls != 1) {
        return "Temperature cache leaked across calculator settings";
    }
    calculator.getMeltingTemperature({});
    if (calculator.getMeltingTemperature({}) != TmCalculator::INVALID_TM || calculator.calls != 2) {
        return "Cached invalid temperature changed the result";
    }

    const QByteArray oversized(100000, 'A');
    calculator.getMeltingTemperature(oversized);
    const int calls = calculator.calls;
    if (calculator.getMeltingTemperature(oversized) != 100000 || calculator.calls != calls + 1 ||
        calculator.getMeltingTemperature(primer) != temperature || calculator.calls != calls + 1) {
        return "Oversized sequence was cached or evicted reusable entries";
    }

    char buffer[] = "GGGGGGGGGGGGGGGGGGGG";
    const QByteArray original(buffer);
    const double rawTemperature = calculator.getMeltingTemperature(QByteArray::fromRawData(buffer, sizeof(buffer) - 1));
    buffer[0] = 'A';
    const int rawCalls = calculator.calls;
    if (calculator.getMeltingTemperature(original) != rawTemperature || calculator.calls != rawCalls) {
        return "Temperature cache retained externally owned sequence data";
    }

    CacheProbeTmCalculator bounded;
    bounded.getMeltingTemperature(primer);
    QByteArray first;
    for (int i = 0; i < 2000; ++i) {
        QByteArray sequence(1024, 'A');
        for (int j = 0; j < 16; ++j) {
            sequence[j] = "ACGT"[(i >> (2 * j)) & 3];
        }
        if (i == 0) {
            first = sequence;
        }
        bounded.getMeltingTemperature(sequence);
        bounded.getMeltingTemperature(primer);
    }
    const int boundedCalls = bounded.calls;
    bounded.getMeltingTemperature(primer);
    if (bounded.calls != boundedCalls) {
        return "LRU cache evicted a frequently reused primer";
    }
    bounded.getMeltingTemperature(first);
    if (bounded.calls != boundedCalls + 1) {
        return "Temperature cache did not evict old sequences";
    }

    CacheProbeTmCalculator shared;
    std::vector<std::future<bool>> workers;
    for (int worker = 0; worker < 4; ++worker) {
        workers.push_back(std::async(std::launch::async, [&shared, worker] {
            for (int i = 0; i < 500; ++i) {
                QByteArray sequence(100, 'A');
                const int id = (i + worker) % 64;
                for (int j = 0; j < 3; ++j) {
                    sequence[j] = "ACGT"[(id >> (2 * j)) & 3];
                }
                if (shared.getMeltingTemperature(sequence) != sequence.size() + sequence.count('G')) {
                    return false;
                }
            }
            return true;
        }));
    }
    for (auto& worker : workers) {
        if (!worker.get()) {
            return "Concurrent temperature-cache access returned an incorrect value";
        }
    }
    return {};
}

class CountingTmCalculator : public TmCalculator {
public:
    CountingTmCalculator() : TmCalculator({}), calculator(RoughTmCalculatorCmdFactory().createCalculator({})) {
    }

    int calls = 0;

protected:
    double calculateMeltingTemperature(const QByteArray& sequence) override {
        ++calls;
        return calculator->getMeltingTemperature(sequence);
    }

private:
    QSharedPointer<TmCalculator> calculator;
};

QString testDegenerateTemperature() {
    const QByteArray forwardPrimer("ACGTCAGTACGATCGTACGA");
    const QByteArray reversePrimer("TGCACTGACTGACGTAGCTA");
    const QByteArray amplicon = forwardPrimer + QByteArray(40, 'C') + DNASequenceUtils::reverseComplement(reversePrimer);
    auto forward = forwardPrimer;
    auto reverse = reversePrimer;
    forward[0] = 'R';
    reverse[0] = 'Y';
    QByteArray secondProduct = amplicon;
    secondProduct[0] = 'G';
    QByteArray secondForward = forwardPrimer;
    secondForward[0] = 'G';
    CountingTmCalculator calculator;
    auto reference = RoughTmCalculatorCmdFactory().createCalculator({});
    const double first = calculator.getAnnealingTemperature(amplicon, forward, reverse);
    const int firstCalls = calculator.calls;
    if (first != reference->getAnnealingTemperature(amplicon, forwardPrimer, reversePrimer) ||
        calculator.getAnnealingTemperature(amplicon, forward, reverse) != first || calculator.calls != firstCalls) {
        return "Repeated degenerate-primer temperature was not reused";
    }
    const double second = calculator.getAnnealingTemperature(secondProduct, forward, reverse);
    if (second == first || second != reference->getAnnealingTemperature(secondProduct, secondForward, reversePrimer)) {
        return "Degenerate primer cache ignored the product-specific binding sequence";
    }
    return {};
}

QString testPrimer3Concurrency() {
    const auto factory = AppContext::getTmCalculatorRegistry()->getById("primer3-tm-algorithm");
    if (factory == nullptr) {
        return "Primer3 temperature calculator is not registered";
    }
    QList<QByteArray> sequences;
    for (int i = 0; i < 64; ++i) {
        QByteArray sequence("ACGTCAGTACGATCGTACGA");
        for (int j = 0; j < 3; ++j) {
            sequence[j] = "ACGT"[(i >> (2 * j)) & 3];
        }
        sequences << sequence;
    }
    QList<QVariantMap> configurations;
    QList<QList<double>> expected;
    for (int salt : {0, 5, 10, 20, 50, 75, 100, 200}) {
        auto settings = factory->createDefaultSettings();
        settings["salt_correction"] = 2;
        settings["salt_conc"] = salt;
        configurations << settings;
        const auto calculator = factory->createCalculator(settings);
        QList<double> temperatures;
        for (const auto& sequence : std::as_const(sequences)) {
            const double temperature = calculator->getMeltingTemperature(sequence);
            if (!std::isfinite(temperature) || temperature == TmCalculator::INVALID_TM) {
                return "Invalid serial Primer3 reference temperature";
            }
            temperatures << temperature;
        }
        expected << temperatures;
    }

    std::atomic<bool> start{false};
    std::vector<std::future<QString>> workers;
    for (int worker = 0; worker < configurations.size(); ++worker) {
        workers.push_back(std::async(std::launch::async, [&, worker] {
            while (!start.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            for (int round = 0; round < 256; ++round) {
                // A fresh instance keeps every sequence cold in the cache.
                const auto calculator = factory->createCalculator(configurations.at(worker));
                for (int i = 0; i < sequences.size(); ++i) {
                    const double actual = calculator->getMeltingTemperature(sequences.at(i));
                    const double reference = expected.at(worker).at(i);
                    if (!std::isfinite(actual) || std::abs(actual - reference) > 1e-9) {
                        return QString("Concurrent Primer3 mismatch: salt=%1, round=%2, sequence=%3, expected=%4, actual=%5")
                            .arg(configurations.at(worker).value("salt_conc").toInt()).arg(round).arg(i)
                            .arg(reference, 0, 'g', 17).arg(actual, 0, 'g', 17);
                    }
                }
            }
            return QString();
        }));
    }
    start.store(true, std::memory_order_release);
    QString error;
    for (auto& worker : workers) {
        const auto result = worker.get();
        if (error.isEmpty()) {
            error = result;
        }
    }
    return error;
}

}  // namespace

void GTest_TmCalculator::init(XMLTestFormat*, const QDomElement& element) {
    scenario = element.attribute("scenario");
}

void GTest_TmCalculator::run() {
    QString error;
    if (scenario == "temperature_cache") {
        error = testTemperatureCache();
    } else if (scenario == "degenerate_temperature") {
        error = testDegenerateTemperature();
    } else if (scenario == "primer3_concurrency") {
        error = testPrimer3Concurrency();
    } else {
        error = "Unknown temperature test scenario: " + scenario;
    }
    if (!error.isEmpty()) {
        setError(error);
    }
}

QList<XMLTestFactory*> DnaStatisticsTests::createTestFactories() {
    return {GTest_DnaStatisticsTest::createFactory(), GTest_TmCalculator::createFactory()};
}

}  // namespace U2
