/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_QUERY_VIEW_CONTROLLER_H_
#define _U2_QUERY_VIEW_CONTROLLER_H_

#include "ui_AddConstraintDialog.h"

#include <QActionGroup>

#include <U2Lang/QDConstraint.h>

#include <U2Gui/MainWindow.h>

#include <QGraphicsScene>

namespace U2 {

class QueryViewController;
class QDElement;
class QueryEditor;
class QDLabelItem;
class QDDescriptionItem;
class QDRulerItem;

class QueryScene : public QGraphicsScene {
    friend class LoadSequenceTask;
    friend class QueryViewController;
    friend class QDViewAdapter;
    Q_OBJECT
public:
    QueryScene(QueryViewController* parent=0);
    ~QueryScene();

    void initTitle();
    void initRuler();
    void initDescription();

    QDScheme* getScheme() const { return scheme; }
    QRectF rulerArea() const;
    QRectF annotationsArea() const;
    QRectF footnotesArea() const;
    QList<QDElement*> getElements() const;
    QList<QGraphicsItem*> getFootnotes() const;
    QDElement* getUnitView(QDSchemeUnit* su) const;
    void updateSceneRect();
    void setRowsNumber(int count);
    int getRowsNumber() { return rowsNum; }
    void insertRow(int idx);
    int getRow(QDElement* const uv) const;
    QDLabelItem* getLabelItem() const { return labelTxtItem; }
    QString getLabel() const;
    void setLabel(const QString& lbl);
    QString getDescription() const;
    void setDescription(const QString& dsc);
    QueryViewController* getViewController() const { return view; }
    bool showActorDesc() const { return showDesc; }
    bool showActorOrder() const { return showOrder; }

    void updateDescription();
    void addActor(QDActor* actor, const QPointF& pos);
    void addDistanceConstraint(QDElement* src, QDElement* dst, QDDistanceType distType, int min, int max);
    void removeActor(QDActor* actor);
    void removeActors(const QList<QDActor*>& actors);
    void removeConstraint(QDConstraint* c);
    void clearScene();

    bool isModified() const { return modified; }
    void setModified(bool b);
signals:
    void si_schemeChanged();
    void si_itemAdded();
public slots:
    void sl_showLabel(bool show);
    void sl_showSchemeDesc(bool show);
    void sl_showItemDesc(bool show);
    void sl_showOrder(bool show);
    void sl_adaptRowsNumber();
    void sl_updateRulerText();
    void sl_setModified() { setModified(true); }
public:
    static const QSizeF MAX_SCENE_SIZE;
    static const QSizeF DEFAULT_SCENE_SIZE;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void emit_schemeChanged() { emit si_schemeChanged(); }
private:
    QList<QGraphicsItem*> unitsIntersectedByRow(int idx) const;
    QList<QGraphicsItem*> getElements(const QRectF& area);
    void setupDistanceDialog(QDDistanceType kind);
    bool ajustPosForNewItem(QDElement *item, QPointF &posToAjust);
private:
    QDElement*              dropCandidateLeft;
    QDElement*              dropCandidateRight;
    QueryViewController*    view;
    int                     rowsNum;
    QDScheme*               scheme;

    QDLabelItem*            labelTxtItem;
    QDDescriptionItem*      descTxtItem;
    QDRulerItem*            ruler;

    bool                    showSchemeLbl;
    bool                    showDesc;
    bool                    showOrder;

    bool                    modified;
};

class GlassView;
class QueryPalette;
class QDGroupsEditor;
class QDDocument;

class QueryViewController : public MWMDIWindow {
    Q_OBJECT
public:
    QueryViewController();
    virtual void setupMDIToolbar(QToolBar* tb);
    virtual void setupViewMenu(QMenu* m);
    void loadScene(const QString& content);
    void switchToGroupsTab();
    void setDefaultInFile(const QString& inFile) { inFile_=inFile; }
    void setDefaultOutFile(const QString& outFile) { outFile_=outFile; }
    QDScheme* getScheme() const { return scene->getScheme(); }
    static void setupConstraintEditor(QDConstraint* c);
    void enableSaveAction(bool enable);
    QDActor *getActor() {return currentActor;}
    void setSchemeUri(QString uri) {schemeUri = uri;};
protected:
    bool onCloseEvent();
private slots:
    void sl_newScene();
    void sl_loadScene();
    void sl_saveScene();
    void sl_saveSceneAs();
    void sl_run();
    void sl_deleteItem();
    void sl_pasteSample(QDDocument* content);
    void sl_editItem();
    void sl_elementSelected(QDActorPrototype* proto);
    void sl_selectEditorCell(const QString& link);
    void sl_updateTitle();
    void sl_setGlobalStrand(QAction* a);
    void sl_itemAdded();

    void sl_scrollUp();
private:
    void createActions();
    void saveState();
    void setupViewModeMenu(QMenu* m);
    void setupQuerySequenceModeMenu(QMenu* m);
    void setupStrandMenu(QMenu* m);
    bool confirmModified();
private:
    QueryScene*     scene;
    GlassView*      sceneView;
    QTabWidget*     tabs;
    QueryEditor*    editor;
    QueryPalette*   palette;
    QDGroupsEditor* groupsEditor;

    QAction*        runAction;
    QAction*        newAction;
    QAction*        loadAction;
    QAction*        saveAsAction;
    QAction*        saveAction;
    QAction*        deleteAction;

    QAction*        showLabelAction;
    QAction*        showDescAction;
    QAction*        showItemDescAction;
    QAction*        showOrderAction;

    QActionGroup*   strandActions;
    QAction*        directStrandAction;
    QAction*        complementStrandAction;
    QAction*        bothStrandsAction;

    QString         inFile_, outFile_;
    QString         schemeUri;
    QDActor         *currentActor;
};

class AddConstraintDialog : public QDialog, public Ui_AddConstraintDialog {
    Q_OBJECT
public:
    AddConstraintDialog(QueryScene* scene, QDDistanceType kind, QDElement* defSrc, QDElement* defDst);
    void accept();
private:
    QueryScene*     scene;
    QDDistanceType  kind;
};

class QDUtils {
public:
    static QPixmap generateSnapShot(QDDocument* doc, const QRect& rect);
    static QPixmap generateSnapShot(QueryScene* scene, const QRect& rect);
};

} //namespace

#endif
