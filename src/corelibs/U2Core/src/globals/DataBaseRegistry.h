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

#ifndef _DATABASE_REGISTRY_
#define _DATABASE_REGISTRY_

#include <QMap>
#include <QString>
#include <QList>
#include <QObject>

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

namespace U2 {

class HttpRequest {
public:
    virtual void sendRequest(const QString &program, const QString &query) = 0;
    virtual QList<SharedAnnotationData> getAnnotations() { return result; }
    virtual QString getError() { return error; }
    virtual QByteArray getOutputFile() { return NULL; }
    virtual ~HttpRequest() {};
protected:
    HttpRequest(Task *_task) :error(""), task(_task) {};
    QString error;
    QList<SharedAnnotationData> result;
    Task *task;
};

class DataBaseFactory {
public:
    virtual ~DataBaseFactory() {}
    virtual HttpRequest * getRequest(Task *t) = 0;
};

class U2CORE_EXPORT DataBaseRegistry :public QObject {
    Q_OBJECT
public:
    DataBaseRegistry(QObject *o = NULL);
    ~DataBaseRegistry();
    bool registerDataBase(DataBaseFactory *f, const QString &id);
    DataBaseFactory *getFactoryById(const QString& id);
    bool isRegistered(const QString& id);

private:
    QMap<QString, DataBaseFactory *> factories;
};

}

#endif
