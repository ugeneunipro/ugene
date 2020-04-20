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

#include <U2Algorithm/RepeatFinderTaskFactory.h>
#include <U2Algorithm/RepeatFinderTaskFactoryRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ToolsMenu.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/AnnotatedDNAViewFactory.h>

#include "DotPlotFilesDialog.h"
#include "DotPlotPlugin.h"
#include "DotPlotSplitter.h"
#include "DotPlotTasks.h"
#include "DotPlotWidget.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    DotPlotPlugin* plug = new DotPlotPlugin();
    return plug;
}

DotPlotPlugin::DotPlotPlugin() : Plugin(tr("Dotplot"), tr("Build dotplot for sequences")), viewCtx(NULL) {
    connect( AppContext::getPluginSupport(), SIGNAL( si_allStartUpPluginsLoaded() ), SLOT(sl_initDotPlotView()));
}

void DotPlotPlugin::sl_initDotPlotView() {
    if (AppContext::getMainWindow()) {
        RepeatFinderTaskFactoryRegistry *fr = AppContext::getRepeatFinderTaskFactoryRegistry();
        RepeatFinderTaskFactory *factory = fr->getFactory("");
        if (factory) {
            viewCtx = new DotPlotViewContext(this);
            viewCtx->init();
        }
    }
}

DotPlotViewContext::DotPlotViewContext(QObject* p)
: GObjectViewWindowContext(p, AnnotatedDNAViewFactory::ID), createdByWizard(false)
{
    // add dotplot item to the tools menu
    QAction* showDlgAction = new QAction( QIcon(":dotplot/images/dotplot.png"), tr("Build dotplot..."), this );
    showDlgAction->setObjectName(ToolsMenu::DOTPLOT);
    connect( showDlgAction, SIGNAL( triggered() ), SLOT( sl_showDotPlotDialog() ) );
    ToolsMenu::addAction(ToolsMenu::TOOLS, showDlgAction);

    connect(AppContext::getMainWindow()->getMDIManager(), SIGNAL(si_windowActivated(MWMDIWindow*)), SLOT(sl_windowActivated(MWMDIWindow*)));

    // need to know it build dotplot wizard finished work
    connect( AppContext::getTaskScheduler(), SIGNAL( si_stateChanged(Task*) ), SLOT( sl_loadTaskStateChanged(Task*) ) );
}

// wizard finished loading needed files for dotplot
void DotPlotViewContext::sl_loadTaskStateChanged(Task* task) {

    DotPlotLoadDocumentsTask *loadTask = qobject_cast<DotPlotLoadDocumentsTask*>(task);
    if (!loadTask || !loadTask->isFinished()) {
        return;
    }

    if (loadTask->getStateInfo().hasError()) {
        QMessageBox::critical(NULL, tr("Error"), tr("Error opening files"));
        return;
    }

    if(loadTask->isNoView()){
        return;
    }

    // prepare document objects to open view with
    GObjectSelection os;

    QList <Document *> docs = loadTask->getDocuments();
    foreach (Document* doc, docs) {
        os.addToSelection(doc->getObjects());
    }

    MultiGSelection ms; ms.addSelection(&os);

    GObjectViewFactoryRegistry* reg = AppContext::getObjectViewFactoryRegistry();
    GObjectViewFactory* f = reg->getFactoryById(AnnotatedDNAViewFactory::ID);

    bool canCreate = f->canCreateView(ms);
    if (canCreate) {
        AppContext::getTaskScheduler()->registerTopLevelTask(f->createViewTask(ms, false));
        createdByWizard = true; // set flag that we need to show a dotplot settings dialog
        firstFile = loadTask->getFirstFile();
        secondFile = loadTask->getSecondFile();
    }
}

// DotPlot wizard dialog
void DotPlotViewContext::sl_showDotPlotDialog() {

    Task *tasks = new Task("Creating dotplot", TaskFlag_NoRun);

    QObjectScopedPointer<DotPlotFilesDialog> d = new DotPlotFilesDialog(QApplication::activeWindow());
    d->exec();
    CHECK(!d.isNull(), );

    if (QDialog::Accepted == d->result()) {
        if (!AppContext::getProject()) {
            tasks->addSubTask(AppContext::getProjectLoader()->createNewProjectTask());
        }

        DotPlotLoadDocumentsTask *t = new DotPlotLoadDocumentsTask(d->getFirstFileName(), d->getFirstGap(), d->getSecondFileName(), d->getSecondGap());
        tasks->addSubTask(t);
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(tasks);
}

static U2SequenceObject * getSequenceByFile(QString file) {
    Project *project = AppContext::getProject();
    SAFE_POINT(project != NULL, "No project loaded", NULL);

    Document *doc = project->findDocumentByURL(GUrl(file));
    CHECK(doc != NULL, NULL);

    QList<GObject*> sequences = GObjectUtils::select(doc->getObjects(), GObjectTypes::SEQUENCE, UOF_LoadedOnly);
    if(! sequences.isEmpty()) {
        return qobject_cast<U2SequenceObject*>(sequences.first());
    }
    return NULL;
}

// called from the context menu
void DotPlotViewContext::sl_buildDotPlot() {
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender());
    CHECK(action != NULL, )
    showBuildDotPlotDialog(action->getObjectView());
}

void DotPlotViewContext::showBuildDotPlotDialog(GObjectView *ov) {
    AnnotatedDNAView *dnaView = qobject_cast<AnnotatedDNAView*>(ov);
    CHECK(dnaView != NULL, )

    DotPlotWidget *dotPlot = new DotPlotWidget(dnaView);
    dotPlot->setSequences(getSequenceByFile(firstFile), getSequenceByFile(secondFile));

    // show settings dialog
    if (dotPlot->sl_showSettingsDialog(createdByWizard)) {
        DotPlotSplitter* splitter = getView(dnaView, true); // create new splitter
        Q_ASSERT(splitter);
        splitter->addView(dotPlot);

        connect (dotPlot, SIGNAL(si_removeDotPlot()), SLOT(sl_removeDotPlot()));
    }
    else {
        delete dotPlot; // user clicked cancel button
        dotPlot = NULL;
    }
    createdByWizard = false;
}

// DotPlotWidget said we should remove it from the splitter
void DotPlotViewContext::sl_removeDotPlot() {

    DotPlotWidget *dotPlot = qobject_cast<DotPlotWidget*>(sender());
    if (!dotPlot) {
        return;
    }

    // get dnaView that contains that DotPlotWidget
    AnnotatedDNAView *dnaView = dotPlot->getDnaView();
    Q_ASSERT(dnaView);

    // get DotPlotSplitter that contained in dnaView
    DotPlotSplitter* splitter = getView(dnaView, false);
    if (splitter) {

        splitter->removeView(dotPlot);
        delete dotPlot;
        dotPlot = NULL;

        // remove empty splitter from dnaView
        if (splitter->isEmpty()) {
            removeDotPlotView(dnaView);
        }
    }
}


#define SHOW_BUILD_DOT_PLOT_DIALOG_FLAG "trigger_dot_plot_dialog"
#define BUILD_DOT_PLOT_ACTION_NAME "build_dotplot_action"

// new view context is opened
void DotPlotViewContext::initViewContext(GObjectView* v) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(v);
    Q_ASSERT(av);

    // add the dotplot menu item to an analyze menu
    QString dotPlotBuildString = tr("Build dotplot...");
    ADVGlobalAction* act = new ADVGlobalAction(av, QIcon(":dotplot/images/dotplot.png"), dotPlotBuildString, 40, ADVGlobalActionFlags(ADVGlobalActionFlag_AddToAnalyseMenu));
    act->setObjectName(BUILD_DOT_PLOT_ACTION_NAME);
    connect(act, SIGNAL(triggered()), SLOT(sl_buildDotPlot()));

    ADVGlobalAction* tb = new ADVGlobalAction(av, QIcon(":dotplot/images/dotplot.png"), dotPlotBuildString, 40, ADVGlobalActionFlags(ADVGlobalActionFlag_AddToToolbar));
    tb->setObjectName("build_dotplot_action");
    connect(tb, SIGNAL(triggered()), SLOT(sl_buildDotPlot()));

    // this view context is created by dotplot wizard
    if (createdByWizard) {
//        createdByWizard = false;

        QWidget* widget = av->getWidget()->parentWidget();
        Q_ASSERT(widget != NULL);
        widget->showMaximized();

        // once view is ready and activated we will show build-dot-plot dialog
        av->setProperty(SHOW_BUILD_DOT_PLOT_DIALOG_FLAG, 1);
    }
}

// create if needed and return DotPlotSplitter in the dnaView
DotPlotSplitter* DotPlotViewContext::getView(GObjectView* view, bool create) {

    DotPlotSplitter* dotPlotView = NULL;

    // search for DotPlotSpliter in the view
    QList<QObject*> resources = viewResources.value(view);
    foreach(QObject* r, resources) {
        dotPlotView = qobject_cast<DotPlotSplitter*>(r);
        if (dotPlotView!=NULL) {
            return dotPlotView;
        }
    }

    // create new DotPlotSplitter
    if (create) {
        AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(view);
        Q_ASSERT(av);

        dotPlotView = new DotPlotSplitter(av);
        av->insertWidgetIntoSplitter(dotPlotView);
        resources.append(dotPlotView);
        viewResources.insert(view, resources);
    }
    return dotPlotView;
}

// context menu opened
void DotPlotViewContext::buildMenu(U2::GObjectView *v, QMenu *m) {

    QList<QObject*> resources = viewResources.value(v);
    foreach(QObject* r, resources) {

        DotPlotSplitter* dotPlotView = qobject_cast<DotPlotSplitter*>(r);
        if (dotPlotView && !dotPlotView->isEmpty()) {
            dotPlotView->buildPopupMenu(m);
            return;
        }
    }
}

// remove a splitter from the view
void DotPlotViewContext::removeDotPlotView(GObjectView* view) {

    QList<QObject*> resources = viewResources.value(view);
    foreach(QObject* r, resources) {
        DotPlotSplitter* dotPlotView = qobject_cast<DotPlotSplitter*>(r);

        if (dotPlotView) {
            assert(dotPlotView->isEmpty());
            AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(view);
            Q_ASSERT(av);

            av->unregisterSplitWidget(dotPlotView);
            resources.removeOne(dotPlotView);
            viewResources.insert(view, resources);

            delete dotPlotView;
        }
    }
}

void DotPlotViewContext::sl_windowActivated(MWMDIWindow* w) {
    // check if we need to show DP dialog for this window
    GObjectViewWindow* ow = qobject_cast<GObjectViewWindow*>(w);
    CHECK(ow != NULL, )
    GObjectView* view = ow->getObjectView();
    if (view->property(SHOW_BUILD_DOT_PLOT_DIALOG_FLAG).toInt() != 1) {
        return;
    }
    // mark this window as processed
    view->setProperty(SHOW_BUILD_DOT_PLOT_DIALOG_FLAG, 0);
    showBuildDotPlotDialog(view);
}

}//namespace
