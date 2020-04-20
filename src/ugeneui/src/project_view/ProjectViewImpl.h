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

#ifndef _U2_PROJECT_VIEW_WIDGET_
#define _U2_PROJECT_VIEW_WIDGET_

#include <U2Core/ClipboardController.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/Task.h>

#include <U2Gui/LoadDocumentTaskProvider.h>
#include <U2Gui/ProjectView.h>
#include <U2Gui/MainWindow.h>

#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ObjectViewTreeController.h>

#include "ui_ProjectViewWidget.h"

namespace U2 {

class EnableProjectViewTask;
class DisableProjectViewTask;
class ProjectViewWidgetFactory;
class ExportDocumentDialogController;

class DocumentUpdater : public QObject {
    Q_OBJECT
public:
    DocumentUpdater(QObject* p=NULL);
    void reloadDocuments( QList<Document*> docs2Reload ); //reload and reopen views for given documents

private slots:
    void sl_update();
    void sl_updateTaskStateChanged();
private:
    void excludeDocumentsInTasks(const QList<Task*>& tasks, QList<Document*>& documents);
    void update();
    void notifyUserAndReloadDocuments(const QList<Document*> &outdatedDocs);
    void notifyUserAndProcessRemovedDocuments(const QList<Document*> &removedDocs);
    bool isAnyDialogOpened() const;
    bool makeDecision(Document *doc, QListIterator<Document*> &iter);

    Task* updateTask;
    bool recursion;
};

class ProjectViewWidget : public QWidget, public Ui_ProjectViewWidget {
    Q_OBJECT
public:
    ProjectViewWidget();
private:
    DocumentUpdater* updater;
};

class ProjectViewImpl: public ProjectView , public LoadDocumentTaskProvider {
    Q_OBJECT

    friend class ProjectViewWidgetFactory;
    friend class EnableProjectViewTask;
    friend class DisableProjectViewTask;
public:
    ProjectViewImpl();
    ~ProjectViewImpl();

    virtual const DocumentSelection* getDocumentSelection() const {return projectTreeController->getDocumentSelection();}

    virtual const GObjectSelection* getGObjectSelection() const  {return projectTreeController->getGObjectSelection();}
    
    void initView();

    //QAction* getAddExistingDocumentAction() const {return NULL;}

    virtual QList<Task*> createLoadDocumentTasks(const QList<Document*>& docs) const;

    void highlightItem(Document*);

    void setSaveProjectOnCloseEnabled(bool enabled) { saveProjectOnClose = enabled; }

    static const QString SETTINGS_ROOT;

protected:
    /// returns NULL if no actions are required to enable service
    virtual Task* createServiceEnablingTask();

    /// returns NULL if no actions are required to disable service
    virtual Task* createServiceDisablingTask();


    void enable();
    void disable();


    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void sl_onProjectModifiedStateChanged();
    void sl_onDocumentRemoved(Document* d);
    void sl_onSaveSelectedDocs();
    void sl_onDocTreePopupMenuRequested(QMenu& popup);
    void sl_openNewView();
    void sl_openStateView();
    void sl_activateView();
    void sl_addToView();
    void sl_onMDIWindowAdded(MWMDIWindow*);
    void sl_onViewPersistentStateChanged(GObjectViewWindow* thiz);
    void sl_onActivated(GObject*);
    void sl_onActivated(Document*);
    void sl_filterTextChanged(const QString& t);
    void sl_relocate();
    void sl_exportDocument();
    void sl_onToggleCircular();
    void sl_onOpenContainingFolder();

private:
    void saveWidgetState(ProjectViewWidget* w);
    void restoreWidgetState(ProjectViewWidget* w);
    void buildViewMenu(QMenu& m);
    void buildOpenViewMenu(const MultiGSelection& ms, QMenu* m);
    void buildAddToViewMenu(const MultiGSelection& ms, QMenu* m);
    void buildRelocateMenu(QMenu* m);
    QList<QAction*> selectOpenViewActions(GObjectViewFactory* f, const MultiGSelection& ms, QObject* actionsParent, bool tryActivate = false);
    void saveViewState(GObjectViewWindow* v, const QString& stateName);
    void updateMWTitle();
    
    //todo: find a better place to do this
    void registerBuiltInObjectViews();
    void unregisterBuiltInObjectViews();
    void export2Document(ExportDocumentDialogController &dialog, bool tracePath = true) const;


    QAction*    saveSelectedDocsAction;
    QAction*    relocateDocumentAction;
    QAction*    exportDocumentAction;
    QAction*    toggleCircularAction;
    QAction*    openContainingFolder;

    bool saveProjectOnClose;

    ProjectViewWidget* w;
    ProjectViewWidgetFactory* f;
    ProjectTreeController* projectTreeController;
    ObjectViewTreeController* objectViewController;
};


class EnableProjectViewTask : public Task {
    Q_OBJECT

public:
    EnableProjectViewTask(ProjectViewImpl* pvi);

    ReportResult report();
private:
    ProjectViewImpl* pvi;
};

class DisableProjectViewTask : public Task {
    Q_OBJECT

public:
    DisableProjectViewTask(ProjectViewImpl* pvi, bool saveProjectOnClose);
    void prepare();
    ReportResult report();
private:
    ProjectViewImpl* pvi;
    bool saveProject;
};



}//namespace
#endif
