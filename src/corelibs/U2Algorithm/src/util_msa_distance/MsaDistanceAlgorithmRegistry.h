/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QStringList>

#include "MsaDistanceAlgorithm.h"

namespace U2 {

class MsaDistanceAlgorithm;

class U2ALGORITHM_EXPORT MsaDistanceAlgorithmRegistry : public QObject {
    Q_OBJECT

public:
    MsaDistanceAlgorithmRegistry(QObject* p = nullptr);

    ~MsaDistanceAlgorithmRegistry() override;

    MsaDistanceAlgorithmFactory* getAlgorithmFactory(const QString& algoId);

    void addAlgorithm(MsaDistanceAlgorithmFactory* algo);

    QStringList getAlgorithmIds() const;

    QList<MsaDistanceAlgorithmFactory*> getAlgorithmFactories() const {
        return algorithms.values();
    }

    QList<MsaDistanceAlgorithmFactory*> getAlgorithmFactories(DistanceAlgorithmFlags flags) const;

private:
    QMap<QString, MsaDistanceAlgorithmFactory*> algorithms;
};

}  // namespace U2
