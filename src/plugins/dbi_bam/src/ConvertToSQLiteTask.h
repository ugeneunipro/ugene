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

#ifndef _U2_BAM_CONVERT_TO_SQLITE_TASK_H_
#define _U2_BAM_CONVERT_TO_SQLITE_TASK_H_

#include <U2Core/AssemblyImporter.h>
#include <U2Core/GUrl.h>
#include <U2Core/Task.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2Type.h>

#include "LoadBamInfoTask.h"

namespace U2 {

class IOAdapter;

namespace BAM {

class BamReader;
class Iterator;
class Reader;
class SamReader;

class ConvertToSQLiteTask : public Task {
    Q_OBJECT
public:
    ConvertToSQLiteTask(const GUrl &sourceUrl, const U2DbiRef &dstDbiRef, BAMInfo& bamInfo, bool sam);
    virtual void run();

    GUrl getDestinationUrl() const;
    QList<U2Assembly> getAssemblies() const;

private:
    bool isSorted(Reader *reader) const;

    qint64 importReads();
    void packReads();
    void updateAttributes();

    qint64 importSortedReads(SamReader *samReader, BamReader *bamReader, Reader *reader, IOAdapter *ioAdapter);
    qint64 importMappedSortedReads(BamReader *bamReader, Reader *reader, Iterator *iterator, IOAdapter *ioAdapter);
    qint64 importUnmappedSortedReads(BamReader *bamReader, Reader *reader, QScopedPointer<Iterator> &iterator, IOAdapter *ioAdapter);

    qint64 importUnsortedReads(SamReader *samReader, BamReader *bamReader, Reader *reader, QMap<int, U2::U2AssemblyReadsImportInfo> &importInfos);
    void createAssemblyObjectForUnsortedReads(int referenceId, Reader *reader, QMap<int, U2::U2AssemblyReadsImportInfo> &importInfos);
    qint64 importReadsSequentially(Iterator *iterator);
    void flushReads(const QMap<int, QList<U2AssemblyRead> > &reads);

    void updateReferenceLengthAttribute(int length, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);
    void updateReferenceMd5Attribute(const QByteArray &md5, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);
    void updateReferenceSpeciesAttribute(const QByteArray &species, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);
    void updateReferenceUriAttribute(const QString &uri, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);

    void updateImportInfoMaxProwAttribute(const U2AssemblyReadsImportInfo &importInfo, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);
    void updateImportInfoReadsCountAttribute(const U2AssemblyReadsImportInfo &importInfo, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);
    void updateImportInfoCoverageStatAttribute(const U2AssemblyReadsImportInfo &importInfo, const U2Assembly &assembly, U2AttributeDbi *attributeDbi);

    IOAdapter *prepareIoAdapter();

    const GUrl sourceUrl;
    const U2DbiRef dstDbiRef;
    BAMInfo bamInfo;

    bool sam;

    QList<Header::Reference> references;
    QMap<int, U2AssemblyReadsImportInfo> importInfos;
    QMap<int, AssemblyImporter *> importers;
    QList<U2Assembly> importedAssemblies;
};

class Iterator {
public:
    virtual ~Iterator() {}

    virtual bool hasNext() = 0;

    virtual U2AssemblyRead next() = 0;

    virtual void skip() = 0;

    virtual const U2AssemblyRead &peek() = 0;

    virtual int peekReferenceId() = 0;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_CONVERT_TO_SQLITE_TASK_H_
