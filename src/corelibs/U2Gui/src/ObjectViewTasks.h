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

#include <QPointer>

#include <U2Core/GObjectReference.h>
#include <U2Core/Task.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class Document;
class GObject;
class Project;
class U2OpStatus;

class U2GUI_EXPORT ObjectViewTask : public Task {
    Q_OBJECT
public:
    enum Type {
        Type_Open,
        Type_Update
    };

    ObjectViewTask(GObjectViewController* view, const QString& stateName, const QVariantMap& s = QVariantMap());

    ObjectViewTask(GObjectViewFactoryId fid, const QString& viewName = QString(), const QVariantMap& s = QVariantMap());

    void prepare() override;
    ReportResult report() override;

    virtual void open() {};
    virtual void update() {};

    virtual void onDocumentLoaded(Document* d) {
        Q_UNUSED(d);
    }

    static Document* createDocumentAndAddToProject(const QString& docUrl, Project* p, U2OpStatus& os);

protected:
    Type taskType;
    QVariantMap stateData;
    QPointer<GObjectViewController> view;
    QString viewName;
    bool stateIsIllegal;
    QStringList objectsNotFound;
    QStringList documentsNotFound;

    QList<QPointer<Document>> documentsToLoad;
    QList<QPointer<Document>> documentsFailedToLoad;

    QList<QPointer<GObject>> selectedObjects;
};

class U2GUI_EXPORT AddToViewTask : public Task {
    Q_OBJECT
public:
    AddToViewTask(GObjectViewController* v, GObject* obj);
    ReportResult report() override;

    QPointer<GObjectViewController> objView;
    QString viewName;
    GObjectReference objRef;
    QPointer<Document> objDoc;
};

}  // namespace U2
