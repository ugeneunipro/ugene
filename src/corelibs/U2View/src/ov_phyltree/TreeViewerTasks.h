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

#ifndef _U2_TREE_VIEWER_TASKS_H_
#define _U2_TREE_VIEWER_TASKS_H_

#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Gui/ObjectViewTasks.h>

#include <QSplitter>

namespace U2 {

class PhyTreeObject;
class UnloadedObject;
class TreeViewer;
class CreateRectangularBranchesTask;
class OpenTreeViewerTask;
class MSAEditor;
class MSAEditorTreeManager;



class OpenTreeViewerTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenTreeViewerTask(PhyTreeObject* obj, QObject* _parent = NULL);
    OpenTreeViewerTask(UnloadedObject* obj, QObject* _parent = NULL);
    OpenTreeViewerTask(Document* doc, QObject* _parent = NULL);
    virtual ~OpenTreeViewerTask();

    virtual void open();

    virtual void createTreeViewer();

    static void updateTitle(TreeViewer* tv);


protected:
    QPointer<PhyTreeObject>     phyObject;
    GObjectReference            unloadedReference;
    QObject*                    parent;
    bool                        createMDIWindow;
};

class MSAEditorOpenTreeViewerTask : public OpenTreeViewerTask {
    Q_OBJECT
public:
    MSAEditorOpenTreeViewerTask(PhyTreeObject* obj, MSAEditorTreeManager* _parent);
    MSAEditorOpenTreeViewerTask(UnloadedObject* obj, MSAEditorTreeManager* _parent);
    MSAEditorOpenTreeViewerTask(Document* doc, MSAEditorTreeManager* _parent);
    virtual ~MSAEditorOpenTreeViewerTask(){}

    virtual void createTreeViewer();
private:
    MSAEditorTreeManager* treeManager;
};

class OpenSavedTreeViewerTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenSavedTreeViewerTask(const QString& viewName, const QVariantMap& stateData);
    virtual void open();

    static void updateRanges(const QVariantMap& stateData, TreeViewer* ctx);
};


class UpdateTreeViewerTask : public ObjectViewTask {
public:
    UpdateTreeViewerTask(GObjectView* v, const QString& stateName, const QVariantMap& stateData);
    virtual void update();
};

class CreateMSAEditorTreeViewerTask: public Task {
    Q_OBJECT
public:
    CreateMSAEditorTreeViewerTask(const QString& name, const QPointer<PhyTreeObject>& obj, const QVariantMap& stateData);
    virtual void prepare();
    virtual ReportResult report();
    TreeViewer* getTreeViewer();
    const QVariantMap& getStateData();
private:
    QString viewName;
    QPointer<PhyTreeObject> phyObj;
    CreateRectangularBranchesTask* subTask;
    QVariantMap stateData;
    TreeViewer* view;
    const PhyTree tempTree;
};

class CreateTreeViewerTask: public Task {
    Q_OBJECT
public:
    CreateTreeViewerTask(const QString& name, const QPointer<PhyTreeObject>& obj, const QVariantMap& stateData);
    virtual void prepare();
    virtual ReportResult report();
private:
    QString viewName;
    QPointer<PhyTreeObject> phyObj;
    CreateRectangularBranchesTask* subTask;
    QVariantMap stateData;
    const PhyTree tempTree;
};

} // namespace

#endif
