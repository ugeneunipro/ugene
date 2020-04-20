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

#ifndef _U2_POSITION_WEIGHT_MATRIX_H_
#define _U2_POSITION_WEIGHT_MATRIX_H_

#include <QMap>
#include <QVarLengthArray>

#include <U2Core/global.h>

namespace U2 {
//Type of Weight matrix
enum PWMatrixType {
    PWM_MONONUCLEOTIDE,
    PWM_DINUCLEOTIDE
};

class U2CORE_EXPORT UniprobeInfo {
public:
    //default empty constructor
    UniprobeInfo();

    //constructor from parsed data
    UniprobeInfo(const QMap<QString, QString>& properties);

    //constructor from raw data
    UniprobeInfo(const QString& data);

    QString getProperty (const QString& name) const;

    QMap<QString, QString> getProperties() const;

    bool operator == (const UniprobeInfo &i1) const {
        return i1.properties == properties;
    }

private:
    QMap<QString, QString> properties;
};

//Position frequency matrix
//Also known as PSSM - Position score-specific matrix
class U2CORE_EXPORT PWMatrix {
    friend class WMatrixSerializer;
public:
    //create empty matrix
    PWMatrix();

    //create matrix from pre-counted data
    PWMatrix(const QVarLengthArray<float> &matrix, const PWMatrixType& type);

    //get internal index of position in 1-dimensional array
    int index (int row, int column) const;

    //get length of matrix row
    int getLength () const;

    //get type of matrix: mono- or dinucleotide
    PWMatrixType getType() const;

    //get value at specified position
    float getValue (int row, int column) const;

    //get sum of minimum values in each column
    float getMinSum () const;

    //get sum of maximum values in each column
    float getMaxSum () const;

    //set UniPROBE info for matrix
    void setInfo (const UniprobeInfo& info);

    //get specified UniPROBE property
    QString getProperty (const QString& propertyName) const;

    //get all UniPROBE properties
    QMap<QString, QString> getProperties() const;

    bool operator == (const PWMatrix &m1) const {
        //FIXME: for some reasons QT == operator didn't work for gcc compiler
        if (m1.data.size() != data.size())
            return false;
        for (int i = 0; i < m1.data.size(); i++) {
            if (m1.data.at(i) != data.at(i))
                return false;
        }
        return  m1.length == length &&
                m1.type == type &&
                m1.minSum == minSum &&
                m1.maxSum == maxSum &&
                m1.info == info;
    };

private:
    QVarLengthArray<float> data;
    int length;
    PWMatrixType type;
    float minSum;
    float maxSum;
    UniprobeInfo info;

};

}//namespace

#endif
