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

#ifndef _U2_ANNOTATIONS_TREE_VIEW_
#define _U2_ANNOTATIONS_TREE_VIEW_

#include <QCloseEvent>
#include <QFlags>
#include <QLabel>
#include <QPair>
#include <QQueue>
#include <QTimer>
#include <QTreeWidget>

#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include <U2View/SearchQualifierDialog.h>

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class Annotation;
class AnnotationGroup;
class AnnotationGroupSelection;
class AnnotationModification;
class AnnotationSelection;
class AnnotationTableObject;
class AVAnnotationItem;
class AVGroupItem;
class AVItem;
class AVQualifierItem;
class GObjectView;
class RemoveItemsTask;
class U2Qualifier;

enum ATVAnnUpdateFlag {
    ATVAnnUpdateFlag_BaseColumns = 0x1,
    ATVAnnUpdateFlag_QualColumns = 0x2
};

typedef QFlags<ATVAnnUpdateFlag> ATVAnnUpdateFlags;

class AnnotationsTreeWidget : public QTreeWidget {
    Q_OBJECT
public:
    AnnotationsTreeWidget(QWidget* parent);
    QTreeWidgetItem* itemFromIndex(const QModelIndex & index) const;
};

class U2VIEW_EXPORT AnnotationsTreeView : public QWidget {
    Q_OBJECT
    friend class AnnotatedDNAView;
public:
    AnnotationsTreeView(AnnotatedDNAView* ctx);

    void saveWidgetState();
    void restoreWidgetState();

    void adjustStaticMenu(QMenu *m) const {adjustMenu(m);}

    QTreeWidget* getTreeWidget() const { return tree; }

    QStringList getQualifierColumnNames() const {return qColumns;}
    void addQualifierColumn(const QString& q);
    void removeQualifierColumn(const QString& q);

    void saveState(QVariantMap& map) const;
    void updateState(const QVariantMap& map);

    void setSortingEnabled(bool v);

    AVItem* currentItem();

    static const int COLUMN_NAME;
    static const int COLUMN_TYPE;
    static const int COLUMN_VALUE;

private slots:
    void sl_paste();
    void sl_pasteFinished(Task* pasteTask);

    void sl_onAnnotationObjectAdded(AnnotationTableObject *obj);
    void sl_onAnnotationObjectRemoved(AnnotationTableObject *obj);
    void sl_onAnnotationObjectRenamed(const QString &oldName);

    void sl_onAnnotationsAdded(const QList<Annotation *> &);
    void sl_onAnnotationsRemoved(const QList<Annotation *> &);
    void sl_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications);
    void sl_annotationObjectModifiedStateChanged();

    void sl_onGroupCreated(AnnotationGroup *);
    void sl_onGroupRemoved(AnnotationGroup *parent, AnnotationGroup *removed);
    void sl_onGroupRenamed(AnnotationGroup *);

    void sl_onAnnotationSettingsChanged(const QStringList &changedSettings);

    void sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &);
    void sl_onAnnotationGroupSelectionChanged(AnnotationGroupSelection *, const QList<AnnotationGroup *> &, const QList<AnnotationGroup *> &);
    void sl_onItemSelectionChanged();
    void sl_onAddAnnotationObjectToView();
    void sl_removeObjectFromView();
    void sl_removeAnnsAndQs();
    void sl_onBuildPopupMenu(GObjectView* thiz, QMenu* menu);
    void sl_onCopyQualifierValue();
    void sl_onCopyQualifierURL();
    void sl_onToggleQualifierColumn();
    void sl_onRemoveColumnByHeaderClick();
    void sl_onCopyColumnText();
    void sl_onCopyColumnURL();
    void sl_exportAutoAnnotationsGroup();
    void sl_searchQualifier();
    void sl_invertSelection();

    void sl_edit();
    void sl_addQualifier();

    void sl_itemEntered(QTreeWidgetItem *i, int column);
    void sl_itemClicked(QTreeWidgetItem *item, int column);
    void sl_itemPressed(QTreeWidgetItem *item);
    void sl_itemDoubleClicked (QTreeWidgetItem *item, int column);
    void sl_itemExpanded(QTreeWidgetItem *);

    void sl_sortTree();
    void sl_annotationClicked(Annotation* annotation);

    /*
     * Called when annotation activated by some external action.
     * Makes the annotation current and adds it into selection.
     */
    void sl_annotationActivated(Annotation* annotation, int regionIndex);

    /*
     * Called when annotation is double clicked anywhere in the view.
     * 'regionIndex' indicates which region was clicked.
     * A special '-1' value is supported when region is undefined or all regions should be processed.
     */
    void sl_annotationDoubleClicked(Annotation *annotation, int regionIndex);

    void sl_clearSelectedAnnotations();
    void sl_sequenceAdded(ADVSequenceObjectContext* advContext);
    void sl_sequenceRemoved(ADVSequenceObjectContext* advContext);

protected:
    bool eventFilter(QObject *o, QEvent *e);

signals:
    void si_setCopyQualifierActionStatus(bool isEnabled, QString text);

private:
    void editItem(AVItem *i);
    void editGroupItem(AVGroupItem *gi);
    void editQualifierItem(AVQualifierItem *qi);
    void editAnnotationItem(AVAnnotationItem *ai);
    void expandItemRecursevly(QTreeWidgetItem* item);
    QMap<AVAnnotationItem*, QList<U2Region> > sortAnnotationSelection(QList<AnnotationTableObject*> annotationObjects);

    QString renameDialogHelper(AVItem *i, const QString &defText, const QString &title);
    bool editQualifierDialogHelper(AVQualifierItem *i, bool ro, U2Qualifier &res);
    void moveDialogToItem(QTreeWidgetItem *item, QDialog *d);

    void adjustMenu(QMenu *m_) const;
    AVGroupItem * buildGroupTree(AVGroupItem *parentGroup, AnnotationGroup *g, bool areAnnotationsNew = true);
    AVAnnotationItem * buildAnnotationTree(AVGroupItem *parentGroup, Annotation *a, bool areAnnotationsNew = true);
    void populateAnnotationQualifiers(AVAnnotationItem *ai);
    void updateAllAnnotations(ATVAnnUpdateFlags flags);
    QMenu * getAutoAnnotationsHighligtingMenu(AnnotationTableObject *aObj);

    AVGroupItem * findGroupItem(AnnotationGroup *g) const;
    AVAnnotationItem * findAnnotationItem(AnnotationGroup *g, Annotation *a) const;
    AVAnnotationItem * findAnnotationItem(const AVGroupItem *gi, Annotation *a) const;
    // searches for annotation items that has not-null document added to the view
    QList<AVAnnotationItem *> findAnnotationItems(Annotation *a) const;
    // searches all annotation items recursively in a group
    QList<AVAnnotationItem *> findAnnotationItems(const AVGroupItem *gi) const;
    void removeGroupAnnotationsFromCache(const AVGroupItem *groupItem);

    void connectSequenceObjectContext(ADVSequenceObjectContext* advContext);
    void disconnectSequenceObjectContext(ADVSequenceObjectContext* advContext);

    void connectAnnotationSelection();
    void connectAnnotationGroupSelection();

    void updateState();
    void updateColumnContextActions(AVItem* item, int col);

    void resetDragAndDropData();
    bool initiateDragAndDrop(QMouseEvent* me);
    void finishDragAndDrop(Qt::DropAction dndAction);

    void annotationClicked(AVAnnotationItem* item, QMap<AVAnnotationItem*, QList<U2Region> > selectedAnnotations, const QList<U2Region>& selectedRegions = QList<U2Region>());
    void annotationDoubleClicked(AVAnnotationItem* item, const QList<U2Region>& selectedRegions);
    void clearSelectedNotAnnotations();
    void emitAnnotationActivated(Annotation *annotation);

    AnnotationsTreeWidget* tree;

    AnnotatedDNAView*   ctx;
    QAction*            addAnnotationObjectAction;
    QAction*            removeObjectsFromViewAction;
    QAction*            removeAnnsAndQsAction;
    QAction*            copyQualifierURLAction;
    QAction*            toggleQualifierColumnAction;
    QAction*            removeColumnByHeaderClickAction;
    QAction*            copyColumnTextAction;
    QAction*            copyColumnURLAction;
    QAction*            exportAutoAnnotationsGroup;

    QAction*            editAction;         // action to edit active group/qualifier/annotation only
    QAction*            addQualifierAction; // action to create qualifier. Editable annotation or editable qualifier must be selected

    QAction*            searchQualifierAction;
    QAction*            invertAnnotationSelectionAction;

    Qt::MouseButton     lastMB;
    QStringList         headerLabels;
    QStringList         qColumns;
    int                 lastClickedColumn;
    QIcon               addColumnIcon;
    QIcon               removeColumnIcon;
    QTimer              sortTimer;
    QPoint              dragStartPos;
    QMap<AVAnnotationItem*, QList<U2Region> > selectedAnnotation;
    // drag&drop related data
    bool                isDragging;
    bool                dndCopyOnly;
    QList<AVItem *>     dndSelItems;

    /**
     * Used for cross-view drag and drop: each time an annotation is added to the reciever AnnotationsTreeView,
     * the counter is increased by 1.
     * As soon as it equals to the overall number of the dragged annotations,
     * the annotations are checked for their range within the reciever sequences.
     */
    int                 dndHit;

    /** Dragged annotations, "static" for cross-view support */
    static QList<Annotation *> dndAdded;

    static AVGroupItem *    dropDestination;
    static const QString    annotationMimeType;

    friend class RemoveItemsTask;
    friend class FindQualifierTask;
    friend class SearchQualifierDialog;
};

//////////////////////////////////////////////////////////////////////////
/// Tree model

//TODO: create qualifiers subtrees only when qualifier node is opened (usually qualifiers get ~ 90% of memory)

enum AVItemType {
    AVItemType_Group,
    AVItemType_Annotation,
    AVItemType_Qualifier
};

class AVItem : public QTreeWidgetItem {
public:
                                    AVItem(QTreeWidgetItem *parent, AVItemType type);
    bool                            processLinks(const QString &qname, const QString &qval, int col);
    bool                            isColumnLinked(int col) const;
    QString                         buildLinkURL(int col) const;
    QString                         getFileUrl(int col) const;
    virtual bool                    isReadonly() const;

    virtual AnnotationsTreeView *   getAnnotationTreeView() const;
    virtual AnnotationTableObject * getAnnotationTableObject() const;
    virtual AnnotationGroup *       getAnnotationGroup();

    const AVItemType                type;
};


class AVGroupItem : public AVItem {
public:
    AVGroupItem(AnnotationsTreeView *atv, AVGroupItem *parent, AnnotationGroup *g);

    // @removedAnnotationCount and @removedSubgroupCount represent count of annotation and subgroups
    // that are about to be deleted
    void updateVisual(int removedAnnotationCount = 0);
    void updateAnnotations(const QString &nameFilter, ATVAnnUpdateFlags flags);
    void findAnnotationItems(QList<AVAnnotationItem *> &result, Annotation *a) const;

    static const QIcon & getGroupIcon();
    static const QIcon & getDocumentIcon();
    virtual AnnotationsTreeView * getAnnotationTreeView() const { return atv; }
    virtual bool isReadonly() const;
    virtual AnnotationTableObject * getAnnotationTableObject() const;
    virtual AnnotationGroup * getAnnotationGroup();

    AnnotationGroup *group;
    AnnotationsTreeView *atv;
};

class U2VIEW_EXPORT AVAnnotationItem : public AVItem {
public:
    AVAnnotationItem(AVGroupItem *parent, Annotation *a);

    Annotation *annotation;
    mutable QString locationString;

    virtual QVariant data (int column, int role) const;
    void updateVisual(ATVAnnUpdateFlags flags);
    virtual bool operator <(const QTreeWidgetItem &other) const;
    bool isColumnNumeric(int col) const;
    double getNumericVal(int col) const;

    void removeQualifier(const U2Qualifier &q);
    void addQualifier(const U2Qualifier &q);
    AVQualifierItem * findQualifierItem(const QString &name, const QString &val) const;

    static QMap<QString, QIcon> & getIconsCache();
    bool hasNumericQColumns;
};

class AVQualifierItem: public AVItem {
public:
    AVQualifierItem(AVAnnotationItem *parent, const U2Qualifier &q);

    static QString simplifyText(const QString& origValue);

    //TODO: keep values in U2Qualifier struct
    const QString qName;
    QString qValue;
};

class FindQualifierTaskSettings{
public:
    FindQualifierTaskSettings(AVItem* _groupToSearchIn, const QString& _name, const QString& _value, bool _isExactMatch, bool _searchAll, AVItem* _prevAnnotation = NULL, int _prevIndex = -1)
    :groupToSearchIn(_groupToSearchIn)
    ,name(_name)
    ,value(_value)
    ,isExactMatch(_isExactMatch)
    ,prevAnnotation(_prevAnnotation)
    ,prevIndex(_prevIndex)
    ,searchAll(_searchAll)
    {
    }

    AVItem* groupToSearchIn;
    QString name;
    QString value;
    bool isExactMatch;
    AVItem* prevAnnotation;
    int prevIndex;
    bool searchAll;
};

class U2VIEW_EXPORT FindQualifierTask: public Task{
    Q_OBJECT
public:
    FindQualifierTask(AnnotationsTreeView *treeView, const FindQualifierTaskSettings &settings);
    void run();
    ReportResult report();

    int getIndexOfResult() const { return indexOfResult; }
    AVItem * getResultAnnotation() const { return resultAnnotation; }
    bool isFound() const {return foundResult; }

private:
    void findInAnnotation(AVItem *annotation, bool &found);
    void findInGroup(AVItem *group, bool &found);
    int getStartIndexGroup(AVItem *group);
    int getStartIndexAnnotation(AVItem *annotation);

    AnnotationsTreeView *       treeView;
    QString                     qname;
    QString                     qvalue;
    AVItem *                    groupToSearchIn;
    bool                        isExactMatch;
    bool                        searchAll;
    bool                        foundResult;

    int indexOfResult;
    AVItem * resultAnnotation;

    QQueue<AVItem*> toExpand; //this queue is needed to expand items in main thread
    QList< QPair<AVAnnotationItem*, int> > foundQuals; //this is needed to set found items as selected
};

}//namespace


#endif
