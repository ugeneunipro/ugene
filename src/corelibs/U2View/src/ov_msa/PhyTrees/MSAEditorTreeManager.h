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

#ifndef _U2_MSAEDITOR_TREE_MANAGER_H_
#define _U2_MSAEDITOR_TREE_MANAGER_H_

#include <QObject>
#include <QPointer>
#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <U2Core/PhyTree.h>

namespace U2 {

class MSAEditor;
class MultipleSequenceAlignmentObject;
class PhyTreeGeneratorLauncherTask;
class Task;
class GObjectViewWindow;
class MSAEditorTreeViewer;
class Document;
class PhyTreeObject;
class MSAEditorMultiTreeViewer;

class MSAEditorTreeManager : public QObject {
    Q_OBJECT
public:
    MSAEditorTreeManager(MSAEditor* _editor);
    ~MSAEditorTreeManager(){}

    void loadRelatedTrees();

    void openTreeFromFile();

    void buildTreeWithDialog();

private slots:
    void sl_openTree(Task* treeBuildTask);
    void sl_openTreeTaskFinished(Task* t);
    void sl_onWindowClosed(GObjectViewWindow* viewWindow);
    void sl_treeRebuildingFinished(Task* treeBuildTask);
    void sl_refreshTree(MSAEditorTreeViewer* treeViewer);
    void sl_onPhyTreeDocLoaded(Task*);
    void sl_onDocumentRemovedFromProject(Document *doc);

private:
    void buildTree(const CreatePhyTreeSettings& buildSettings);
    bool canRefreshTree(MSAEditorTreeViewer* treeViewer);
    void openTreeViewer(PhyTreeObject* treeObj);
    void loadTreeFromFile(const QString& treeFileName);
    void createPhyTreeGeneratorTask(const CreatePhyTreeSettings& buildSettings, bool refreshExistingTree = false, MSAEditorTreeViewer* treeViewer = NULL);
    MSAEditorMultiTreeViewer* getMultiTreeViewer() const;

    MSAEditor*                    editor;
    QPointer<MultipleSequenceAlignmentObject>   msaObject;
    CreatePhyTreeSettings         settings;
    bool                          addExistingTree;
    PhyTree                       phyTree;
    Document                      *d;
    QMap<MSAEditorTreeViewer*, Task*> activeRefreshTasks;
};

}//namespace
#endif
