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

#ifndef _GENOME_ALIGNER_IO_H_
#define _GENOME_ALIGNER_IO_H_

#include <QSharedPointer>
#include <QString>

#include <U2Core/AssemblyImporter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GUrl.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/StreamSequenceReader.h>
#include <U2Formats/StreamSequenceWriter.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "GenomeAlignerSearchQuery.h"
#include "GenomeAlignerIndexPart.h"

namespace U2 {

/************************************************************************/
/* Abstract short reads reader and writer                               */
/************************************************************************/

class GenomeAlignerReader {
public:
    virtual ~GenomeAlignerReader() {}
    virtual SearchQuery *read() = 0;
    virtual bool isEnd() = 0;
    virtual int getProgress() = 0;
    //It's better to make this function pure virtual,
    //so if you need it in an inherited class, where it isn't overloaded yet,
    //you need to overload the needed func
    virtual QString getMemberError() { assert(false); return QString(); }
};

class GenomeAlignerWriter {
public:
    virtual ~GenomeAlignerWriter() {}
    virtual void write(SearchQuery *seq, SAType offset) = 0;
    virtual void close() = 0;
    virtual void setReferenceName(const QString &refName) = 0;
    quint64 getWrittenReadsCount() const {return writtenReadsCount;}
protected:
    quint64 writtenReadsCount;
    QString refName;
};

/************************************************************************/
/* Abstract container for storing in QVariant                            */
/************************************************************************/
class GenomeAlignerReaderContainer {
public:
    GenomeAlignerReaderContainer() : reader(NULL) { }
    GenomeAlignerReaderContainer(GenomeAlignerReader* reader) {
        this->reader = reader;
    }
    GenomeAlignerReader* reader;
};

class GenomeAlignerWriterContainer {
public:
    GenomeAlignerWriterContainer() : writer(NULL) {}
    GenomeAlignerWriterContainer(GenomeAlignerWriter* writer) {
        this->writer = writer;
    }
    GenomeAlignerWriter* writer;
};

/************************************************************************/
/* URL short reads reader and writer                                    */
/************************************************************************/

class GenomeAlignerUrlReader : public GenomeAlignerReader {
public:
    GenomeAlignerUrlReader(const QList<GUrl> &dnaList);
    inline SearchQuery *read();
    inline bool isEnd();
    int getProgress();
    QString getMemberError();

private:
    bool initOk;
    StreamSequenceReader reader;
};

class GenomeAlignerUrlWriter : public GenomeAlignerWriter {
public:
    GenomeAlignerUrlWriter(const GUrl &resultFile, const QString &refName, int refLength);
    inline void write(SearchQuery *seq, SAType offset);
    void close();
    void setReferenceName(const QString &refName);
private:
    StreamShortReadsWriter seqWriter;
};

/************************************************************************/
/* Workflow short reads reader and writer                               */
/************************************************************************/
namespace LocalWorkflow {

class GenomeAlignerCommunicationChanelReader : public GenomeAlignerReader {
public:
    GenomeAlignerCommunicationChanelReader(CommunicationChannel* reads);
    ~GenomeAlignerCommunicationChanelReader();
    inline SearchQuery *read();
    inline bool isEnd();
    int getProgress();
private:
    CommunicationChannel* reads;
};

class GenomeAlignerMsaWriter : public GenomeAlignerWriter {
public:
    GenomeAlignerMsaWriter();
    inline void write(SearchQuery *seq, SAType offset);
    void close();
    void setReferenceName(const QString &refName);
    MultipleSequenceAlignment &getResult();
private:
    MultipleSequenceAlignment result;
};

} //LocalWorkflow

/************************************************************************/
/* DBI short reads reader and writer                                    */
/************************************************************************/
class GenomeAlignerDbiReader : public GenomeAlignerReader {
public:
    GenomeAlignerDbiReader(U2AssemblyDbi *rDbi, U2Assembly assembly);
    inline SearchQuery *read();
    inline bool isEnd();
    int getProgress();
private:
    bool end;
    U2AssemblyDbi *rDbi;
    U2Assembly assembly;
    QList<U2AssemblyRead> reads;
    U2Region wholeAssembly;
    U2OpStatusImpl status;
    QList<U2AssemblyRead>::Iterator currentRead;
    qint64 readNumber;
    qint64 maxRow;
    qint64 readsInAssembly;
    QScopedPointer< U2DbiIterator<U2AssemblyRead> > dbiIterator;

    static const qint64 readBunchSize;
};

class GenomeAlignerDbiWriter : public GenomeAlignerWriter {
public:
    GenomeAlignerDbiWriter(const QString &dbiFilePath, const QString &assemblyName, int refLength,
                           const QString& referenceObjectName = QString(),
                           const QString& referenceUrlForCrossLink = QString());
    inline void write(SearchQuery *seq, SAType offset);
    void close();
    void setReferenceName(const QString &) {}
private:
    U2OpStatusImpl status;
    QSharedPointer<DbiConnection> dbiHandle;
    U2Dbi* sqliteDbi;
    AssemblyImporter importer;
    U2AssemblyDbi *wDbi;
    U2Assembly assembly;
    QList<U2AssemblyRead> reads;
    QMutex writeLock;

    static const qint64 readBunchSize;
};

bool checkAndLogError(const U2OpStatusImpl & status);

} //U2
Q_DECLARE_METATYPE(U2::GenomeAlignerReaderContainer);
Q_DECLARE_METATYPE(U2::GenomeAlignerWriterContainer);
#endif //_GENOME_ALIGNER_IO_H_
