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

#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "TabulatedFormatReader.h"

namespace U2 {

TabulatedFormatReader::TabulatedFormatReader(U2OpStatus &os, IOAdapter *ioAdapter) :
    ioAdapter(ioAdapter),
    currentLine(0)
{
    CHECK_EXT(NULL != ioAdapter, os.setError(L10N::nullPointerError("IO adapter")), );
    CHECK_EXT(ioAdapter->isOpen(), os.setError(tr("IO adapter is not opened")), );

    readNextLine();
}

bool TabulatedFormatReader::hasNextLine() const {
    return !storedLine.isEmpty();
}

QStringList TabulatedFormatReader::getNextLine() {
    QStringList line = storedLine;
    readNextLine();
    return line;
}

qint64 TabulatedFormatReader::getCurrentLineNumber() const {
    return currentLine;
}

const QStringList & TabulatedFormatReader::getComments() const {
    return comments;
}

void TabulatedFormatReader::readNextLine() {
    storedLine.clear();

    do {
        const QString line = read();
        CHECK(!line.isEmpty(), );
        storeLine(line);
    } while (storedLine.isEmpty() && !ioAdapter->isEof());
}

QString TabulatedFormatReader::read() {
    int len = 0;
    bool terminatorFound = false;
    QString line;
    do {
        len = ioAdapter->readLine(buffer, BUFFER_SIZE - 1, &terminatorFound);
        buffer[len] = '\0';
        line += buffer;
    } while (len == BUFFER_SIZE - 1 && !terminatorFound);
    return line;
}

bool TabulatedFormatReader::isComment(const QString &line) {
    return line.startsWith('#');
}

void TabulatedFormatReader::storeLine(const QString &line) {
    if (isComment(line)) {
        comments << line;
    } else {
        storedLine = line.split('\t');
    }
    currentLine++;
}

}   // namespace U2
