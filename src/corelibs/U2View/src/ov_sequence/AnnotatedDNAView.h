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

#ifndef _U2_ANNOTATED_DNA_VIEW_H_
#define _U2_ANNOTATED_DNA_VIEW_H_

#include <QPointer>
#include <QTextEdit>
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
class OptionsPanel;

class CodonTableView;
class CodonTableAction;

class U2VIEW_EXPORT AnnotatedDNAView : public GObjectView {
    Q_OBJECT
    friend class DetViewSequenceEditor; // TODO_SVEDIT: remove this
public:
    AnnotatedDNAView(const QString &viewName, const QList<U2SequenceObject *> &dnaObjects);
    ~AnnotatedDNAView();

    virtual void buildStaticToolbar(QToolBar* tb);

    virtual void buildStaticMenu(QMenu* n);

    virtual Task* updateViewTask(const QString& stateName, const QVariantMap& stateData);

    virtual QVariantMap saveState();

    virtual OptionsPanel* getOptionsPanel();

    // view content
    const QList<ADVSequenceObjectContext*>& getSequenceContexts() const {return seqContexts;}

    QList<U2SequenceObject*> getSequenceObjectsWithContexts() const;

    QList<GObject*> getSequenceGObjectsWithContexts() const;

    QList<AnnotationTableObject *> getAnnotationObjects( bool includeAutoAnnotations = false) const;

    AnnotationSelection * getAnnotationsSelection() const {return annotationSelection;}

    AnnotationGroupSelection * getAnnotationsGroupSelection() const {return annotationGroupSelection;}

    const QList<ADVSequenceWidget *> getSequenceWidgets() const {return seqViews;}

    virtual bool canAddObject(GObject* obj);

    void addSequenceWidget(ADVSequenceWidget* v);

    void removeSequenceWidget(ADVSequenceWidget* v);

    void insertWidgetIntoSplitter(ADVSplitWidget* widget);

    void unregisterSplitWidget(ADVSplitWidget* widget);

    virtual QString addObject(GObject* o);

    void saveWidgetState();

    ADVSequenceObjectContext* getSequenceContext(AnnotationTableObject *obj) const;

    ADVSequenceObjectContext* getSequenceContext(U2SequenceObject*) const;

    ADVSequenceObjectContext* getSequenceContext(const GObjectReference& r) const;

    QWidget* getScrolledWidget() const {return scrolledWidget;}

    ADVSequenceWidget* getSequenceWidgetInFocus() const {return focusedWidget;}

    ADVSequenceObjectContext* getSequenceInFocus() const;

    QList<ADVSequenceObjectContext*> getAllSeqContextsInFocus() const;

    QList<ADVSequenceObjectContext*> findRelatedSequenceContexts(GObject* obj) const;

    void setFocusedSequenceWidget(ADVSequenceWidget* v);

    void updateState(const AnnotatedDNAViewState& s);

    QAction* getCreateAnnotationAction() const {return createAnnotationAction;}

    void addADVAction(ADVGlobalAction* a);

    void removeADVAction(ADVGlobalAction* a) {advActions.removeAll(a);}

    AnnotationsTreeView* getAnnotationsView() {return annotationsView;}

    void updateAutoAnnotations();

    void addAutoAnnotationsUpdated(AutoAnnotationsUpdater* updater);

    /**
     * Returns "true" if all input annotations are within the bounds of the associated sequences.
     * Returns "true" in case of an error.
     * Otherwise, returns "false", i.e. the method returns "false", even if an annotation intersects a sequence only partially.
     */
    bool areAnnotationsInRange(const QList<Annotation *> &toCheck);

    /**
     * Tries to add object to the view. Uses GUI functions to ask user if some data if needed
     * Returns error message if failed.
     * If object is unloaded - intitiates async object loading
     */
    QString tryAddObject(GObject* obj);

    const CodonTableView* getCodonTableView() const {return codonTableView; }

protected:
    virtual QWidget* createWidget();
    virtual bool onObjectRemoved(GObject* o);
    virtual void onObjectRenamed(GObject* obj, const QString& oldName);
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void timerEvent(QTimerEvent *e);

    virtual bool isChildWidgetObject(GObject* o) const;
    virtual void addAnalyseMenu(QMenu* m);
    virtual void addAddMenu(QMenu* m);
    virtual void addExportMenu(QMenu* m);
    virtual void addAlignMenu(QMenu* m);
    virtual void addRemoveMenu(QMenu* m);
    virtual void addEditMenu(QMenu* m);
    virtual ADVSequenceWidget* findSequenceWidgetByPos(const QPoint& globalPos) const;

    virtual bool onCloseEvent();

signals:
    void si_sequenceAdded(ADVSequenceObjectContext* c);
    void si_sequenceRemoved(ADVSequenceObjectContext* c);

    void si_annotationObjectAdded( AnnotationTableObject *obj );
    void si_annotationObjectRemoved( AnnotationTableObject *obj );

    void si_sequenceWidgetAdded(ADVSequenceWidget* w);
    void si_sequenceWidgetRemoved(ADVSequenceWidget* w);

    void si_focusChanged(ADVSequenceWidget *from, ADVSequenceWidget *to);
    /** Emitted when a part was added to a sequence, or it was removed or replaced */
    void si_sequenceModified(ADVSequenceObjectContext*);
    void si_onClose(AnnotatedDNAView* v);
    void si_proxyCopyQualifierActionTriggered();

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

    virtual void sl_onDocumentAdded(Document*);
    virtual void sl_onDocumentLoadedStateChanged();
    virtual void sl_removeSelectedSequenceObject();

private:
    void updateScrollAreaHeight();
    void updateMultiViewActions();

    void addRelatedAnnotations(ADVSequenceObjectContext* seqCtx);
    void addAutoAnnotations(ADVSequenceObjectContext* seqCtx);
    void removeAutoAnnotations(ADVSequenceObjectContext* seqCtx);
    void cancelAutoAnnotationUpdates(AutoAnnotationObject* aaObj, bool* existsRemovedTasks = NULL);
    void addGraphs(ADVSequenceObjectContext* seqCtx);
    void importDocAnnotations(Document* doc);
    void seqWidgetMove(const QPoint& pos);
    void finishSeqWidgetMove();
    void createCodonTableAction();

    void reverseComplementSequence(bool reverse = true, bool complement = true);

    static QAction* getEditActionFromSequenceWidget(ADVSequenceWidget* seqWgt);

    QAction * createPasteAction();

    QSplitter*          mainSplitter;
    QScrollArea*        scrollArea;
    QWidget*            scrolledWidget;
    QVBoxLayout*        scrolledWidgetLayout;

    CodonTableView*     codonTableView;

    QAction*            createAnnotationAction;
    QAction*            findPatternAction;
    QAction*            posSelectorAction;
    QAction*            toggleHLAction;
    QAction*            posSelectorWidgetAction;
    QAction*            removeAnnsAndQsAction;

    QAction*            editSettingsAction;
    QAction*            addSequencePart;
    QAction*            removeSequencePart;
    QAction*            replaceSequencePart;
    QAction*            removeSequenceObjectAction;

    QAction*            reverseComplementSequenceAction;
    QAction*            reverseSequenceAction;
    QAction*            complementSequenceAction;

    PositionSelector*   posSelector;

    QList<ADVSequenceObjectContext*>    seqContexts;
    QList<AnnotationTableObject*>       annotations;
    QList<ADVObjectHandler*>            handlers;
    QList<ADVGlobalAction*>             advActions;

    QMap<ADVSequenceObjectContext*, AutoAnnotationObject*>   autoAnnotationsMap;

    AnnotationsTreeView*        annotationsView;
    QList<ADVSequenceWidget*>   seqViews;
    QList<ADVSplitWidget*>      splitWidgets;

    AnnotationSelection *       annotationSelection;
    AnnotationGroupSelection *  annotationGroupSelection;

    ADVClipboard*               clipb;
    ADVSyncViewManager*         syncViewManager;

    ADVSequenceWidget*          focusedWidget;
    ADVSequenceWidget*          replacedSeqWidget;     // not NULL when any sequence widget is dragging to the new place.

    int                         timerId;

    // Used to detect 'expandable sequences' <-> 'fixed sequences' transition event for the mainSplitter.
    bool hadExpandableSequenceWidgetsLastResize;
    // Used to restore mainSplitter state on 'fixed sequences'-> 'expandable sequences' transition.
    QList<int> savedMainSplitterSizes;
};

} // namespace U2

#endif
