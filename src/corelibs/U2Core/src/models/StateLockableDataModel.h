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

#ifndef _U2_STATE_LOCKS_H_
#define _U2_STATE_LOCKS_H_

#include <QPointer>
#include <QSet>

#include <U2Core/global.h>

namespace U2 {

class StateLock;
class StateLockableItem;

#define StateLockModType_AddChild "state-lock-mod-add-child"

enum StateLockFlag {
    StateLockFlag_NoFlags  = 0x00,
    StateLockFlag_LiveLock = 0x01,
    StateLockFlag_AnyFlags = 0xFF
};

typedef QFlags<StateLockFlag> StateLockFlags;

/**
 * StateLocker takes ownership over the StackLock object.
 */
class U2CORE_EXPORT StateLocker {
public:
    StateLocker(StateLockableItem *lockableItem, StateLock *lock = NULL);
    ~StateLocker();

private:
    StateLockableItem *lockableItem;
    StateLock *lock;
};

class U2CORE_EXPORT StateLock : public QObject {
    Q_OBJECT
public:
    StateLock() : flags(StateLockFlag_NoFlags){}
    StateLock(const QString& _userDesc, StateLockFlags _flags = StateLockFlag_NoFlags) : userDesc(_userDesc), flags(_flags){}

    QString getUserDesc() const {return userDesc;}
    void setUserDesc(const QString& d) {userDesc = d;}

    StateLockFlags getFlags() const {return flags;}

private:
    QString         userDesc;
    StateLockFlags  flags;
};

class U2CORE_EXPORT StateLockableItem : public QObject {
    Q_OBJECT
public:
    StateLockableItem(QObject* p = NULL);
    virtual ~StateLockableItem();

    int getStateLocksCount() const {return locks.size();}

    virtual bool isStateLocked() const {return !locks.isEmpty();}

    const QList<StateLock*>& getStateLocks() const {return locks;}

    virtual void lockState(StateLock* lock);

    virtual void unlockState(StateLock* lock);

    virtual void setModified(bool modified, const QString& modType = QString());

    virtual bool isModificationAllowed(const QString& modType);

    virtual bool isItemModified() const {return itemIsModified;}

    virtual bool isModificationTracked() const { return trackModifications; }

    virtual void setModificationTrack(bool track);

    /** Main thread model object can only be modified in main thread */
    virtual bool isMainThreadModificationOnly() const {return mainThreadModificationOnly;}

    virtual void setMainThreadModificationOnly(bool v) { mainThreadModificationOnly = v;}

    virtual bool isMainThreadObject() const;

    //returns number of modifications done to this item
    virtual int getModificationVersion() const {return modificationVersion;}

signals:
    void si_lockedStateChanged();
    void si_modifiedStateChanged();

protected:
    QList<StateLock*> locks;
    bool itemIsModified;
    bool trackModifications;
    bool mainThreadModificationOnly;
    int  modificationVersion;
};

//////////////////////////////////////////////////////////////////////////
/// Tree model
enum StateLockableTreeItemBranchFlag {
    StateLockableTreeItemBranch_Item     = 0x1,
    StateLockableTreeItemBranch_Parents  = 0x2,
    StateLockableTreeItemBranch_Children = 0x4
};

typedef QFlags<StateLockableTreeItemBranchFlag> StateLockableTreeItemBranchFlags;

#define StateLockableTreeFlags_ItemAndChildren (StateLockableTreeItemBranchFlags(StateLockableTreeItemBranch_Item) | StateLockableTreeItemBranch_Children)
#define StateLockableTreeFlags_ItemAndParents  (StateLockableTreeItemBranchFlags(StateLockableTreeItemBranch_Item) | StateLockableTreeItemBranch_Parents)
#define StateLockableTreeFlags_ItemAndChildrenAndParents  (StateLockableTreeFlags_ItemAndParents | StateLockableTreeItemBranch_Children)


class U2CORE_EXPORT StateLockableTreeItem : public StateLockableItem {
    Q_OBJECT
public:
    StateLockableTreeItem() : StateLockableItem(), childLocksCount(0), numModifiedChildren(0) {}

    virtual ~StateLockableTreeItem();

    virtual bool isStateLocked() const;

    virtual void lockState(StateLock* lock);

    virtual void unlockState(StateLock* lock);

    virtual void setModified(bool modified, const QString& modType = QString());

    virtual bool isTreeItemModified () const;

    virtual bool hasModifiedChildren() const {return numModifiedChildren != 0;}

    StateLockableTreeItem* getParentStateLockItem() const { return qobject_cast<StateLockableTreeItem*>(parent()); }

    bool hasLocks(StateLockableTreeItemBranchFlags treeFlags, StateLockFlag lockFlag = StateLockFlag_AnyFlags) const {
        return !findLocks(treeFlags, lockFlag).isEmpty();
    }

    int countLocks(StateLockableTreeItemBranchFlags treeFlags, StateLockFlag lockFlag = StateLockFlag_AnyFlags) const {
        return findLocks(treeFlags, lockFlag).size();
    }

    QList<StateLock*> findLocks(StateLockableTreeItemBranchFlags treeFlags, StateLockFlag lockFlag = StateLockFlag_AnyFlags) const;

    virtual bool isMainThreadModificationOnly() const;

    virtual void setModificationTrack(bool track);

protected:
    static void setParentStateLockItem_static(StateLockableTreeItem* child, StateLockableTreeItem* newParent) {
        child->setParentStateLockItem(newParent);
    }

    virtual void setParentStateLockItem(StateLockableTreeItem* p);

    void increaseNumModifiedChilds(int n);
    void decreaseNumModifiedChilds(int n);

    void onParentStateLocked();
    void onParentStateUnlocked();

    const QSet<StateLockableTreeItem*>& getChildItems() const {return childItems;}

private:
    QSet<StateLockableTreeItem*>    childItems;
    int                             childLocksCount;
    int                             numModifiedChildren;
};

}//namespace

#endif
