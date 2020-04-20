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

#ifndef _U2_STRUCTURAL_ALIGNMENT_ALGORITHM_REGISTRY_H_
#define _U2_STRUCTURAL_ALIGNMENT_ALGORITHM_REGISTRY_H_

#include <QMap>
#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class StructuralAlignmentAlgorithm;
class StructuralAlignmentAlgorithmFactory;
class StructuralAlignmentTask;
class StructuralAlignmentTaskSettings;

class U2ALGORITHM_EXPORT StructuralAlignmentAlgorithmRegistry : public QObject {
    Q_OBJECT

public:
    StructuralAlignmentAlgorithmRegistry(QObject *parent = 0);
    ~StructuralAlignmentAlgorithmRegistry();

    void registerAlgorithmFactory(StructuralAlignmentAlgorithmFactory *factory, const QString &id);
    StructuralAlignmentAlgorithmFactory* getAlgorithmFactory(const QString &id);

    StructuralAlignmentAlgorithm* createStructuralAlignmentAlgorithm(const QString &algorithm);
    StructuralAlignmentTask* createStructuralAlignmentTask(const QString &algorithm, const StructuralAlignmentTaskSettings &settings);

    QList<QString> getFactoriesIds() const;

private:
    StructuralAlignmentAlgorithmRegistry(const StructuralAlignmentAlgorithmRegistry&);
    StructuralAlignmentAlgorithmRegistry& operator= (const StructuralAlignmentAlgorithmRegistry&);

private:
    QMap<QString, StructuralAlignmentAlgorithmFactory*> factories;
};  // class StructuralAlignmentAlgorithmRegistry

}   // namespace U2

#endif  // #ifndef _U2_STRUCTURAL_ALIGNMENT_ALGORITHM_REGISTRY_H_
