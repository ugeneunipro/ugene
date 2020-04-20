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

#ifndef _U2_SUBST_MATRIX_REGISTRY_H_
#define _U2_SUBST_MATRIX_REGISTRY_H_

#include <U2Core/global.h>
#include <U2Core/SMatrix.h>

#include <QMutex>
#include <QMap>
#include <QStringList>

namespace U2 {

class U2ALGORITHM_EXPORT SubstMatrixRegistry : public QObject {
    Q_OBJECT
public:
    SubstMatrixRegistry(QObject* pOwn = 0);

    SMatrix getMatrix(const QString& name);

    QList<SMatrix> getMatrices() const;

    QStringList getMatrixNames() const;

    QList<SMatrix> selectMatricesByAlphabet(const DNAAlphabet* al) const;

    QStringList selectMatrixNamesByAlphabet(const DNAAlphabet* al) const;

    void registerMatrix(const SMatrix& m);

    static SMatrix readMatrixFromFile(const QString& fileName, QString& error);

private:
    void readMatrices();
    static SMatrix parseMatrix(const QString& name, const QByteArray& text, QString& error);

    mutable QMutex mutex;
    QMap<QString, SMatrix> matrixByName;
};

} // namespace

#endif
