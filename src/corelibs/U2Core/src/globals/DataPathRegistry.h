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

#ifndef _U2_DATA_PATH_REGISTRY_H
#define _U2_DATA_PATH_REGISTRY_H

#include <U2Core/global.h>

#include <QMap>
#include <QDir>

namespace U2 {

/**
 * class to access data that different tools might need e.g. data bases, samples, test files, ...
 * registers data name and path to data
 * it will analyze the path storing all the data in the map data_file_name -> data_full_path where data_file_name is a name of a file
 */
class U2CORE_EXPORT U2DataPath : public QObject {
    Q_OBJECT
public:
    enum Option {
        None = 0,
        AddOnlyFolders = 1 << 0,
        AddRecursively = 1 << 1,
        CutFileExtension = 1 << 2,
        AddTopLevelFolder = 1 << 3
    };
    Q_DECLARE_FLAGS(Options, Option)

    U2DataPath(const QString &name, const QString &path, const QString &descr = "", Options options = None);

    const QString &getName()  const;
    const QString &getPath()  const;
    const QString &getDescription()  const;

    const QMap<QString, QString> &getDataItems() const;
    QList<QString> getDataNames() const;

    bool isValid()  const;
    bool isFolders() const;

    QVariantMap getDataItemsVariantMap() const;
    QString getPathByName(const QString &name) const; //first name found is returned. if your items have similar names use getDataItems()

    bool operator ==(const U2DataPath &other) const;
    bool operator !=(const U2DataPath &other) const;

private:
    void init();
    void fillDataItems(const QDir &dir, bool recursive);
    QString chopExtention(QString name);

    QString                 name;
    QString                 path;
    QString                 description;
    QMap<QString, QString>  dataItems; //data_file_name -> data_full_path
    Options                 options;
    bool                    valid;
};

class U2CORE_EXPORT U2DataPathRegistry : public QObject {
    Q_OBJECT
public:
    ~U2DataPathRegistry();

    U2DataPath *getDataPathByName(const QString &name);

    bool registerEntry(U2DataPath *dp);
    void unregisterEntry(const QString &name);

    QList<U2DataPath *> getAllEntries() const;

private:
    QMap<QString, U2DataPath *> registry;
};

}   // namespace U2

Q_DECLARE_OPERATORS_FOR_FLAGS(U2::U2DataPath::Options)

#endif // _U2_DATA_PATH_REGISTRY_H
