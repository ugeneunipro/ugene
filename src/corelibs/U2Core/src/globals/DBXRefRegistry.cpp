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

#include "DBXRefRegistry.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define DB_XREF_FILE_NAME "DBXRefRegistry.txt"

DBXRefRegistry::DBXRefRegistry(QObject* p)
    : QObject(p) {
    QFile file(QString(PATH_PREFIX_DATA) + ":" + DB_XREF_FILE_NAME);

    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        coreLog.error(tr("File with db_xref mappings not found: %1").arg(DB_XREF_FILE_NAME));
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.length() == 0 || line.startsWith("#")) {
            continue;
        }
        QStringList fields = line.split("|");
        if (fields.size() != 4) {
            coreLog.error(tr("Illegal db_xref file entry: %1").arg(line));
            continue;
        }
        DBXRefInfo info(fields[0], fields[1], fields[2], fields[3].trimmed());
        refsByKey[info.name] = info;
    }
    file.close();
}

QPair<QString, QString> DBXRefRegistry::getDbAndAccessionBytUrl(const QString& url) const {
    const auto& dbNames = refsByKey.keys();
    QPair<QString, QString> result;
    for (const auto& dbName : qAsConst(dbNames)) {
        const auto& dbXref = refsByKey.value(dbName);
        CHECK_CONTINUE(!dbXref.fileUrl.isEmpty());

        auto splitUrl = dbXref.fileUrl.split("%1");
        QString regex;
        static const QString MATCH_REGEX = "(.*)";
        if (dbXref.fileUrl.startsWith("%1")) {
            regex += MATCH_REGEX;
        }
        for (const auto& partUrl : qAsConst(splitUrl)) {
            regex += QString(R"(%1)").arg(partUrl);
            regex += MATCH_REGEX;
        }
        regex = regex.left(regex.size() - MATCH_REGEX.size());
        if (dbXref.fileUrl.endsWith("%1")) {
            regex += MATCH_REGEX;
        }

        QRegularExpression regExp(regex);
        QRegularExpressionMatch match = regExp.match(url);
        CHECK_CONTINUE(match.hasMatch());

        result.first = dbName;
        result.second = match.captured(1);
        break;
    }
    SAFE_POINT(!result.first.isEmpty(), QString("Unexpected URL: %1").arg(url), result);

    return result;
}

QScriptValue DBXRefRegistry::toScriptValue(QScriptEngine* engine, DBXRefRegistry* const& in) {
    return engine->newQObject(in);
}

void DBXRefRegistry::fromScriptValue(const QScriptValue& object, DBXRefRegistry*& out) {
    out = qobject_cast<DBXRefRegistry*>(object.toQObject());
}

void DBXRefRegistry::setupToEngine(QScriptEngine* engine) {
    DBXRefInfo::setupToEngine(engine);
    qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);
}

QScriptValue DBXRefInfo::toScriptValue(QScriptEngine* engine, DBXRefInfo const& in) {
    QScriptValue res = engine->newObject();
    res.setProperty("name", QScriptValue(engine, in.name));
    res.setProperty("url", QScriptValue(engine, in.url));
    res.setProperty("fileUrl", QScriptValue(engine, in.fileUrl));
    res.setProperty("comment", QScriptValue(engine, in.comment));
    return res;
}

void DBXRefInfo::fromScriptValue(const QScriptValue& object, DBXRefInfo& out) {
    auto info = dynamic_cast<DBXRefInfo*>(object.toQObject());
    out = info != nullptr ? *info : DBXRefInfo();
}

void DBXRefInfo::setupToEngine(QScriptEngine* engine) {
    qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);
}

}  // namespace U2
