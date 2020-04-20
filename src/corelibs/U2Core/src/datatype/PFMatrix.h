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

#ifndef _U2_POSITION_FREQUENCY_MATRIX_H_
#define _U2_POSITION_FREQUENCY_MATRIX_H_

#include <U2Core/global.h>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/DNASequence.h>

#include <QVarLengthArray>
#include <QMap>

namespace U2 {

//info for JASPAR matrices, contained in matrix_list.txt
class U2CORE_EXPORT JasparInfo {
public:
    //default empty constructor
    JasparInfo();

    //constructor from parsed data
    JasparInfo(const QMap<QString, QString>& properties);

    //constructor from matrix_list.txt string
    JasparInfo(const QString& line);

    QString getProperty(const QString& name) const;

    QMap<QString, QString> getProperties() const;

private:
    QMap<QString, QString> properties;
};

//Type of Position frequency matrix
enum PFMatrixType {
    PFM_MONONUCLEOTIDE,
    PFM_DINUCLEOTIDE
};

//Position frequency matrix
class U2CORE_EXPORT PFMatrix {
    friend class FMatrixSerializer;
public:
    //create empty matrix
    PFMatrix() : data(QVarLengthArray<int>()), length(0), type(PFM_MONONUCLEOTIDE) {};

    //create matrix from pre-counted data
    PFMatrix(const QVarLengthArray<int>& data, const PFMatrixType type);

    //create matrix from alignment (without gaps)
    PFMatrix(const MultipleSequenceAlignment& data, const PFMatrixType type);

    //create matrix from set of sequences with equal length
    PFMatrix(const QList<DNASequence*>& data, const PFMatrixType type);

    PFMatrix(const PFMatrix &m) :data(m.data), length(m.length), type(m.type), info(m.info) {};

    //get internal index of position in 1-dimensional array
    int index(int row, int column) const;

    //get length of matrix row
    int getLength() const;

    //get type of matrix: mono- or dinucleotide
    PFMatrixType getType() const;

    //get value at specified position
    int getValue(int row, int column) const;

    //set JASPAR info for matrix
    void setInfo(const JasparInfo& info);

    //get specified Jaspar property
    QString getProperty(const QString& propertyName) const;

    //get all Jaspar properties
    QMap<QString, QString> getProperties() const;

    //convert dinucleotide matrix to mononucleotide one
    static PFMatrix convertDi2Mono(const PFMatrix& source);

private:
    QVarLengthArray<int> data;
    int length;
    PFMatrixType type;
    JasparInfo info;
};

}//namespace

#endif
