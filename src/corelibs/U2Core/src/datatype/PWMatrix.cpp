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

#include "PWMatrix.h"

#include <QStringList>

namespace U2 {

UniprobeInfo::UniprobeInfo(): properties(QMap<QString, QString>()) {
}

UniprobeInfo::UniprobeInfo(const QMap<QString, QString>& props): properties(props) {
}

QString UniprobeInfo::getProperty(const QString &name) const {
    return properties.value(name, QString(""));
}

QMap<QString, QString> UniprobeInfo::getProperties() const {
    return properties;
}

UniprobeInfo::UniprobeInfo(const QString& data) {
    QStringList parsedData = data.split("\t");
    QMap<QString, QString> props;
    for (int i = 0, n = parsedData.length(); i < n; i++) {
        QString curr = parsedData[i];
        props.insert(curr.section(":", 0).trimmed(), curr.section(":", 1).trimmed());
    }
}

PWMatrix::PWMatrix()
    : length(0),
      type(PWM_MONONUCLEOTIDE),
      minSum(0),
      maxSum(0)
{

}

PWMatrix::PWMatrix(const QVarLengthArray<float>& matrix, const PWMatrixType& _type)
    : data(matrix), type(_type)
{
    length = (type == PWM_MONONUCLEOTIDE) ? (matrix.size() / 4) : (matrix.size() / 16);
    minSum = 0, maxSum = 0;

    for (int i = 0; i < length; i++) {
        float min = 1e6;
        float max = -1e6;
        for (int j = 0, n = (type == PWM_MONONUCLEOTIDE) ? 4 : 16; j < n; j++) {
            float curr = data[j * length + i];
            min = qMin(min, curr);
            max = qMax(max, curr);
        }
        minSum += min;
        maxSum += max;
    }
}

int PWMatrix::index(int row, int column) const {
    assert (row >= 0);
    assert (row < 16);
    if (type == PWM_MONONUCLEOTIDE) assert (row < 4);
    assert (column >= 0);
    assert (column < length);
    return row * length + column;
}

PWMatrixType PWMatrix::getType() const {
    return type;
}

int PWMatrix::getLength() const {
    return length;
}

float PWMatrix::getValue(int row, int column) const {
    int pos = index(row, column);
    return data[pos];
}

float PWMatrix::getMinSum() const {
    return minSum;
}

float PWMatrix::getMaxSum() const {
    return maxSum;
}

void PWMatrix::setInfo(const UniprobeInfo& _info) {
    info = _info;
}

QString PWMatrix::getProperty(const QString &p) const {
    return info.getProperty(p);
}

QMap<QString, QString> PWMatrix::getProperties() const {
    return info.getProperties();
}

}
