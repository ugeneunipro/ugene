/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppResources.h>
#include <U2Core/ProjectModel.h>

namespace U2 {

class MWMDIWindow;

class ProjectImpl : public Project {
    Q_OBJECT

public:
    ProjectImpl(const QString& _name, const QString& _url, const QList<Document*>& _docs = QList<Document*>(), const QList<GObjectViewState*>& _states = QList<GObjectViewState*>());
    ~ProjectImpl() override = default;

    const QString& getProjectName() const override;

    void setProjectName(const QString& name) override;

    const QString& getProjectURL() const override;

    void setProjectURL(const QString&) override;

    const QList<Document*>& getDocuments() const override;

    void addDocument(Document* d) override;

    void removeDocument(Document* d, bool autodelete = true) override;

    bool lockResources(int sizeMB, const QString& url, QString& error) override;

    Document* findDocumentByURL(const QString& url) const override;

    Document* findDocumentByURL(const GUrl& url) const override;

    const QList<GObjectViewState*>& getGObjectViewStates() const override;

    void addGObjectViewState(GObjectViewState* s) override;

    void updateGObjectViewState(const GObjectViewState& updatedState) override;

    void removeGObjectViewState(GObjectViewState* s) override;

    void makeClean() override;

    quint64 getObjectIdCounter() const override;

    void setObjectIdCounter(quint64 c) override;

    void removeRelations(const QString& docUrl) override;

    void updateDocInRelations(const QString& oldDocUrl, const QString& newDocUrl) override;

private slots:
    void sl_onStateModified(GObjectViewState*);
    void sl_onObjectAdded(GObject*);
    void sl_onObjectRemoved(GObject* o);
    void sl_onObjectRenamed(const QString& oldName);
    void sl_onObjectRelationChanged(const QList<GObjectRelation>& previousRelations);

    void sl_onMdiWindowAdded(MWMDIWindow* w);
    void sl_onMdiWindowClosing(MWMDIWindow* w);
    void sl_onViewRenamed(const QString& oldName);
    void sl_removeAllrelationsWithObject(GObject* obj);

private:
    // returns number of reference fields updated
    QString genNextObjectId();

    int idGen;

    QString name;
    QString url;
    QList<Document*> docs;
};

}  // namespace U2

