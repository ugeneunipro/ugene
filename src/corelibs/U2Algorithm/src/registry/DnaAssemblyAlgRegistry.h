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

#ifndef _U2_DNA_ASSEMBLY_ALG_REGISTRY_H_
#define _U2_DNA_ASSEMBLY_ALG_REGISTRY_H_

#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QStringList>

#include <U2Core/global.h>

namespace U2 {

class DnaAssemblyToRefTaskFactory;
class DnaAssemblyGUIExtensionsFactory;
class DnaAssemblyAlgorithmEnv;


class U2ALGORITHM_EXPORT DnaAssemblyAlgRegistry : public QObject {
    Q_OBJECT
public:
    DnaAssemblyAlgRegistry(QObject* pOwn = 0);
    ~DnaAssemblyAlgRegistry();

    bool registerAlgorithm(DnaAssemblyAlgorithmEnv* env);
    DnaAssemblyAlgorithmEnv* unregisterAlgorithm(const QString& id);
    DnaAssemblyAlgorithmEnv* getAlgorithm(const QString& id) const;

    QStringList getRegisteredAlgorithmIds() const;
    QStringList getRegisteredAlgorithmsWithIndexFileSupport() const;
private:
    mutable QMutex mutex;
    QMap<QString, DnaAssemblyAlgorithmEnv*> algorithms;

    Q_DISABLE_COPY(DnaAssemblyAlgRegistry);
};

class U2ALGORITHM_EXPORT DnaAssemblyAlgorithmEnv {
public:
    DnaAssemblyAlgorithmEnv(const QString &id,
        DnaAssemblyToRefTaskFactory *tf ,
        DnaAssemblyGUIExtensionsFactory *guiExt,
        bool supportsIndexFiles,
        bool supportsDbi,
        bool supportsPairedEndLibrary,
        const QStringList &refrerenceFormats,
        const QStringList &readsFormats);

    virtual ~DnaAssemblyAlgorithmEnv();

    const QString& getId()  const {return id;}
    bool isIndexFilesSupported() const {return supportsIndexFiles;}
    bool isDbiSupported() const {return supportsDbi;}
    bool supportsPairedEndLibrary() const { return supportsPEReads; }
    QStringList getRefrerenceFormats() const { return refrerenceFormats; }
    QStringList getReadsFormats() const { return readsFormats; }

    DnaAssemblyToRefTaskFactory* getTaskFactory() const {return taskFactory;}
    DnaAssemblyGUIExtensionsFactory* getGUIExtFactory() const {return guiExtFactory;}

private:
    Q_DISABLE_COPY(DnaAssemblyAlgorithmEnv);

protected:
    QString id;
    DnaAssemblyToRefTaskFactory* taskFactory;
    DnaAssemblyGUIExtensionsFactory* guiExtFactory;
    bool supportsIndexFiles;
    bool supportsDbi;
    bool supportsPEReads;
    QStringList refrerenceFormats;
    QStringList readsFormats;
};

} // namespace

#endif
