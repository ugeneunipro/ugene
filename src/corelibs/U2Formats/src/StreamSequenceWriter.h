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

#ifndef _U2_STREAM_SEQUENCE_WRITER_H_
#define _U2_STREAM_SEQUENCE_WRITER_H_

#include <QList>
#include <QString>

#include <U2Core/GUrl.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Formats/SAMFormat.h>

namespace U2 {


/**
*
* Class provides stream writing for large alignment files.
* It supports only SAM format.
*
*/

class U2FORMATS_EXPORT StreamShortReadsWriter {
    IOAdapter*  io;
    SAMFormat   format;
    int         numSeqWritten;
    int         refSeqLength;
    QByteArray  refSeqName;

public:
    StreamShortReadsWriter(const GUrl& url, const QString& refName = QString(), int refLength = 0);
    ~StreamShortReadsWriter();
    bool writeNextAlignedRead(int offset, const DNASequence& seq);
    void setRefSeqLength(int l) { refSeqLength = l;}
    void setRefSeqName(const QString& name ) { refSeqName = QString(name).replace(QRegExp("\\s|\\t"), "_").toLatin1(); }
    int getNumSeqWritten() { return numSeqWritten; }
    void close();
};

/**
*
* Class provides stream writing for DNA short reads.
* It currently supports FASTA formats and allows to write quality in separate file.
*
*/

class FastaFormat;

class U2FORMATS_EXPORT StreamShortReadWriter {
public:
    StreamShortReadWriter();
    virtual ~StreamShortReadWriter();

    bool init(const GUrl& url);
    bool writeNextSequence(const DNASequence& seq);
    bool writeNextSequence(const U2SequenceObject *seq);
    const GUrl& getOutputPath() { return ouputPath; }
    void close();

protected:
    IOAdapter* io;

private:
    FastaFormat* fastaFormat;
    GUrl ouputPath;
};

class U2FORMATS_EXPORT StreamGzippedShortReadWriter : public StreamShortReadWriter {
public:
    StreamGzippedShortReadWriter();
};


} //namespace

#endif //_U2_STREAM_SEQUENCE_WRITER_H_
