/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <QObject>
#include <QPointer>

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/PhyTree.h>

namespace U2 {

class MsaEditor;
class MsaObject;
class PhyTreeGeneratorLauncherTask;
class Task;
class GObjectViewWindow;
class MsaEditorTreeViewer;
class Document;
class PhyTreeObject;
class MsaEditorMultiTreeViewer;

class MsaEditorTreeManager : public QObject {
    Q_OBJECT
public:
    MsaEditorTreeManager(MsaEditor* msaEditor);

    void loadRelatedTrees();

    void openTreeFromFile();

    void buildTreeWithDialog();

    MsaEditor* getMsaEditor() const;

private slots:
    void sl_openTree(Task* treeBuildTask);
    void sl_openTreeTaskFinished(Task* task);
    void sl_onWindowClosed(GObjectViewWindow* viewWindow);
    void sl_treeRebuildingFinished(Task* treeBuildTask);
    void sl_refreshTree(MsaEditorTreeViewer* treeViewer);
    void sl_onPhyTreeDocLoaded(Task*);
    void sl_onDocumentRemovedFromProject(Document* doc);

private:
    void buildTree(const CreatePhyTreeSettings& buildSettings);
    bool canRefreshTree(MsaEditorTreeViewer* treeViewer);
    void openTreeViewer(PhyTreeObject* treeObj);
    void loadTreeFromFile(const QString& treeFileName);

    /**
     * Adds all tree objects found in the document into the view.
     * Ignores trees already added to the view.
     */
    void addTreesFromDocument(Document* document);

    void createPhyTreeGeneratorTask(const CreatePhyTreeSettings& buildSettings, bool refreshExistingTree = false, MsaEditorTreeViewer* treeViewer = nullptr);
    MsaEditorMultiTreeViewer* getMultiTreeViewer() const;

    MsaEditor* editor;
    QPointer<MsaObject> msaObject;
    CreatePhyTreeSettings settings;
    bool addExistingTree = false;
    PhyTree phyTree;
    Document* treeDocument = nullptr;
    QMap<MsaEditorTreeViewer*, Task*> activeRefreshTasks;
};

}  // namespace U2
