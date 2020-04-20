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

#ifndef _U2_TABULATED_FORMAT_READER_H_
#define _U2_TABULATED_FORMAT_READER_H_

#include <QObject>
#include <QStringList>

#include <U2Core/global.h>

namespace U2 {

class IOAdapter;
class U2OpStatus;

class U2FORMATS_EXPORT TabulatedFormatReader : public QObject {
    Q_OBJECT
public:
    TabulatedFormatReader(U2OpStatus &os, IOAdapter *ioAdapter);

    bool hasNextLine() const;
    QStringList getNextLine();
    qint64 getCurrentLineNumber() const;

    const QStringList &getComments() const;

private:
    void readNextLine();
    QString read();
    static bool isComment(const QString &line);
    void storeLine(const QString &line);

    IOAdapter *ioAdapter;

    QStringList storedLine;
    QStringList comments;

    static const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    qint64 currentLine;
};

}   // namespace U2

#endif // _U2_TABULATED_FORMAT_READER_H_
