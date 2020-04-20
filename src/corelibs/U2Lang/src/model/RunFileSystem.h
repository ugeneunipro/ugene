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

#ifndef _U2_RUNFILESYSTEM_H_
#define _U2_RUNFILESYSTEM_H_

#include <QObject>
#include <QVector>

#include <U2Core/U2OpStatus.h>

namespace U2 {

namespace Workflow {
    class Actor;
}
class Attribute;
class RunFileSystem;
class SchemaConfig;

class U2LANG_EXPORT FSItem {
    friend class RunFileSystem;
public:
    FSItem();
    FSItem(const QString &name, bool isDirectory, FSItem *parent = NULL);
    ~FSItem();

    FSItem * parent() const;
    QVector<FSItem*> children() const;
    FSItem * child(int pos) const;
    int row() const;

    bool isDir() const;
    QString name() const;
    bool contains(const QString &name) const;

    void rename(const QString &newName);
    void addChild(FSItem *item);
    int posToInsert(FSItem *item) const;
    void removeChild(const QString &name, U2OpStatus &os);
    void noChildren();

    static FSItem * getItem(const QVector<FSItem*> &items, const QString &name);

private:
    FSItem *parentItem;
    QString itemName;
    bool dir;
    QVector<FSItem*> items;
};

class U2LANG_EXPORT RunFileSystem : public QObject {
    Q_OBJECT
public:
    RunFileSystem(QObject *parent = NULL);
    ~RunFileSystem();

    bool contains(const QString &path);
    bool canAdd(const QString &path, bool isDirectory);
    void addItem(const QString &path, bool isDirectory, U2OpStatus &os);
    void removeItem(const QString &path, U2OpStatus &os);
    void reset();

    FSItem * getRoot();

    void test(const QString &file);

private:
    FSItem *root;

private:
    QStringList getPath(const QString &pathStr) const;
    bool getPath(const QString &pathStr, QStringList &parentPath, QString &name) const;
    FSItem * find(const QStringList &path, bool &found);
    FSItem * createPath(const QStringList &path, U2OpStatus &os);
    QStringList test(FSItem &root);
};

class U2LANG_EXPORT RFSUtils {
public:
    static void initRFS(RunFileSystem &rfs, const QList<Workflow::Actor*> &actors, SchemaConfig *cfg);
    static bool isCorrectUrl(const QString &url);
    static bool isOutUrlAttribute(Attribute *attr, Workflow::Actor *actor, bool &dir);
};

} // U2

#endif // _U2_RUNFILESYSTEM_H_
