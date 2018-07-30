/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include "DataPathRegistry.h"

namespace U2 {

////////////////////////////////////////
//U2DataPath
U2DataPath::U2DataPath(const QString& _name, const QString& _path, const QString& _descr, Options _options)
    : name(_name),
      path(_path),
      description (_descr),
      options(_options),
      valid(false)
{
    init();
}

QString U2DataPath::getPathByName(const QString &name) const{
    QString res = "";

    if (dataItems.contains(name)) {
        res = dataItems.value(name, "");
    }

    return res;
}

bool U2DataPath::operator ==(const U2DataPath &other) const {
    return (name == other.name) && (options == other.options);
}

bool U2DataPath::operator !=(const U2DataPath &other) const {
    return !(*this == other);
}

void U2DataPath::init() {
    if (path.isEmpty() || !QFile::exists(path)) {
        valid = false;
        return;
    }

    QFileInfo fi(path);
    QString filePath = fi.absoluteFilePath();
    path = filePath;

    if (fi.isDir()) {
        if (options.testFlag(AddTopLevelFolder)) {
            dataItems.insertMulti(fi.fileName(), filePath);
        }
        fillDataItems(fi.absoluteFilePath(), options.testFlag(AddRecursively));

    } else if (fi.isFile()) {
        if (!options.testFlag(AddOnlyFolders)) {
            QString fileName = chopExtention(fi.fileName());
            dataItems.insertMulti(fileName, filePath);
        }
    }

    valid = true;
}

void U2DataPath::fillDataItems(const QDir &dir, bool recursive) {
    QFileInfoList infoList = dir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files);

    foreach (const QFileInfo &fi, infoList) {
        if (fi.isFile()) {
            if (!options.testFlag(AddOnlyFolders)) {
                QString fileName = chopExtention(fi.fileName());
                QString filePath = fi.absoluteFilePath();

                dataItems.insertMulti(fileName, filePath);
            }
        } else if (fi.isDir()) {
            if (options.testFlag(AddOnlyFolders)) {
                QString fileName = fi.fileName();
                QString filePath = fi.absoluteFilePath();

                dataItems.insertMulti(fileName, filePath);
            }

            if (recursive) {
                fillDataItems(fi.absoluteFilePath(), recursive);
            }
        }
    }
}

const QString &U2DataPath::getName() const {
    return name;
}

const QString &U2DataPath::getPath() const {
    return path;
}

const QString &U2DataPath::getDescription() const {
    return description;
}

const QMap<QString, QString> &U2DataPath::getDataItems() const {
    return dataItems;
}

QList<QString> U2DataPath::getDataNames() const {
    return dataItems.keys();
}

bool U2DataPath::isValid() const {
    return valid;
}

bool U2DataPath::isFolders() const {
    return options.testFlag(AddOnlyFolders);
}

QVariantMap U2DataPath::getDataItemsVariantMap() const {
    QVariantMap vm;

    foreach (const QString &key, dataItems.keys()) {
        vm.insert(key, dataItems[key]);
    }

    return vm;
}

QString U2DataPath::chopExtention(QString name) {
    CHECK(options.testFlag(CutFileExtension), name);
    if (name.endsWith(".gz")) {
        name.chop(3);
    }
    int dot = name.lastIndexOf('.');
    if (dot > 0) {
        name.chop(name.size() - dot);
    }

    return name;
}

////////////////////////////////////////
//U2DataPathRegistry
U2DataPathRegistry::~U2DataPathRegistry() {
    qDeleteAll(registry.values());
}

U2DataPath *U2DataPathRegistry::getDataPathByName(const QString& name) {
    return registry.value(name, NULL);
}

bool U2DataPathRegistry::registerEntry(U2DataPath *dp) {
    if (registry.contains(dp->getName()) || !dp->isValid()) {
        return false;
    } else {
        registry.insert(dp->getName(), dp);
    }
    return true;
}

void U2DataPathRegistry::unregisterEntry(const QString &name) {
    delete registry.take(name);
}

QList<U2DataPath *> U2DataPathRegistry::getAllEntries() const {
    return registry.values();
}
}//namespace
