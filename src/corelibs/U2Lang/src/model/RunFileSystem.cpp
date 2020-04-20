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

#include <QFileInfo>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/SchemaConfig.h>

#include "RunFileSystem.h"

namespace U2 {

/************************************************************************/
/* FSItem */
/************************************************************************/
FSItem::FSItem()
: parentItem(NULL), dir(false)
{

}

FSItem::FSItem(const QString &name, bool isDirectory, FSItem *parent)
: parentItem(parent), itemName(name), dir(isDirectory)
{

}

FSItem::~FSItem() {
    qDeleteAll(items);
}

bool FSItem::isDir() const {
    return dir;
}

QString FSItem::name() const {
    return itemName;
}

QVector<FSItem*> FSItem::children() const {
    SAFE_POINT(isDir(), "Files can not have children", QVector<FSItem*>());
    return items;
}

FSItem * FSItem::parent() const {
    return parentItem;
}

FSItem * FSItem::child(int pos) const {
    SAFE_POINT(isDir(), "Files can not have children", NULL);
    if (pos >= items.size() || pos < 0) {
        return NULL;
    }
    return items[pos];
}

int FSItem::row() const {
    CHECK(NULL != parentItem, 0);
    return parentItem->items.indexOf(const_cast<FSItem*>(this));
}

bool FSItem::contains(const QString &name) const {
    SAFE_POINT(isDir(), "Files can not have children", false);
    return (NULL != getItem(children(), name));
}

void FSItem::rename(const QString &newName) {
    itemName = newName;
}

void FSItem::addChild(FSItem *item) {
    SAFE_POINT(isDir(), "Files can not have children", );
    SAFE_POINT(!contains(item->name()), "This item already exists: " + item->name(), );

    int pos = posToInsert(item);
    items.insert(pos, item);
    item->parentItem = this;
}

int FSItem::posToInsert(FSItem *item) const {
    int pos = 0;
    while (pos<items.size()) {
        FSItem *i = items[pos];
        if (i->isDir() && !item->isDir()) {
            pos++;
            continue;
        } else if (!i->isDir() && item->isDir()) {
            break;
        }
        if (item->name() < i->name()) {
            break;
        }
        pos++;
    }
    return pos;
}

void FSItem::removeChild(const QString &name, U2OpStatus &os) {
    SAFE_POINT(isDir(), "Files can not have children", );
    FSItem *item = FSItem::getItem(children(), name);
    SAFE_POINT(NULL != item, "No child with the name " + name, );

    if (item->isDir() && !item->children().isEmpty()) {
        os.setError(item->name() + " is not empty");
        return;
    }
    items.remove(items.indexOf(item));
}

FSItem * FSItem::getItem(const QVector<FSItem*> &items, const QString &name) {
    foreach (FSItem *item, items) {
        if (item->name() == name) {
            return item;
        }
    }
    return NULL;
}

void FSItem::noChildren() {
    foreach (FSItem *item, items) {
        item->parentItem = NULL;
    }
    items.clear();
}

/************************************************************************/
/* RunFileSystem */
/************************************************************************/
RunFileSystem::RunFileSystem(QObject *parent)
: QObject(parent)
{
    root = new FSItem(tr("Workflow-run output"), true);
}

RunFileSystem::~RunFileSystem() {
    delete root;
}

bool RunFileSystem::contains(const QString &pathStr) {
    bool found = false;
    find(getPath(pathStr), found);
    return found;
}

bool RunFileSystem::canAdd(const QString &pathStr, bool isDirectory) {
    CHECK(RFSUtils::isCorrectUrl(pathStr), false);
    QStringList parentPath;
    QString name;
    CHECK(getPath(pathStr, parentPath, name), false);

    FSItem *current = root;
    foreach (const QString &dirName, parentPath) {
        CHECK(current->isDir(), false);
        FSItem *item = FSItem::getItem(current->children(), dirName);
        CHECK(NULL != item, true);
        current = item;
    }

    CHECK(current->contains(name), true);
    FSItem *target = FSItem::getItem(current->children(), name);
    return (target->isDir() == isDirectory);
}

void RunFileSystem::addItem(const QString &pathStr, bool isDirectory, U2OpStatus &os) {
    QStringList path = getPath(pathStr);
    FSItem *dir = createPath(path.mid(0, path.size() - 1), os);
    CHECK_OP(os, );

    if (dir->contains(path.last())) {
        os.setError(pathStr + " is already used");
        return;
    }

    dir->addChild(new FSItem(path.last(), isDirectory));
}

void RunFileSystem::removeItem(const QString &pathStr, U2OpStatus &os) {
    QStringList path = getPath(pathStr);
    QStringList parentPath = path.mid(0, path.size() - 1);
    QString name = path.last();

    bool found = true;
    FSItem *parent = find(parentPath, found);
    if (!found) {
        os.setError(parentPath.join("/") + " does not exist");
        return;
    }

    if (!parent->contains(name)) {
        os.setError(pathStr + " does not exist");
        return;
    }

    parent->removeChild(name, os);
}

void RunFileSystem::reset() {
    qDeleteAll(root->items);
    root->items.clear();
}

FSItem * RunFileSystem::getRoot() {
    return root;
}

FSItem * RunFileSystem::find(const QStringList &path, bool &found) {
    found = true;
    FSItem *current = root;
    foreach (const QString &name, path) {
        if (!current->isDir()) {
            found = false;
            break;
        }

        FSItem *item = FSItem::getItem(current->children(), name);
        if (NULL == item) {
            found = false;
            break;
        }
        current = item;
    }
    return current;
}

FSItem * RunFileSystem::createPath(const QStringList &path, U2OpStatus &os) {
    FSItem *current = root;
    QString pathStr = root->name();
    foreach (const QString &dirName, path) {
        FSItem *item = FSItem::getItem(current->children(), dirName);
        if (NULL == item) {
            item = new FSItem(dirName, true);
            current->addChild(item);
        }
        pathStr += "/" + dirName;
        if (!item->isDir()) {
            os.setError(pathStr + " is a file, not a folder");
            return root;
        }
        current = item;
    }
    return current;
}

QStringList RunFileSystem::getPath(const QString &pathStr) const {
    QString correctPath = pathStr;
    correctPath.replace("\\", "/");
    return correctPath.split("/", QString::SkipEmptyParts);
}

bool RunFileSystem::getPath(const QString &pathStr, QStringList &parentPath, QString &name) const {
    QStringList path = getPath(pathStr);
    CHECK(!path.isEmpty(), false);
    parentPath = path.mid(0, path.size() - 1);
    name = path.last();
    return true;
}

void RunFileSystem::test(const QString &file) {
    QFile f(file);
    f.open(QIODevice::WriteOnly);
    f.write(test(*root).join("\n").toLatin1());
    f.write("\n===============================\n");
    f.close();
    return;
}

QStringList RunFileSystem::test(FSItem &root) {
    QStringList result;
    for (QVector<FSItem*>::Iterator i=root.items.begin(); i!=root.items.end(); i++) {
        if ((*i)->isDir()) {
            QStringList mid = test(*(*i));
            if (mid.isEmpty()) {
                result << root.name() + "/" + (*i)->name() + "/";
            } else {
                foreach (const QString &m, mid) {
                    result << root.name() + "/" + m;
                }
            }
        } else {
            result << root.name() + "/" + (*i)->name();
        }
    }
    return result;
}

/************************************************************************/
/* RFSUtils */
/************************************************************************/
void RFSUtils::initRFS(RunFileSystem &rfs, const QList<Workflow::Actor*> &actors, SchemaConfig *cfg) {
    rfs.reset();
    { // add report dir
        U2OpStatus2Log os;
        rfs.addItem("report", true, os);
    }
    foreach (Workflow::Actor *actor, actors) {
        foreach (Attribute *attr, actor->getParameters()) {
            bool dir = false;
            if (!isOutUrlAttribute(attr, actor, dir)) {
                continue;
            }

            AttributeInfo attrId = AttributeInfo(actor->getId(), attr->getId());
            QString url = cfg->getAttributeValue(attrId).toString();
            if (!isCorrectUrl(url)) {
                continue;
            }

            if (rfs.contains(url)) {
                continue;
            }

            U2OpStatus2Log os;
            rfs.addItem(url, dir, os);

            if (os.hasError()) {
                cfg->setAttributeValue(attrId, "");
            }
        }
    }
}

bool RFSUtils::isOutUrlAttribute(Attribute *attr, Workflow::Actor *actor, bool &dir) {
    ConfigurationEditor *editor = actor->getEditor();
    CHECK(NULL != editor, false);
    PropertyDelegate *delegate = editor->getDelegate(attr->getId());
    CHECK(NULL != delegate, false);

    dir = (PropertyDelegate::OUTPUT_DIR == delegate->type());
    if (dir) {
        return true;
    } else if (PropertyDelegate::OUTPUT_FILE == delegate->type()) {
        return true;
    }

    return false;
}

bool RFSUtils::isCorrectUrl(const QString &url) {
    if (url.isEmpty()) {
        return false;
    }
    QFileInfo info(url);
    if (info.isAbsolute()) {
        return false;
    }
    return true;
}

} // U2
