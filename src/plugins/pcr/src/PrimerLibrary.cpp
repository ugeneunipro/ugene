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

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrDbi.h>
#include <U2Core/UdrRecord.h>
#include <U2Core/UdrSchemaRegistry.h>
#include <U2Core/UserApplicationsSettings.h>

#include "PrimerStatistics.h"
#include "PrimerLibrary.h"

namespace U2 {

QScopedPointer<PrimerLibrary> PrimerLibrary::instance(NULL);
QMutex PrimerLibrary::mutex;

namespace {
    const QString libraryName = "primer_library.ugenedb";
    const UdrSchemaId PRIMER_UDR_ID = "Primer";
    const int NAME_FILED = 0;
    const int SEQ_FILED = 1;
    const int GC_FILED = 2;
    const int TM_FILED = 3;
}

PrimerLibrary * PrimerLibrary::getInstance(U2OpStatus &os) {
    QMutexLocker lock(&mutex);
    if (NULL != instance.data()) {
        return instance.data();
    }

    initPrimerUdr(os);
    CHECK_OP(os, NULL);

    UserAppsSettings *settings = AppContext::getAppSettings()->getUserAppsSettings();
    SAFE_POINT_EXT(NULL != settings, os.setError(L10N::nullPointerError("UserAppsSettings")), NULL);

    // open DBI connection
    const QString path = settings->getFileStorageDir() + "/" + libraryName;

    U2DbiRef dbiRef(DEFAULT_DBI_ID, path);
    QHash<QString, QString> properties;
    properties[U2DbiOptions::U2_DBI_LOCKING_MODE] = "normal";

    QScopedPointer<DbiConnection> connection(new DbiConnection(dbiRef, true, os, properties)); // create if not exists
    SAFE_POINT_OP(os, NULL);

    instance.reset(new PrimerLibrary(connection.take()));

    return instance.data();
}

void PrimerLibrary::release() {
    QMutexLocker lock(&mutex);
    delete instance.take();
}

PrimerLibrary::PrimerLibrary(DbiConnection *connection)
: connection(connection), udrDbi(NULL)
{
    udrDbi = connection->dbi->getUdrDbi();
}

PrimerLibrary::~PrimerLibrary() {
    delete connection;
}

void PrimerLibrary::initPrimerUdr(U2OpStatus &os) {
    CHECK(NULL == AppContext::getUdrSchemaRegistry()->getSchemaById(PRIMER_UDR_ID), );

    UdrSchema::FieldDesc name("name", UdrSchema::STRING);
    UdrSchema::FieldDesc sequence("sequence", UdrSchema::STRING);
    UdrSchema::FieldDesc gc("GC", UdrSchema::DOUBLE);
    UdrSchema::FieldDesc tm("Tm", UdrSchema::DOUBLE);

    QScopedPointer<UdrSchema> primerSchema(new UdrSchema(PRIMER_UDR_ID));
    primerSchema->addField(name, os);
    primerSchema->addField(sequence, os);
    primerSchema->addField(gc, os);
    primerSchema->addField(tm, os);
    CHECK_OP(os, );

    AppContext::getUdrSchemaRegistry()->registerSchema(primerSchema.data(), os);
    if (!os.hasError()) {
        primerSchema.take();
    }
}

void PrimerLibrary::addPrimer(const Primer &primer, U2OpStatus &os) {
    QList<UdrValue> values;
    values << UdrValue(primer.name);
    values << UdrValue(primer.sequence);
    values << UdrValue(primer.gc);
    values << UdrValue(primer.tm);
    UdrRecordId record = udrDbi->addRecord(PRIMER_UDR_ID, values, os);
    CHECK_OP(os, );

    emit si_primerAdded(record.getRecordId());
}

void PrimerLibrary::updatePrimer(const Primer &primer, U2OpStatus &os) {
    QList<UdrValue> values;
    values << UdrValue(primer.name);
    values << UdrValue(primer.sequence);
    values << UdrValue(primer.gc);
    values << UdrValue(primer.tm);
    UdrRecordId recordId(PRIMER_UDR_ID, primer.id);
    udrDbi->updateRecord(recordId, values, os);

    emit si_primerChanged(primer.id);
}

Primer PrimerLibrary::getPrimer(const U2DataId &primerId, U2OpStatus &os) const {
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

QList<Primer> PrimerLibrary::getPrimers(U2OpStatus &os) const {
    QList<Primer> result;

    QList<UdrRecord> records = udrDbi->getRecords(PRIMER_UDR_ID, os);
    CHECK_OP(os, result);

    foreach (const UdrRecord &record, records) {
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

void PrimerLibrary::removePrimer(const Primer &primer, U2OpStatus &os) {
    emit si_primerRemoved(primer.id);
    UdrRecordId recordId(PRIMER_UDR_ID, primer.id);
    udrDbi->removeRecord(recordId, os);
}

void PrimerLibrary::addRawPrimer(Primer primer, U2OpStatus &os) {
    setTmAndGcOfPrimer(primer);
    addPrimer(primer, os);
}

void PrimerLibrary::updateRawPrimer(Primer primer, U2OpStatus &os) {
    setTmAndGcOfPrimer(primer);
    updatePrimer(primer, os);
}

void PrimerLibrary::setTmAndGcOfPrimer(Primer &primer) {
    if (PrimerStatistics::validate(primer.sequence)) {
        PrimerStatisticsCalculator calc(primer.sequence.toLocal8Bit());
        primer.gc = calc.getGC();
        primer.tm = calc.getTm();
    } else {
        primer.gc = Primer::INVALID_GC;
        primer.tm = Primer::INVALID_TM;
    }
}

} // U2
