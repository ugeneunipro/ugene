/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_STREAM_SEQUENCE_READER_H_
#define _U2_STREAM_SEQUENCE_READER_H_

#include <QList>
#include <QString>

#include <U2Core/GUrl.h>
#include <U2Core/Task.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequence.h>

namespace U2 {

class Document;
class DocumentFormat;
class IOAdapter;

/**
*
* Class provides stream reading for large sequence files.
* For example, dna assembly short reads usually are
* of size 1GB and more, it is impossible to store whole file in RAM.
* Note, that document format has to support DocumentReadMode_SingleObject
* to be read by StreamSequenceReader.
* In case of multiple files, they will be read subsequently.
*
*/

class U2FORMATS_EXPORT StreamSequenceReader : public QObject {
    struct ReaderContext {
        ReaderContext() : io(NULL), format(NULL) {}
        IOAdapter* io;
        DocumentFormat* format;
    };
    QList<ReaderContext> readers;
    int currentReaderIndex;
    QScopedPointer<DNASequence> currentSeq;
    bool errorOccured;
    bool lookupPerformed;
    QString errorMessage;
    TaskStateInfo taskInfo;

public:
    StreamSequenceReader();
    ~StreamSequenceReader();

    bool init(const QStringList& urls);
    bool init(const QList<GUrl>& urls);

    const IOAdapter* getIO() const;
    DocumentFormat* getFormat() const;

    bool hasNext();
    bool hasError() { return errorOccured; }
    int getProgress();
    QString getErrorMessage();
    DNASequence* getNextSequenceObject();
};


} //namespace

#endif //_U2_STREAM_SEQUENCE_READER_H_
