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

#ifndef _U2_ASSEMBLY_CONSENSUS_ALGORITHM_H_
#define _U2_ASSEMBLY_CONSENSUS_ALGORITHM_H_

#include <U2Core/global.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class AssemblyConsensusAlgorithm;

class U2ALGORITHM_EXPORT AssemblyConsensusAlgorithmFactory : public QObject {
    Q_OBJECT
public:
    AssemblyConsensusAlgorithmFactory(const QString& algoId) : algorithmId(algoId) {}

    virtual QString getId() const { return algorithmId; }

    virtual QString getDescription() const = 0;
    virtual QString getName() const = 0;

    virtual AssemblyConsensusAlgorithm* createAlgorithm() = 0;
private:
    QString algorithmId;
};

class U2ALGORITHM_EXPORT AssemblyConsensusAlgorithm {
public:
    AssemblyConsensusAlgorithm(AssemblyConsensusAlgorithmFactory* factory_) : factory(factory_) {}
    virtual ~AssemblyConsensusAlgorithm();

    AssemblyConsensusAlgorithmFactory* getFactory() { return factory; }

    QString getDescription() const {return factory->getDescription();}
    QString getName() const {return factory->getName();}
    QString getId() const {return factory->getId();}

    virtual QByteArray getConsensusRegion(const U2Region &region, U2DbiIterator<U2AssemblyRead>* reads, QByteArray referenceFragment, U2OpStatus &os) = 0;

    /** The character that will be placed in consensus array if it is undefined (no bases covered current position) */
    static const char EMPTY_CHAR = '-';
private:
    AssemblyConsensusAlgorithmFactory* factory;
};

}//namespace

#endif
