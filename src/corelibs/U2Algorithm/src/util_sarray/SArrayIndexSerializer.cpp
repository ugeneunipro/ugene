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

#include <QFile>

#include "SArrayIndexSerializer.h"

namespace U2 {

const QString SArrayIndexSerializer::SARRAY_HEADER("#UGENE suffix array index\n");
const QString SArrayIndexSerializer::SARRAY_PARAMETERS("#file \"%1\", sequence's length = %2, w = %3, gap offset = %4\n");

void SArrayIndexSerializer::serialize(const SArrayIndex *index, const QString &indexFileName, const QString &refFileName) {
    assert(NULL != index);
    QFile file(indexFileName);
    if (indexFileName.isEmpty() || !file.open(QIODevice::WriteOnly)) {
        return;
    }
    QByteArray data;
    data = SARRAY_HEADER.toLatin1();
    data += SARRAY_PARAMETERS.arg(refFileName).arg(index->seqLen).arg(index->w).arg(index->gapOffset).toLatin1();

    data += QByteArray::number(index->w, 10) + ", ";
    data += QByteArray::number(index->w4, 10) + ", ";
    data += QByteArray::number(index->wRest, 10) + ", ";
    data += QByteArray::number(index->skipGap, 10) + ", ";
    data += QByteArray::number(index->gapOffset, 10) + ", ";
    data += QByteArray::number(index->arrLen, 10) + ", ";
    data += QByteArray::number(index->seqLen, 10) + ", ";
    data += QByteArray::number(index->bitFilter, 10) + ", ";
    data += QByteArray::number(index->wCharsInMask, 10) + ", ";
    data += QByteArray::number(index->wAfterBits, 10) + ", ";
    data += QByteArray::number(index->l1Step, 10) + ", ";
    data += QByteArray::number(index->L1_SIZE, 10) + "\n";
    file.write(data);

    QByteArray writeBuffer(BUFF_SIZE, '\0');
    char *buff = writeBuffer.data();

    writeArray(file, buff, index->sArray, index->arrLen);

    if (NULL != index->bitMask) {
        writeArray(file, buff, index->bitMask, index->arrLen);
        writeArray(file, buff, index->l1bitMask, index->L1_SIZE);
    }
    file.close();

}

void SArrayIndexSerializer::writeArray(QFile &file, char *buff, quint32 *array, int len) {
    int pos = 0;
    QByteArray line;
    quint32 number = 0;
    quint32 tmpNum = 0;
    char c = 0;
    long long divider = 10;
    int numberLength = 0;
    for (int i = 0; i < len; ++i) {
        number = array[i];
        if (0 == number) {
            divider = 10;
            numberLength = 1;
        } else {
            tmpNum = number;
            divider = 1;
            numberLength = 0;
            while (0 != tmpNum) {
                tmpNum /=10;
                divider *= 10;
                numberLength++;
            }
            divider /= 10;
        }
        assert(numberLength > 0);
        while (1) {
            if (pos == BUFF_SIZE) {
                file.write(buff, BUFF_SIZE);
                pos = 0;
            }
            if (0 == numberLength) {
                buff[pos] = '\n';
                pos++;
                break;
            } else {
                tmpNum = number/divider;
                c = tmpNum + '0';
                buff[pos] = c;
                pos++;
                number = number - tmpNum*divider;
                numberLength--;
                divider /= 10;
            }
        }
    }
    file.write(buff, pos);
}

inline int getNextInt(QByteArray &data, bool &eol, bool &intErr) {
    int commaIdx = data.indexOf(',');
    if (-1 == commaIdx) {
        commaIdx = data.length();
        eol = true;
    }

    QByteArray result = data.left(commaIdx).trimmed();
    data = data.mid(commaIdx+1).trimmed();

    char c = 0;
    for (int i = 0; i < result.length(); i++) {
        c = result[i];
        if (c <'0' || c>'9') {
            intErr = true;
            return -1;
        }
    }
    return result.toInt();
}

void SArrayIndexSerializer::readArray(QFile &file, char *buff, int *len, int *pos, int *bytes,
    int *lineIdx, quint32 *array, int arrLen, TaskStateInfo& ti) {
        quint32 number = 0;
        int numberLength = 0;
        bool newLine = true; Q_UNUSED(newLine); //used only in assertions
        for (int i = 0; i < arrLen; i++) {
            number = 0;
            numberLength = 0;
            assert(newLine);
            while (1) {
                if (*pos == *len) {
                    ti.progress = (int)(100 *(*bytes/(double)file.size()));
                    *len = file.read(buff, BUFF_SIZE);
                    *bytes += *len;
                    *pos = 0;
                    if (*len <= 0) {
                        break;
                    }
                    if (ti.cancelFlag) {
                        return;
                    }
                }
                char c = buff[*pos];
                if ('\n' == c || 0 == c) {
                    (*pos)++;
                    (*lineIdx)++;
                    newLine = true;
                    break;
                } else if (c >='0'  && c<='9') {
                    number = number*10+(c-'0');
                    (*pos)++;
                    numberLength++;
                    newLine = false;
                } else {
                    ti.setError(QString("Not digit in the number at line %1").arg(*lineIdx));
                    return;
                }
            }

            if (0 == *len && (arrLen - 1) != i) {
                ti.setError("There is not enough array's values it the file-index");
                return;
            }
            if (0 == numberLength) {
                ti.setError(QString("Empty array's value at line %1").arg(*lineIdx-1));
                return;
            }
            array[i] = number;
        }
}

void SArrayIndexSerializer::deserialize(SArrayIndex *index, const QString &indexFileName, TaskStateInfo& ti) {
    QFile file(indexFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        ti.setError("Can't open file-index");
        return;
    }
    QByteArray data;
    bool eol = false;
    bool intErr = false;
    int lineIdx = 0;

    do {
        data = file.readLine().trimmed();
        lineIdx++;
    } while (data.length() > 0 && '#' == data[0]);

    if (0 == data.length()) {
        ti.setError("Empty parameters' line in the file-index");
        return;
    }
    index->w = getNextInt(data, eol, intErr);
    index->w4 = getNextInt(data, eol, intErr);
    index->wRest = getNextInt(data, eol, intErr);
    index->skipGap = getNextInt(data, eol, intErr);
    index->gapOffset = getNextInt(data, eol, intErr);
    index->arrLen = getNextInt(data, eol, intErr);
    index->seqLen = getNextInt(data, eol, intErr);
    index->bitFilter = getNextInt(data, eol, intErr);
    index->wCharsInMask = getNextInt(data, eol, intErr);
    index->wAfterBits = getNextInt(data, eol, intErr);
    index->l1Step = getNextInt(data, eol, intErr);
    if (eol) {
        ti.setError("Too little amount of parameters in the file-index");
        return;
    }
    index->L1_SIZE = getNextInt(data, eol, intErr);
    if (intErr) {
        ti.setError("Bad integer for some parameter in the file-index");
        return;
    }

    if (index->w <= 0 || index->arrLen <= 0) {
        ti.setError("Negative index's parameters");
        return;
    }

    index->sArray = new quint32[index->arrLen];
    QByteArray readBuffer(BUFF_SIZE, '\0');
    char *buff = readBuffer.data();
    lineIdx++;
    int len = 0;
    int pos = 0;
    int bytes = len;
    readArray(file, buff, &len, &pos, &bytes, &lineIdx, index->sArray, index->arrLen, ti);
    if (ti.cancelFlag || ti.hasError()) {
        file.close();
        return;
    }

    index->bitMask = new quint32[index->arrLen];
    readArray(file, buff, &len, &pos, &bytes, &lineIdx, index->bitMask, index->arrLen, ti);
    if (ti.cancelFlag || ti.hasError()) {
        file.close();
        return;
    }

    index->l1bitMask = new quint32[index->L1_SIZE];
    readArray(file, buff, &len, &pos, &bytes, &lineIdx, index->l1bitMask, index->L1_SIZE, ti);
    file.close();
}

} //namespace
