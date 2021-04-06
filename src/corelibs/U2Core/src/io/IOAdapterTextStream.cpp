/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "IOAdapterTextStream.h"

#include <QTextCodec>

#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

///////////////////////////////////////////////
////      IOAdapterDevice
///////////////////////////////////////////////
IOAdapterDevice::IOAdapterDevice(IOAdapter *_ioAdapter, QObject *parent)
    : QIODevice(parent), ioAdapter(_ioAdapter), currentPos(0) {
    SAFE_POINT(ioAdapter->isOpen(), "IOAdapter must be opened", );
    bool isReadable = ioAdapter->isIOModeSupported(IOAdapterMode_Read);
    bool isWritable = ioAdapter->isIOModeSupported(IOAdapterMode_Write);
    SAFE_POINT(isReadable || isWritable, "IOAdapter must support read, write or both ops", );

    OpenMode deviceMode(QIODevice::Unbuffered);
    if (isReadable && isWritable) {
        deviceMode.setFlag(QIODevice::ReadWrite);
    } else if (isWritable) {
        deviceMode.setFlag(QIODevice::WriteOnly);
    } else {
        deviceMode.setFlag(QIODevice::ReadOnly);
    }
    QIODevice::open(deviceMode);
}

bool IOAdapterDevice::atEnd() const {
    return ioAdapter->isEof();
}

qint64 IOAdapterDevice::readData(char *data, qint64 length) {
    qint64 bytesRead = ioAdapter->readBlock(data, length);
    currentPos += bytesRead;
    return bytesRead;
}

qint64 IOAdapterDevice::writeData(const char *data, qint64 length) {
    qint64 bytesWritten = ioAdapter->writeBlock(data, length);
    currentPos += bytesWritten;
    return bytesWritten;
}

qint64 IOAdapterDevice::pos() const {
    return currentPos;
}

bool IOAdapterDevice::seek(qint64 seekPos) {
    qint64 delta = seekPos - currentPos;
    bool isOk = ioAdapter->skip(delta);
    if (isOk) {
        currentPos = seekPos;
    }
    return isOk;
}

///////////////////////////////////////////////
////      IOAdapterReaderAndWriterBase
///////////////////////////////////////////////
IOAdapterReaderAndWriterBase::IOAdapterReaderAndWriterBase(IOAdapter *_ioAdapter, QTextCodec *codec)
    : ioAdapter(_ioAdapter) {
    ioDevice.reset(new IOAdapterDevice(ioAdapter));
    stream.setDevice(ioDevice.data());

    // Set the provided codec. If no codec is provided use autodetection and UTF-8 as the default.
    if (codec != nullptr) {
        stream.setCodec(codec);
    } else {
        stream.setCodec("UTF-8");
    }
}

GUrl IOAdapterReaderAndWriterBase::getURL() const {
    return ioAdapter->getURL();
}

IOAdapterFactory *IOAdapterReaderAndWriterBase::getFactory() const {
    return ioAdapter->getFactory();
}

///////////////////////////////////////////////
////      IOAdapterReader
///////////////////////////////////////////////
IOAdapterReader::IOAdapterReader(IOAdapter *ioAdapter)
    : IOAdapterReaderAndWriterBase(ioAdapter), unreadCharsBufferPos(0) {
}

IOAdapterReader::~IOAdapterReader() {
    if (unreadCharsBufferPos < unreadCharsBuffer.length() && ioAdapter->isOpen()) {
        // Roll IOAdapter back to the size of the data left in the unreadCharsBuffer.
        // This is needed to correctly support streaming (reading multiple documents from the same file using IOAdapterReader)
        int nCharsToReturn = unreadCharsBuffer.length() - unreadCharsBufferPos;
        int nBytesToReturn = stream.codec()->fromUnicode(unreadCharsBuffer.right(nCharsToReturn)).length();
        stream.seek(stream.pos() - nBytesToReturn);
    }
}

int IOAdapterReader::read(U2OpStatus &os, QString &result, int maxLength, const QBitArray &terminators, IOAdapter::TerminatorHandling terminatorMode, bool *terminatorFound) {
    CHECK_OP(os, 0);
    result.clear();
    textForUndo.clear();
    bool isReadingTerminatorSequence = false;
    while (!stream.atEnd() && result.length() != maxLength) {
        QChar unicodeChar = readChar(os);
        CHECK_OP(os, 0);
        uchar latin1Char = unicodeChar.toLatin1();
        bool isTerminatorChar = terminators.at(latin1Char);
        if (isTerminatorChar) {
            isReadingTerminatorSequence = true;
            if (terminatorFound != nullptr) {
                *terminatorFound = true;
            }
            if (terminatorMode == IOAdapter::Term_Exclude) {
                unreadChar();    // Push back the current terminator char and break.
                break;
            } else if (terminatorMode == IOAdapter::Term_Skip) {
                continue;
            }
            // The terminator char will be included below as a normal one.
        } else if (isReadingTerminatorSequence) {
            unreadChar();    // Push back the current non-terminator char and break.
            break;
        }
        result.append(unicodeChar);
    }

    if (ioAdapter->hasError()) {
        os.setError(ioAdapter->errorString());
    }
    return result.length();
}

bool IOAdapterReader::readLine(U2OpStatus &os, QString &result, int maxLength) {
    bool terminatorsFound = false;
    result.clear();
    read(os, result, maxLength, TextUtils::LINE_BREAKS, IOAdapter::Term_Exclude, &terminatorsFound);
    if (terminatorsFound) {
        // Skip all chars from the TextUtils::LINE_BREAKS. Stop on '\n'.
        // '\n' is the last character on supported OSes: '\n' on Linux and MacOS and '\r\n' on Windows.
        while (!atEnd() && readChar(os) != '\n') {
            CHECK_OP(os, false);
        }
    }
    return terminatorsFound;
}

QString IOAdapterReader::readLine(U2OpStatus &os, int maxLength) {
    QString result;
    readLine(os, result, maxLength);
    return result;
}

QChar IOAdapterReader::readChar(U2OpStatus &os) {
    QChar ch;
    if (unreadCharsBuffer.isEmpty()) {
        stream >> ch;
    } else {
        SAFE_POINT_EXT(unreadCharsBufferPos < unreadCharsBuffer.size(), os.setError(L10N::internalError()), 0);
        ch = unreadCharsBuffer[unreadCharsBufferPos];
        unreadCharsBufferPos++;
        if (unreadCharsBufferPos == unreadCharsBuffer.length()) {
            unreadCharsBufferPos = 0;
            unreadCharsBuffer.clear();
        }
    }
    textForUndo.append(ch);
    return ch;
}

void IOAdapterReader::unreadChar() {
    SAFE_POINT(!textForUndo.isEmpty(), "Can't unreadChar(), textForUndo is empty!", );
    SAFE_POINT(unreadCharsBuffer.isEmpty(), "unreadCharsBuffer must be empty during unreadChar()", );
    unreadCharsBuffer.append(textForUndo[textForUndo.length() - 1]);
}

void IOAdapterReader::undo() {
    SAFE_POINT(!textForUndo.isEmpty(), "Only 1 undo() per read() is supported!", );
    SAFE_POINT(unreadCharsBuffer.length() <= 1, "unreadCharsBuffer size must be <= 1!", );
    unreadCharsBuffer.clear();
    unreadCharsBuffer.append(textForUndo);
    unreadCharsBufferPos = 0;
    textForUndo.clear();
}

int IOAdapterReader::getProgress() const {
    return ioAdapter->getProgress();
}

bool IOAdapterReader::atEnd() const {
    return unreadCharsBuffer.isEmpty() && stream.atEnd();
}

///////////////////////////////////////////////
////      IOAdapterWriter
///////////////////////////////////////////////
IOAdapterWriter::IOAdapterWriter(IOAdapter *ioAdapter, QTextCodec *codec)
    : IOAdapterReaderAndWriterBase(ioAdapter, codec) {
}

void IOAdapterWriter::write(U2OpStatus &os, const QString &text) {
    CHECK_OP(os, );
    stream << text;
    if (ioAdapter->hasError()) {
        os.setError(ioAdapter->errorString());
    }
}

}    // namespace U2
