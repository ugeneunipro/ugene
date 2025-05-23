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

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QScrollArea>
#include <QToolBar>
#include <QVBoxLayout>

#include <U2Algorithm/EnzymeModel.h>

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
#include <U2Core/QObjectScopedPointer.h>
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
#include <U2Core/CollectionUtils.h>

namespace U2 {

AnnotatedDNAView::AnnotatedDNAView(const QString& viewName, const QList<U2SequenceObject*>& dnaObjects)
    : GObjectViewController(AnnotatedDNAViewFactory::ID, viewName) {
    optionsPanelController = new OptionsPanelController(this);
    timerId = 0;
    hadExpandableSequenceWidgetsLastResize = false;

    annotationSelection = new AnnotationSelection(this);
    annotationGroupSelection = new AnnotationGroupSelection(this);

    clipb = nullptr;

    mainSplitter = nullptr;
    scrollArea = nullptr;
    posSelector = nullptr;
    posSelectorWidgetAction = nullptr;
    annotationsView = nullptr;
    activeSequenceWidget = nullptr;
    replacedSeqWidget = nullptr;

    codonTableView = new CodonTableView(this);
    connect(this, SIGNAL(si_activeSequenceWidgetChanged(ADVSequenceWidget*, ADVSequenceWidget*)), codonTableView, SLOT(sl_onActiveSequenceChanged(ADVSequenceWidget*, ADVSequenceWidget*)));
    createCodonTableAction();
    createAnnotationAction = (new ADVAnnotationCreation(this))->getCreateAnnotationAction();

    posSelectorAction = new QAction(QIcon(":core/images/goto.png"), tr("Go to position..."), this);
    posSelectorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    posSelectorAction->setShortcutContext(Qt::WindowShortcut);
    posSelectorAction->setObjectName(ADV_GOTO_ACTION);
    connect(posSelectorAction, SIGNAL(triggered()), SLOT(sl_onShowPosSelectorRequest()));

    toggleHLAction = new QAction("", this);
    connect(toggleHLAction, SIGNAL(triggered()), SLOT(sl_toggleHL()));

    removeAnnsAndQsAction = new QAction("", this);
    removeAnnsAndQsAction->setShortcut(QKeySequence(Qt::Key_Delete));
    removeAnnsAndQsAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    syncViewManager = new ADVSyncViewManager(this);

    foreach (U2SequenceObject* dnaObj, dnaObjects) {
        addObject(dnaObj);
    }

    findPatternAction = new ADVGlobalAction(this, QIcon(":core/images/find_dialog.png"), tr("Find pattern..."), 10);
    findPatternAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    findPatternAction->setShortcutContext(Qt::WindowShortcut);
    connect(findPatternAction, SIGNAL(triggered()), SLOT(sl_onFindPatternClicked()));

    editSettingsAction = new QAction(tr("Annotation settings on editing..."), this);
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
    replaceSequencePart->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    replaceSequencePart->setObjectName(ACTION_EDIT_REPLACE_SUBSEQUENCE);
    connect(replaceSequencePart, SIGNAL(triggered()), this, SLOT(sl_replaceSequencePart()));

    removeSequenceObjectAction = new QAction(tr("Selected sequence from view"), this);
    removeSequenceObjectAction->setObjectName(ACTION_EDIT_SELECT_SEQUENCE_FROM_VIEW);
    connect(removeSequenceObjectAction, SIGNAL(triggered()), SLOT(sl_removeSelectedSequenceObject()));

    reverseComplementSequenceAction = new QAction(tr("Complementary (5'-3') sequence"), this);
    reverseComplementSequenceAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    reverseComplementSequenceAction->setObjectName(ACTION_EDIT_RESERVE_COMPLEMENT_SEQUENCE);
    connect(reverseComplementSequenceAction, SIGNAL(triggered()), SLOT(sl_reverseComplementSequence()));

    reverseSequenceAction = new QAction(tr("Reverse (3'-5') sequence"), this);
    reverseSequenceAction->setObjectName(ACTION_EDIT_RESERVE_SEQUENCE);
    connect(reverseSequenceAction, SIGNAL(triggered()), SLOT(sl_reverseSequence()));

    complementSequenceAction = new QAction(tr("Complementary (3'-5') sequence"), this);
    complementSequenceAction->setObjectName(ACTION_EDIT_COMPLEMENT_SEQUENCE);
    connect(complementSequenceAction, SIGNAL(triggered()), SLOT(sl_complementSequence()));

    SecStructPredictViewAction::createAction(this);
}

QAction* AnnotatedDNAView::createPasteAction() {
    QAction* action = ADVClipboard::createPasteSequenceAction(this);
    connect(action, SIGNAL(triggered()), this, SLOT(sl_paste()));
    return action;
}

QWidget* AnnotatedDNAView::createViewWidget(QWidget* parent) {
    GTIMER(c1, t1, "AnnotatedDNAView::createWidget");
    SAFE_POINT(viewWidget == nullptr, "Widget is already created", viewWidget);

    mainSplitter = new QSplitter(Qt::Vertical, parent);
    mainSplitter->setObjectName("annotated_DNA_splitter");
    connect(mainSplitter, SIGNAL(splitterMoved(int, int)), SLOT(sl_splitterMoved(int, int)));

    mainSplitter->addWidget(codonTableView);
    mainSplitter->setCollapsible(mainSplitter->indexOf(codonTableView), false);

    mainSplitter->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mainSplitter, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(sl_onContextMenuRequested()));

    scrollArea = new QScrollArea();
    scrollArea->setObjectName("annotated_DNA_scrollarea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    mainSplitter->addWidget(scrollArea);
    mainSplitter->setHandleWidth(1);  // make smaller the distance between the Annotations Editor and the  sequence sub-views
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
        auto block = new ADVSingleSequenceWidget(seqCtx, this);
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

    // TODO: scroll area does not restore focus for last active child widget after Alt-Tab...
    scrollArea->setWidget(scrolledWidget);

    mainSplitter->installEventFilter(this);
    mainSplitter->setAcceptDrops(true);

    if (!seqViews.isEmpty()) {
        setActiveSequenceWidget(seqViews.last());
    }

    // add view global shortcuts

    connect(removeAnnsAndQsAction, SIGNAL(triggered()), annotationsView->removeAnnsAndQsAction, SIGNAL(triggered()));

    mainSplitter->addAction(toggleHLAction);
    mainSplitter->addAction(removeSequenceObjectAction);

    mainSplitter->addAction(removeAnnsAndQsAction);

    mainSplitter->setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::SEQUENCE).icon);

    // Init the Options Panel
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;

    ADVSequenceObjectContext* ctx;
    QList<DNAAlphabetType> alphabets;

    for (int i = 0; i < seqViews.size(); i++) {
        if (seqViews[i] != nullptr) {
            ctx = seqViews[i]->getActiveSequenceContext();
            if (ctx) {
                const DNAAlphabet* alphabet = ctx->getAlphabet();
                if (alphabet) {
                    alphabets.append(alphabet->getType());
                }
            }
        }
    }
    filters.append(new OPFactoryFilterVisitor(ObjViewType_SequenceView, alphabets));

    QList<OPWidgetFactory*> opWidgetFactoriesForSeqView = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory* factory, opWidgetFactoriesForSeqView) {
        optionsPanelController->addGroup(factory);
    }

    qDeleteAll(filters);
    return mainSplitter;
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
    // see comment for sl_splitterMoved()
    assert(timerId != 0);
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
    foreach (ADVSequenceWidget* v, seqViews) {
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
            auto gomd = qobject_cast<const GObjectMimeData*>(md);
            if (gomd != nullptr) {
                if (e->type() == QEvent::DragEnter) {
                    de->acceptProposedAction();
                } else {
                    GObject* obj = gomd->objPtr.data();
                    if (obj != nullptr) {
                        QString err = tryAddObject(obj);
                        if (!err.isEmpty()) {
                            QMessageBox::critical(nullptr, tr("Error!"), err);
                        }
                    }
                }
            }
        }
    } else if (o == scrolledWidget) {
        if (replacedSeqWidget && e->type() == QEvent::MouseMove) {
            auto event = dynamic_cast<QMouseEvent*>(e);
            if (event->buttons() == Qt::LeftButton) {
                seqWidgetMove(event->pos());
            }
        } else if (replacedSeqWidget && e->type() == QEvent::MouseButtonRelease) {
            auto event = dynamic_cast<QMouseEvent*>(e);
            if (event->buttons() == Qt::LeftButton) {
                finishSeqWidgetMove();
            }
        }
        // try to restore mainSplitter state on sequence views fixed <-> expandable state transition. Usually this happens when user toggles sequence views.
        if (e->type() == QEvent::Resize) {
            bool hasExpandableSequenceWidgetsNow = false;  // expandable state: any of the sequence view widgets has unlimited height.
            foreach (const ADVSequenceWidget* w, getSequenceWidgets()) {
                if (w->maximumHeight() == QWIDGETSIZE_MAX) {
                    hasExpandableSequenceWidgetsNow = true;
                    break;
                }
            }
            if (hasExpandableSequenceWidgetsNow != hadExpandableSequenceWidgetsLastResize) {  // transition from fixed <-> expandable state
                if (hasExpandableSequenceWidgetsNow) {  // try restore state from the saved sizes if possible.
                    if (savedMainSplitterSizes.size() > 0 && savedMainSplitterSizes.size() == mainSplitter->sizes().size()) {
                        mainSplitter->setSizes(savedMainSplitterSizes);
                    }
                }
                hadExpandableSequenceWidgetsLastResize = hasExpandableSequenceWidgetsNow;
            }
            if (hasExpandableSequenceWidgetsNow) {  // update saved sizes for a future use.
                savedMainSplitterSizes = mainSplitter->sizes();
            }
        }
        return false;
    } else if (e->type() == QEvent::Resize) {
        auto v = qobject_cast<ADVSequenceWidget*>(o);
        if (v != nullptr) {
            updateScrollAreaHeight();
        }
    } else if (e->type() == QEvent::KeyPress) {
        sl_selectionChanged();
    }

    return false;
}

void AnnotatedDNAView::setActiveSequenceWidget(ADVSequenceWidget* sequenceWidget) {
    if (sequenceWidget == activeSequenceWidget) {
        return;
    }
    ADVSequenceWidget* prevFocus = activeSequenceWidget;
    activeSequenceWidget = sequenceWidget;
    updateMultiViewActions();
    sl_updatePasteAction();
    emit si_activeSequenceWidgetChanged(prevFocus, activeSequenceWidget);
}

bool AnnotatedDNAView::onCloseEvent() {
    QList<AutoAnnotationObject*> aaList = autoAnnotationsMap.values();
    bool waitFinishedRemovedTasks = false;
    foreach (AutoAnnotationObject* aa, aaList) {
        bool existRemovedTask = false;
        cancelAutoAnnotationUpdates(aa, &existRemovedTask);
        waitFinishedRemovedTasks = waitFinishedRemovedTasks || existRemovedTask;
    }
    if (waitFinishedRemovedTasks) {
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
        auto ao = qobject_cast<AnnotationTableObject*>(o);
        annotationSelection->removeObjectAnnotations(ao);
        foreach (ADVSequenceObjectContext* seqCtx, seqContexts) {
            if (seqCtx->getAnnotationObjects().contains(ao)) {
                seqCtx->removeAnnotationObject(ao);
                break;
            }
        }
        annotations.removeOne(ao);
        emit si_annotationObjectRemoved(ao);
    } else if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
        auto seqObj = qobject_cast<U2SequenceObject*>(o);
        ADVSequenceObjectContext* seqCtx = getSequenceContext(seqObj);
        seqObj->disconnect(this);
        if (seqCtx != nullptr) {
            foreach (ADVSequenceWidget* w, seqCtx->getSequenceWidgets()) {
                removeSequenceWidget(w);
            }
            QSet<AnnotationTableObject*> aObjs = seqCtx->getAnnotationObjects();
            foreach (AnnotationTableObject* ao, aObjs) {
                removeObject(ao);
            }
            emit si_sequenceRemoved(seqCtx);
            seqContexts.removeOne(seqCtx);
            removeAutoAnnotations(seqCtx);
            delete seqCtx;
        }
    }

    GObjectViewController::onObjectRemoved(o);
    return seqContexts.isEmpty();
}

void AnnotatedDNAView::addADVAction(ADVGlobalAction* a1) {
    for (int i = 0; i < advActions.size(); i++) {
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
    tb->addAction(clipb->getCopyAnnotationSequenceTranslationAction());
    tb->addAction(clipb->getCopyQualifierAction());
    tb->addAction(clipb->getPasteSequenceAction());
    tb->addSeparator();

    if (posSelector == nullptr && !seqContexts.isEmpty()) {
        qint64 len = seqContexts.first()->getSequenceLength();
        posSelector = new PositionSelector(tb, 1, len);
        connect(posSelector, SIGNAL(si_positionChanged(int)), SLOT(sl_onPosChangeRequest(int)));
        posSelectorWidgetAction = tb->addWidget(posSelector);
    } else {
        tb->addAction(posSelectorWidgetAction);
    }

    tb->addSeparator();
    syncViewManager->updateToolbar1(tb);
    tb->addSeparator();

    foreach (ADVGlobalAction* a, advActions) {
        if (a->getFlags().testFlag(ADVGlobalActionFlag_AddToToolbar)) {
            tb->addAction(a);
            QWidget* w = tb->widgetForAction(a);
            if (w) {
                w->setObjectName(a->objectName() + "_widget");
            }
        }
    }

    GObjectViewController::buildStaticToolbar(tb);

    tb->addSeparator();
    syncViewManager->updateToolbar2(tb);
}

void AnnotatedDNAView::buildMenu(QMenu* m, const QString& type) {
    if (type != GObjectViewMenuType::STATIC) {
        GObjectViewController::buildMenu(m, type);
        return;
    }
    m->addAction(posSelectorAction);
    clipb->addCopyMenu(m);
    m->addSeparator();
    addAddMenu(m);
    addAnalyseMenu(m);
    addAlignMenu(m);
    addExportMenu(m);
    addEditMenu(m);
    addRemoveMenu(m);
    m->addSeparator();

    annotationsView->adjustStaticMenu(m);

    GObjectViewController::buildMenu(m, type);
}

void AnnotatedDNAView::addAnalyseMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Analyze"));
    am->menuAction()->setObjectName(ADV_MENU_ANALYSE);
    foreach (ADVGlobalAction* a, advActions) {
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
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    SAFE_POINT(seqCtx != nullptr, "Sequence in focus is NULL", );

    U2SequenceObject* seqObj = seqCtx->getSequenceObject();
    SAFE_POINT(seqObj != nullptr, "Sequence object in focus is NULL", );

    Document* curDoc = seqObj->getDocument();
    SAFE_POINT(curDoc != nullptr, "Current document is NULL", );

    QMenu* editMenu = m->addMenu(tr("Edit"));
    editMenu->setEnabled(!(curDoc->findGObjectByType(GObjectTypes::SEQUENCE).isEmpty() || seqObj->isStateLocked()));
    editMenu->menuAction()->setObjectName(ADV_MENU_EDIT);

    QAction* editAction = getEditActionFromSequenceWidget(activeSequenceWidget);
    if (editAction != nullptr) {
        editMenu->addAction(editAction);
    }
    if (annotationSelection->getAnnotations().size() == 1 && annotationsView->editAction->isEnabled()) {
        editMenu->addAction(annotationsView->editAction);
    }
    editMenu->addAction(editSettingsAction);
    editMenu->addSeparator();

    editMenu->addAction(addSequencePart);
    editMenu->addAction(replaceSequencePart);
    sl_selectionChanged();
    editMenu->addAction(removeSequencePart);
    editMenu->addSeparator();

    if (seqObj->getAlphabet()->isNucleic() && seqCtx->getComplementTT() != nullptr) {
        QMenu* replaceMenu = editMenu->addMenu(tr("Replace the whole sequence by"));
        replaceMenu->menuAction()->setObjectName(ADV_MENU_REPLACE_WHOLE_SEQUENCE);
        replaceMenu->addAction(reverseComplementSequenceAction);
        replaceMenu->addSeparator();
        replaceMenu->addAction(complementSequenceAction);
        replaceMenu->addAction(reverseSequenceAction);
    }
}

Task* AnnotatedDNAView::updateViewTask(const QString& stateName, const QVariantMap& stateData) {
    return new UpdateAnnotatedDNAViewTask(this, stateName, stateData);
}

QVariantMap AnnotatedDNAView::saveState() {
    if (closing) {
        return QVariantMap();
    }
    QVariantMap state = AnnotatedDNAViewState::saveState(this);
    foreach (ADVSequenceWidget* sw, seqViews) {
        sw->saveState(state);
    }
    foreach (ADVSplitWidget* w, splitWidgets) {
        w->saveState(state);
    }
    annotationsView->saveState(state);
    return state;
}

void AnnotatedDNAView::saveWidgetState() {
    annotationsView->saveWidgetState();
}

bool AnnotatedDNAView::canAddObject(GObject* obj) {
    if (GObjectViewController::canAddObject(obj)) {
        return true;
    }
    if (isChildWidgetObject(obj)) {
        return true;
    }
    if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        return true;
    }
    if (obj->getGObjectType() != GObjectTypes::ANNOTATION_TABLE) {
        return false;
    }
    // todo: add annotations related to sequence object not in view (sobj) and add 'sobj' too the view ?
    bool hasRelation = false;
    foreach (ADVSequenceObjectContext* soc, seqContexts) {
        if (obj->hasObjectRelation(soc->getSequenceObject(), ObjectRole_Sequence)) {
            hasRelation = true;
            break;
        }
    }
    return hasRelation;
}

bool AnnotatedDNAView::isChildWidgetObject(GObject* obj) const {
    foreach (ADVSequenceWidget* lv, seqViews) {
        SAFE_POINT(lv != nullptr, "AnnotatedDNAView::isChildWidgetObject::No sequence widget", false);
        if (lv->isWidgetOnlyObject(obj)) {
            return true;
        }
    }
    foreach (ADVSplitWidget* sw, splitWidgets) {
        SAFE_POINT(sw != nullptr, "AnnotatedDNAView::isChildWidgetObject::No split widget", false);
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
    SAFE_POINT(editAction != nullptr, "Edit action is not found", );

    connect(editAction, SIGNAL(triggered()), SLOT(sl_updatePasteAction()));

    QList<ADVSequenceObjectContext*> contexts = v->getSequenceContexts();
    foreach (ADVSequenceObjectContext* c, contexts) {
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

void AnnotatedDNAView::removeSequenceWidget(ADVSequenceWidget* sequenceWidget) {
    int widgetIndex = seqViews.indexOf(sequenceWidget);
    SAFE_POINT(widgetIndex >= 0, "removeSequenceWidget is called for an unknown widget", );

    if (activeSequenceWidget == sequenceWidget) {
        int newActiveWidgetIndex = widgetIndex + 1 < seqViews.size() ? widgetIndex + 1 : widgetIndex - 1;
        setActiveSequenceWidget(newActiveWidgetIndex < 0 ? nullptr : seqViews[newActiveWidgetIndex]);
    }

    // remove widget
    seqViews.removeOne(sequenceWidget);
    sequenceWidget->hide();

    QList<ADVSequenceObjectContext*> contexts = sequenceWidget->getSequenceContexts();
    foreach (ADVSequenceObjectContext* c, contexts) {
        c->removeSequenceWidget(sequenceWidget);
        disconnect(c->getSequenceSelection(), SIGNAL(si_selectionChanged(LRegionsSelection*, QVector<U2Region>, QVector<U2Region>)));
    }
    updateMultiViewActions();
    emit si_sequenceWidgetRemoved(sequenceWidget);
    scrolledWidgetLayout->removeWidget(sequenceWidget);
    delete sequenceWidget;  // v->deleteLater(); //problem: updates for 'v' after seqCtx is destroyed
    updateScrollAreaHeight();
}

void AnnotatedDNAView::updateMultiViewActions() {
    bool canRemoveActiveSequence = seqViews.size() > 1 && activeSequenceWidget != nullptr && activeSequenceWidget->getActiveSequenceContext() != nullptr;
    removeSequenceObjectAction->setEnabled(canRemoveActiveSequence);

    if (posSelector != nullptr) {
        qint64 currentSequenceLength = 0;
        if (activeSequenceWidget != nullptr && activeSequenceWidget->getActiveSequenceContext() != nullptr) {
            currentSequenceLength = activeSequenceWidget->getActiveSequenceContext()->getSequenceLength();
        }
        posSelector->updateRange(1, currentSequenceLength);
    }
}

void AnnotatedDNAView::sl_updatePasteAction() {
    CHECK(activeSequenceWidget != nullptr, );

    QAction* editAction = getEditActionFromSequenceWidget(activeSequenceWidget);
    SAFE_POINT(editAction != nullptr, "Edit action is not found", );

    const bool isEditModeChecked = editAction->isChecked();

    QAction* pasteAction = clipb->getPasteSequenceAction();
    SAFE_POINT(pasteAction != nullptr, "Paste action is NULL", );

    pasteAction->setEnabled(isEditModeChecked);
}

void AnnotatedDNAView::sl_relatedObjectRelationChanged() {
    auto o = qobject_cast<GObject*>(sender());
    CHECK(o != nullptr, );
    QList<AnnotationTableObject*> currentAnnotations = getAnnotationObjects(false);
    QList<GObject*> objectsToAdd;
    QList<GObject*> allObjs = GObjectUtils::findObjectsRelatedToObjectByRole(o, GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence, GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::ANNOTATION_TABLE), UnloadedObjectFilter::UOF_LoadedOnly);

    foreach (GObject* obj, allObjs) {
        if (!currentAnnotations.contains(qobject_cast<AnnotationTableObject*>(obj))) {
            objectsToAdd << obj;
        }
    }

    foreach (GObject* obj, objectsToAdd) {
        QString error = addObject(obj);
        if (!error.isEmpty()) {
            coreLog.error(error);
        }
    }
}

void AnnotatedDNAView::sl_sequenceCircularStateChanged() {
    auto sequenceObject = qobject_cast<U2SequenceObject*>(sender());
    SAFE_POINT(sequenceObject != nullptr, "casting to 'U2SequenceObject' failed", );
    for (ADVSequenceObjectContext* ctx : qAsConst(getSequenceContexts())) {
        if (ctx->getSequenceObject() == sequenceObject) {
            auto* toggleAAAction = AutoAnnotationUtils::findAutoAnnotationsToggleAction(ctx, ANNOTATION_GROUP_ENZYME);
            if (toggleAAAction != nullptr && toggleAAAction->isChecked()) {
                AutoAnnotationUtils::triggerAutoAnnotationsUpdate(ctx, ANNOTATION_GROUP_ENZYME);
            }
        }
    }
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
        Annotation* a = annotationSelection->getAnnotations().first();
        const SharedAnnotationData& aData = a->getData();
        AnnotationSettingsRegistry* registry = AppContext::getAnnotationsSettingsRegistry();
        AnnotationSettings* as = registry->getAnnotationSettings(aData);
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

    if (activeSequenceWidget != nullptr) {
        activeSequenceWidget->buildPopupMenu(m);
    }
    emit si_buildMenu(this, &m, GObjectViewMenuType::CONTEXT);

    m.exec(QCursor::pos());
}

void AnnotatedDNAView::sl_onFindPatternClicked() {
    const QString& findPatternGroupId = FindPatternWidgetFactory::getGroupId();
    optionsPanelController->openGroupById(findPatternGroupId);
}

void AnnotatedDNAView::sl_toggleHL() {
    if (annotationSelection->isEmpty()) {
        return;
    }
    const Annotation* a = annotationSelection->getAnnotations().first();
    AnnotationSettingsRegistry* registry = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = registry->getAnnotationSettings(a->getData());
    as->visible = !as->visible;
    registry->changeSettings(QList<AnnotationSettings*>() << as, true);
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
            // ask user if to create new association
            QObjectScopedPointer<CreateObjectRelationDialogController> d = new CreateObjectRelationDialogController(o, getSequenceGObjectsWithContexts(), ObjectRole_Sequence, true, tr("Select sequence to associate annotations with:"));
            d->exec();
            CHECK(!d.isNull(), "");
            bool objectAlreadyAdded = d->relationIsSet;
            rCtx = findRelatedSequenceContexts(o);
            if (rCtx.isEmpty() || objectAlreadyAdded) {
                return "";
            }
        }
    }
    return addObject(o);
}

QString AnnotatedDNAView::addObject(GObject* o) {
    QList<ADVSequenceObjectContext*> rCtx;
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        rCtx = findRelatedSequenceContexts(o);
        if (rCtx.isEmpty()) {
            return tr("No sequence object found for annotations");
        }
    }
    QString res = GObjectViewController::addObject(o);
    if (!res.isEmpty()) {
        return res;
    }

    bool internalViewObject = isChildWidgetObject(o);
    if (internalViewObject) {
        return "";
    }

    if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
        auto dnaObj = qobject_cast<U2SequenceObject*>(o);
        U2OpStatusImpl status;
        if (!dnaObj->isValidDbiObject(status)) {
            return "";
        }
        auto sc = new ADVSequenceObjectContext(this, dnaObj);
        seqContexts.append(sc);
        // if mainSplitter==NULL -> its view initialization and widgets will be added later
        if (mainSplitter != nullptr && !isChildWidgetObject(dnaObj)) {
            auto block = new ADVSingleSequenceWidget(sc, this);
            connect(block, SIGNAL(si_titleClicked(ADVSequenceWidget*)), SLOT(sl_onSequenceWidgetTitleClicked(ADVSequenceWidget*)));
            block->setObjectName("ADV_single_sequence_widget_" + QString::number(seqViews.count()));
            addSequenceWidget(block);
            block->addAction(createPasteAction());
            setActiveSequenceWidget(block);
        }
        addRelatedAnnotations(sc);
        emit si_sequenceAdded(sc);
        connect(o, SIGNAL(si_relatedObjectRelationChanged()), SLOT(sl_relatedObjectRelationChanged()));
        connect(dnaObj, &U2SequenceObject::si_sequenceCircularStateChanged, this, &AnnotatedDNAView::sl_sequenceCircularStateChanged);
    } else if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        auto ao = qobject_cast<AnnotationTableObject*>(o);
        SAFE_POINT(ao != nullptr, "Invalid annotation table!", QString());
        annotations.append(ao);
        foreach (ADVSequenceObjectContext* sc, rCtx) {
            sc->addAnnotationObject(ao);
        }
        emit si_annotationObjectAdded(ao);
    }
    return "";
}

QList<ADVSequenceObjectContext*> AnnotatedDNAView::findRelatedSequenceContexts(GObject* obj) const {
    QList<GObject*> relatedObjects = GObjectUtils::selectRelations(obj, GObjectTypes::SEQUENCE, ObjectRole_Sequence, objects, UOF_LoadedOnly);
    QList<ADVSequenceObjectContext*> res;
    foreach (GObject* seqObj, relatedObjects) {
        auto dnaObj = qobject_cast<U2SequenceObject*>(seqObj);
        ADVSequenceObjectContext* ctx = getSequenceContext(dnaObj);
        res.append(ctx);
    }
    return res;
}

void AnnotatedDNAView::sl_onPosChangeRequest(int pos) {
    uiLog.trace(QString("ADV: center change request: %1").arg(pos));
    ADVSequenceWidget* seqBlock = getActiveSequenceWidget();
    assert(seqBlock != nullptr);
    seqBlock->centerPosition(pos - 1);
}

void AnnotatedDNAView::sl_onShowPosSelectorRequest() {
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    assert(seqCtx != nullptr);

    QObjectScopedPointer<QDialog> dlg = new QDialog(getWidget());
    dlg->setModal(true);
    dlg->setWindowTitle(tr("Go to Position"));

    auto ps = new PositionSelector(dlg.data(), 1, seqCtx->getSequenceLength(), true);
    connect(ps, SIGNAL(si_positionChanged(int)), SLOT(sl_onPosChangeRequest(int)));

    dlg->exec();
}

void AnnotatedDNAView::insertWidgetIntoSplitter(ADVSplitWidget* splitWidget) {
    assert(mainSplitter != nullptr);
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

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(AnnotationTableObject* obj) const {
    SAFE_POINT(getAnnotationObjects(true).contains(obj),
               "Unexpected annotation table detected!",
               nullptr);
    foreach (ADVSequenceObjectContext* seqCtx, seqContexts) {
        QSet<AnnotationTableObject*> aObjs = seqCtx->getAnnotationObjects(true);
        if (aObjs.contains(obj)) {
            return seqCtx;
        }
    }
    return nullptr;
}

ADVSequenceObjectContext* AnnotatedDNAView::getActiveSequenceContext() const {
    ADVSequenceWidget* w = getActiveSequenceWidget();
    return w == nullptr ? nullptr : w->getActiveSequenceContext();
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(U2SequenceObject* obj) const {
    foreach (ADVSequenceObjectContext* seqCtx, seqContexts) {
        if (seqCtx->getSequenceObject() == obj) {
            return seqCtx;
        }
    }
    return nullptr;
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(const GObjectReference& r) const {
    foreach (ADVSequenceObjectContext* seqCtx, seqContexts) {
        GObjectReference ref(seqCtx->getSequenceObject());
        if (ref == r) {
            return seqCtx;
        }
    }
    return nullptr;
}

void AnnotatedDNAView::addRelatedAnnotations(ADVSequenceObjectContext* seqCtx) {
    QList<GObject*> allLoadedAnnotations = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::ANNOTATION_TABLE);
    QList<GObject*> loadedAndRelatedAnnotations = GObjectUtils::findObjectsRelatedToObjectByRole(
        seqCtx->getSequenceObject(),
        GObjectTypes::ANNOTATION_TABLE,
        ObjectRole_Sequence,
        allLoadedAnnotations,
        UOF_LoadedOnly);

    foreach (GObject* ao, loadedAndRelatedAnnotations) {
        if (objects.contains(ao)) {
            seqCtx->addAnnotationObject(qobject_cast<AnnotationTableObject*>(ao));
        } else {
            addObject(ao);
        }
    }
}

void AnnotatedDNAView::addAutoAnnotations(ADVSequenceObjectContext* seqCtx) {
    auto aa = new AutoAnnotationObject(seqCtx->getSequenceObject(), seqCtx->getAminoTT(), seqCtx);
    seqCtx->addAutoAnnotationObject(aa->getAnnotationObject());
    autoAnnotationsMap.insert(seqCtx, aa);

    emit si_annotationObjectAdded(aa->getAnnotationObject());

    foreach (ADVSequenceWidget* w, seqCtx->getSequenceWidgets()) {
        auto aaAction = new AutoAnnotationsADVAction(w, aa);
        w->addADVSequenceWidgetAction(aaAction);
    }
}

void AnnotatedDNAView::removeAutoAnnotations(ADVSequenceObjectContext* seqCtx) {
    AutoAnnotationObject* aa = autoAnnotationsMap.take(seqCtx);
    cancelAutoAnnotationUpdates(aa);
    emit si_annotationObjectRemoved(aa->getAnnotationObject());
    delete aa;
}

void AnnotatedDNAView::cancelAutoAnnotationUpdates(AutoAnnotationObject* aa, bool* removeTaskExist) {
    QList<Task*> tasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    foreach (Task* t, tasks) {
        auto aaUpdateTask = qobject_cast<AutoAnnotationsUpdateTask*>(t);
        if (aaUpdateTask != nullptr) {
            if (aaUpdateTask->getAutoAnnotationObject() == aa && !aaUpdateTask->isFinished()) {
                aaUpdateTask->setAutoAnnotationInvalid();
                aaUpdateTask->cancel();
                if (removeTaskExist) {
                    *removeTaskExist = false;
                    QList<QPointer<Task>> subtasks = aaUpdateTask->getSubtasks();
                    for (const QPointer<Task>& subTask : qAsConst(subtasks)) {
                        auto rTask = qobject_cast<RemoveAnnotationsTask*>(subTask.data());
                        if (rTask && !rTask->isFinished()) {
                            *removeTaskExist = true;
                        }
                    }
                }
            }
        }
    }
}

/**
 * Adds common graphs menu to the current for each sequence
 */
void AnnotatedDNAView::addGraphs(ADVSequenceObjectContext* seqCtx) {
    foreach (ADVSequenceWidget* seqWidget, seqCtx->getSequenceWidgets()) {
        auto singleSeqWidget = qobject_cast<ADVSingleSequenceWidget*>(seqWidget);
        SAFE_POINT(singleSeqWidget != nullptr, "singleSeqWidget is NULL", );
        auto graphMenuAction = new GraphMenuAction(singleSeqWidget->getSequenceObject()->getAlphabet());
        if (singleSeqWidget != nullptr) {
            singleSeqWidget->addADVSequenceWidgetActionToViewsToolbar(graphMenuAction);
        } else {
            seqWidget->addADVSequenceWidgetAction(graphMenuAction);
        }
    }
}

void AnnotatedDNAView::sl_onDocumentAdded(Document* d) {
    GObjectViewController::sl_onDocumentAdded(d);
    importDocAnnotations(d);
}

void AnnotatedDNAView::importDocAnnotations(Document* doc) {
    QList<GObject*> docObjects = doc->getObjects();

    foreach (GObject* obj, objects) {
        if (obj->getGObjectType() != GObjectTypes::SEQUENCE) {
            continue;
        }
        QList<GObject*> relatedAnns = GObjectUtils::findObjectsRelatedToObjectByRole(obj, GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence, docObjects, UOF_LoadedOnly);
        for (GObject* annObj : qAsConst(relatedAnns)) {
            addObject(annObj);
        }
    }
}

void AnnotatedDNAView::seqWidgetMove(const QPoint& pos) {
    SAFE_POINT(replacedSeqWidget, "Moving the NULL widget", );
    CHECK_EXT(seqViews.contains(replacedSeqWidget), replacedSeqWidget = nullptr, );

    int index = seqViews.indexOf(replacedSeqWidget);
    QRect replacedWidgetRect = replacedSeqWidget->geometry();
    CHECK(!replacedWidgetRect.contains(pos), );

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
        std::swap(seqViews[index - 1], seqViews[index]);
        scrolledWidgetLayout->insertWidget(index - 1, scrolledWidgetLayout->takeAt(index)->widget());
    }
    if (nextWidgetRect.isValid() && pos.y() > nextWidgetRect.top()) {
        std::swap(seqViews[index], seqViews[index + 1]);
        scrolledWidgetLayout->insertWidget(index, scrolledWidgetLayout->takeAt(index + 1)->widget());
    }
}

void AnnotatedDNAView::finishSeqWidgetMove() {
    replacedSeqWidget = nullptr;
}

void AnnotatedDNAView::createCodonTableAction() {
    QAction* showCodonTableAction = new ADVGlobalAction(this, QIcon(":core/images/codon_table.png"), tr("Show codon table"), INT_MAX - 1, ADVGlobalActionFlag_AddToToolbar);
    showCodonTableAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    showCodonTableAction->setShortcutContext(Qt::WindowShortcut);
    connect(showCodonTableAction, SIGNAL(triggered()), codonTableView, SLOT(sl_setVisible()));
    showCodonTableAction->setObjectName("Codon table");
    showCodonTableAction->setCheckable(true);
}

void AnnotatedDNAView::sl_onDocumentLoadedStateChanged() {
    auto d = qobject_cast<Document*>(sender());
    importDocAnnotations(d);
    GObjectViewController::sl_onDocumentLoadedStateChanged();
}

QList<U2SequenceObject*> AnnotatedDNAView::getSequenceObjectsWithContexts() const {
    QList<U2SequenceObject*> res;
    foreach (ADVSequenceObjectContext* cx, seqContexts) {
        res.append(cx->getSequenceObject());
    }
    return res;
}

QList<GObject*> AnnotatedDNAView::getSequenceGObjectsWithContexts() const {
    QList<GObject*> res;
    foreach (ADVSequenceObjectContext* cx, seqContexts) {
        res.append(cx->getSequenceObject());
    }
    return res;
}

void AnnotatedDNAView::updateState(const AnnotatedDNAViewState& s) {
    if (!s.isValid()) {
        return;
    }
    QList<GObjectReference> objs = s.getSequenceObjects();
    QVector<U2Region> regs = s.getSequenceSelections();
    assert(objs.size() == regs.size());

    // TODO: sync seq object lists
    // TODO: sync annotation object lists

    for (int i = 0; i < objs.size(); i++) {
        const GObjectReference& ref = objs[i];
        const U2Region& reg = regs[i];
        ADVSequenceObjectContext* seqCtx = getSequenceContext(ref);
        if (seqCtx == nullptr) {
            continue;
        }
        U2Region wholeSeq(0, seqCtx->getSequenceLength());
        U2Region finalSel = reg.intersect(wholeSeq);
        seqCtx->getSequenceSelection()->setRegion(finalSel);
    }
    foreach (ADVSequenceWidget* sw, seqViews) {
        sw->updateState(s.stateData);
    }

    foreach (ADVSplitWidget* w, splitWidgets) {
        w->updateState(s.stateData);
    }

    annotationsView->updateState(s.stateData);
}

void AnnotatedDNAView::sl_editSettings() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT_NN(s, );
    EditSettings settings;
    settings.annotationStrategy =
        (U1AnnotationUtils::AnnotationStrategyForResize)s->getValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_ANNOTATION_STRATEGY,
                                                                    U1AnnotationUtils::AnnotationStrategyForResize_Resize)
            .toInt();
    settings.recalculateQualifiers = s->getValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_RECALC_QUALIFIERS, false).toBool();

    QObjectScopedPointer<EditSettingsDialog> dlg = new EditSettingsDialog(settings, getActiveSequenceWidget());
    int res = dlg->exec();
    SAFE_POINT(!dlg.isNull(), "EditSettingsDialog is null!", );

    if (res == QDialog::Accepted) {
        const EditSettings& newSettings = dlg->getSettings();
        s->setValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_ANNOTATION_STRATEGY, newSettings.annotationStrategy);
        s->setValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_RECALC_QUALIFIERS, newSettings.recalculateQualifiers);
    }
}

void AnnotatedDNAView::sl_addSequencePart() {
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    U2SequenceObject* seqObj = seqCtx->getSequenceObject();

    EditSequencDialogConfig cfg;

    cfg.mode = EditSequenceMode_Insert;
    cfg.source = U2Region(0, seqObj->getSequenceLength());
    cfg.alphabet = seqObj->getAlphabet();
    cfg.position = 1;

    auto wgt = qobject_cast<ADVSingleSequenceWidget*>(activeSequenceWidget);
    if (wgt != nullptr) {
        QList<GSequenceLineView*> views = wgt->getLineViews();
        foreach (GSequenceLineView* v, views) {
            if (v->hasFocus()) {
                cfg.position = v->getLastPressPos();
                break;
            }
        }
    }

    const QVector<U2Region>& selection = seqCtx->getSequenceSelection()->getSelectedRegions();
    cfg.selectionRegions = selection;

    QObjectScopedPointer<EditSequenceDialogController> dialog = new EditSequenceDialogController(cfg, getActiveSequenceWidget(), "96665661");
    const int result = dialog->exec();
    CHECK(!dialog.isNull(), );
    CHECK(result == QDialog::Accepted, );

    Task* t = new ModifySequenceContentTask(dialog->getDocumentFormatId(), seqObj, U2Region(dialog->getPosToInsert(), 0), dialog->getNewSequence(), dialog->recalculateQualifiers(), dialog->getAnnotationStrategy(), dialog->getDocumentPath(), dialog->mergeAnnotations());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_removeSequencePart() {
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    U2SequenceObject* seqObj = seqCtx->getSequenceObject();

    Document* curDoc = seqObj->getDocument();
    U2Region source(0, seqObj->getSequenceLength());

    U2Region selection = source;
    if (seqCtx->getSequenceSelection()->getSelectedRegions().size() > 0) {
        selection = seqCtx->getSequenceSelection()->getSelectedRegions().first();
    }

    QObjectScopedPointer<RemovePartFromSequenceDialogController> dialog = new RemovePartFromSequenceDialogController(selection, source, curDoc->getURLString(), getActiveSequenceWidget());
    const int result = dialog->exec();
    CHECK(!dialog.isNull(), );
    CHECK(result == QDialog::Accepted, );

    Task* t = nullptr;
    if (dialog->modifyCurrentDocument()) {
        t = new ModifySequenceContentTask(dialog->getDocumentFormatId(), seqObj, dialog->getRegionToDelete(), DNASequence(), dialog->recalculateQualifiers(), dialog->getStrategy(), seqObj->getDocument()->getURL());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    } else {
        t = new ModifySequenceContentTask(dialog->getDocumentFormatId(), seqObj, dialog->getRegionToDelete(), DNASequence(), dialog->recalculateQualifiers(), dialog->getStrategy(), dialog->getNewDocumentPath(), dialog->mergeAnnotations());
    }
    SAFE_POINT_NN(t, );
    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_replaceSequencePart() {
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    U2SequenceObject* seqObj = seqCtx->getSequenceObject();

    if (seqCtx->getSequenceSelection()->getSelectedRegions().isEmpty()) {
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

    QObjectScopedPointer<EditSequenceDialogController> dlg = new EditSequenceDialogController(cfg, getActiveSequenceWidget(), "96665651");
    const int result = dlg->exec();
    CHECK(!dlg.isNull(), );

    CHECK(result == QDialog::Accepted, );

    Task* t = new ModifySequenceContentTask(dlg->getDocumentFormatId(), seqObj, selection, dlg->getNewSequence(), dlg->recalculateQualifiers(), dlg->getAnnotationStrategy(), dlg->getDocumentPath(), dlg->mergeAnnotations());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_removeSelectedSequenceObject() {
    ADVSequenceWidget* sw = getActiveSequenceWidget();
    ADVSequenceObjectContext* soc = sw->getActiveSequenceContext();
    U2SequenceObject* so = soc->getSequenceObject();
    removeObject(so);
}

QList<AnnotationTableObject*> AnnotatedDNAView::getAnnotationObjects(bool includeAutoAnnotations) const {
    QList<AnnotationTableObject*> result = annotations;
    if (includeAutoAnnotations) {
        foreach (AutoAnnotationObject* aa, autoAnnotationsMap.values()) {
            result += aa->getAnnotationObject();
        }
    }
    return result;
}

void AnnotatedDNAView::updateAutoAnnotations() {
    QList<AutoAnnotationObject*> autoAnnotations = autoAnnotationsMap.values();
    foreach (AutoAnnotationObject* aa, autoAnnotations) {
        aa->updateAll();
    }
}

void AnnotatedDNAView::sl_sequenceModifyTaskStateChanged() {
    auto t = qobject_cast<Task*>(sender());
    if (t == nullptr) {
        return;
    }

    if (t->getState() == Task::State_Finished && !(t->hasError() || t->isCanceled())) {
        updateAutoAnnotations();
        // TODO: there must be better way to do this
        bool reverseComplementTask = false;
        if (qobject_cast<ReverseComplementSequenceTask*>(t) != nullptr ||
            qobject_cast<ReverseSequenceTask*>(t) != nullptr ||
            qobject_cast<ComplementSequenceTask*>(t) != nullptr) {
            reverseComplementTask = true;
        }

        ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
        if (reverseComplementTask && seqCtx != nullptr) {
            const QVector<U2Region> regions = seqCtx->getSequenceSelection()->getSelectedRegions();
            if (regions.count() == 1) {
                const U2Region r = regions.first();
                foreach (ADVSequenceWidget* w, seqCtx->getSequenceWidgets()) {
                    w->centerPosition((int)r.startPos);
                }
            }
        }

        auto modifyContentTask = qobject_cast<ModifySequenceContentTask*>(t);
        if (modifyContentTask != nullptr) {
            qint64 seqSizeDelta = modifyContentTask->getSequenceLengthDelta();
            if (seqSizeDelta > 0) {  // try keeping all maximized zooms in max state
                U2Region newMaxRange(0, modifyContentTask->getSequenceObject()->getSequenceLength());
                U2Region oldMaxRange(0, newMaxRange.length - seqSizeDelta);
                foreach (ADVSequenceObjectContext* ctx, seqContexts) {
                    if (ctx->getSequenceGObject() == modifyContentTask->getSequenceObject()) {
                        QList<ADVSequenceWidget*> widgets = seqCtx->getSequenceWidgets();
                        for (ADVSequenceWidget* w : qAsConst(widgets)) {
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

void AnnotatedDNAView::sl_paste() {
    PasteFactory* pasteFactory = AppContext::getPasteFactory();
    SAFE_POINT(pasteFactory != nullptr, "pasteFactory is null", );

    auto wgt = qobject_cast<ADVSingleSequenceWidget*>(activeSequenceWidget);
    CHECK(wgt != nullptr, );

    DetView* detView = wgt->getDetView();
    SAFE_POINT(detView, "DetView is unexpectedly NULL", );
    CHECK(detView->hasFocus(), );
    SAFE_POINT(detView->getEditor(), "DetViewEditor is NULL", );
    CHECK(detView->getEditor()->isEditMode(), );

    PasteTask* task = pasteFactory->createPasteTask(false);
    CHECK(task != nullptr, );
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task*)), detView->getEditor(), SLOT(sl_paste(Task*)));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void AnnotatedDNAView::onObjectRenamed(GObject* obj, const QString& oldName) {
    if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        // 1. update title
        OpenAnnotatedDNAViewTask::updateTitle(this);

        // 2. update components
        auto seqObj = qobject_cast<U2SequenceObject*>(obj);
        ADVSequenceObjectContext* ctx = getSequenceContext(seqObj);
        foreach (ADVSequenceWidget* w, ctx->getSequenceWidgets()) {
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
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    CHECK(seqCtx != nullptr, );
    auto selection = qobject_cast<DNASequenceSelection*>(sender());
    CHECK(selection != nullptr && seqCtx->getSequenceGObject() == selection->getSequenceObject(), );

    replaceSequencePart->setEnabled(!seqCtx->getSequenceSelection()->isEmpty());
}

void AnnotatedDNAView::sl_aminoTranslationChanged() {
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    U2SequenceObject* seqObj = seqCtx->getSequenceObject();
    QList<AutoAnnotationObject*> autoAnnotations = autoAnnotationsMap.values();
    foreach (AutoAnnotationObject* aa, autoAnnotations) {
        if (aa->getSequenceObject() == seqObj) {
            aa->updateTranslationDependent(seqCtx->getAminoTT());
        }
    }
}

void AnnotatedDNAView::reverseComplementSequence(bool reverse, bool complement) {
    ADVSequenceObjectContext* seqCtx = getActiveSequenceContext();
    U2SequenceObject* seqObj = seqCtx->getSequenceObject();
    QList<AnnotationTableObject*> annotationObjects = toList(seqCtx->getAnnotationObjects(false));

    DNATranslation* complTT = nullptr;
    if (seqObj->getAlphabet()->isNucleic()) {
        complTT = seqCtx->getComplementTT();
    }

    Task* t = nullptr;
    if (reverse && complement) {
        t = new ReverseComplementSequenceTask(seqObj, annotationObjects, seqCtx->getSequenceSelection(), complTT);
    } else if (reverse) {
        t = new ReverseSequenceTask(seqObj, annotationObjects, seqCtx->getSequenceSelection());
    } else if (complement) {
        t = new ComplementSequenceTask(seqObj, annotationObjects, seqCtx->getSequenceSelection(), complTT);
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
}

QAction* AnnotatedDNAView::getEditActionFromSequenceWidget(ADVSequenceWidget* seqWgt) {
    auto wgt = qobject_cast<ADVSingleSequenceWidget*>(seqWgt);
    SAFE_POINT(wgt != nullptr, "ADVSingleSequenceWidget is NULL", nullptr);

    DetView* detView = wgt->getDetView();
    SAFE_POINT(detView != nullptr, "DetView is NULL", nullptr);

    DetViewSequenceEditor* editor = detView->getEditor();
    SAFE_POINT(editor != nullptr, "DetViewSequenceEditor is NULL", nullptr);

    QAction* editAction = editor->getEditAction();
    SAFE_POINT(editAction != nullptr, "EditAction is NULL", nullptr);

    return editAction;
}

bool AnnotatedDNAView::areAnnotationsInRange(const QList<Annotation*>& toCheck) {
    foreach (Annotation* a, toCheck) {
        QList<ADVSequenceObjectContext*> relatedSeqObjects = findRelatedSequenceContexts(a->getGObject());
        for (ADVSequenceObjectContext* seq : qAsConst(relatedSeqObjects)) {
            SAFE_POINT(seq != nullptr, "Sequence is NULL", true);
            QVector<U2Region> regions = a->getRegions();
            for (const U2Region& r : qAsConst(regions)) {
                if (r.endPos() > seq->getSequenceLength()) {
                    return false;
                }
            }
        }
    }
    return true;
}

ADVSequenceWidget* AnnotatedDNAView::getActiveSequenceWidget() const {
    return activeSequenceWidget;
}

}  // namespace U2
