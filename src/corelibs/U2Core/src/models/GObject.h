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

#ifndef _U2_GOBJECT_H_
#define _U2_GOBJECT_H_

#include <QMimeData>
#include <QMutex>

#include "StateLockableDataModel.h"
#include "GObjectReference.h"
#include <U2Core/U2Type.h>

/** List of object relations */
#define GObjectHint_RelatedObjects          "gobject-hint-related-objects"

/** The same, but used in previous versions */
#define GObjectHint_RelatedObjectsLegacy    "related_objs"

/** Last used object name. Used in case if object is renamed but name is not saved to file */
#define GObjectHint_LastUsedObjectName      "gobject-hint-last-used-object-name"

/** When object is added to project, in-project-id is assigned that is unique per project */
#define GObjectHint_InProjectId             "gobject-hint-in-project-id"

/** Defines how lower/upper case characters in sequence should be handled */
#define GObjectHint_CaseAnns                "use-case-annotations"

namespace U2 {

class Document;
class GObjectConstraints;
class GHints;
class U2DbiRef;
class U2OpStatus;

enum GObjectModLock {
    GObjectModLock_IO       // locked by IO reasons, e.g. object is stored in a remote database
};

class U2CORE_EXPORT GObject : public StateLockableTreeItem {
    friend class DocumentFormat;
    friend class Document;

    Q_OBJECT
public:
    GObject(QString _type, const QString& _name, const QVariantMap& hints = QVariantMap());
    virtual ~GObject();

    virtual Document* getDocument() const;

    const QString& getGObjectName() const { return name; }

    virtual void setGObjectName(const QString& newName);

    GObjectType getGObjectType() const { return type; }

    virtual bool checkConstraints(const GObjectConstraints* c) const { Q_UNUSED(c); return true; }

    GHints* getGHints() const { return hints; }

    QVariantMap getGHintsMap() const;

    void setGHints(GHints*);

    QList<GObjectRelation> getObjectRelations() const;

    void setObjectRelations(const QList<GObjectRelation>& obj);

    QList<GObjectRelation> findRelatedObjectsByRole(const GObjectRelationRole& role) const;

    QList<GObjectRelation> findRelatedObjectsByType(const GObjectType& objType) const;

    void addObjectRelation(const GObjectRelation& ref);

    void addObjectRelation(const GObject* obj, const GObjectRelationRole& role);

    void removeObjectRelation(const GObjectRelation& ref);

    void updateRefInRelations(const GObjectReference& oldRef, const GObjectReference& newRef);

    void removeRelations(const QString& removedDocUrl);

    void updateDocInRelations(const QString& oldDocUrl, const QString& newDocUrl);

    bool hasObjectRelation(const GObject* obj, const GObjectRelationRole& role) const;

    bool hasObjectRelation(const GObjectRelation& r) const;

    QHash< QString, QString > getIndexInfo() const { return indexInfo; }

    void setIndexInfo(const QHash<QString, QString>& ii) { indexInfo = ii; }

    const U2EntityRef& getEntityRef() const { return entityRef; }

    bool isUnloaded() const;

    virtual GObject* clone(const U2DbiRef& dbiRef, U2OpStatus& os, const QVariantMap &hints = QVariantMap()) const = 0;

    StateLock* getGObjectModLock(GObjectModLock type) const;

    void relatedObjectRelationChanged();

    static bool objectLessThan(GObject *first, GObject *second);

signals:
    void si_nameChanged(const QString& oldName);
    void si_relationChanged(const QList<GObjectRelation>& previousRelations);
    void si_relatedObjectRelationChanged();

protected:
    void setGObjectNameNotDbi(const QString &newName);
    void ensureDataLoaded() const;
    void ensureDataLoaded(U2OpStatus &os) const;
    virtual void loadDataCore(U2OpStatus &os);

protected:
    mutable QMutex              dataGuard;
    mutable bool                dataLoaded;
    GObjectType                 type;
    QString                     name;
    GHints*                     hints;
    QHash<QString, QString>     indexInfo;
    U2EntityRef                 entityRef;

private:
    virtual void setParentStateLockItem(StateLockableTreeItem* p);
    void checkIfBelongToSharedDatabase(StateLockableTreeItem *parent);
    void setRelationsInDb(QList<GObjectRelation>& list) const;
    void setupHints(QVariantMap hintsMap);
    void fetchPermanentGObjectRelations(QList<GObjectRelation> &res) const;

    void removeAllLocks();

    bool                             arePermanentRelationsFetched;
    QMap<GObjectModLock, StateLock*> modLocks;
};

class GObjectConstraints : public QObject {
    Q_OBJECT
public:
    GObjectConstraints(const GObjectType& t, QObject* parent = NULL) : QObject(parent), objectType(t) {}
    const GObjectType objectType;

};

class U2CORE_EXPORT GObjectMimeData : public QMimeData {
    Q_OBJECT
public:
    GObjectMimeData(GObject* obj) : objPtr(obj) {}

    QPointer<GObject> objPtr;

    // QMimeData
    bool hasFormat(const QString &mimeType) const;
    QStringList formats() const;

    static const QString MIME_TYPE;
};

}//namespace

#endif
