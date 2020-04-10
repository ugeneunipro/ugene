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

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QScrollArea>
#include <QToolBar>
#include <QVBoxLayout>
#include <limits>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ModifySequenceObjectTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/RemoveAnnotationsTask.h>
#include <U2Core/ReverseSequenceTask.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Gui/CreateObjectRelationDialogController.h>
#include <U2Gui/EditSequenceDialogController.h>
#include <U2Gui/EditSettingsDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/PositionSelector.h>
#include <U2Gui/RemovePartFromSequenceDialogController.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/CodonTable.h>
#include <U2View/FindPatternWidgetFactory.h>
#include <U2View/SecStructPredictUtils.h>

#include "ADVAnnotationCreation.h"
#include "ADVClipboard.h"
#include "ADVConstants.h"
#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "ADVSyncViewManager.h"
#include "AnnotatedDNAView.h"
#include "AnnotatedDNAViewFactory.h"
#include "AnnotatedDNAViewState.h"
#include "AnnotatedDNAViewTasks.h"
#include "AnnotationsTreeView.h"
#include "AutoAnnotationUtils.h"
#include "DetView.h"
#include "DetViewSequenceEditor.h"
#include "GraphMenu.h"

#ifdef max
#undef max
#endif

namespace U2 {

AnnotatedDNAView::AnnotatedDNAView(const QString& viewName, const QList<U2SequenceObject*>& dnaObjects)
: GObjectView(AnnotatedDNAViewFactory::ID, viewName)
{
    timerId = 0;
    hadExpandableSequenceWidgetsLastResize = false;

    annotationSelection = new AnnotationSelection(this);
    annotationGroupSelection = new AnnotationGroupSelection(this);

    clipb = NULL;

    mainSplitter = NULL;
    scrollArea = NULL;
    posSelector = NULL;
    posSelectorWidgetAction = NULL;
    annotationsView = NULL;
    focusedWidget = NULL;
    replacedSeqWidget = NULL;

    codonTableView = new CodonTableView(this);
    connect(this, SIGNAL(si_focusChanged(ADVSequenceWidget*,ADVSequenceWidget*)),
            codonTableView, SLOT(sl_onSequenceFocusChanged(ADVSequenceWidget*,ADVSequenceWidget*)));
    createCodonTableAction();
    createAnnotationAction = (new ADVAnnotationCreation(this))->getCreateAnnotationAction();

    posSelectorAction = new QAction(QIcon(":core/images/goto.png"), tr("Go to position..."), this);
    posSelectorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    posSelectorAction->setShortcutContext(Qt::WindowShortcut);
    posSelectorAction->setObjectName(ADV_GOTO_ACTION);
    connect(posSelectorAction, SIGNAL(triggered()), SLOT(sl_onShowPosSelectorRequest()));

    toggleHLAction = new QAction("", this);
    connect(toggleHLAction, SIGNAL(triggered()), SLOT(sl_toggleHL()));

    removeAnnsAndQsAction = new QAction("",this);
    removeAnnsAndQsAction->setShortcut(QKeySequence(Qt::Key_Delete));
    removeAnnsAndQsAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    syncViewManager = new ADVSyncViewManager(this);

    foreach(U2SequenceObject* dnaObj, dnaObjects) {
        addObject(dnaObj);
    }

    findPatternAction = new ADVGlobalAction(this, QIcon(":core/images/find_dialog.png"), tr("Find pattern..."), 10);
    findPatternAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    findPatternAction->setShortcutContext(Qt::WindowShortcut);
    connect(findPatternAction, SIGNAL(triggered()), SLOT(sl_onFindPatternClicked()));

    editSettingsAction = new QAction(tr("Annotations settings on sequence editing..."), this);
    editSettingsAction->setObjectName(ACTION_EDIT_SEQUENCE_SETTINGS);
    connect(editSettingsAction, SIGNAL(triggered()), this, SLOT(sl_editSettings()));

    addSequencePart = new QAction(tr("Insert subsequence..."), this);
    addSequencePart->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    addSequencePart->setObjectName(ACTION_EDIT_INSERT_SUBSEQUENCE);
    connect(addSequencePart, SIGNAL(triggered()), this, SLOT(sl_addSequencePart()));

    removeSequencePart = new QAction(tr("Remove subsequence..."), this);
    removeSequencePart->setObjectName(ACTION_EDIT_REMOVE_SUBSEQUENCE);
    connect(removeSequencePart, SIGNAL(triggered()), this, SLOT(sl_removeSequencePart()));

    replaceSequencePart = new QAction(tr("Replace subsequence..."), this);
    replaceSequencePart ->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    replaceSequencePart->setObjectName(ACTION_EDIT_REPLACE_SUBSEQUENCE);
    connect(replaceSequencePart, SIGNAL(triggered()), this, SLOT(sl_replaceSequencePart()));

    removeSequenceObjectAction = new QAction(tr("Selected sequence from view"), this);
    removeSequenceObjectAction->setObjectName(ACTION_EDIT_SELECT_SEQUENCE_FROM_VIEW);
    connect(removeSequenceObjectAction, SIGNAL(triggered()), SLOT(sl_removeSelectedSequenceObject()));

    reverseComplementSequenceAction = new QAction(tr("Reverse-complement sequence"), this);
    reverseComplementSequenceAction ->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    reverseComplementSequenceAction->setObjectName(ACTION_EDIT_RESERVE_COMPLEMENT_SEQUENCE);
    connect(reverseComplementSequenceAction, SIGNAL(triggered()), SLOT(sl_reverseComplementSequence()));

    reverseSequenceAction = new QAction(tr("Reverse sequence"), this);
    reverseSequenceAction->setObjectName(ACTION_EDIT_RESERVE_SEQUENCE);
    connect(reverseSequenceAction, SIGNAL(triggered()), SLOT(sl_reverseSequence()));

    complementSequenceAction = new QAction(tr("Complement sequence"), this);
    complementSequenceAction->setObjectName(ACTION_EDIT_COMPLEMENT_SEQUENCE);
    connect(complementSequenceAction, SIGNAL(triggered()), SLOT(sl_complementSequence()));

    SecStructPredictViewAction::createAction(this);
}

QAction * AnnotatedDNAView::createPasteAction() {
    QAction *action = ADVClipboard::createPasteSequenceAction(this);
    connect(action, SIGNAL(triggered()), this, SLOT(sl_paste()));
    return action;
}

QWidget* AnnotatedDNAView::createWidget() {
    GTIMER(c1,t1,"AnnotatedDNAView::createWidget");
    assert(scrollArea == NULL);

    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->setObjectName("annotated_DNA_splitter");
    connect(mainSplitter, SIGNAL(splitterMoved(int, int)), SLOT(sl_splitterMoved(int, int)));

    mainSplitter->addWidget(codonTableView);
    mainSplitter->setCollapsible(mainSplitter->indexOf(codonTableView), false);

    mainSplitter->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mainSplitter, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested()));

    scrollArea = new QScrollArea();
    scrollArea->setObjectName("annotated_DNA_scrollarea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    mainSplitter->addWidget(scrollArea);
    mainSplitter->setHandleWidth(1); // make smaller the distance between the Annotations Editor and the  sequence sub-views
    mainSplitter->setCollapsible(mainSplitter->indexOf(scrollArea), false);

    scrolledWidget = new QWidget(scrollArea);
    scrolledWidgetLayout = new QVBoxLayout();
    scrolledWidgetLayout->setContentsMargins(0, 0, 0, 0);
    scrolledWidgetLayout->setSpacing(0);
    scrolledWidgetLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    scrolledWidget->setBackgroundRole(QPalette::Light);
    scrolledWidget->installEventFilter(this);
    scrolledWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    clipb = new ADVClipboard(this);
    QAction* pasteAction = clipb->getPasteSequenceAction();
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(sl_paste()));

    annotationsView = new AnnotationsTreeView(this);
    annotationsView->setParent(mainSplitter);
    annotationsView->setObjectName("annotations_tree_view");
    connect(annotationsView, SIGNAL(si_setCopyQualifierActionStatus(bool, QString)), clipb, SLOT(sl_setCopyQualifierActionStatus(bool, QString)));
    connect(clipb->getCopyQualifierAction(), SIGNAL(triggered()), annotationsView, SLOT(sl_onCopyQualifierValue()));

    for (int i = 0; i < seqContexts.size(); ++i) {
        ADVSequenceObjectContext* seqCtx = seqContexts[i];
        ADVSingleSequenceWidget* block = new ADVSingleSequenceWidget(seqCtx, this);
        connect(block, SIGNAL(si_titleClicked(ADVSequenceWidget*)), SLOT(sl_onSequenceWidgetTitleClicked(ADVSequenceWidget*)));
        connect(seqCtx, SIGNAL(si_aminoTranslationChanged()), SLOT(sl_aminoTranslationChanged()));
        block->setObjectName("ADV_single_sequence_widget_" + QString::number(i));
        addSequenceWidget(block);
        block->addAction(createPasteAction());
    }

    mainSplitter->addWidget(annotationsView);
    mainSplitter->setCollapsible(mainSplitter->indexOf(annotationsView), false);

    scrolledWidget->setLayout(scrolledWidgetLayout);
    scrolledWidget->setObjectName("scrolled_widget_layout");

    //TODO: scroll area does not restore focus for last active child widget after Alt-Tab...
    scrollArea->setWidget(scrolledWidget);

    mainSplitter->installEventFilter(this);
    mainSplitter->setAcceptDrops(true);

    if (!seqViews.isEmpty()) {
        setFocusedSequenceWidget(seqViews.last());
    }

//add view global shortcuts

    connect(removeAnnsAndQsAction, SIGNAL(triggered()),annotationsView->removeAnnsAndQsAction, SIGNAL(triggered()));

    mainSplitter->addAction(toggleHLAction);
    mainSplitter->addAction(removeSequenceObjectAction);

    mainSplitter->addAction(removeAnnsAndQsAction);

    mainSplitter->setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::SEQUENCE).icon);

    // Init the Options Panel
    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;

    ADVSequenceObjectContext* ctx;
    QList <DNAAlphabetType> alphabets;

    for (int i = 0; i < seqViews.size(); i++) {
        if (seqViews[i] != NULL) {
            ctx = seqViews[i]->getActiveSequenceContext();
            if (ctx) {
                const DNAAlphabet *alphabet = ctx->getAlphabet();
                if (alphabet) {
                    alphabets.append(alphabet->getType());
                }
            }
        }
    }
    filters.append(new OPFactoryFilterVisitor(ObjViewType_SequenceView, alphabets));

    QList<OPWidgetFactory*> opWidgetFactoriesForSeqView = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory* factory, opWidgetFactoriesForSeqView) {
        optionsPanel->addGroup(factory);
    }

    qDeleteAll(filters);
    return mainSplitter;
}

OptionsPanel* AnnotatedDNAView::getOptionsPanel()
{
    return optionsPanel;
}

void AnnotatedDNAView::sl_splitterMoved(int, int) {
    // WORKAROUND: looks like a QT bug:
    // ADVSequenceWidgets get paint events as needed, but scrolledWidget is over-painted by splitter's handle
    // to reproduce it open any complex (like 3d structure) view and pull the splitter handle upward slowly
    // -> workaround: update geometry for scrollArea or repaint main splitter's ares (todo: recheck effect)
    mainSplitter->repaint(scrollArea->geometry());
    mainSplitter->refresh();
}

void AnnotatedDNAView::sl_onSequenceWidgetTitleClicked(ADVSequenceWidget* seqWidget) {
    replacedSeqWidget = seqWidget;
}

void AnnotatedDNAView::timerEvent(QTimerEvent*) {
    //see comment for sl_splitterMoved()
    assert(timerId!=0);
    killTimer(timerId);
    timerId = 0;

    QWidget* w = scrollArea;
    QRect orig = w->geometry();
    QRect tmp = orig;
    tmp.adjust(0, 0, 1, 1);
    w->setGeometry(tmp);
    w->setGeometry(orig);
}

void AnnotatedDNAView::updateScrollAreaHeight() {
    if (!scrolledWidget->isVisible()) {
        return;
    }

    int newScrollAreaMaxHeight = 0;
    foreach(ADVSequenceWidget* v, seqViews) {
        int widgetMaxHeight = v->maximumHeight();
        if (widgetMaxHeight == QWIDGETSIZE_MAX) {
            scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
            return;
        }
        newScrollAreaMaxHeight += v->maximumHeight();
    }
    newScrollAreaMaxHeight += 2;  // magic '+2' is for the borders, without it unneccessary scroll bar will appear
    if (newScrollAreaMaxHeight <= scrollArea->height()) {
        scrollArea->setMaximumHeight(newScrollAreaMaxHeight);
    }
}

AnnotatedDNAView::~AnnotatedDNAView() {
    delete posSelector;
}

bool AnnotatedDNAView::eventFilter(QObject* o, QEvent* e) {
    if (o == mainSplitter) {
        if (e->type() == QEvent::DragEnter || e->type() == QEvent::Drop) {
            QDropEvent* de = (QDropEvent*)e;
            const QMimeData* md = de->mimeData();
            const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
            if (gomd != NULL) {
                if (e->type() == QEvent::DragEnter) {
                    de->acceptProposedAction();
                } else {
                    GObject* obj = gomd->objPtr.data();
                    if (obj!=NULL) {
                        QString err = tryAddObject(obj);
                        if (!err.isEmpty()) {
                            QMessageBox::critical(NULL, tr("Error!"), err);
                        }
                    }
                }
            }
        }
    } else if (o == scrolledWidget) {
        if (replacedSeqWidget && e->type() == QEvent::MouseMove) {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(e);
            if (event->buttons() == Qt::LeftButton) {
                seqWidgetMove(event->pos());
            }
        } else if (replacedSeqWidget && e->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(e);
            if (event->buttons() == Qt::LeftButton) {
                finishSeqWidgetMove();
            }
        }
        // try to restore mainSplitter state on sequence views fixed <-> expandable state transition. Usually this happens when user toggles sequence views.
        if (e->type() == QEvent::Resize) {
            bool hasExpandableSequenceWidgetsNow = false; // expandable state: any of the sequence view widgets has unlimited height.
            foreach (const ADVSequenceWidget* w, getSequenceWidgets()){
                if (w->maximumHeight() == QWIDGETSIZE_MAX) {
                    hasExpandableSequenceWidgetsNow = true;
                    break;
                }
            }
            if (hasExpandableSequenceWidgetsNow != hadExpandableSequenceWidgetsLastResize) { // transition from fixed <-> expandable state
                if (hasExpandableSequenceWidgetsNow) { // try restore state from the saved sizes if possible.
                    if (savedMainSplitterSizes.size() > 0 && savedMainSplitterSizes.size() == mainSplitter->sizes().size()) {
                        mainSplitter->setSizes(savedMainSplitterSizes);
                    }
                }
                hadExpandableSequenceWidgetsLastResize = hasExpandableSequenceWidgetsNow;
            }
            if (hasExpandableSequenceWidgetsNow) { // update saved sizes for a future use.
                savedMainSplitterSizes = mainSplitter->sizes();
            }
        }
        return false;
    } else if (e->type() == QEvent::Resize) {
        ADVSequenceWidget* v = qobject_cast<ADVSequenceWidget*>(o);
        if (v!=NULL) {
            updateScrollAreaHeight();
        }
    } else if(e->type() == QEvent::KeyPress){
        sl_selectionChanged();
    }

    return false;
}

void AnnotatedDNAView::setFocusedSequenceWidget(ADVSequenceWidget* v) {
    if (v == focusedWidget) {
        return;
    }
    ADVSequenceWidget* prevFocus = focusedWidget;
    focusedWidget = v;
    updateMultiViewActions();
    sl_updatePasteAction();
    emit si_focusChanged(prevFocus, focusedWidget);
}

bool AnnotatedDNAView::onCloseEvent() {
    QList<AutoAnnotationObject*> aaList = autoAnnotationsMap.values();
    bool waitFinishedRemovedTasks = false;
    foreach(AutoAnnotationObject* aa, aaList) {
        bool existRemovedTask = false;
        cancelAutoAnnotationUpdates(aa, &existRemovedTask);
        waitFinishedRemovedTasks = waitFinishedRemovedTasks || existRemovedTask;
    }
    if(waitFinishedRemovedTasks){
        QMessageBox::information(this->getWidget(), "information", "Can not close view while there are annotations being processed");
        return false;
    }
    foreach (ADVSplitWidget* w, splitWidgets) {
        bool canClose = w->onCloseEvent();
        if (!canClose) {
            return false;
        }
    }
    emit si_onClose(this);
    return true;
}

bool AnnotatedDNAView::onObjectRemoved(GObject* o) {
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        AnnotationTableObject *ao = qobject_cast<AnnotationTableObject *>(o);
        annotationSelection->removeObjectAnnotations(ao);
        foreach (ADVSequenceObjectContext *seqCtx, seqContexts) {
            if (seqCtx->getAnnotationObjects().contains(ao)) {
                seqCtx->removeAnnotationObject(ao);
                break;
            }
        }
        annotations.removeOne(ao);
        emit si_annotationObjectRemoved(ao);
    } else if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
        U2SequenceObject *seqObj = qobject_cast<U2SequenceObject *>(o);
        ADVSequenceObjectContext *seqCtx = getSequenceContext(seqObj);
        if (NULL != seqCtx) {
            foreach (ADVSequenceWidget *w, seqCtx->getSequenceWidgets()) {
                removeSequenceWidget(w);
            }
            QSet<AnnotationTableObject *> aObjs = seqCtx->getAnnotationObjects();
            foreach (AnnotationTableObject *ao, aObjs) {
                removeObject(ao);
            }
            emit si_sequenceRemoved(seqCtx);
            seqContexts.removeOne(seqCtx);
            removeAutoAnnotations(seqCtx);
            delete seqCtx;
        }
    }

    GObjectView::onObjectRemoved(o);
    return seqContexts.isEmpty();
}

void AnnotatedDNAView::addADVAction(ADVGlobalAction* a1) {
    for (int i=0; i<advActions.size(); i++) {
        ADVGlobalAction* a2 = advActions[i];
        int p1 = a1->getPosition();
        int p2 = a2->getPosition();
        if (p1 < p2 || (p1 == p2 && a1->text() < a2->text())) {
            advActions.insert(i, a1);
            return;
        }
    }
    advActions.append(a1);
}

void AnnotatedDNAView::buildStaticToolbar(QToolBar* tb) {
    tb->addAction(createAnnotationAction);

    tb->addSeparator();
    tb->addAction(clipb->getCopySequenceAction());
    tb->addAction(clipb->getCopyComplementAction());
    tb->addAction(clipb->getCopyTranslationAction());
    tb->addAction(clipb->getCopyComplementTranslationAction());
    tb->addAction(clipb->getCopyAnnotationSequenceAction());
    tb->addAction(clipb->getCopyComplementAnnotationSequenceAction());
    tb->addAction(clipb->getCopyAnnotationSequenceTranslationAction());
    tb->addAction(clipb->getCopyComplementAnnotationSequenceTranslationAction());
    tb->addAction(clipb->getCopyQualifierAction());
    tb->addAction(clipb->getPasteSequenceAction());
    tb->addSeparator();

    if (posSelector == NULL && !seqContexts.isEmpty()) {
        qint64 len = seqContexts.first()->getSequenceLength();
        posSelector = new PositionSelector(tb, 1, len);
        connect(posSelector, SIGNAL(si_positionChanged(int)), SLOT(sl_onPosChangeRequest(int)));
        posSelectorWidgetAction = tb->addWidget(posSelector);
    }  else {
        tb->addAction(posSelectorWidgetAction);
    }

    tb->addSeparator();
    syncViewManager->updateToolbar1(tb);
    tb->addSeparator();

    foreach(ADVGlobalAction* a, advActions) {
        if (a->getFlags().testFlag(ADVGlobalActionFlag_AddToToolbar)) {
            tb->addAction(a);
            QWidget* w = tb->widgetForAction(a);
            if (w) {
                w->setObjectName(a->objectName() + "_widget");
            }
        }
    }

    GObjectView::buildStaticToolbar(tb);

    tb->addSeparator();
    syncViewManager->updateToolbar2(tb);
}

void AnnotatedDNAView::buildStaticMenu(QMenu* m) {
    m->addAction(posSelectorAction);
    clipb->addCopyMenu(m);
    m->addSeparator();
    addAddMenu(m);
    addAnalyseMenu(m);
    addAlignMenu(m);
    addExportMenu(m);
    addRemoveMenu(m);
    addEditMenu(m);
    m->addSeparator();

    annotationsView->adjustStaticMenu(m);

    GObjectView::buildStaticMenu(m);
}

ADVSequenceWidget * AnnotatedDNAView::findSequenceWidgetByPos(const QPoint& globalPos) const {
    foreach(ADVSequenceWidget* slv, seqViews) {
        const QRect& rect = slv->rect();
        QPoint localPos = slv->mapFromGlobal(globalPos);
        if (rect.contains(localPos)) {
            return slv;
        }
    }
    return NULL;
}

void AnnotatedDNAView::addAnalyseMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Analyze"));
    am->menuAction()->setObjectName(ADV_MENU_ANALYSE);
    foreach(ADVGlobalAction* a, advActions) {
        if (a->getFlags().testFlag(ADVGlobalActionFlag_AddToAnalyseMenu)) {
            am->addAction(a);
        }
    }
}

void AnnotatedDNAView::addAddMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Add"));
    am->menuAction()->setObjectName(ADV_MENU_ADD);
    am->addAction(createAnnotationAction);
}

void AnnotatedDNAView::addExportMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Export"));
    em->menuAction()->setObjectName(ADV_MENU_EXPORT);
}


void AnnotatedDNAView::addAlignMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Align"));
    am->menuAction()->setObjectName(ADV_MENU_ALIGN);
}


void AnnotatedDNAView::addRemoveMenu(QMenu* m) {
    QMenu* rm = m->addMenu(tr("Remove"));
    rm->menuAction()->setObjectName(ADV_MENU_REMOVE);

    rm->addAction(removeSequenceObjectAction);
}

void AnnotatedDNAView::addEditMenu(QMenu* m) {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    SAFE_POINT(NULL != seqCtx, "Sequence in focus is NULL",);
    U2SequenceObject *seqObj = seqCtx->getSequenceObject();
    SAFE_POINT(NULL != seqObj, "Sequence object in focus is NULL",);
    Document *curDoc = seqObj->getDocument();
    SAFE_POINT(NULL != curDoc, "Current document is NULL",);
    QMenu* rm = m->addMenu(tr("Edit"));

    if(curDoc->findGObjectByType(GObjectTypes::SEQUENCE).isEmpty() || seqObj->isStateLocked()) {
        rm->setDisabled(true);
    }else{
        rm->setEnabled(true);
    };
    rm->menuAction()->setObjectName(ADV_MENU_EDIT);

    if (annotationSelection->getAnnotations().size() == 1 && annotationsView->editAction->isEnabled()) {
        rm->addAction(annotationsView->editAction);
    }

    rm->addAction(editSettingsAction);
    rm->addAction(addSequencePart);
    rm->addAction(replaceSequencePart);
    sl_selectionChanged();
    rm->addAction(removeSequencePart);
    if (seqCtx->getComplementTT() != NULL) {
        reverseComplementSequenceAction->setEnabled(true);
        complementSequenceAction->setEnabled(true);
    } else {
        reverseComplementSequenceAction->setEnabled(false);
        complementSequenceAction->setEnabled(false);
    }
    rm->addAction(reverseComplementSequenceAction);
    rm->addAction(reverseSequenceAction);
    rm->addAction(complementSequenceAction);
}

Task* AnnotatedDNAView::updateViewTask(const QString& stateName, const QVariantMap& stateData) {
    return new UpdateAnnotatedDNAViewTask(this, stateName, stateData);
}

QVariantMap AnnotatedDNAView::saveState() {
    if (closing) {
        return QVariantMap();
    }
    QVariantMap state = AnnotatedDNAViewState::saveState(this);
    foreach(ADVSequenceWidget* sw, seqViews) {
        sw->saveState(state);
    }
    foreach(ADVSplitWidget* w, splitWidgets) {
        w->saveState(state);
    }
    annotationsView->saveState(state);
    return state;
}

void AnnotatedDNAView::saveWidgetState() {
    annotationsView->saveWidgetState();
}


bool AnnotatedDNAView::canAddObject(GObject* obj) {
    if (GObjectView::canAddObject(obj)) {
        return true;
    }
    if (isChildWidgetObject(obj)) {
        return true;
    }
    if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        return true;
    }
    if (obj->getGObjectType()!=GObjectTypes::ANNOTATION_TABLE) {
            return false;
    }
    //todo: add annotations related to sequence object not in view (sobj) and add 'sobj' too the view ?
    bool hasRelation = false;
    foreach(ADVSequenceObjectContext* soc, seqContexts) {
        if (obj->hasObjectRelation(soc->getSequenceObject(), ObjectRole_Sequence)) {
            hasRelation = true;
            break;
        }
    }
    return hasRelation;
}

bool AnnotatedDNAView::isChildWidgetObject(GObject* obj) const {
    foreach(ADVSequenceWidget* lv, seqViews) {
        SAFE_POINT(lv != NULL, "AnnotatedDNAView::isChildWidgetObject::No sequence widget", false);
        if (lv->isWidgetOnlyObject(obj)) {
            return true;
        }
    }
    foreach(ADVSplitWidget* sw,  splitWidgets) {
        SAFE_POINT(sw != NULL, "AnnotatedDNAView::isChildWidgetObject::No split widget", false);
        if (sw->acceptsGObject(obj)) {
            return true;
        }
    }
     return false;
}

void AnnotatedDNAView::addSequenceWidget(ADVSequenceWidget* v) {
    assert(!seqViews.contains(v));
    seqViews.append(v);

    QAction* editAction = getEditActionFromSequenceWidget(v);
    SAFE_POINT(editAction != NULL, "Edit action is not found", );

    connect(editAction, SIGNAL(triggered()), SLOT(sl_updatePasteAction()));

    QList<ADVSequenceObjectContext*> contexts = v->getSequenceContexts();
    foreach(ADVSequenceObjectContext* c, contexts) {
        c->addSequenceWidget(v);
        addAutoAnnotations(c);
        addGraphs(c);
        connect(c->getSequenceSelection(), SIGNAL(si_selectionChanged(LRegionsSelection*, QVector<U2Region>, QVector<U2Region>)), SLOT(sl_selectionChanged()));
        clipb->connectSequence(c);
    }
    scrolledWidgetLayout->addWidget(v);
    v->setVisible(true);
    v->installEventFilter(this);
    updateScrollAreaHeight();
    updateMultiViewActions();
    emit si_sequenceWidgetAdded(v);
}

void AnnotatedDNAView::removeSequenceWidget(ADVSequenceWidget* v) {
    assert(seqViews.contains(v));
    int idx = seqViews.indexOf(v);
    assert(idx>=0);

    //fix focus
    if (focusedWidget == v) {
        if (idx + 1 < seqViews.size()) {
            setFocusedSequenceWidget(seqViews[idx+1]);
        } else if (idx -1 >= 0) {
            setFocusedSequenceWidget(seqViews[idx-1]);
        } else {
            setFocusedSequenceWidget(NULL);
        }
    }

    //remove widget
    seqViews.removeOne(v);
    v->hide();

    QList<ADVSequenceObjectContext*> contexts = v->getSequenceContexts();
    foreach(ADVSequenceObjectContext* c, contexts) {
        c->removeSequenceWidget(v);
        disconnect(c->getSequenceSelection(), SIGNAL(si_selectionChanged(LRegionsSelection*, QVector<U2Region>, QVector<U2Region>)));
    }
    updateMultiViewActions();
    emit si_sequenceWidgetRemoved(v);
    scrolledWidgetLayout->removeWidget(v);
    delete v;
    //v->deleteLater(); //problem: updates for 'v' after seqCtx is destroyed
    updateScrollAreaHeight();
}

void AnnotatedDNAView::updateMultiViewActions() {
    bool canRemoveFocusedSequence = seqViews.size() > 1 && focusedWidget != NULL && focusedWidget->getActiveSequenceContext() != NULL;
    removeSequenceObjectAction->setEnabled(canRemoveFocusedSequence);

    if (NULL != posSelector) {
        qint64 currentSequenceLength = 0;
        if (NULL != focusedWidget && NULL != focusedWidget->getActiveSequenceContext()) {
            currentSequenceLength = focusedWidget->getActiveSequenceContext()->getSequenceLength();
        }
        posSelector->updateRange(1, currentSequenceLength);
    }
}

void AnnotatedDNAView::sl_updatePasteAction() {
    CHECK(focusedWidget != NULL, );

    QAction* editAction = getEditActionFromSequenceWidget(focusedWidget);
    SAFE_POINT(editAction != NULL, "Edit action is not found", );

    const bool isEditModeChecked = editAction->isChecked();

    QAction* pasteAction = clipb->getPasteSequenceAction();
    SAFE_POINT(pasteAction != NULL, "Paste action is NULL", );

    pasteAction->setEnabled(isEditModeChecked);
}

void AnnotatedDNAView::sl_onContextMenuRequested() {
    QMenu m;

    m.addAction(posSelectorAction);

    m.addSeparator()->setObjectName("FIRST_SEP");
    clipb->addCopyMenu(&m);
    m.addSeparator()->setObjectName(ADV_MENU_SECTION1_SEP);
    addAddMenu(&m);
    addAnalyseMenu(&m);
    addAlignMenu(&m);
    addExportMenu(&m);
    addEditMenu(&m);
    addRemoveMenu(&m);
    m.addSeparator()->setObjectName(ADV_MENU_SECTION2_SEP);

    if (annotationSelection->getAnnotations().size() == 1) {
        Annotation *a = annotationSelection->getAnnotations().first();
        const SharedAnnotationData &aData = a->getData();
        AnnotationSettingsRegistry *registry = AppContext::getAnnotationsSettingsRegistry();
        AnnotationSettings *as = registry->getAnnotationSettings(aData);
        if (as->visible) {
            toggleHLAction->setText(tr("Disable '%1' highlighting").arg(aData->name));
        } else {
            toggleHLAction->setText(tr("Enable '%1' highlighting").arg(aData->name));
        }

        const QIcon icon = GUIUtils::createSquareIcon(as->color, 10);
        toggleHLAction->setIcon(icon);

        toggleHLAction->setObjectName("toggle_HL_action");
        m.addAction(toggleHLAction);
    }

    if (NULL != focusedWidget) {
        focusedWidget->buildPopupMenu(m);
    }
    emit si_buildPopupMenu(this, &m);

    m.exec(QCursor::pos());
}

void AnnotatedDNAView::sl_onFindPatternClicked() {
    OptionsPanel* optionsPanel = getOptionsPanel();
    SAFE_POINT(NULL != optionsPanel, "Internal error: options panel is NULL"
        " when pattern search has been initiated!",);

    const QString &findPatternGroupId = FindPatternWidgetFactory::getGroupId();
    optionsPanel->openGroupById(findPatternGroupId);
}

void AnnotatedDNAView::sl_toggleHL() {
    if (annotationSelection->isEmpty()) {
        return;
    }
    const Annotation *a = annotationSelection->getAnnotations().first();
    AnnotationSettingsRegistry *registry = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings *as = registry->getAnnotationSettings(a->getData());
    as->visible = !as->visible;
    registry->changeSettings(QList<AnnotationSettings *>() << as, true);
}

QString AnnotatedDNAView::tryAddObject(GObject* o) {
    if (o->getGObjectType() == GObjectTypes::UNLOADED) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new AddToViewTask(this, o));
        return "";
    }
    QList<ADVSequenceObjectContext*> rCtx;
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        rCtx = findRelatedSequenceContexts(o);
        if (rCtx.isEmpty()) {
            //ask user if to create new association
            QObjectScopedPointer<CreateObjectRelationDialogController> d = new CreateObjectRelationDialogController(o, getSequenceGObjectsWithContexts(),
                ObjectRole_Sequence, true,
                tr("Select sequence to associate annotations with:"));
            d->exec();
            CHECK(!d.isNull(), "");

            rCtx = findRelatedSequenceContexts(o);
            if (rCtx.isEmpty()) {
                return "";
            }
        }
    }
    return addObject(o);
}

QString AnnotatedDNAView::addObject(GObject *o) {
    QList<ADVSequenceObjectContext *> rCtx;
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        rCtx = findRelatedSequenceContexts(o);
        if (rCtx.isEmpty()) {
            return tr("No sequence object found for annotations");
        }
    }
    QString res = GObjectView::addObject(o);
    if (!res.isEmpty()) {
        return res;
    }

    bool internalViewObject = isChildWidgetObject(o);
    if (internalViewObject) {
        return "";
    }

    if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
        U2SequenceObject *dnaObj = qobject_cast<U2SequenceObject *>(o);
        U2OpStatusImpl status;
        if (!dnaObj->isValidDbiObject(status)) {
            return "";
        }
        ADVSequenceObjectContext *sc = new ADVSequenceObjectContext(this, dnaObj);
        seqContexts.append(sc);
        //if mainSplitter==NULL -> its view initialization and widgets will be added later
        if (NULL != mainSplitter && !isChildWidgetObject(dnaObj)) {
            ADVSingleSequenceWidget *block = new ADVSingleSequenceWidget(sc, this);
            connect(block, SIGNAL(si_titleClicked(ADVSequenceWidget *)),
                SLOT(sl_onSequenceWidgetTitleClicked(ADVSequenceWidget *)));
            block->setObjectName("ADV_single_sequence_widget_" + QString::number(seqViews.count()));
            addSequenceWidget(block);
            block->addAction(createPasteAction());
            setFocusedSequenceWidget(block);
        }
        addRelatedAnnotations(sc);
        emit si_sequenceAdded(sc);
    } else if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        AnnotationTableObject *ao = qobject_cast<AnnotationTableObject *>(o);
        SAFE_POINT(NULL != ao, "Invalid annotation table!", QString::null);
        annotations.append(ao);
        foreach (ADVSequenceObjectContext *sc, rCtx) {
            sc->addAnnotationObject(ao);
        }
        emit si_annotationObjectAdded(ao);
    }
    return "";
}

QList<ADVSequenceObjectContext *> AnnotatedDNAView::findRelatedSequenceContexts(GObject *obj) const {
    QList<GObject *> relatedObjects = GObjectUtils::selectRelations(obj, GObjectTypes::SEQUENCE,
        ObjectRole_Sequence, objects, UOF_LoadedOnly);
    QList<ADVSequenceObjectContext *> res;
    foreach (GObject *seqObj, relatedObjects) {
        U2SequenceObject *dnaObj = qobject_cast<U2SequenceObject *>(seqObj);
        ADVSequenceObjectContext *ctx = getSequenceContext(dnaObj);
        res.append(ctx);
    }
    return res;
}

void AnnotatedDNAView::sl_onPosChangeRequest(int pos) {
    uiLog.trace(QString("ADV: center change request: %1").arg(pos));
    ADVSequenceWidget* seqBlock = getSequenceWidgetInFocus();
    assert(seqBlock!=NULL);
    seqBlock->centerPosition(pos-1);
}

void AnnotatedDNAView::sl_onShowPosSelectorRequest() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    assert(seqCtx!=NULL);

    QObjectScopedPointer<QDialog> dlg = new QDialog(getWidget());
    dlg->setModal(true);
    dlg->setWindowTitle(tr("Go To"));

    PositionSelector *ps = new PositionSelector(dlg.data(), 1, seqCtx->getSequenceLength(), true);
    connect(ps, SIGNAL(si_positionChanged(int)), SLOT(sl_onPosChangeRequest(int)));

    dlg->exec();
}

void AnnotatedDNAView::insertWidgetIntoSplitter(ADVSplitWidget* splitWidget) {
    assert(mainSplitter!=NULL);
    if (splitWidgets.contains(splitWidget)) {
        return;
    }
    mainSplitter->insertWidget(0, splitWidget);
    mainSplitter->setStretchFactor(0, 1);
    splitWidgets.append(splitWidget);
}

void AnnotatedDNAView::unregisterSplitWidget(ADVSplitWidget* splitWidget) {
    splitWidgets.removeOne(splitWidget);
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(AnnotationTableObject *obj) const {
    SAFE_POINT(getAnnotationObjects(true).contains(obj),
        "Unexpected annotation table detected!", NULL);
    foreach (ADVSequenceObjectContext *seqCtx, seqContexts) {
        QSet<AnnotationTableObject *> aObjs = seqCtx->getAnnotationObjects(true);
        if (aObjs.contains(obj)) {
            return seqCtx;
        }
    }
    return NULL;
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceInFocus() const {
    ADVSequenceWidget* w = getSequenceWidgetInFocus();
    return w == NULL ? NULL : w->getActiveSequenceContext();
}

QList<ADVSequenceObjectContext*> AnnotatedDNAView::getAllSeqContextsInFocus() const {
    return getSequenceWidgetInFocus()->getSequenceContexts();
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(U2SequenceObject* obj) const {
    foreach(ADVSequenceObjectContext* seqCtx, seqContexts) {
        if(seqCtx->getSequenceObject() == obj) {
            return seqCtx;
        }
    }
    return NULL;
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(const GObjectReference& r) const {
    foreach(ADVSequenceObjectContext* seqCtx, seqContexts) {
        GObjectReference ref(seqCtx->getSequenceObject());
        if (ref == r) {
            return seqCtx;
        }
    }
    return NULL;
}

void AnnotatedDNAView::addRelatedAnnotations(ADVSequenceObjectContext *seqCtx) {
    QList<GObject *> allLoadedAnnotations = GObjectUtils::findAllObjects(UOF_LoadedOnly,
        GObjectTypes::ANNOTATION_TABLE);
    QList<GObject *> annotations = GObjectUtils::findObjectsRelatedToObjectByRole(seqCtx->getSequenceObject(),
                                    GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence,
                                    allLoadedAnnotations, UOF_LoadedOnly);

    foreach (GObject *ao, annotations) {
        if (objects.contains(ao)) {
            seqCtx->addAnnotationObject(qobject_cast<AnnotationTableObject *>(ao));
        } else {
            addObject(ao);
        }
    }
}

void AnnotatedDNAView::addAutoAnnotations(ADVSequenceObjectContext* seqCtx) {
    AutoAnnotationObject *aa = new AutoAnnotationObject(seqCtx->getSequenceObject(), seqCtx->getAminoTT(), seqCtx);
    seqCtx->addAutoAnnotationObject(aa->getAnnotationObject());
    autoAnnotationsMap.insert(seqCtx, aa);

    emit si_annotationObjectAdded(aa->getAnnotationObject());

    foreach (ADVSequenceWidget *w, seqCtx->getSequenceWidgets()) {
        AutoAnnotationsADVAction *aaAction = new AutoAnnotationsADVAction(w, aa);
        w->addADVSequenceWidgetAction(aaAction);
    }
}

void AnnotatedDNAView::removeAutoAnnotations(ADVSequenceObjectContext *seqCtx)
{
    AutoAnnotationObject* aa = autoAnnotationsMap.take(seqCtx);
    cancelAutoAnnotationUpdates(aa);
    emit si_annotationObjectRemoved(aa->getAnnotationObject());
    delete aa;
}

void AnnotatedDNAView::cancelAutoAnnotationUpdates(AutoAnnotationObject* aa, bool* removeTaskExist)
{
    QList<Task*> tasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    foreach (Task* t, tasks) {
        AutoAnnotationsUpdateTask* aaUpdateTask = qobject_cast<AutoAnnotationsUpdateTask*> (t);
        if (aaUpdateTask != NULL) {
            if (aaUpdateTask->getAutoAnnotationObject() == aa && !aaUpdateTask->isFinished()) {
                aaUpdateTask->setAutoAnnotationInvalid();
                aaUpdateTask->cancel();
                if(removeTaskExist){
                    *removeTaskExist = false;
                    foreach(const QPointer<Task> &subTask, aaUpdateTask->getSubtasks()){
                        RemoveAnnotationsTask* rTask = qobject_cast<RemoveAnnotationsTask*> (subTask.data());
                        if(rTask && !rTask->isFinished()){*removeTaskExist = true;}
                    }
                }
            }
        }
    }
}

/**
 * Adds common graphs menu to the current for each sequence
 */
void AnnotatedDNAView::addGraphs(ADVSequenceObjectContext* seqCtx)
{
    foreach (ADVSequenceWidget* seqWidget, seqCtx->getSequenceWidgets())
    {
        ADVSingleSequenceWidget* singleSeqWidget = qobject_cast<ADVSingleSequenceWidget*>(seqWidget);
        SAFE_POINT(NULL != singleSeqWidget, "singleSeqWidget is NULL", );
        GraphMenuAction* graphMenuAction = new GraphMenuAction(singleSeqWidget->getSequenceObject()->getAlphabet());
        if (singleSeqWidget != NULL) {
            singleSeqWidget->addADVSequenceWidgetActionToViewsToolbar(graphMenuAction);
        } else {
            seqWidget->addADVSequenceWidgetAction(graphMenuAction);
        }
    }
}

void AnnotatedDNAView::sl_onDocumentAdded(Document* d) {
    GObjectView::sl_onDocumentAdded(d);
    importDocAnnotations(d);
}

void AnnotatedDNAView::importDocAnnotations(Document* doc) {
    QList<GObject*> docObjects = doc->getObjects();

    foreach (GObject *obj, objects) {
        if (obj->getGObjectType() != GObjectTypes::SEQUENCE) {
            continue;
        }
        QList<GObject*> relatedAnns = GObjectUtils::findObjectsRelatedToObjectByRole(obj, GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence, docObjects, UOF_LoadedOnly);
        foreach (GObject *annObj, relatedAnns) {
            addObject(annObj);
        }
    }
}

void AnnotatedDNAView::seqWidgetMove(const QPoint &pos) {
    SAFE_POINT(replacedSeqWidget, "Moving the NULL widget",);
    CHECK_EXT(seqViews.contains(replacedSeqWidget), replacedSeqWidget = NULL,);

    int index = seqViews.indexOf(replacedSeqWidget);
    QRect replacedWidgetRect = replacedSeqWidget->geometry();
    CHECK(!replacedWidgetRect.contains(pos),);

    QRect prevWidgetRect;
    // If previous widget exists, define its rectangle
    if (index > 0) {
        prevWidgetRect = seqViews[index - 1]->geometry();
    }

    QRect nextWidgetRect;
    // If next widget exists, define its rectangle
    if (index < seqViews.count() - 1) {
        nextWidgetRect = seqViews[index + 1]->geometry();
    }

    if (prevWidgetRect.isValid() && pos.y() < prevWidgetRect.center().y()) {
        seqViews.swap(index - 1, index);
        scrolledWidgetLayout->insertWidget(index - 1, scrolledWidgetLayout->takeAt(index)->widget());
    }
    if (nextWidgetRect.isValid() && pos.y() > nextWidgetRect.top()) {
        seqViews.swap(index, index + 1);
        scrolledWidgetLayout->insertWidget(index, scrolledWidgetLayout->takeAt(index + 1)->widget());
    }
}

void AnnotatedDNAView::finishSeqWidgetMove() {
    replacedSeqWidget = NULL;
}

void AnnotatedDNAView::createCodonTableAction() {
    QAction *showCodonTableAction = new ADVGlobalAction(this, QIcon(":core/images/codon_table.png"), tr("Show codon table"), std::numeric_limits<int>::max() - 1, ADVGlobalActionFlag_AddToToolbar);
    showCodonTableAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    showCodonTableAction->setShortcutContext(Qt::WindowShortcut);
    connect(showCodonTableAction, SIGNAL(triggered()), codonTableView, SLOT(sl_setVisible()));
    showCodonTableAction->setObjectName("Codon table");
    showCodonTableAction->setCheckable(true);
}

void AnnotatedDNAView::sl_onDocumentLoadedStateChanged() {
    Document* d = qobject_cast<Document*>(sender());
    importDocAnnotations(d);
    GObjectView::sl_onDocumentLoadedStateChanged();
}

QList<U2SequenceObject*> AnnotatedDNAView::getSequenceObjectsWithContexts() const {
    QList<U2SequenceObject*> res;
    foreach(ADVSequenceObjectContext* cx, seqContexts) {
        res.append(cx->getSequenceObject());
    }
    return res;
}

QList<GObject*> AnnotatedDNAView::getSequenceGObjectsWithContexts() const {
    QList<GObject*> res;
    foreach(ADVSequenceObjectContext* cx, seqContexts) {
        res.append(cx->getSequenceObject());
    }
    return res;
}

void AnnotatedDNAView::updateState(const AnnotatedDNAViewState& s) {
    if (!s.isValid()) {
        return;
    }
    QList<GObjectReference> objs =  s.getSequenceObjects();
    QVector<U2Region> regs =  s.getSequenceSelections();
    assert(objs.size() == regs.size());

    //TODO: sync seq object lists
    //TODO: sync annotation object lists

    for (int i=0; i < objs.size(); i++) {
        const GObjectReference& ref = objs[i];
        const U2Region& reg = regs[i];
        ADVSequenceObjectContext* seqCtx = getSequenceContext(ref);
        if (seqCtx == NULL) {
            continue;
        }
        U2Region wholeSeq(0, seqCtx->getSequenceLength());
        U2Region finalSel = reg.intersect(wholeSeq);
        seqCtx->getSequenceSelection()->setRegion(finalSel);
    }
    foreach(ADVSequenceWidget* sw, seqViews) {
        sw->updateState(s.stateData);
    }

    foreach (ADVSplitWidget* w, splitWidgets) {
        w->updateState(s.stateData);
    }

    annotationsView->updateState(s.stateData);
}

void AnnotatedDNAView::sl_editSettings() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != NULL, L10N::nullPointerError("AppContext::settings"), );
    EditSettings settings;
    settings.annotationStrategy =
                (U1AnnotationUtils::AnnotationStrategyForResize)s->getValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_ANNOTATION_STRATEGY,
                            U1AnnotationUtils::AnnotationStrategyForResize_Resize).toInt();
    settings.recalculateQualifiers = s->getValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_RECALC_QUALIFIERS, false).toBool();

    QObjectScopedPointer<EditSettingsDialog> dlg = new EditSettingsDialog(settings, getSequenceWidgetInFocus());
    int res = dlg->exec();
    SAFE_POINT(!dlg.isNull(), "EditSettingsDialog is null!", );

    if (res == QDialog::Accepted) {
        const EditSettings& newSettings = dlg->getSettings();
        s->setValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_ANNOTATION_STRATEGY, newSettings.annotationStrategy);
        s->setValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_RECALC_QUALIFIERS, newSettings.recalculateQualifiers);
    }
}

void AnnotatedDNAView::sl_addSequencePart() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    U2SequenceObject *seqObj = seqCtx->getSequenceObject();

    EditSequencDialogConfig cfg;

    cfg.mode = EditSequenceMode_Insert;
    cfg.source = U2Region(0, seqObj->getSequenceLength());
    cfg.alphabet = seqObj->getAlphabet();
    cfg.position = 1;

    ADVSingleSequenceWidget *wgt = qobject_cast<ADVSingleSequenceWidget*> (focusedWidget);
    if (wgt != NULL) {
        QList<GSequenceLineView*> views = wgt->getLineViews();
        foreach (GSequenceLineView* v, views) {
            if (v->hasFocus()) {
                cfg.position = v->getLastPressPos();
                break;
            }
        }
    }

    const QVector<U2Region> &selection = seqCtx->getSequenceSelection()->getSelectedRegions();
    cfg.selectionRegions = selection;

    QObjectScopedPointer<EditSequenceDialogController> dialog = new EditSequenceDialogController(cfg, getSequenceWidgetInFocus());
    const int result = dialog->exec();
    CHECK(!dialog.isNull(), );
    CHECK(result == QDialog::Accepted, );

    Task *t = new ModifySequenceContentTask(dialog->getDocumentFormatId(), seqObj, U2Region(dialog->getPosToInsert(), 0),
        dialog->getNewSequence(), dialog->recalculateQualifiers(), dialog->getAnnotationStrategy(),
        dialog->getDocumentPath(), dialog->mergeAnnotations());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_removeSequencePart() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    U2SequenceObject *seqObj = seqCtx->getSequenceObject();

    Document *curDoc = seqObj->getDocument();
    U2Region source(0, seqObj->getSequenceLength());

    U2Region selection = source;
    if(seqCtx->getSequenceSelection()->getSelectedRegions().size() > 0){
        selection = seqCtx->getSequenceSelection()->getSelectedRegions().first();
    }

    QObjectScopedPointer<RemovePartFromSequenceDialogController> dialog = new RemovePartFromSequenceDialogController(selection, source, curDoc->getURLString(), getSequenceWidgetInFocus());
    const int result = dialog->exec();
    CHECK(!dialog.isNull(), );
    CHECK(result == QDialog::Accepted, );

    Task *t = NULL;
    if (dialog->modifyCurrentDocument()) {
        t = new ModifySequenceContentTask(dialog->getDocumentFormatId(), seqObj, dialog->getRegionToDelete(), DNASequence(),
            dialog->recalculateQualifiers(), dialog->getStrategy(), seqObj->getDocument()->getURL());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    } else {
        t = new ModifySequenceContentTask(dialog->getDocumentFormatId(), seqObj, dialog->getRegionToDelete(), DNASequence(),
            dialog->recalculateQualifiers(), dialog->getStrategy(), dialog->getNewDocumentPath(), dialog->mergeAnnotations());
    }
    SAFE_POINT(NULL != t, L10N::nullPointerError("Edit sequence task"), );
    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_replaceSequencePart() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    U2SequenceObject *seqObj = seqCtx->getSequenceObject();

    if(seqCtx->getSequenceSelection()->getSelectedRegions().isEmpty()){
        return;
    }

    EditSequencDialogConfig cfg;

    cfg.mode = EditSequenceMode_Replace;
    cfg.source = U2Region(0, seqObj->getSequenceLength());
    cfg.alphabet = seqObj->getAlphabet();
    U2Region selection = seqCtx->getSequenceSelection()->getSelectedRegions().first();
    cfg.initialText = seqObj->getSequenceData(selection);
    cfg.position = 1;

    cfg.selectionRegions.append(selection);

    QObjectScopedPointer<EditSequenceDialogController> dlg = new EditSequenceDialogController(cfg, getSequenceWidgetInFocus());
    const int result = dlg->exec();
    CHECK(!dlg.isNull(), );

    CHECK(result == QDialog::Accepted, );

    Task *t = new ModifySequenceContentTask(dlg->getDocumentFormatId(), seqObj, selection, dlg->getNewSequence(),
        dlg->recalculateQualifiers(), dlg->getAnnotationStrategy(), dlg->getDocumentPath(), dlg->mergeAnnotations());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_removeSelectedSequenceObject() {
    ADVSequenceWidget* sw = getSequenceWidgetInFocus();
    ADVSequenceObjectContext* soc = sw->getActiveSequenceContext();
    U2SequenceObject* so = soc->getSequenceObject();
    removeObject(so);
}

QList<AnnotationTableObject *> AnnotatedDNAView::getAnnotationObjects(bool includeAutoAnnotations) const {
    QList<AnnotationTableObject *> result = annotations;
    if (includeAutoAnnotations) {
        foreach (AutoAnnotationObject *aa, autoAnnotationsMap.values()) {
            result += aa->getAnnotationObject();
        }
    }
    return result;
}

void AnnotatedDNAView::updateAutoAnnotations() {
    QList<AutoAnnotationObject *> autoAnnotations = autoAnnotationsMap.values();
    foreach (AutoAnnotationObject *aa, autoAnnotations) {
        aa->updateAll();
    }
}

void AnnotatedDNAView::addAutoAnnotationsUpdated(AutoAnnotationsUpdater *updater) {
    foreach (ADVSequenceObjectContext *advContext, seqContexts) {
        AutoAnnotationsADVAction* autoAnnotAction = AutoAnnotationUtils::findAutoAnnotationADVAction(advContext);
        if (autoAnnotAction) {
            autoAnnotAction->addUpdaterToMenu(updater);
        }
    }
}

void AnnotatedDNAView::sl_sequenceModifyTaskStateChanged() {
    Task *t = qobject_cast<Task *>(sender());
    if (NULL == t) {
        return;
    }

    if (t->getState() == Task::State_Finished && !(t->hasError() || t->isCanceled())) {
        updateAutoAnnotations();
        // TODO: there must be better way to do this
        bool reverseComplementTask = false;
        if (qobject_cast<ReverseComplementSequenceTask*>(t) != NULL ||
                qobject_cast<ReverseSequenceTask*>(t) != NULL ||
                qobject_cast<ComplementSequenceTask*>(t) != NULL) {
            reverseComplementTask = true;
        }

        ADVSequenceObjectContext *seqCtx = getSequenceInFocus();
        if (reverseComplementTask && NULL != seqCtx) {
            const QVector<U2Region> regions = seqCtx->getSequenceSelection()->getSelectedRegions();
            if (regions.count() == 1) {
                const U2Region r = regions.first();
                foreach (ADVSequenceWidget *w, seqCtx->getSequenceWidgets()) {
                    w->centerPosition((int) r.startPos);
                }
            }
        }

        ModifySequenceContentTask* modifyContentTask = qobject_cast<ModifySequenceContentTask*>(t);
        if (modifyContentTask != NULL) {
            qint64 seqSizeDelta = modifyContentTask->getSequenceLengthDelta();
            if (seqSizeDelta > 0) { // try keeping all maximized zooms in max state
                U2Region newMaxRange(0, modifyContentTask->getSequenceObject()->getSequenceLength());
                U2Region oldMaxRange(0,  newMaxRange.length - seqSizeDelta);
                foreach (ADVSequenceObjectContext * seqCtx, seqContexts) {
                    if (seqCtx->getSequenceGObject() == modifyContentTask->getSequenceObject()) {
                        foreach (ADVSequenceWidget *w, seqCtx->getSequenceWidgets()) {
                            if (w->getVisibleRange() == oldMaxRange) {
                                w->setVisibleRange(newMaxRange);
                            }
                        }
                    }
                }
            }
        }
        updateMultiViewActions();
        emit si_sequenceModified(seqCtx);
    }
}

void AnnotatedDNAView::sl_paste(){
    PasteFactory* pasteFactory = AppContext::getPasteFactory();
    SAFE_POINT(pasteFactory != NULL, "adFactory is null", );

    ADVSingleSequenceWidget *wgt = qobject_cast<ADVSingleSequenceWidget*> (focusedWidget);
    CHECK(wgt != NULL, );

    DetView* detView = wgt->getDetView();
    SAFE_POINT(detView, "DetView is unexpectedly NULL", );
    CHECK(detView->hasFocus(), );
    SAFE_POINT(detView->getEditor(), "DetViewEditor is NULL", );
    CHECK(detView->getEditor()->isEditMode(), );

    PasteTask* task = pasteFactory->pasteTask(false);
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), detView->getEditor(), SLOT(sl_paste(Task*)));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void AnnotatedDNAView::onObjectRenamed(GObject* obj, const QString& oldName) {
    if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        // 1. update title
        OpenAnnotatedDNAViewTask::updateTitle(this);

        // 2. update components
        U2SequenceObject* seqObj = qobject_cast<U2SequenceObject*>(obj);
        ADVSequenceObjectContext* ctx = getSequenceContext(seqObj);
        foreach(ADVSequenceWidget* w, ctx->getSequenceWidgets()) {
            w->onSequenceObjectRenamed(oldName);
        }
    }
}

void AnnotatedDNAView::sl_reverseComplementSequence() {
    reverseComplementSequence();
}

void AnnotatedDNAView::sl_reverseSequence() {
    reverseComplementSequence(true, false);
}

void AnnotatedDNAView::sl_complementSequence() {
    reverseComplementSequence(false, true);
}

void AnnotatedDNAView::sl_selectionChanged() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    CHECK(seqCtx != NULL, );
    DNASequenceSelection* selection = qobject_cast<DNASequenceSelection*>(sender());
    CHECK(selection != NULL && seqCtx->getSequenceGObject() == selection->getSequenceObject(), );

    replaceSequencePart->setEnabled(!seqCtx->getSequenceSelection()->isEmpty());
}

void AnnotatedDNAView::sl_aminoTranslationChanged() {
    ADVSequenceObjectContext *seqCtx = getSequenceInFocus();
    U2SequenceObject *seqObj = seqCtx->getSequenceObject();
    QList<AutoAnnotationObject *> autoAnnotations = autoAnnotationsMap.values();
    foreach(AutoAnnotationObject *aa, autoAnnotations) {
        if (aa->getSeqObject() == seqObj) {
            aa->updateTranslationDependent(seqCtx->getAminoTT());
        }
    }
}

void AnnotatedDNAView::reverseComplementSequence(bool reverse, bool complement) {
    ADVSequenceObjectContext *seqCtx = getSequenceInFocus();
    U2SequenceObject *seqObj = seqCtx->getSequenceObject();
    QList<AnnotationTableObject *> annotations = seqCtx->getAnnotationObjects(false).toList();

    DNATranslation *complTT = NULL;
    if (seqObj->getAlphabet()->isNucleic()) {
        complTT = seqCtx->getComplementTT();
    }

    Task *t = NULL;
    if (reverse && complement) {
        t = new ReverseComplementSequenceTask(seqObj,annotations, seqCtx->getSequenceSelection(), complTT);
    } else if (reverse) {
        t = new ReverseSequenceTask(seqObj,annotations, seqCtx->getSequenceSelection());
    } else if (complement) {
        t = new ComplementSequenceTask(seqObj,annotations, seqCtx->getSequenceSelection(), complTT);
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
}

QAction* AnnotatedDNAView::getEditActionFromSequenceWidget(ADVSequenceWidget* seqWgt) {
    ADVSingleSequenceWidget *wgt = qobject_cast<ADVSingleSequenceWidget*> (seqWgt);
    SAFE_POINT(wgt != NULL, "ADVSingleSequenceWidget is NULL", NULL);

    DetView* detView = wgt->getDetView();
    SAFE_POINT(detView != NULL, "DetView is NULL", NULL);

    DetViewSequenceEditor* editor = detView->getEditor();
    SAFE_POINT(editor != NULL, "DetViewSequenceEditor is NULL", NULL);

    QAction* editAction = editor->getEditAction();
    SAFE_POINT(editAction != NULL, "EditAction is NULL", NULL);

    return editAction;
}

bool AnnotatedDNAView::areAnnotationsInRange(const QList<Annotation *> &toCheck) {
    foreach (Annotation *a, toCheck) {
        QList<ADVSequenceObjectContext *> relatedSeqObjects = findRelatedSequenceContexts(a->getGObject());

        foreach (ADVSequenceObjectContext *seq, relatedSeqObjects) {
            SAFE_POINT(NULL != seq, "Sequence is NULL", true);
            foreach (const U2Region &r, a->getRegions()) {
                if (r.endPos() > seq->getSequenceLength()) {
                    return false;
                }
            }
       }
    }
    return true;
}

}//namespace
