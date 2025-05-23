/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QMap>
#include <QObject>
#include <QStringList>

#include <U2Core/ScriptEngine.h>
#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT DBXRefInfo {
public:
    DBXRefInfo() {
    }
    DBXRefInfo(const QString& _name, const QString& _url, const QString& _fileUrl, const QString& _comment)
        : name(_name), url(_url), fileUrl(_fileUrl), comment(_comment) {
    }

    QString name;
    QString url;
    QString fileUrl;
    QString comment;

    static void setupToEngine(QScriptEngine* engine);

private:
    static QScriptValue toScriptValue(QScriptEngine* engine, DBXRefInfo const& in);
    static void fromScriptValue(const QScriptValue& object, DBXRefInfo& out);
};

class U2CORE_EXPORT DBXRefRegistry : public QObject {
    Q_OBJECT
public:
    DBXRefRegistry(QObject* p = nullptr);

    Q_INVOKABLE DBXRefInfo getRefByKey(const QString& dbxrefKey) const {
        return refsByKey.value(dbxrefKey);
    }

    static void setupToEngine(QScriptEngine* engine);
    const QMap<QString, DBXRefInfo>& getEntries() {
        return refsByKey;
    }

    // Finds a database and accession, which have been used when @url was created
    // Return pair, where database name is the first, accession number is the second
    QPair<QString, QString> getDbAndAccessionBytUrl(const QString& url) const;

private:
    static QScriptValue toScriptValue(QScriptEngine* engine, DBXRefRegistry* const& in);
    static void fromScriptValue(const QScriptValue& object, DBXRefRegistry*& out);

    QMap<QString, DBXRefInfo> refsByKey;
};
}  // namespace U2
Q_DECLARE_METATYPE(U2::DBXRefRegistry*)
Q_DECLARE_METATYPE(U2::DBXRefInfo)
