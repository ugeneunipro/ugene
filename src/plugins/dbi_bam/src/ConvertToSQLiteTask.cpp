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

#include <limits>
#include <time.h>

#include <QFile>
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BAMDbiPlugin.h"
#include "CancelledException.h"
#include "ConvertToSQLiteTask.h"
#include "Dbi.h"
#include "IOException.h"
#include "Index.h"
#include "LoadBamInfoTask.h"
#include "Reader.h"
#include "SamReader.h"

namespace U2 {
namespace BAM {

ConvertToSQLiteTask::ConvertToSQLiteTask(const GUrl &_sourceUrl, const U2DbiRef &dstDbiRef, BAMInfo& _bamInfo, bool _sam):
    Task(tr("Convert BAM to UGENE database (%1)").arg(_sourceUrl.fileName()), TaskFlag_None),
    sourceUrl(_sourceUrl),
    dstDbiRef(dstDbiRef),
    bamInfo(_bamInfo),
    sam(_sam)
{
    GCOUNTER( cvar, tvar, "ConvertBamToUgenedb" );
    tpm = Progress_Manual;
}

static void enableCoverageOnImport(U2AssemblyCoverageImportInfo &cii, int referenceLength) {
    cii.computeCoverage = true;
    int coverageInfoSize = qMin(U2AssemblyUtils::MAX_COVERAGE_VECTOR_SIZE, referenceLength);
    cii.coverageBasesPerPoint = qMax(1.0, ((double)referenceLength)/coverageInfoSize);
    cii.coverage.resize(coverageInfoSize);
}

namespace {

class BamIterator : public Iterator {
public:
    BamIterator(BamReader &reader):
        reader(reader),
        alignmentReader(NULL, 0, 0),
        alignmentReaderValid(false),
        readValid(false)
    {
    }

    virtual bool hasNext() {
        return readValid || alignmentReaderValid || !reader.isEof();
    }

    virtual U2AssemblyRead next() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            if(!alignmentReaderValid) {
                alignmentReader = reader.getAlignmentReader();
            }
            alignmentReaderValid = false;
            read = AssemblyDbi::alignmentToRead(alignmentReader.read());
        }
        readValid = false;
        return read;
    }

    virtual void skip() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            if(!alignmentReaderValid) {
                alignmentReader = reader.getAlignmentReader();
            }
            alignmentReaderValid = false;
            alignmentReader.skip();
        }
        readValid = false;
    }

    virtual const U2AssemblyRead &peek() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            if(!alignmentReaderValid) {
                alignmentReader = reader.getAlignmentReader();
            }
            alignmentReaderValid = false;
            read = AssemblyDbi::alignmentToRead(alignmentReader.read());
            readValid = true;
        }
        return read;
    }

    virtual int peekReferenceId() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            if(!alignmentReaderValid) {
                alignmentReader = reader.getAlignmentReader();
                alignmentReaderValid = true;
            }
        }
        return alignmentReader.getId();
    }

private:
    BamReader &reader;
    BamReader::AlignmentReader alignmentReader;
    bool alignmentReaderValid;
    U2AssemblyRead read;
    bool readValid;
};

class SamIterator : public Iterator {
public:
    SamIterator(SamReader &reader):
        reader(reader),
        readReferenceId(-1),
        readValid(false)
    {
    }

    virtual bool hasNext() {
        return readValid || !reader.isEof();
    }

    virtual U2AssemblyRead next() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            bool eof = false;
            read = AssemblyDbi::alignmentToRead(reader.readAlignment(eof));
        }
        readValid = false;
        return read;
    }

    virtual void skip() {
        next();
    }

    virtual const U2AssemblyRead &peek() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            bool eof = false;
            Alignment alignemnt = reader.readAlignment(eof);
            readReferenceId = alignemnt.getReferenceId();
            read = AssemblyDbi::alignmentToRead(alignemnt);
            readValid = true;
        }
        return read;
    }

    virtual int peekReferenceId() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        if(!readValid) {
            bool eof = false;
            Alignment alignemnt = reader.readAlignment(eof);
            readReferenceId = alignemnt.getReferenceId();
            read = AssemblyDbi::alignmentToRead(alignemnt);
            readValid = true;
        }
        return readReferenceId;
    }

private:
    SamReader &reader;
    U2AssemblyRead read;
    int readReferenceId;
    bool readValid;
};

class ReferenceIterator : public Iterator {
public:
    ReferenceIterator(int referenceId, Iterator &iterator):
        referenceId(referenceId),
        iterator(iterator)
    {
    }

    virtual bool hasNext() {
        return iterator.hasNext() && (iterator.peekReferenceId() == referenceId);
    }

    virtual U2AssemblyRead next() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator.next();
    }

    virtual void skip() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        iterator.skip();
    }

    virtual const U2AssemblyRead &peek() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator.peek();
    }

    virtual int peekReferenceId() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator.peekReferenceId();
    }

private:
    int referenceId;
    Iterator &iterator;
};

class SkipUnmappedIterator : public Iterator {
public:
    SkipUnmappedIterator(Iterator &iterator):
        iterator(iterator)
    {
    }

    virtual bool hasNext() {
        skipUnmappedReads();
        return iterator.hasNext();
    }

    virtual U2AssemblyRead next() {
        skipUnmappedReads();
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator.next();
    }

    virtual void skip() {
        skipUnmappedReads();
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        iterator.skip();
    }

    virtual const U2AssemblyRead &peek() {
        skipUnmappedReads();
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator.peek();
    }

    virtual int peekReferenceId() {
        skipUnmappedReads();
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator.peekReferenceId();
    }

private:
    void skipUnmappedReads() {
        while(iterator.hasNext()) {
            if(-1 == iterator.peekReferenceId() ||
               ReadFlagsUtils::isUnmappedRead(iterator.peek()->flags) ||
               iterator.peek()->cigar.isEmpty()) {
                iterator.skip();
            } else {
                break;
            }
        }
    }

private:
    Iterator &iterator;
};

class DbiIterator : public U2DbiIterator<U2AssemblyRead> {
public:
    virtual ~DbiIterator() {}

    virtual bool hasNext() = 0;

    virtual U2AssemblyRead next() = 0;

    virtual U2AssemblyRead peek() = 0;

    virtual qint64 getReadsImported() = 0;
};

class SequentialDbiIterator : public DbiIterator {
public:
    SequentialDbiIterator(int referenceId, bool skipUnmapped, Iterator &inputIterator, TaskStateInfo &stateInfo, const IOAdapter &ioAdapter):
        referenceIterator(referenceId, inputIterator),
        skipUnmappedIterator(skipUnmapped? new SkipUnmappedIterator(referenceIterator):NULL),
        iterator(skipUnmapped ? dynamic_cast<Iterator *>(skipUnmappedIterator.data()) : dynamic_cast<Iterator *>(&referenceIterator)),
        readsImported(0),
        stateInfo(stateInfo),
        ioAdapter(ioAdapter)
    {
    }

    virtual bool hasNext() {
        if(stateInfo.isCanceled()) {
            throw CancelledException(BAMDbiPlugin::tr("Task was cancelled"));
        }
        return iterator->hasNext();
    }

    virtual U2AssemblyRead next() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        stateInfo.progress = ioAdapter.getProgress();
        readsImported++;
        return iterator->next();
    }

    virtual U2AssemblyRead peek() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return iterator->peek();
    }

    virtual qint64 getReadsImported() {
        return readsImported;
    }

private:
    ReferenceIterator referenceIterator;
    QScopedPointer<SkipUnmappedIterator> skipUnmappedIterator;
    Iterator *iterator;
    qint64 readsImported;
    TaskStateInfo &stateInfo;
    const IOAdapter &ioAdapter;
};

class IndexedBamDbiIterator : public DbiIterator {
public:
    IndexedBamDbiIterator(int referenceId, bool skipUnmapped, BamReader &reader, const Index &index, TaskStateInfo &stateInfo, const IOAdapter &ioAdapter):
        iterator(reader),
        dbiIterator(referenceId, skipUnmapped, iterator, stateInfo, ioAdapter),
        hasReads(false)
    {
        {
            VirtualOffset minOffset = VirtualOffset(0xffffffffffffLL, 0xffff);
            foreach(const Index::ReferenceIndex::Bin &bin, index.getReferenceIndices()[referenceId].getBins()) {
                foreach(const Index::ReferenceIndex::Chunk &chunk, bin.getChunks()) {
                    if(minOffset > chunk.getStart()) {
                        minOffset = chunk.getStart();
                        hasReads = true;
                    }
                }
            }
            if(hasReads) {
                reader.seek(minOffset);
            }
        }
    }

    virtual bool hasNext() {
        return hasReads && dbiIterator.hasNext();
    }

    virtual U2AssemblyRead next() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return dbiIterator.next();
    }

    virtual U2AssemblyRead peek() {
        if(!hasNext()) {
            throw Exception(BAMDbiPlugin::tr("The iteration has no next element"));
        }
        return dbiIterator.peek();
    }

    virtual qint64 getReadsImported() {
        return dbiIterator.getReadsImported();
    }

private:
    BamIterator iterator;
    SequentialDbiIterator dbiIterator;
    bool hasReads;
};

static const int READS_CHUNK_SIZE = 250*1000;

} // namespace

void ConvertToSQLiteTask::run() {
    try {
        taskLog.info(tr("Converting assembly from %1 to %2 started")
                     .arg(sourceUrl.fileName())
                     .arg(getDestinationUrl().fileName()));

        time_t startTime = time(0);

        qint64 totalReadsImported = importReads();

        time_t packStart = time(0);
        packReads();
        time_t packTime = time(0) - packStart;

        updateAttributes();

        foreach (AssemblyImporter *importer, importers) {
            importedAssemblies << importer->getAssembly();
        }
        qDeleteAll(importers);
        importers.clear();

        time_t totalTime = time(0) - startTime;

        taskLog.info(QString("Converting assembly from %1 to %2 successfully finished: imported %3 reads, total time %4 s, pack time %5 s")
                     .arg(sourceUrl.fileName())
                     .arg(getDestinationUrl().fileName())
                     .arg(totalReadsImported)
                     .arg(totalTime)
                     .arg(packTime));

    } catch(const CancelledException & /*e*/) {
        qDeleteAll(importers);
        importers.clear();
        if (getDestinationUrl().isLocalFile()) {
            QFile::remove(getDestinationUrl().getURLString());
        }
        taskLog.info(tr("Converting assembly from %1 to %2 cancelled")
                     .arg(sourceUrl.fileName())
                     .arg(getDestinationUrl().fileName()));
    } catch(const Exception &e) {
        qDeleteAll(importers);
        importers.clear();
        setError(tr("Converting assembly from %1 to %2 failed: %3")
                 .arg(sourceUrl.fileName())
                 .arg(getDestinationUrl().fileName())
                 .arg(e.getMessage()));
        if (getDestinationUrl().isLocalFile()) {
            QFile::remove(getDestinationUrl().getURLString());
        }
    }
}

GUrl ConvertToSQLiteTask::getDestinationUrl() const {
    return GUrl(U2DbiUtils::ref2Url(dstDbiRef));
}

QList<U2Assembly> ConvertToSQLiteTask::getAssemblies() const {
    return importedAssemblies;
}

bool ConvertToSQLiteTask::isSorted(Reader *reader) const {
    return Header::Coordinate == reader->getHeader().getSortingOrder() ||
            Header::QueryName == reader->getHeader().getSortingOrder() ||
            bamInfo.hasIndex();
}

qint64 ConvertToSQLiteTask::importReads() {
    qint64 totalReadsImported = 0;
    QScopedPointer<IOAdapter> ioAdapter(prepareIoAdapter());

    BamReader *bamReader = NULL;
    SamReader *samReader = NULL;
    QScopedPointer<Reader> reader(NULL);
    if (sam) {
        samReader = new SamReader(*ioAdapter);
        reader.reset(samReader);
    } else {
        bamReader = new BamReader(*ioAdapter);
        reader.reset(bamReader);
    }

    references = reader->getHeader().getReferences();

    for (int referenceId = -1;  referenceId < references.size(); referenceId++) {
        importers[referenceId] = new AssemblyImporter(stateInfo);
    }

    stateInfo.setDescription("Importing reads");

    if (isSorted(reader.data())) {
        totalReadsImported += importSortedReads(samReader, bamReader, reader.data(), ioAdapter.data());
    } else {
        totalReadsImported += importUnsortedReads(samReader, bamReader, reader.data(), importInfos);
    }

    foreach (int referenceId, importers.keys()) {
        if (!importers[referenceId]->isObjectExist()) {
            delete importers[referenceId];
            importers.remove(referenceId);
        }
    }

    return totalReadsImported;
}

void ConvertToSQLiteTask::packReads() {
    stateInfo.setDescription("Packing reads");

    U2OpStatusImpl opStatus;
    foreach (int referenceId, importers.keys()) {
        SAFE_POINT_EXT(importers.contains(referenceId), throw Exception("An unexpected assembly"), );
        taskLog.details(tr("Packing reads for assembly '%1' (%2 of %3)")
                        .arg(importers[referenceId]->getAssembly().visualName)
                        .arg(referenceId + 1)
                        .arg(importInfos.size()));

        importers[referenceId]->packReads(importInfos[referenceId]);
        CHECK_EXT(!opStatus.isCoR(), throw Exception(opStatus.getError()), );
    }
}

void ConvertToSQLiteTask::updateAttributes() {
    DbiConnection connection(dstDbiRef, stateInfo);
    SAFE_POINT_EXT(!stateInfo.hasError(), throw Exception(getError()), );
    U2AttributeDbi *attributeDbi = connection.dbi->getAttributeDbi();
    CHECK(NULL != attributeDbi, );

    foreach (int referenceId, importers.keys()) {
        const U2Assembly &assembly = importers[referenceId]->getAssembly();

        if (-1 != referenceId) {
            updateReferenceLengthAttribute(references[referenceId].getLength(), assembly, attributeDbi);
            updateReferenceMd5Attribute(references[referenceId].getMd5(), assembly, attributeDbi);
            updateReferenceSpeciesAttribute(references[referenceId].getSpecies(), assembly, attributeDbi);
            updateReferenceUriAttribute(references[referenceId].getUri(), assembly, attributeDbi);
        }

        const U2AssemblyReadsImportInfo &importInfo = importInfos[referenceId];

        updateImportInfoMaxProwAttribute(importInfo, assembly, attributeDbi);
        updateImportInfoReadsCountAttribute(importInfo, assembly, attributeDbi);
        updateImportInfoCoverageStatAttribute(importInfo, assembly, attributeDbi);
    }
}

qint64 ConvertToSQLiteTask::importSortedReads(SamReader *samReader, BamReader *bamReader, Reader *reader, IOAdapter *ioAdapter) {
    qint64 totalReadsImported = 0;

    QScopedPointer<Iterator> iterator;
    if (!bamInfo.hasIndex()) {
        if (sam) {
            iterator.reset(new SamIterator(*samReader));
        } else {
            iterator.reset(new BamIterator(*bamReader));
        }
    }

    totalReadsImported += importMappedSortedReads(bamReader, reader, iterator.data(), ioAdapter);

    if (bamInfo.isUnmappedSelected()) {
        totalReadsImported += importUnmappedSortedReads(bamReader, reader, iterator, ioAdapter);
    }

    return totalReadsImported;
}

qint64 ConvertToSQLiteTask::importMappedSortedReads(BamReader *bamReader, Reader *reader, Iterator *iterator, IOAdapter *ioAdapter) {
    qint64 totalReadsImported = 0;

    const QList<Header::Reference> references = reader->getHeader().getReferences();

    for (int referenceId = 0; referenceId < references.size(); referenceId++) {
        if (bamInfo.isReferenceSelected(referenceId)) {
            U2Assembly assembly;
            assembly.visualName = references[referenceId].getName();
            taskLog.details(tr("Importing assembly '%1' (%2 of %3)")
                            .arg(assembly.visualName)
                            .arg(referenceId + 1)
                            .arg(references.size()));

            U2AssemblyReadsImportInfo &importInfo = importInfos[referenceId];
            enableCoverageOnImport(importInfo.coverageInfo, references[referenceId].getLength());

            QScopedPointer<DbiIterator> dbiIterator;
            if(bamInfo.hasIndex()) {
                dbiIterator.reset(new IndexedBamDbiIterator(referenceId, !bamInfo.isUnmappedSelected(), *bamReader, bamInfo.getIndex(), stateInfo, *ioAdapter));
            } else {
                dbiIterator.reset(new SequentialDbiIterator(referenceId, !bamInfo.isUnmappedSelected(), *iterator, stateInfo, *ioAdapter));
            }

            SAFE_POINT_EXT(importers.contains(referenceId), throw Exception("An unexpected assembly"), totalReadsImported);
            importers[referenceId]->createAssembly(dstDbiRef, U2ObjectDbi::ROOT_FOLDER, dbiIterator.data(), importInfo, assembly);
            CHECK_EXT(!hasError(), throw Exception(getError()), totalReadsImported);
            CHECK_EXT(!isCanceled(), throw CancelledException(BAMDbiPlugin::tr("Task was cancelled")), totalReadsImported);

            totalReadsImported += dbiIterator->getReadsImported();
            taskLog.details(tr("Successfully imported %1 reads for assembly '%2' (total %3 reads imported)")
                            .arg(dbiIterator->getReadsImported())
                            .arg(assembly.visualName)
                            .arg(totalReadsImported));
        } else {
            if (!bamInfo.hasIndex()) {
                while(iterator->hasNext() && iterator->peekReferenceId() == referenceId) {
                    iterator->skip();
                }
                if (isCanceled()) {
                    throw CancelledException(BAMDbiPlugin::tr("Task was cancelled"));
                }
            }
        }
    }

    return totalReadsImported;
}

qint64 ConvertToSQLiteTask::importUnmappedSortedReads(BamReader *bamReader, Reader *reader, QScopedPointer<Iterator> &iterator, IOAdapter *ioAdapter) {
    taskLog.details(tr("Importing unmapped reads"));

    if (bamInfo.hasIndex() && !reader->getHeader().getReferences().isEmpty()) {
        const Index &index = bamInfo.getIndex();
        bool maxOffsetFound = false;
        VirtualOffset maxOffset = VirtualOffset(0, 0);

        for (int refId = 0; refId < reader->getHeader().getReferences().size(); ++refId) {
            foreach (const Index::ReferenceIndex::Bin &bin, index.getReferenceIndices()[refId].getBins()) {
                foreach (const Index::ReferenceIndex::Chunk &chunk, bin.getChunks()) {
                    if (chunk.getStart() < chunk.getEnd() && maxOffset < chunk.getStart()) {
                        maxOffset = chunk.getStart();
                        maxOffsetFound = true;
                    }
                }
            }
        }

        if (maxOffsetFound) {
            bamReader->seek(maxOffset);
            iterator.reset(new BamIterator(*bamReader));
            while (iterator->hasNext() && iterator->peekReferenceId() != -1) {
                iterator->skip();
            }
        } else{
            iterator.reset(new BamIterator(*bamReader));
        }
    }

    SequentialDbiIterator dbiIterator(-1, false, *iterator, stateInfo, *ioAdapter);

    U2Assembly assembly;
    assembly.visualName = "Unmapped";

    SAFE_POINT_EXT(importers.contains(-1), throw Exception("An unexpected assembly"), 0);
    importers[-1]->createAssembly(dstDbiRef, U2ObjectDbi::ROOT_FOLDER, &dbiIterator, importInfos[-1], assembly);
    CHECK_EXT(!hasError(), throw Exception(getError()), dbiIterator.getReadsImported());
    CHECK_EXT(!isCanceled(), throw CancelledException(BAMDbiPlugin::tr("Task was cancelled")), dbiIterator.getReadsImported());

    return dbiIterator.getReadsImported();
}

qint64 ConvertToSQLiteTask::importUnsortedReads(SamReader *samReader, BamReader *bamReader, Reader *reader, QMap<int, U2AssemblyReadsImportInfo> &importInfos) {
    taskLog.details(tr("No bam index given, preparing sequential import"));

    for (int referenceId = 0; referenceId < reader->getHeader().getReferences().size(); referenceId++) {
        if (bamInfo.isReferenceSelected(referenceId)) {
            createAssemblyObjectForUnsortedReads(referenceId, reader, importInfos);
        }
    }

    if (bamInfo.isUnmappedSelected()) {
        createAssemblyObjectForUnsortedReads(-1, reader, importInfos);
    }

    taskLog.details(tr("Importing reads sequentially"));

    QScopedPointer<Iterator> inputIterator;
    if (sam) {
        inputIterator.reset(new SamIterator(*samReader));
    } else {
        inputIterator.reset(new BamIterator(*bamReader));
    }

    QScopedPointer<SkipUnmappedIterator> skipUnmappedIterator;
    Iterator *iterator = NULL;
    if (!bamInfo.isUnmappedSelected()) {
        skipUnmappedIterator.reset(new SkipUnmappedIterator(*inputIterator));
        iterator = skipUnmappedIterator.data();
    } else {
        iterator = inputIterator.data();
    }

    return importReadsSequentially(iterator);
}

void ConvertToSQLiteTask::createAssemblyObjectForUnsortedReads(int referenceId, Reader *reader, QMap<int, U2::U2AssemblyReadsImportInfo> &importInfos) {
    U2Assembly assembly;
    assembly.visualName = (referenceId == -1 ? "Unmapped" : reader->getHeader().getReferences()[referenceId].getName());

    SAFE_POINT_EXT(importers.contains(referenceId), throw Exception("An unexpected assembly"), );
    importers[referenceId]->createAssembly(dstDbiRef, U2ObjectDbi::ROOT_FOLDER, assembly);

    CHECK_EXT(!hasError(), throw Exception(getError()), );
    CHECK_EXT(!isCanceled(), throw CancelledException(BAMDbiPlugin::tr("Task was cancelled")), );

    importInfos[referenceId].packed = false;
}

qint64 ConvertToSQLiteTask::importReadsSequentially(Iterator *iterator) {
    qint64 totalReadsImported = 0;

    U2OpStatusImpl opStatus;

    while (iterator->hasNext()) {
        QMap<int, QList<U2AssemblyRead> > reads;

        int readCount = 0;
        while (iterator->hasNext() && readCount < READS_CHUNK_SIZE) {
            const int referenceId = iterator->peekReferenceId();
            if ((-1 == referenceId && bamInfo.isUnmappedSelected()) ||
                    bamInfo.isReferenceSelected(referenceId)) {
                U2AssemblyReadsImportInfo &importInfo = importInfos[referenceId];
                reads[referenceId] << iterator->next();
                readCount++;
                importInfo.nReads++;
            } else {
                iterator->skip();
            }
        }

        CHECK_EXT(!isCanceled(), throw CancelledException(BAMDbiPlugin::tr("Task was cancelled")), totalReadsImported);

        flushReads(reads);
        CHECK_EXT(!opStatus.isCoR(), throw Exception(opStatus.getError()), totalReadsImported);
        totalReadsImported += readCount;
    }

    return totalReadsImported;
}

void ConvertToSQLiteTask::flushReads(const QMap<int, QList<U2AssemblyRead> > &reads) {
    foreach (int index, reads.keys()) {
        if (!reads[index].isEmpty()) {
            BufferedDbiIterator<U2AssemblyRead> readsIterator(reads[index]);
            SAFE_POINT_EXT(importers.contains(index), throw Exception("An unexpected assembly"), );
            importers[index]->addReads(&readsIterator);
        }
    }
}

void ConvertToSQLiteTask::updateReferenceLengthAttribute(int length, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    U2IntegerAttribute lenAttr;
    lenAttr.objectId = assembly.id;
    lenAttr.name = U2BaseAttributeName::reference_length;
    lenAttr.version = assembly.version;
    lenAttr.value = length;

    U2OpStatusImpl status;
    attributeDbi->createIntegerAttribute(lenAttr, status);
    if (status.hasError()) {
        throw Exception(status.getError());
    }
}

void ConvertToSQLiteTask::updateReferenceMd5Attribute(const QByteArray &md5, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    CHECK(!md5.isEmpty(), );

    U2ByteArrayAttribute md5Attr;
    md5Attr.objectId = assembly.id;
    md5Attr.name = U2BaseAttributeName::reference_md5;
    md5Attr.version = assembly.version;
    md5Attr.value = md5;

    U2OpStatusImpl status;
    attributeDbi->createByteArrayAttribute(md5Attr, status);
    if (status.hasError()) {
        throw Exception(status.getError());
    }
}

void ConvertToSQLiteTask::updateReferenceSpeciesAttribute(const QByteArray &species, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    CHECK(!species.isEmpty(), );

    U2ByteArrayAttribute speciesAttr;
    speciesAttr.objectId = assembly.id;
    speciesAttr.name = U2BaseAttributeName::reference_species;
    speciesAttr.version = assembly.version;
    speciesAttr.value = species;

    U2OpStatusImpl status;
    attributeDbi->createByteArrayAttribute(speciesAttr, status);
    if (status.hasError()) {
        throw Exception(status.getError());
    }
}

void ConvertToSQLiteTask::updateReferenceUriAttribute(const QString &uri, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    CHECK(!uri.isEmpty(), );

    U2StringAttribute uriAttr;
    uriAttr.objectId = assembly.id;
    uriAttr.name = U2BaseAttributeName::reference_uri;
    uriAttr.version = assembly.version;
    uriAttr.value = uri;

    U2OpStatusImpl status;
    attributeDbi->createStringAttribute(uriAttr, status);
    if (status.hasError()) {
        throw Exception(status.getError());
    }
}

void ConvertToSQLiteTask::updateImportInfoMaxProwAttribute(const U2AssemblyReadsImportInfo &importInfo, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    const qint64 maxProw = importInfo.packStat.maxProw;

    if (maxProw > 0) {
        U2IntegerAttribute maxProwAttr;
        maxProwAttr.objectId = assembly.id;
        maxProwAttr.name = U2BaseAttributeName::max_prow;
        maxProwAttr.version = assembly.version;
        maxProwAttr.value = maxProw;

        U2OpStatusImpl opStatus;
        attributeDbi->createIntegerAttribute(maxProwAttr, opStatus);
        if (opStatus.hasError()) {
            throw Exception(opStatus.getError());
        }
    } else if (importInfo.packStat.readsCount > 0){
        // if there are reads, but maxProw == 0 => error
        taskLog.details(tr("Warning: incorrect maxProw == %1, probably packing was not done! Attribute was not set").arg(maxProw));
    }
}

void ConvertToSQLiteTask::updateImportInfoReadsCountAttribute(const U2AssemblyReadsImportInfo &importInfo, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    const qint64 readsCount = importInfo.packStat.readsCount;
    CHECK(readsCount > 0, );

    U2IntegerAttribute countReadsAttr;
    countReadsAttr.objectId = assembly.id;
    countReadsAttr.name = "count_reads_attribute";
    countReadsAttr.version = assembly.version;
    countReadsAttr.value = readsCount;

    U2OpStatusImpl opStatus;
    attributeDbi->createIntegerAttribute(countReadsAttr, opStatus);
    if (opStatus.hasError()) {
        throw Exception(opStatus.getError());
    }
}

void ConvertToSQLiteTask::updateImportInfoCoverageStatAttribute(const U2AssemblyReadsImportInfo &importInfo, const U2Assembly &assembly, U2AttributeDbi *attributeDbi) {
    const U2AssemblyCoverageStat &coverageStat = importInfo.coverageInfo.coverage;
    CHECK(!coverageStat.isEmpty(), );

    U2ByteArrayAttribute attribute;
    attribute.objectId = assembly.id;
    attribute.name = U2BaseAttributeName::coverage_statistics;
    attribute.value = U2AssemblyUtils::serializeCoverageStat(coverageStat);
    attribute.version = assembly.version;

    U2OpStatusImpl opStatus;
    attributeDbi->createByteArrayAttribute(attribute, opStatus);
    if (opStatus.hasError()) {
        throw Exception(opStatus.getError());
    }
}

IOAdapter *ConvertToSQLiteTask::prepareIoAdapter() {
    IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(sourceUrl));
    SAFE_POINT_EXT(NULL != factory, throw IOException(L10N::nullPointerError("IO adapter factory")), NULL);
    QScopedPointer<IOAdapter> ioAdapter(factory->createIOAdapter());

    if (!ioAdapter->open(sourceUrl, IOAdapterMode_Read)) {
        throw IOException(L10N::errorReadingFile(sourceUrl));
    }

    return ioAdapter.take();
}

} // namespace BAM
} // namespace U2
