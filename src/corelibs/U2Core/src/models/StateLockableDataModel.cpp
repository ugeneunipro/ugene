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

#include <QCoreApplication>
#include <QThread>

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "StateLockableDataModel.h"

namespace U2 {

StateLockableItem::StateLockableItem(QObject* p)
    : QObject(p),
      itemIsModified(false),
      trackModifications(true),
      mainThreadModificationOnly(false),
      modificationVersion(0)
{
    QThread* appThread = QCoreApplication::instance()->thread();
    QThread* objectThread = thread();
    bool mainThread = appThread == objectThread;
    setMainThreadModificationOnly(mainThread);
}

StateLockableItem::~StateLockableItem(){
    foreach(StateLock* lock, locks) {
        assert(!lock->getFlags().testFlag(StateLockFlag_LiveLock));
        delete lock;
    }
}

static void checkThread(const StateLockableItem* i) {
#ifdef _DEBUG
    if (i->isMainThreadModificationOnly()) {
        QThread* appThread = QCoreApplication::instance()->thread();
        QThread* thisThread = QThread::currentThread();
        QThread* itemThread = i->thread();
        assert(appThread == thisThread);
        assert(appThread == itemThread);
    }
#else
    Q_UNUSED(i);
#endif
}


void StateLockableItem::lockState(StateLock* lock) {
    assert(!locks.contains(lock));
    checkThread(this);

    locks.append(lock);
    if (locks.size() == 1) {
        emit si_lockedStateChanged();
    }
}


void StateLockableItem::unlockState(StateLock* lock) {
    assert(locks.contains(lock));
    checkThread(this);

    locks.removeOne(lock);

    if (locks.isEmpty()) {
        emit si_lockedStateChanged();
    }
}


void StateLockableItem::setModified(bool newModifiedState, const QString& modType) {
    SAFE_POINT(newModifiedState == false || isModificationAllowed(modType), "Item modification not allowed", );
    CHECK(isModificationTracked(), );
    checkThread(this);
    if (newModifiedState) {
        modificationVersion++;
    }
    if (itemIsModified == newModifiedState) {
        return;
    }
    itemIsModified = newModifiedState;
    emit si_modifiedStateChanged();
}

bool StateLockableItem::isModificationAllowed(const QString& ) {
    return !isStateLocked();
}

void StateLockableItem::setModificationTrack(bool track) {
    trackModifications = track;
}

bool StateLockableItem::isMainThreadObject() const {
    QThread* at = QCoreApplication::instance()->thread();
    QThread* ot = thread();
    return at == ot;
}

//////////////////////////////////////////////////////////////////////////
// Tree

StateLockableTreeItem::~StateLockableTreeItem() {
    // TODO: check that there are no live-locks and delete all child items
    assert(!hasLocks(StateLockableTreeItemBranch_Item, StateLockFlag_LiveLock));
}

bool StateLockableTreeItem::isStateLocked() const {
    StateLockableTreeItem* parentItem = qobject_cast<StateLockableTreeItem*>(parent());
    return StateLockableItem::isStateLocked() || (parentItem != NULL ? parentItem->isStateLocked() : false);
}

bool StateLockableTreeItem::isMainThreadModificationOnly() const {
    StateLockableTreeItem* parentItem = qobject_cast<StateLockableTreeItem*>(parent());
    return StateLockableItem::isMainThreadModificationOnly() ||
            (NULL != parentItem && parentItem->isMainThreadModificationOnly());
}

void StateLockableTreeItem::setModificationTrack(bool track) {
    StateLockableItem::setModificationTrack(track);
    foreach (StateLockableTreeItem* item, childItems) {
        item->setModificationTrack(track);
    }
}

void StateLockableTreeItem::lockState(StateLock* lock) {
    assert(!locks.contains(lock));
    checkThread(this);

    bool wasUnlocked = !isStateLocked();

    locks.append(lock);

    //notify all children on parent lock state change
    if (wasUnlocked) {
        foreach(StateLockableTreeItem* c, childItems) {
            c->onParentStateLocked();
        }
    }
    emit si_lockedStateChanged();
}

void StateLockableTreeItem::unlockState(StateLock* lock) {
    assert(locks.contains(lock));
    checkThread(this);

    locks.removeOne(lock);

    bool becomeUnlocked = !isStateLocked();

    emit si_lockedStateChanged();

    if (becomeUnlocked) {
        foreach(StateLockableTreeItem* c, childItems) {
            c->onParentStateUnlocked();
        }
    }
}

void StateLockableTreeItem::onParentStateLocked() {
    //parent has become locked -> check if my state is changed
    if (!locks.isEmpty()) {
        return; //nothing changed - was locked too
    }

    //notify children
    assert(isStateLocked());

    foreach(StateLockableTreeItem* c, childItems) {
        c->onParentStateLocked();
    }

    emit si_lockedStateChanged();
}

void StateLockableTreeItem::onParentStateUnlocked() {
    //parent has become unlocked -> check if my state is changed
    if (!locks.isEmpty()) {
        return; //nothing changed - was still locked
    }

    //notify children
    assert(!isStateLocked());

    emit si_lockedStateChanged();

    foreach(StateLockableTreeItem* c, childItems) {
        c->onParentStateUnlocked();
    }
}

void StateLockableTreeItem::setParentStateLockItem(StateLockableTreeItem* newParent) {
    StateLockableTreeItem* parentStateLockItem = qobject_cast<StateLockableTreeItem*>(parent());
    SAFE_POINT(parentStateLockItem == NULL || newParent == NULL, "Parent item is already assigned", );
    SAFE_POINT(newParent == NULL || newParent->isModificationAllowed(StateLockModType_AddChild),
        "Add-child modification is not allowed for new parent item!",);
    SAFE_POINT(parentStateLockItem == NULL || parentStateLockItem->isModificationAllowed(StateLockModType_AddChild),
        "Add-child modification is not allowed for old parent item!",);
    SAFE_POINT((NULL == newParent) || (newParent->thread() == thread()),
        "Parent item has a different thread",);

    StateLockableTreeItem* oldParent = parentStateLockItem;
    parentStateLockItem = newParent;
    setParent(newParent);

    bool treeMod = isTreeItemModified();
    if (newParent!=NULL) {
        setMainThreadModificationOnly(newParent->isMainThreadModificationOnly());
        setModificationTrack(newParent->isModificationTracked());
        checkThread(this);
        newParent->childItems.insert(this);
        newParent->setModified(true, StateLockModType_AddChild);
        if (treeMod) {
            newParent->increaseNumModifiedChilds(numModifiedChildren + 1);
        }

    } else if (oldParent!=NULL) {
        oldParent->childItems.remove(this);
        oldParent->setModified(true, StateLockModType_AddChild);
        if (treeMod) {
            oldParent->decreaseNumModifiedChilds(numModifiedChildren + 1);
        }
    }
}

void StateLockableTreeItem::setModified(bool newModifiedState, const QString& modType) {
    SAFE_POINT(newModifiedState == false || isModificationAllowed(modType), "Item modification not allowed", );
    CHECK(isModificationTracked(), );

    checkThread(this);
    if (newModifiedState) {
        modificationVersion++;
    }
    if (itemIsModified == newModifiedState) {
        return;
    }
    itemIsModified = newModifiedState;

    StateLockableTreeItem* parentStateLockItem = qobject_cast<StateLockableTreeItem*>(parent());
    bool parentUpdate = (NULL != parentStateLockItem && numModifiedChildren == 0);

    if (itemIsModified && parentUpdate) { // let parent become modified first
        parentStateLockItem->increaseNumModifiedChilds(1);
    }

    emit si_modifiedStateChanged();

    if (!itemIsModified && parentUpdate) { // let parent become clean last
        parentStateLockItem->decreaseNumModifiedChilds(1);
    }

    if (!hasModifiedChildren()) {
        checkThread(this);
        emit si_modifiedStateChanged();
    }
}

bool StateLockableTreeItem::isTreeItemModified () const {
    return (numModifiedChildren > 0 || itemIsModified);
}

void StateLockableTreeItem::increaseNumModifiedChilds(int n) {
    assert(n > 0);
    numModifiedChildren+=n;

    bool becomeModified = numModifiedChildren == n && !itemIsModified;
    StateLockableTreeItem* parentStateLockItem = qobject_cast<StateLockableTreeItem*>(parent());
    if (NULL != parentStateLockItem) {
        parentStateLockItem->increaseNumModifiedChilds(n + (becomeModified  ? 1 : 0) );
    }
    if (becomeModified) {
        assert(isTreeItemModified());
        checkThread(this);
        emit si_modifiedStateChanged();
    }
}

void StateLockableTreeItem::decreaseNumModifiedChilds(int n) {
    assert(n > 0);
    assert(numModifiedChildren >= n);
    numModifiedChildren-=n;

    bool becomeClean = numModifiedChildren == 0 && !itemIsModified;

    StateLockableTreeItem* parentStateLockItem = qobject_cast<StateLockableTreeItem*>(parent());
    if (NULL != parentStateLockItem) {
        parentStateLockItem->decreaseNumModifiedChilds(n + (becomeClean ? 1 : 0));
    }

    if (becomeClean)  {
        assert(!isTreeItemModified());
        checkThread(this);
        emit si_modifiedStateChanged();
    }

}

QList<StateLock*> StateLockableTreeItem::findLocks(StateLockableTreeItemBranchFlags tf, StateLockFlag lf) const {
    QList<StateLock*> res;

    if (tf.testFlag(StateLockableTreeItemBranch_Item)) {
        foreach(StateLock* lock, locks) {
            if (lock->getFlags().testFlag(lf)) {
                res.append(lock);
            }
        }
    }

    StateLockableTreeItem* parentStateLockItem = qobject_cast<StateLockableTreeItem*>(parent());
    if (tf.testFlag(StateLockableTreeItemBranch_Parents) && parentStateLockItem!=NULL) {
        StateLockableTreeItemBranchFlags tflags(StateLockableTreeItemBranch_Parents | StateLockableTreeItemBranch_Item);
        res+=parentStateLockItem->findLocks(tflags, lf);
    }

    if (tf.testFlag(StateLockableTreeItemBranch_Children)) {
        StateLockableTreeItemBranchFlags tflags(StateLockableTreeItemBranch_Children| StateLockableTreeItemBranch_Item);
        foreach(const StateLockableTreeItem* child, childItems) {
            res+=child->findLocks(tflags, lf);
        }
    }

    return res;
}

StateLocker::StateLocker(StateLockableItem *lockableItem, StateLock *lock) :
    lockableItem(lockableItem),
    lock(NULL == lock ? lock = new StateLock() : lock)
{
    SAFE_POINT(NULL != lockableItem, L10N::nullPointerError("StateLockableItem"), );
    SAFE_POINT(NULL != lock, L10N::nullPointerError("StateLock"), );
    lockableItem->lockState(lock);
}

StateLocker::~StateLocker() {
    lockableItem->unlockState(lock);
    delete lock;
}

}//namespace

