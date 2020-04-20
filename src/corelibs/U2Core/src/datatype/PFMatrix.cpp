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

#include "PFMatrix.h"
#include "DIProperties.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2OpStatusUtils.h>


namespace U2 {

JasparInfo::JasparInfo(): properties(QMap<QString, QString>()) {
}

JasparInfo::JasparInfo(const QMap<QString, QString>& props): properties(props) {
}

JasparInfo::JasparInfo(const QString& line) {
    QStringList parsedData = line.split(";");
    QString idData = parsedData.first();
    QStringList base = idData.split(QRegExp("\\s"));
    QString id = base[0];
    properties.insert(QString("id"), id);
    QString name = base[2];
    properties.insert(QString("name"), name);
    base.removeFirst();
    base.removeFirst();
    base.removeFirst();
    QString clss = base.join(" ");
    properties.insert(QString("class"), clss);
    for (int i = 1, n = parsedData.length(); i < n; i++) {
        QString curr = parsedData[i];
        int left = curr.indexOf("\"");
        QString propName = curr.left(left).trimmed();
        int right = curr.indexOf("\"", left + 1);
        QString propValue = curr.mid(left + 1, right - left - 1);
        properties.insert(propName, propValue);
    }
}

QString JasparInfo::getProperty(const QString &name) const {
    return properties.value(name);
}

QMap<QString, QString> JasparInfo::getProperties() const {
    return properties;
}

PFMatrix::PFMatrix(const MultipleSequenceAlignment &align, PFMatrixType _type): type(_type) {
    assert(align->hasEqualLength());
    const int sequenceLength = align->getMsaRows().first()->getUngappedLength();
    length = (type == PFM_MONONUCLEOTIDE) ? sequenceLength : sequenceLength - 1;
    assert (length > 0);
    int size = (type == PFM_MONONUCLEOTIDE) ? 4 : 16;
    data.resize(size * length);
    memset(data.data(), 0, size*length*sizeof(int));
    U2OpStatus2Log os;
    if (type == PFM_MONONUCLEOTIDE) {
        for (int i = 0, n = align->getNumRows(); i < n; i++) {
            const QByteArray row = align->getMsaRow(i)->getSequence().seq;
            for (int j = 0; j < length; j++) {
                char curr = row[j];
                data[DiProperty::index(curr) * length + j]++;
            }
        }
    } else {
        for (int i = 0, n = align->getNumRows(); i < n; i++) {
            const QByteArray row = align->getMsaRow(i)->getSequence().seq;
            for (int j = 0; j < length; j++) {
                char curr = row[j];
                char next = row[j+1];
                data[DiProperty::index(curr, next) * length + j]++;
            }
        }
    }
}

PFMatrix::PFMatrix(const QList<DNASequence *> &seq, PFMatrixType _type) : type(_type) {
    assert(seq.length() > 0);

    length = seq[0]->length();
    for (int i = 0, n = seq.length(); i < n; i++) {
        assert (seq[i]->alphabet->getType() == DNAAlphabet_NUCL);
        assert (length == seq[i]->length());
    }
    int size = (type == PFM_MONONUCLEOTIDE) ? 4 : 16;
    length = (type == PFM_MONONUCLEOTIDE) ? seq[0]->length() : seq[0]->length() - 1;
    data.resize(size * length);
    memset(data.data(), 0, size*length*sizeof(int));
    if (type == PFM_MONONUCLEOTIDE) {
        for (int i = 0, n = seq.size(); i < n; i++) {
            const QByteArray& row = seq[i]->seq;
            for (int j = 0; j < length; j++) {
                char curr = row[j];
                data[DiProperty::index(curr) * length + j]++;
            }
        }
    } else {
        for (int i = 0, n = seq.size(); i < n; i++) {
            const QByteArray& row = seq[i]->seq;
            for (int j = 0; j < length; j++) {
                char curr = row[j];
                char next = row[j+1];
                data[DiProperty::index(curr, next) * length + j]++;
            }
        }
    }
}

PFMatrix::PFMatrix(const QVarLengthArray<int>& _data, const PFMatrixType _type):
    type(_type) {
    assert (_data.size() % 4 == 0);
    if (_type == PFM_DINUCLEOTIDE) {
        assert (_data.size() % 16 == 0);
    }
    data = _data;
    length = (_type == PFM_MONONUCLEOTIDE) ? (_data.size() / 4) : (_data.size() / 16);
}

PFMatrix PFMatrix::convertDi2Mono(const PFMatrix& source) {
    assert (source.getType() == PFM_DINUCLEOTIDE);
    int size = 4;
    int len = source.getLength();
    QVarLengthArray<int> matrix(size*(len+1));
    memset(matrix.data(), 0, size*(len+1)*sizeof(int));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < len; j++) {
            for (int k = 0; k < 4; k++) {
                matrix[i*(len+1) + j] += source.data[source.index((i << 2) + k, j)];
            }
        }
        for (int k = 0; k < 4; k++) {
            matrix[i*(len+1) + len] += source.data[source.index((k << 2) + i, len - 1)];
        }
    }
    return PFMatrix(matrix, PFM_MONONUCLEOTIDE);
}


int PFMatrix::index(int row, int column) const {
    assert (row >= 0);
    assert (row < 16);
    if (type == PFM_MONONUCLEOTIDE) assert (row < 4);
    assert (column >= 0);
    assert (column < length);
    return row * length + column;
}

PFMatrixType PFMatrix::getType() const {
    return type;
}

int PFMatrix::getLength() const {
    return length;
}

int PFMatrix::getValue(int row, int column) const {
    int pos = index(row, column);
    return data[pos];
}

void PFMatrix::setInfo(const U2::JasparInfo &in){
    info = in;
}

QString PFMatrix::getProperty(const QString &propertyName) const {
    return info.getProperty(propertyName);
}

QMap<QString, QString> PFMatrix::getProperties() const {
    return info.getProperties();
}

}
