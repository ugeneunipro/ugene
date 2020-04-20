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

#include <U2Core/U2SafePoints.h>

#include "ColumnDataParser.h"

namespace U2 {

ColumnDataParser::Column::Column(const QString &_name,
    ColumnDataParser::ColumnType _type, const QString _defaultValue, bool _required)
: name(_name), type(_type), defaultValue(_defaultValue), required(_required)
{

}

ColumnDataParser::Column::Column(const QString &_name)
: name(_name), type(STRING)
{
    required = false;
}

bool ColumnDataParser::Column::operator == (const ColumnDataParser::Column &other) const {
    return name == other.name;
}

ColumnDataParser::Iterator::Iterator(const QList<Column> &_columns, const QStringList &_values)
: columns(_columns), values(_values)
{
    currentNum = 0;
    end = columns.size();
    if (columns.size() != values.size()) {
        end = 0;
    }
}

bool ColumnDataParser::Iterator::check() const {
    SAFE_POINT(currentNum < end, "Iterator is ended", false);
    return true;
}

bool ColumnDataParser::Iterator::isEnded() const {
    return currentNum < end;
}

ColumnDataParser::ColumnType ColumnDataParser::Iterator::currentType() const {
    CHECK(check(), STRING);
    return columns[currentNum].type;
}

QString ColumnDataParser::Iterator::currentName() const {
    CHECK(check(), "");
    return columns[currentNum].name;
}

QString ColumnDataParser::Iterator::takeString() {
    return take();
}

int ColumnDataParser::Iterator::takeInt(U2OpStatus &os) {
    bool ok = true;
    QString str = take();
    int result = str.toInt(&ok);
    if (!ok) {
        os.setError(QString("Can not parse integer from %1").arg(str));
    }
    return result;
}

double ColumnDataParser::Iterator::takeDouble(U2OpStatus &os) {
    bool ok = true;
    QString str = take();
    double result = str.toDouble(&ok);
    if (!ok) {
        os.setError(QString("Can not parse double from %1").arg(str));
    }
    return result;
}

QString ColumnDataParser::Iterator::take() {
    CHECK(check(), "");
    previous = current;
    current = values[currentNum];
    currentNum++;
    return current;
}

QString ColumnDataParser::Iterator::getPrevious() const {
    return previous;
}

QString ColumnDataParser::Iterator::look() const {
    CHECK(check(), "");
    return values[currentNum];
}

ColumnDataParser::ColumnDataParser(const QList<Column> &_formatColumns, const QString &_separator)
: inited(false), formatColumns(_formatColumns), separator(_separator)
{

}

void ColumnDataParser::init(const QString &headerLine, U2OpStatus &os) {
    QStringList names = headerLine.split(separator, QString::SkipEmptyParts);
    foreach (const QString &name, names) {
        if (formatColumns.contains(Column(name))) {
            foreach (const Column &c, formatColumns) {
                if (name == c.name) {
                    currentColumns << c;
                    break;
                }
            }
        } else {
            os.setError(QString("Unknown column name: %1").arg(name));
        }
    }
    // check required colums
    foreach (const Column &c, formatColumns) {
        if (c.required && !currentColumns.contains(c)) {
            os.setError(QString("Required column is missed: %1").arg(c.name));
            return;
        }
    }
    inited = true;
}

ColumnDataParser::Iterator ColumnDataParser::parseLine(const QString &line, U2OpStatus &os) const {
    SAFE_POINT(inited, "ColumnDataParser is not inited", Iterator(QList<Column>(), QStringList()));
    QStringList values = line.split(separator, QString::SkipEmptyParts);
    if (currentColumns.size() != values.size()) {
        os.setError("Wrong columns count");
        return Iterator(QList<Column>(), QStringList());
    }
    return Iterator(currentColumns, values);
}

const QList<ColumnDataParser::Column> & ColumnDataParser::getCurrentColumns() const {
    return currentColumns;
}

} // U2
