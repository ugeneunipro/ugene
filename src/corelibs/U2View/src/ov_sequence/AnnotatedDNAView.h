/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
#include <QSplitter>

#include <U2Core/Annotation.h>
#include <U2Core/Task.h>

#include <U2Gui/ObjectViewModel.h>

class QScrollArea;
class QVBoxLayout;

namespace U2 {

class AnnotatedDNAView;
class AnnotatedDNAViewState;
class U2SequenceObject;
class AnnotationTableObject;
class Annotation;
class GSequenceLineView;
class AnnotationsTreeView;
class AnnotationSelection;
class AnnotationGroupSelection;
class Task;
class ADVClipboard;
class ADVSequenceWidget;
class ADVSplitWidget;
class ADVSequenceObjectContext;
class PositionSelector;
class GObjectReference;
class ADVSyncViewManager;
class ADVObjectHandler;
class ADVGlobalAction;
class AutoAnnotationObject;
class AutoAnnotationsUpdater;
class OptionsPanelController;

class CodonTableView;

class U2VIEW_EXPORT AnnotatedDNAView : public GObjectViewController {
    Q_OBJECT
    friend class DetViewSequenceEditor;  // TODO_SVEDIT: remove this
public:
    AnnotatedDNAView(const QString& viewName, const QList<U2SequenceObject*>& dnaObjects);
    ~AnnotatedDNAView();

    void buildStaticToolbar(QToolBar* tb) override;

    void buildMenu(QMenu* menu, const QString& type) override;

    Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) override;

    QVariantMap saveState() override;

    // view content
    const QList<ADVSequenceObjectContext*>& getSequenceContexts() const {
        return seqContexts;
    }

    QList<U2SequenceObject*> getSequenceObjectsWithContexts() const;

    QList<GObject*> getSequenceGObjectsWithContexts() const;

    QList<AnnotationTableObject*> getAnnotationObjects(bool includeAutoAnnotations = false) const;

    AnnotationSelection* getAnnotationsSelection() const {
        return annotationSelection;
    }

    AnnotationGroupSelection* getAnnotationsGroupSelection() const {
        return annotationGroupSelection;
    }

    const QList<ADVSequenceWidget*> getSequenceWidgets() const {
        return seqViews;
    }

    bool canAddObject(GObject* obj) override;

    void addSequenceWidget(ADVSequenceWidget* widgetToAdd);

    void removeSequenceWidget(ADVSequenceWidget* sequenceWidget);

    void insertWidgetIntoSplitter(ADVSplitWidget* widget);

    void unregisterSplitWidget(ADVSplitWidget* widget);

    QString addObject(GObject* o) override;

    void saveWidgetState() override;

    ADVSequenceObjectContext* getSequenceContext(AnnotationTableObject* obj) const;

    ADVSequenceObjectContext* getSequenceContext(U2SequenceObject*) const;

    ADVSequenceObjectContext* getSequenceContext(const GObjectReference& r) const;

    QWidget* getScrolledWidget() const {
        return scrolledWidget;
    }

    /** Returns active sequence widget. See 'activeSequenceWidget' docs. */
    ADVSequenceWidget* getActiveSequenceWidget() const;

    /** Returns context of the active sequence widget. See 'activeSequenceWidget' docs. */
    ADVSequenceObjectContext* getActiveSequenceContext() const;

    /** Sets sequence widget as active. See 'activeSequenceWidget' docs. */
    void setActiveSequenceWidget(ADVSequenceWidget* sequenceWidget);

    QList<ADVSequenceObjectContext*> findRelatedSequenceContexts(GObject* obj) const;

    void updateState(const AnnotatedDNAViewState& s);

    QAction* getCreateAnnotationAction() const {
        return createAnnotationAction;
    }

    void addADVAction(ADVGlobalAction* a);

    AnnotationsTreeView* getAnnotationsView() {
        return annotationsView;
    }

    void updateAutoAnnotations();

    /**
     * Returns "true" if all input annotations are within the bounds of the associated sequences.
     * Returns "true" in case of an error.
     * Otherwise, returns "false", i.e. the method returns "false", even if an annotation intersects a sequence only partially.
     */
    bool areAnnotationsInRange(const QList<Annotation*>& toCheck);

    /**
     * Tries to add object to the view. Uses GUI functions to ask user if some data if needed
     * Returns error message if failed.
     * If object is unloaded - intitiates async object loading
     */
    QString tryAddObject(GObject* obj);

    const CodonTableView* getCodonTableView() const {
        return codonTableView;
    }

protected:
    QWidget* createViewWidget(QWidget* parent) override;
    bool onObjectRemoved(GObject* o) override;
    void onObjectRenamed(GObject* obj, const QString& oldName) override;
    bool eventFilter(QObject*, QEvent*) override;
    void timerEvent(QTimerEvent* e) override;

    bool isChildWidgetObject(GObject* o) const;
    virtual void addAnalyseMenu(QMenu* m);
    virtual void addAddMenu(QMenu* m);
    virtual void addExportMenu(QMenu* m);
    virtual void addAlignMenu(QMenu* m);
    virtual void addRemoveMenu(QMenu* m);
    virtual void addEditMenu(QMenu* m);

    bool onCloseEvent() override;

signals:
    void si_sequenceAdded(ADVSequenceObjectContext* c);
    void si_sequenceRemoved(ADVSequenceObjectContext* c);

    void si_annotationObjectAdded(AnnotationTableObject* obj);
    void si_annotationObjectRemoved(AnnotationTableObject* obj);

    void si_sequenceWidgetAdded(ADVSequenceWidget* w);
    void si_sequenceWidgetRemoved(ADVSequenceWidget* w);

    /** Emitted every time active sequence widget is changed. See docs for 'activeSequenceWidget'. */
    void si_activeSequenceWidgetChanged(ADVSequenceWidget* oldActiveWidget, ADVSequenceWidget* newActiveWidget);

    /** Emitted when a part was added to a sequence, or it was removed or replaced */
    void si_sequenceModified(ADVSequenceObjectContext*);
    void si_onClose(AnnotatedDNAView* v);

public slots:
    void sl_onPosChangeRequest(int pos);

private slots:
    void sl_onContextMenuRequested();
    void sl_onFindPatternClicked();
    void sl_onShowPosSelectorRequest();
    void sl_toggleHL();
    void sl_splitterMoved(int, int);
    void sl_onSequenceWidgetTitleClicked(ADVSequenceWidget* seqWidget);

    void sl_editSettings();
    void sl_addSequencePart();
    void sl_removeSequencePart();
    void sl_replaceSequencePart();
    void sl_sequenceModifyTaskStateChanged();

    void sl_paste();

    void sl_reverseComplementSequence();
    void sl_reverseSequence();
    void sl_complementSequence();
    void sl_selectionChanged();
    void sl_aminoTranslationChanged();
    void sl_updatePasteAction();
    void sl_relatedObjectRelationChanged();
    /* Triggered on circular flag set\unset changed */
    void sl_sequenceCircularStateChanged();

    void sl_onDocumentAdded(Document*) override;
    void sl_onDocumentLoadedStateChanged() override;

    virtual void sl_removeSelectedSequenceObject();

private:
    void updateScrollAreaHeight();
    void updateMultiViewActions();

    void addRelatedAnnotations(ADVSequenceObjectContext* seqCtx);
    void addAutoAnnotations(ADVSequenceObjectContext* seqCtx);
    void removeAutoAnnotations(ADVSequenceObjectContext* seqCtx);
    void cancelAutoAnnotationUpdates(AutoAnnotationObject* aaObj, bool* existsRemovedTasks = nullptr);
    void addGraphs(ADVSequenceObjectContext* seqCtx);
    void importDocAnnotations(Document* doc);
    void seqWidgetMove(const QPoint& pos);
    void finishSeqWidgetMove();
    void createCodonTableAction();

    void reverseComplementSequence(bool reverse = true, bool complement = true);

    static QAction* getEditActionFromSequenceWidget(ADVSequenceWidget* seqWgt);

    QAction* createPasteAction();

    QSplitter* mainSplitter;
    QScrollArea* scrollArea;
    QWidget* scrolledWidget;
    QVBoxLayout* scrolledWidgetLayout;

    CodonTableView* codonTableView;

    QAction* createAnnotationAction;
    QAction* findPatternAction;
    QAction* posSelectorAction;
    QAction* toggleHLAction;
    QAction* posSelectorWidgetAction;
    QAction* removeAnnsAndQsAction;

    QAction* editSettingsAction;
    QAction* addSequencePart;
    QAction* removeSequencePart;
    QAction* replaceSequencePart;
    QAction* removeSequenceObjectAction;

    QAction* reverseComplementSequenceAction;
    QAction* reverseSequenceAction;
    QAction* complementSequenceAction;

    PositionSelector* posSelector;

    QList<ADVSequenceObjectContext*> seqContexts;
    QList<AnnotationTableObject*> annotations;
    QList<ADVObjectHandler*> handlers;
    QList<ADVGlobalAction*> advActions;

    QMap<ADVSequenceObjectContext*, AutoAnnotationObject*> autoAnnotationsMap;

    AnnotationsTreeView* annotationsView;
    QList<ADVSequenceWidget*> seqViews;
    QList<ADVSplitWidget*> splitWidgets;

    AnnotationSelection* annotationSelection;
    AnnotationGroupSelection* annotationGroupSelection;

    ADVClipboard* clipb;
    ADVSyncViewManager* syncViewManager;

    /*
     * activeSequenceWidget serves as a target for all view-global widgets.
     * AnnotatedDNAView always has at least some 'active' sequence widget unless it is in the destroying phase.
     */
    ADVSequenceWidget* activeSequenceWidget;

    ADVSequenceWidget* replacedSeqWidget;  // not NULL when any sequence widget is dragging to the new place.

    int timerId;

    // Used to detect 'expandable sequences' <-> 'fixed sequences' transition event for the mainSplitter.
    bool hadExpandableSequenceWidgetsLastResize;
    // Used to restore mainSplitter state on 'fixed sequences'-> 'expandable sequences' transition.
    QList<int> savedMainSplitterSizes;
};

}  // namespace U2
