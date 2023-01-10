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

#ifndef _U2_IO_ADAPTER_TEXT_STREAM_H_
#define _U2_IO_ADAPTER_TEXT_STREAM_H_

#include <QBitArray>
#include <QObject>
#include <QTextStream>

#include <U2Core/GUrl.h>

#include "IOAdapter.h"

namespace U2 {
class U2OpStatus;

/**
 * IO Adapter device used by text streams.
 * Notes:
 *  The supported feature set was tested to work only with QTextStream.
 *  See original QT docs for the methods with 'override'.
 *
 */
class IOAdapterDevice : public QIODevice {
    Q_DISABLE_COPY(IOAdapterDevice)
public:
    /**
     * Creates a new unbuffered QIODevice over IOAdapter instance.
     * The 'ioAdapter' must be in 'open' state.
     * The QIODevice is writable only if the  'ioAdapter' is writable.
     */
    IOAdapterDevice(IOAdapter* ioAdapter, QObject* parent = nullptr);

    bool atEnd() const override;

    qint64 pos() const override;

    bool seek(qint64 seekPos) override;

protected:
    qint64 readData(char* data, qint64 length) override;

    qint64 writeData(const char* data, qint64 length) override;

private:
    IOAdapter* ioAdapter;
    qint64 currentPos;
};

/** Base class for reader & writer io-adapter text streams. */
class U2CORE_EXPORT IOAdapterReaderAndWriterBase {
    Q_DISABLE_COPY(IOAdapterReaderAndWriterBase)
public:
    /** Initializes underlying qt stream & io-device. If 'codec' is null uses default locale codec. */
    IOAdapterReaderAndWriterBase(IOAdapter* ioAdapter, QTextCodec* codec = nullptr);

    /** Returns url from the ioAdapter. */
    GUrl getURL() const;

    /** Returns factory from the ioAdapter. */
    IOAdapterFactory* getFactory() const;

protected:
    IOAdapter* ioAdapter;
    QScopedPointer<QIODevice> ioDevice;
    QTextStream stream;
};

/**
 * Text stream reader over the opened ioAdapter instance.
 * Automatically detects the encoding and allows reading of the binary data as a text.
 * If the codec can't be detected from the binary data falls back to the most suitable codec for the current user locale (usually UTF-8).
 */
class U2CORE_EXPORT IOAdapterReader : public IOAdapterReaderAndWriterBase {
    Q_DISABLE_COPY(IOAdapterReader)
public:
    /**
     * Initializes text stream with the ioAdapter wrapped into QIODevice.
     * Performs no actual reading from the ioAdapter.
     */
    IOAdapterReader(IOAdapter* ioAdapter);

    /** Pushes back all unused buffers to ioAdapter by calling 'skip' method. */
    ~IOAdapterReader();

    /**
     * Reads the stream until one of the separators is found. Saves the result to 'result' string buffer.
     * Clears the 'result' buffer first before reading.
     * Returns number of characters read, same as result.length().
     */
    int read(U2OpStatus& os,
             QString& result,
             int maxLength,
             const QBitArray& terminators = QBitArray(),
             IOAdapter::TerminatorHandling terminatorMode = IOAdapter::Term_Include,
             bool* terminatorFound = nullptr);

    /**
     * Reads a single line (until '\n' character) from the text stream into the 'result' buffer.
     * Excludes line terminator sequence characters ('\r' and '\n') from the result.
     *
     * Returns 'true' if the line terminator was found.
     * Returns 'false' if this is the last line in the stream or 'maxLength' was reached before the line terminator was found.
     *
     * This operation can be 'undone'.
     */
    bool readLine(U2OpStatus& os, QString& result, int maxLength);

    /**
     * Returns a single line read from the stream.
     * The method behaves exactly like readLine(os, result, ...) with a buffer,
     *  but always creates and returns a new buffer QString and returns it as the result.
     */
    QString readLine(U2OpStatus& os, int maxLength);

    /** Returns true if the end of the stream is reached. */
    bool atEnd() const;

    /*
     * Returns a progress value in the range 0..100, or a negative value if the progress is unknown/not supported
     * Proxies the call to ioAdapter.getProgress method.
     */
    int getProgress() const;

    /** Undo last read() operation. */
    void undo(U2OpStatus& os);

private:
    /** Reads a single character from the stream. Returns '\0' if end of stream is reached. */
    QChar readChar(U2OpStatus& os);

    /**
     * Puts back the last character from the 'textForUndo' into the unreadCharsBuffer.
     * Reduces size of 'textForUndo' block by 1. This method is safe to call only from the 'read()' method: read comments inside.
     */
    void unreadChar(U2OpStatus& os);

    /** The last text read during the last 'read' call. Contains all text (with separators) and is used for undo(). */
    QString textForUndo;

    /**
     * Using a local buffer to support pushBack operation.
     * The QTextStream API has no support for positioning (seek/pos) by character positions and
     *  requires developers to provide byte-level offsets (OMG!!!) for unicode text streams.
     *
     * This way it is easier to support a local 'push-back' character buffer instead
     *  of dynamic computation of chars <-> bytes and dealing with side-effects of QTextStream:pos().
     */
    QString unreadCharsBuffer;

    /** Position of the next character to return in the push-back buffer. */
    int unreadCharsBufferPos;
};

class U2CORE_EXPORT IOAdapterWriter : public IOAdapterReaderAndWriterBase {
    Q_DISABLE_COPY(IOAdapterWriter)
public:
    /** Creates new instance of a text stream with the given codec. */
    IOAdapterWriter(IOAdapter* ioAdapter, QTextCodec* codec = nullptr);

    /**
     * Writes text data to the stream.
     * Sets error flag is the was an error during the operation or 'text' was not fully written.
     */
    void write(U2OpStatus& os, const QString& text);
};

}  // namespace U2

#endif
