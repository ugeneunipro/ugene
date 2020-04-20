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

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyImporter.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2CrossDatabaseReferenceDbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include "ace/ConvertAceToSqliteTask.h"

namespace U2 {

ConvertAceToSqliteTask::ConvertAceToSqliteTask(const GUrl &_sourceUrl, const U2DbiRef &dstDbiRef) :
    Task(tr("Convert ACE to UGENE database (%1)").arg(_sourceUrl.fileName()), TaskFlag_None),
    sourceUrl(_sourceUrl),
    dstDbiRef(dstDbiRef),
    dbi(NULL),
    databaseWasCreated(false),
    countImportedAssembly(0)
{
    GCOUNTER(cvar, tvar, "ConvertAceToUgenedb");
    tpm = Progress_Manual;
}

void ConvertAceToSqliteTask::run() {
    taskLog.info(tr("Converting assembly from %1 to %2 started")
                 .arg(sourceUrl.fileName())
                 .arg(getDestinationUrl().fileName()));

    qint64 startTime = TimeCounter::getCounter();

    QScopedPointer<IOAdapter> ioAdapter;
    IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(sourceUrl));
    SAFE_POINT_EXT(factory, setError(tr("IOAdapterFactory is NULL")), );
    ioAdapter.reset(factory->createIOAdapter());

    if (!ioAdapter->open(sourceUrl, IOAdapterMode_Read)) {
        setError(tr("Can't open file '%1'").arg(sourceUrl.getURLString()));
        return;
    }

    U2OpStatusImpl os;
    DbiConnection dbiHandle(dstDbiRef, false, os);
    if (os.isCoR()) {
        databaseWasCreated = true;
        dbiHandle = DbiConnection(dstDbiRef, true, stateInfo);
        CHECK_OP(stateInfo, );
    }

    dbi = dbiHandle.dbi;
    SAFE_POINT(dbi, tr("DBI is NULL"), );
    U2ObjectDbi* objDbi = dbi->getObjectDbi();
    SAFE_POINT(objDbi, tr("Object DBI is NULL"), );

    stateInfo.setDescription("Importing");
    taskLog.details(tr("Importing"));

    qint64 totalReadsImported = importAssemblies(*ioAdapter);
    CHECK_OP(stateInfo, );

    stateInfo.setDescription("Packing reads");
    qint64 packTime = packReads();
    CHECK_OP(stateInfo, );

    stateInfo.setDescription("Updating attributes");
    updateAttributeDbi();
    CHECK_OP(stateInfo, );

    qint64 totalTime = TimeCounter::getCounter() - startTime;
    taskLog.info(QString("Converting assembly from %1 to %2 successfully finished: imported %3 reads, total time %4 s, pack time %5 s")
                 .arg(sourceUrl.fileName())
                 .arg(getDestinationUrl().fileName())
                 .arg(totalReadsImported)
                 .arg(totalTime)
                 .arg(packTime));
}

Task::ReportResult ConvertAceToSqliteTask::report() {
    if (stateInfo.isCoR() &&
            databaseWasCreated &&
            getDestinationUrl().isLocalFile()) {
        QFile::remove(getDestinationUrl().getURLString());
    }

    return ReportResult_Finished;
}

GUrl ConvertAceToSqliteTask::getDestinationUrl() const {
    return GUrl(U2DbiUtils::ref2Url(dstDbiRef));
}

QMap<U2Sequence, U2Assembly> ConvertAceToSqliteTask::getImportedObjects() const {
    QMap<U2Sequence, U2Assembly> importedObjects;
    foreach (int pairNum, importedReferences.keys()) {
        importedObjects.insert(importedReferences[pairNum], assemblies[pairNum]);
    }
    return importedObjects;
}

qint64 ConvertAceToSqliteTask::importAssemblies(IOAdapter &ioAdapter) {
    qint64 totalReadsImported = 0;

    U2SequenceDbi* seqDbi = dbi->getSequenceDbi();
    SAFE_POINT(seqDbi, tr("Sequence DBI is NULL"), totalReadsImported);

    U2OpStatusChildImpl os(&stateInfo, U2OpStatusMapping(0, 50));
    QScopedPointer<AceReader> aceReader;
    aceReader.reset(new AceReader(ioAdapter, os));
    CHECK_OP(os, totalReadsImported);

    QScopedPointer<AceIterator> iterator;
    iterator.reset(new AceIterator(*aceReader, stateInfo));

    while (iterator->hasNext()) {
        CHECK(!isCanceled(), totalReadsImported);

        TmpDbiObjects tmpObjects(dstDbiRef, os);

        U2Assembly assembly;

        Assembly aceAssembly = iterator->next();
        CHECK_OP(stateInfo, totalReadsImported);
        CHECK_EXT(aceAssembly.isValid(), setError(tr("Invalid source file")), totalReadsImported);
        Assembly::Sequence aceReference = aceAssembly.getReference();
        referencesData.insert(countImportedAssembly, aceReference);

        U2Sequence reference;
        reference.length = aceReference.data.length();
        reference.visualName = aceReference.name;
        reference.alphabet = U2AlphabetUtils::findBestAlphabet(aceReference.data)->getId();

        seqDbi->createSequenceObject(reference, U2ObjectDbi::ROOT_FOLDER, stateInfo);
        CHECK_OP(stateInfo, totalReadsImported);
        importedReferences.insert(countImportedAssembly, reference);
        tmpObjects.objects << reference.id;

        QVariantMap refHints;
        refHints[U2SequenceDbiHints::EMPTY_SEQUENCE] = true;
        refHints[U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH] = true;
        seqDbi->updateSequenceData(reference.id, U2_REGION_MAX, aceReference.data, refHints, stateInfo);
        CHECK_OP(stateInfo, totalReadsImported);

        assembly.visualName = aceAssembly.getName();
        assembly.referenceId = reference.id;

        U2AssemblyReadsImportInfo & importInfo = importInfos[countImportedAssembly];
        AssemblyImporter importer(stateInfo);
        importer.createAssembly(dstDbiRef, U2ObjectDbi::ROOT_FOLDER, NULL, importInfo, assembly);
        CHECK_OP(stateInfo, totalReadsImported);

        importInfo.packed = false;
        importInfo.nReads = aceAssembly.getReadsCount();
        assemblies.insert(countImportedAssembly, assembly);

        QList<U2AssemblyRead> reads = aceAssembly.getReads();

        BufferedDbiIterator<U2AssemblyRead> readsIterator(reads);
        importer.addReads(&readsIterator);
        CHECK_OP(stateInfo, totalReadsImported);

        tmpObjects.objects.removeAll(reference.id);

        totalReadsImported += aceAssembly.getReadsCount();
        countImportedAssembly++;
    }
    CHECK_EXT(aceReader->getContigsCount() == countImportedAssembly, setError(tr("Invalid source file")), totalReadsImported);

    return totalReadsImported;
}

qint64 ConvertAceToSqliteTask::packReads() {
    qint64 packStart = TimeCounter::getCounter();
    int progressStep;
    if (assemblies.count() > 0) {
        progressStep = 40 / assemblies.count();
    } else {
        progressStep = 40;
    }


    U2AssemblyDbi* assDbi = dbi->getAssemblyDbi();
    SAFE_POINT(assDbi, tr("Assembly DBI is NULL"), 0);

    foreach (int assemblyNum, assemblies.keys()) {
        U2AssemblyReadsImportInfo & importInfo = importInfos[assemblyNum];
        // Pack reads only if it were not packed on import
        if (!importInfo.packed) {
            taskLog.details(tr("Packing reads for assembly '%1' (%2 of %3)")
                            .arg(assemblies[assemblyNum].visualName)
                            .arg(assemblyNum + 1)
                            .arg(assemblies.keys().count()));

            U2AssemblyPackStat stat;
            assDbi->pack(assemblies[assemblyNum].id, stat, stateInfo);
            CHECK_OP(stateInfo, 0);

            importInfo.packStat = stat;
        }
        stateInfo.setProgress(stateInfo.getProgress() + progressStep);
    }

    return TimeCounter::getCounter() - packStart;
}

void ConvertAceToSqliteTask::updateAttributeDbi() {
    int progressStep;
    if (assemblies.count() > 0) {
        progressStep = 10 / assemblies.count();
    } else {
        progressStep = 10;
    }

    U2AttributeDbi* attrDbi = dbi->getAttributeDbi();
    SAFE_POINT(attrDbi, tr("Attribute DBI is NULL"), );

    foreach (int assemblyNum, assemblies.keys()) {
        const U2Assembly &assembly = assemblies[assemblyNum];
        const Assembly::Sequence &reference = referencesData[assemblyNum];
        {
            U2IntegerAttribute lenAttr;
            lenAttr.objectId = assembly.id;
            lenAttr.name = U2BaseAttributeName::reference_length;
            lenAttr.version = assembly.version;
            lenAttr.value = reference.data.length();
            attrDbi->createIntegerAttribute(lenAttr, stateInfo);
            CHECK_OP(stateInfo, );
        }

        U2AssemblyReadsImportInfo & importInfo = importInfos[assemblyNum];
        qint64 maxProw = importInfo.packStat.maxProw;
        qint64 readsCount = importInfo.packStat.readsCount;
        const U2AssemblyCoverageStat& coverageStat = importInfo.coverageInfo.coverage;
        if (maxProw > 0) {
            U2IntegerAttribute maxProwAttr;
            maxProwAttr.objectId = assembly.id;
            maxProwAttr.name = U2BaseAttributeName::max_prow;
            maxProwAttr.version = assembly.version;
            maxProwAttr.value = maxProw;
            attrDbi->createIntegerAttribute(maxProwAttr, stateInfo);
            CHECK_OP(stateInfo, );
        } else if (readsCount > 0) {
            // if there are reads, but maxProw == 0 => error
            taskLog.details(tr("Warning: incorrect maxProw == %1, probably packing was not done! Attribute was not set").arg(maxProw));
        }

        if (readsCount > 0) {
            U2IntegerAttribute countReadsAttr;
            countReadsAttr.objectId = assembly.id;
            countReadsAttr.name = "count_reads_attribute";
            countReadsAttr.version = assembly.version;
            countReadsAttr.value = readsCount;
            attrDbi->createIntegerAttribute(countReadsAttr, stateInfo);
            CHECK_OP(stateInfo, );
        }
        if (!coverageStat.isEmpty()) {
            U2ByteArrayAttribute attribute;
            attribute.objectId = assembly.id;
            attribute.name = U2BaseAttributeName::coverage_statistics;
            attribute.value = U2AssemblyUtils::serializeCoverageStat(coverageStat);
            attribute.version = assembly.version;
            attrDbi->createByteArrayAttribute(attribute, stateInfo);
            CHECK_OP(stateInfo, );
        }
        stateInfo.setProgress(stateInfo.getProgress() + progressStep);
    }
}

}   // namespace U2
