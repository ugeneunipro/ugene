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

#include "PrimerLibrary.h"

#include <QDir>
#include <QFileInfo>

#include <U2Algorithm/TmCalculatorFactory.h>
#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/PrimerStatistics.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrDbi.h>
#include <U2Core/UdrRecord.h>
#include <U2Core/UdrSchemaRegistry.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

QScopedPointer<PrimerLibrary> PrimerLibrary::instance(nullptr);
QMutex PrimerLibrary::mutex;

namespace {
const UdrSchemaId PRIMER_UDR_ID = "Primer";
const UdrSchemaId PRIMER_SETTINGS_UDR_ID = PRIMER_UDR_ID + "Settings";
const int NAME_FILED = 0;
const int SEQ_FILED = 1;
const int GC_FILED = 2;
const int TM_FILED = 3;
const int PRAMETER_FILED = 0;
const int VALUE_FILED = 1;
}  // namespace

PrimerLibrary* PrimerLibrary::getInstance(U2OpStatus& os) {
    QMutexLocker lock(&mutex);
    if (instance.data() != nullptr) {
        instance->initTemperatureCalculator();
        return instance.data();
    }

    initPrimerUdrs(os);
    CHECK_OP(os, nullptr);

    UserAppsSettings* settings = AppContext::getAppSettings()->getUserAppsSettings();
    SAFE_POINT_EXT(settings != nullptr, os.setError(L10N::nullPointerError("UserAppsSettings")), nullptr);

    // open DBI connection
    QString primerLibraryPath = qgetenv("UGENE_PRIMER_LIBRARY_PATH");
    if (!primerLibraryPath.isEmpty()) {
        QDir primerLibraryDir = QFileInfo(primerLibraryPath).dir();
        if (!primerLibraryDir.exists()) {
            bool isCreated = primerLibraryDir.mkpath(primerLibraryDir.absolutePath());
            if (!isCreated) {
                primerLibraryPath = "";
            }
        }
    }
    if (primerLibraryPath.isEmpty()) {
        primerLibraryPath = settings->getFileStorageDir() + "/primer_library.ugenedb";
    }

    U2DbiRef dbiRef(DEFAULT_DBI_ID, primerLibraryPath);
    QHash<QString, QString> properties;
    properties[U2DbiOptions::U2_DBI_LOCKING_MODE] = "normal";

    QScopedPointer<DbiConnection> connection(new DbiConnection(dbiRef, true, os, properties));  // create if not exists
    SAFE_POINT_OP(os, nullptr);

    instance.reset(new PrimerLibrary(connection.take()));

    return instance.data();
}

void PrimerLibrary::release() {
    QMutexLocker lock(&mutex);
    delete instance.take();
}

PrimerLibrary::PrimerLibrary(DbiConnection* connection)
    : connection(connection) {
    udrDbi = connection->dbi->getUdrDbi();
    createPrimerSettingsTableIfNotExists();
    initTemperatureCalculator();
}

PrimerLibrary::~PrimerLibrary() {
    delete connection;
}

void PrimerLibrary::initPrimerUdrs(U2OpStatus& os) {
    auto registerUdrSchema = [](U2OpStatus& state, const UdrSchemaId& id, const QList<QPair<QByteArray, UdrSchema::DataType>>& fields) {
        CHECK(AppContext::getUdrSchemaRegistry()->getSchemaById(id) == nullptr, );

        QScopedPointer<UdrSchema> primerSchema(new UdrSchema(id));
        for (const auto& field : qAsConst(fields)) {
            primerSchema->addField(UdrSchema::FieldDesc(field.first, field.second), state);
            CHECK_OP(state, );
        }

        AppContext::getUdrSchemaRegistry()->registerSchema(primerSchema.data(), state);
        if (!state.hasError()) {
            primerSchema.take();
        }
    };

    {
        QList<QPair<QByteArray, UdrSchema::DataType>> fields;
        fields.append({"name", UdrSchema::STRING});
        fields.append({"sequence", UdrSchema::STRING});
        fields.append({"GC", UdrSchema::DOUBLE});
        fields.append({"Tm", UdrSchema::DOUBLE});
        registerUdrSchema(os, PRIMER_UDR_ID, fields);
        CHECK_OP(os, );
    }
    {
        QList<QPair<QByteArray, UdrSchema::DataType>> fields;
        fields.append({"parameter", UdrSchema::STRING});
        fields.append({"value", UdrSchema::STRING});
        registerUdrSchema(os, PRIMER_SETTINGS_UDR_ID, fields);
        CHECK_OP(os, );
    }
}

void PrimerLibrary::addPrimer(const Primer& primer, U2OpStatus& os) {
    QList<UdrValue> values;
    values << UdrValue(primer.name);
    values << UdrValue(primer.sequence);
    values << UdrValue(primer.gc);
    values << UdrValue(primer.tm);
    UdrRecordId record = udrDbi->addRecord(PRIMER_UDR_ID, values, os);
    CHECK_OP(os, );

    emit si_primerAdded(record.getRecordId());
}

void PrimerLibrary::updatePrimer(const Primer& primer, U2OpStatus& os) {
    QList<UdrValue> values;
    values << UdrValue(primer.name);
    values << UdrValue(primer.sequence);
    values << UdrValue(primer.gc);
    values << UdrValue(primer.tm);
    UdrRecordId recordId(PRIMER_UDR_ID, primer.id);
    udrDbi->updateRecord(recordId, values, os);

    emit si_primerChanged(primer.id);
}

Primer PrimerLibrary::getPrimer(const U2DataId& primerId, U2OpStatus& os) const {
    Primer result;

    const UdrRecord record = udrDbi->getRecord(UdrRecordId(PRIMER_UDR_ID, primerId), os);
    CHECK_OP(os, result);

    result.id = record.getId().getRecordId();
    result.name = record.getString(NAME_FILED, os);
    result.sequence = record.getString(SEQ_FILED, os);
    result.gc = record.getDouble(GC_FILED, os);
    result.tm = record.getDouble(TM_FILED, os);
    CHECK_OP(os, result);

    return result;
}

QList<Primer> PrimerLibrary::getPrimers(U2OpStatus& os) const {
    QList<Primer> result;

    QList<UdrRecord> records = udrDbi->getRecords(PRIMER_UDR_ID, os);
    CHECK_OP(os, result);

    foreach (const UdrRecord& record, records) {
        Primer primer;
        primer.id = record.getId().getRecordId();
        primer.name = record.getString(NAME_FILED, os);
        primer.sequence = record.getString(SEQ_FILED, os);
        primer.gc = record.getDouble(GC_FILED, os);
        primer.tm = record.getDouble(TM_FILED, os);
        CHECK_OP(os, result);
        result << primer;
    }

    return result;
}

void PrimerLibrary::removePrimer(const Primer& primer, U2OpStatus& os) {
    emit si_primerRemoved(primer.id);
    UdrRecordId recordId(PRIMER_UDR_ID, primer.id);
    udrDbi->removeRecord(recordId, os);
}

void PrimerLibrary::addRawPrimer(Primer primer, U2OpStatus& os) {
    setTmAndGcOfPrimer(primer);
    addPrimer(primer, os);
}

void PrimerLibrary::updateRawPrimer(Primer primer, U2OpStatus& os) {
    setTmAndGcOfPrimer(primer);
    updatePrimer(primer, os);
}

const QVariantMap& PrimerLibrary::getTemperatureSettings() const {
    return temperatureCalculator->getSettings();
}

void PrimerLibrary::setTemperatureCalculator(const QSharedPointer<TmCalculator>& newTemperatureCalculator) {
    temperatureCalculator = newTemperatureCalculator;
    auto settingsMap = temperatureCalculator->getSettings();
    U2OpStatusImpl os;
    auto records = udrDbi->getRecords(PRIMER_SETTINGS_UDR_ID, os);
    CHECK_OP(os, );

    auto addAllFromSettingsMap = [this](const QVariantMap& settings, U2OpStatus& state) {
        auto keys = settings.keys();
        for (const auto& key : qAsConst(keys)) {
            QList<UdrValue> values;
            values << UdrValue(key);
            values << UdrValue(settings.value(key).toString());
            udrDbi->addRecord(PRIMER_SETTINGS_UDR_ID, values, state);
            CHECK_OP(state, );
        }
    };

    if (records.isEmpty()) {
        udrDbi->createTable(PRIMER_SETTINGS_UDR_ID, os);
        CHECK_OP(os, );

        addAllFromSettingsMap(settingsMap, os);
        CHECK_OP(os, );

        return;
    }

    auto id = settingsMap.value(TmCalculator::KEY_ID);
    auto idRecords = udrDbi->getRecords(PRIMER_SETTINGS_UDR_ID, os);
    CHECK_OP(os, );

    // if @KEY_ID is the same (method wasn't changed, only parameters) we need just update values
    bool update = false;
    // otherwise - remove everything but @KEY_ID (this one could be updated)
    QList<UdrRecordId> toRemove;
    QList<UdrRecord> toUpdate;
    auto keys = settingsMap.keys();
    for (const auto& record : qAsConst(idRecords)) {
        auto recordParameter = record.getString(PRAMETER_FILED, os);
        CHECK_OP(os, );

        if (keys.contains(recordParameter)) {
            auto recordValue = record.getString(VALUE_FILED, os);
            CHECK_OP(os, );

            if (settingsMap.value(recordParameter) != recordValue) {
                toUpdate.append(record);
            }
        } else {
            toRemove.append(UdrRecordId(PRIMER_SETTINGS_UDR_ID, record.getId().getRecordId()));
        }
        CHECK_CONTINUE(recordParameter == TmCalculator::KEY_ID);

        auto recordValue = record.getString(VALUE_FILED, os);
        CHECK_CONTINUE(recordValue == id);

        update = true;
    }

    // Either update all (if only parameters of algorithm were changed)
    // or only key (if the algorithm itself was changed)
    SAFE_POINT(update || (!update && toUpdate.size() == 1), "Incorrect update size", );
    for (const auto& record : qAsConst(toUpdate)) {
        auto recordParameter = record.getString(PRAMETER_FILED, os);
        auto recordParameterValue = settingsMap.value(recordParameter).toString();
        QList<UdrValue> values;
        values << UdrValue(recordParameter);
        values << UdrValue(recordParameterValue);
        UdrRecordId recirdId(PRIMER_SETTINGS_UDR_ID, record.getId().getRecordId());
        udrDbi->updateRecord(recirdId, values, os);
        CHECK_OP(os, );
    }

    if (!update) {
        for (const auto& record : qAsConst(toRemove)) {
            udrDbi->removeRecord(record, os);
            CHECK_OP(os, );
        }
        settingsMap.remove(TmCalculator::KEY_ID);
        addAllFromSettingsMap(settingsMap, os);
        CHECK_OP(os, );
    }
}

void PrimerLibrary::setTmAndGcOfPrimer(Primer& primer) {
    if (PrimerStatistics::validate(primer.sequence)) {
        PrimerStatisticsCalculator calc(primer.sequence.toLocal8Bit(), temperatureCalculator);
        primer.gc = calc.getGC();
        primer.tm = calc.getTm();
    } else {
        primer.gc = Primer::INVALID_GC;
        primer.tm = TmCalculator::INVALID_TM;
    }
}

void PrimerLibrary::createPrimerSettingsTableIfNotExists() {
    U2OpStatusImpl os;
    // This table appears in v46 and not exist in previous versions
    // need to create it
    udrDbi->createTable(PRIMER_SETTINGS_UDR_ID, os);
    CHECK_OP(os, );
}

void PrimerLibrary::initTemperatureCalculator() {
    CHECK(!initializedFromDb, );

    U2OpStatusImpl os;
    auto records = udrDbi->getRecords(PRIMER_SETTINGS_UDR_ID, os);
    CHECK_OP(os, );

    QVariantMap settings;
    QString calcId;
    for (const auto& record : qAsConst(records)) {
        auto recordParameter = record.getString(PRAMETER_FILED, os);
        CHECK_OP(os, );

        auto recordValue = record.getString(VALUE_FILED, os);
        CHECK_OP(os, );

        settings.insert(recordParameter, recordValue);
        CHECK_CONTINUE(recordParameter == TmCalculator::KEY_ID);

        calcId = recordValue;
    }

    auto factory = AppContext::getTmCalculatorRegistry()->getById(calcId);
    if (factory != nullptr) {
        temperatureCalculator = factory->createCalculator(settings);
        initializedFromDb = true;
    } else {
        temperatureCalculator = AppContext::getTmCalculatorRegistry()->createTmCalculator();
    }
}

}  // namespace U2
