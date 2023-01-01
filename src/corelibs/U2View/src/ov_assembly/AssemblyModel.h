/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef __U2_ASSEMBLY_BROWSER_MODEL_H__
#define __U2_ASSEMBLY_BROWSER_MODEL_H__

#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>

#include <U2Core/GObject.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

class Document;
class Task;
class U2SequenceObject;
class VariantTrackObject;

class U2VIEW_EXPORT AssemblyModel : public QObject {
    Q_OBJECT
public:
    // TODO refactor
    AssemblyModel(const DbiConnection& dbiConnection);
    ~AssemblyModel();

    bool isEmpty() const;

    QList<U2AssemblyRead> getReadsFromAssembly(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os);
    U2DbiIterator<U2AssemblyRead>* getReads(const U2Region& r, U2OpStatus& os);

    void calculateCoverageStat(const U2Region& r, U2AssemblyCoverageStat& coverageStat, U2OpStatus& os);

    const U2AssemblyCoverageStat& getCoverageStat(U2OpStatus& os);

    // returns true if calling getCoverageStat will not cause recomputation and is safe from main thread
    bool hasCachedCoverageStat();

    U2Region getGlobalRegion();

    qint64 getModelLength(U2OpStatus& os);

    QByteArray getReferenceMd5(U2OpStatus& os);

    QByteArray getReferenceSpecies(U2OpStatus& os);

    QString getReferenceUri(U2OpStatus& os);

    qint64 getModelHeight(U2OpStatus& os);

    void setAssembly(U2AssemblyDbi* dbi, const U2Assembly& assm);
    U2Assembly getAssembly() {
        return assembly;
    }
    U2SequenceObject* getRefObj() const;

    bool hasReference() const;
    bool referenceAssociated() const;

    void setReference(U2SequenceObject* seqObj);
    U2EntityRef getRefereneceEntityRef();

    QByteArray getReferenceRegion(const U2Region& region, U2OpStatus& os);
    QByteArray getReferenceRegionOrEmpty(const U2Region& region);

    const DbiConnection& getDbiConnection() const {
        return dbiHandle;
    }

    void associateWithReference(const U2DataId& refId);

    bool isLoadingReference() const {
        return loadingReference;
    }
    void setLoadingReference(bool value);

    qint64 getReadsNumber(U2OpStatus& os);
    bool hasReads(U2OpStatus& os);

    QList<U2AssemblyRead> findMateReads(U2AssemblyRead read, U2OpStatus& os);

    const QList<VariantTrackObject*>& getTrackList() const;
    void addTrackObject(VariantTrackObject* trackObj);
    bool checkPermissions(QFile::Permission permission, bool showDialog = true) const;
    void dissociateReference();

    bool isDbLocked(int timeout = 0);

private:
    /**
        Unsets reference only for current session (e.g. when ref doc is closed),
        association in ugenedb still exists
    */
    void unsetReference();
    void startLoadReferenceTask(Task* t);
    Task* createLoadReferenceAndAddToProjectTask(const U2CrossDatabaseReference& ref);
    void onReferenceRemoved();
    void removeCrossDatabaseReference(const U2DataId& refId);

signals:
    void si_referenceChanged();
    void si_trackAdded(VariantTrackObject* trackObj);
    void si_trackRemoved(VariantTrackObject* trackObj);

public slots:
    void sl_trackObjRemoved(GObject* o);

private slots:
    void sl_referenceLoaded();
    void sl_referenceLoadingFailed();
    void sl_referenceDocLoadedStateChanged();
    void sl_docRemoved(Document*);
    void sl_docAdded(Document*);
    void sl_referenceObjRemoved(GObject* o);

private:
    const static qint64 NO_VAL = -1;
    const static QByteArray COVERAGE_STAT_ATTRIBUTE_NAME;
    // TODO: track model changes and invalidate caches accordingly
    qint64 cachedModelLength;
    qint64 cachedModelHeight;

    U2Assembly assembly;
    U2AssemblyDbi* assemblyDbi;
    DbiConnection dbiHandle;

    bool loadingReference;
    U2SequenceObject* refObj;
    QList<VariantTrackObject*> trackObjList;

    QByteArray referenceMd5;
    bool md5Retrieved;

    qint64 cachedReadsNumber;

    QByteArray referenceSpecies;
    bool speciesRetrieved;

    QString referenceUri;
    bool uriRetrieved;

    U2AssemblyCoverageStat cachedCoverageStat;

    QMutex mutex;
};  // AssemblyModel

}  // namespace U2

#endif  // __U2_ASSEMBLY_BROWSER_MODEL_H__
