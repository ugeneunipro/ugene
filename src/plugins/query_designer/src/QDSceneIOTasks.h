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

#ifndef _U2_QD_SCENE_IO_TASKS_H_
#define _U2_QD_SCENE_IO_TASKS_H_

#include <U2Core/Task.h>

#include "QDDocument.h"


namespace U2 {

static const QString QUERY_DESIGNER_ID = "query_designer";
static const QString QUERY_SCHEME_EXTENSION = "uql";

class QDSceneInfo {
public:
    QString path;
    QString schemeName;
    QString description;
};

class QueryScene;
class QDSaveSceneTask : public Task {
    Q_OBJECT
public:
    QDSaveSceneTask(QueryScene* scene, const QDSceneInfo& info);
    void run();
private:
    QString path;
    QueryScene* scene;
    QByteArray rawData;
};

class QDLoadSceneTask : public Task {
    Q_OBJECT
public:
    QDLoadSceneTask(QueryScene* scene, const QString& url);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
    Task::ReportResult report();
private:
    QueryScene* scene;
    QString url;
    QList<QDDocument*> docs;
};

class QDLoadSchemeTask : public Task {
    Q_OBJECT
public:
    QDLoadSchemeTask(const QString& uri);
    QDScheme* getScheme() const { return scheme; }
    QList<Task*> onSubTaskFinished(Task* subTask);
    Task::ReportResult report();
private:
    QString uri;
    QDScheme* scheme;
    QList<QDDocument*> docs;
};

class QDLoadDocumentTask : public Task {
    Q_OBJECT
public:
    QDLoadDocumentTask(const QString& url);
    void run();
    QDDocument* getDocument() const { return doc; }
private:
    QDDocument* doc;
    QString url;
};

class QDSceneSerializer {
public:
    static bool doc2scene(QueryScene* scene, const QList<QDDocument*>& docs);
    static QDDocument* scene2doc(QueryScene* scene);
    static bool doc2scheme(const QList<QDDocument*>& docs,
        QMap<QDElementStatement*, QDActor*>& stmt2actor, QDScheme* scheme);
    static bool doc2scheme(const QList<QDDocument*>& docs, QDScheme* scheme);
};

class QDActor;
class QDActorParameters;
class QDSchemeUnit;
class QDConstraint;
class QDSchemeSerializer {
public:
    static QDElementStatement* saveActor(QDActor* actor, QDDocument* doc);

    static QDActor* loadActor(QDElementStatement* actorElement, QString& group);

    static QDLinkStatement* saveConstraint(QDConstraint* constraint, QDDocument* doc,
        const QMap<QDSchemeUnit*,QDElementStatement*>& unit2stmt);

    static QDConstraint* loadConstraint(QDLinkStatement* lnk,
        const QMap<QDElementStatement*, QDActor*>& stmt2unit,
        const QList<QDDocument*>& docs);

    static QDSchemeUnit* findSchemeUnit(const QString& id, QDDocument* doc,
        const QMap<QDElementStatement*, QDActor*> stmt2actor,
        const QList<QDDocument*>& docs);

    static void saveGroups(QDScheme* scheme, QDDocument* doc);

    static bool loadGroups(QDScheme* scheme, QDDocument* doc);

    static const QMap<QDStrandOption, QString> STRAND_MAP;
};

}//namespace

#endif
