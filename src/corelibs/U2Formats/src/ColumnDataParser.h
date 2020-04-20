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

#ifndef _U2_COLUMNDATAPARSER_H_
#define _U2_COLUMNDATAPARSER_H_

#include <QStringList>
#include <U2Core/global.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class ColumnDataParser {
public:
    enum ColumnType {
        STRING,
        INTEGER,
        DOUBLE
    };
    struct Column {
        Column(const QString &name);
        Column(const QString &name, ColumnType type,
            const QString defaultValue = "-", bool required = false);
        QString name;
        ColumnType type;
        QString defaultValue;
        bool required;

        bool operator == (const Column &other) const;
    };
    class Iterator {
        friend class ColumnDataParser;
    public:
        bool isEnded() const;
        ColumnType currentType() const;
        QString currentName() const;
        QString takeString();
        int takeInt(U2OpStatus &os);
        double takeDouble(U2OpStatus &os);
        QString getPrevious() const;
        QString look() const;
    private:
        Iterator(const QList<Column> &columns, const QStringList &values);
        QString take();
        bool check() const;

    private:
        const QList<Column> columns;
        const QStringList values;
        int currentNum;
        int end;
        QString current;
        QString previous;
    };

ColumnDataParser(const QList<Column> &formatColumns, const QString &separator);
    void init(const QString &headerLine, U2OpStatus &os);
    Iterator parseLine(const QString &line, U2OpStatus &os) const;
    const QList<Column> & getCurrentColumns() const;

private:
    bool inited;
    const QList<Column> formatColumns;
    QString separator;
    QList<Column> currentColumns;
};

} // U2

#endif // _U2_COLUMNDATAPARSER_H_
