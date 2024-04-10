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

#include <QLabel>
#include <QMenu>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <U2Core/GAutoDeleteList.h>
#include <U2Core/U2Region.h>

#include "ADVSequenceWidget.h"
#include "PanView.h"

#ifdef Q_OS_UNIX
#    include <QTimer>
#endif  // Q_OS_UNIX

namespace U2 {

class ADVSequenceObjectContext;
class ADVSingleSequenceHeaderWidget;
class AnnotatedDNAView;
class Annotation;
class AnnotationSelection;
class DetView;
class DNAAlphabet;
class DNATranslation;
class GSequenceLineView;
class Overview;
class PanView;
class U2SequenceObject;

class U2VIEW_EXPORT ADVSingleSequenceWidget : public ADVSequenceWidget {
    Q_OBJECT
public:
    ADVSingleSequenceWidget(ADVSequenceObjectContext* seqCtx, AnnotatedDNAView* ctx);
    ~ADVSingleSequenceWidget();

    const QList<GSequenceLineView*>& getLineViews() const {
        return lineViews;
    }

    GSequenceLineView* getPanGSLView() const;

    GSequenceLineView* getDetGSLView() const;

    PanView* getPanView() const {
        return panView;
    }

    DetView* getDetView() const {
        return detView;
    }

    void centerPosition(int pos, QWidget* skipView = nullptr) override;

    ADVSequenceObjectContext* getActiveSequenceContext() const override {
        return (seqContexts.isEmpty()) ? nullptr : seqContexts.first();
    }
    ADVSequenceObjectContext* getSequenceContext() const {
        return getActiveSequenceContext();
    }

    DNATranslation* getComplementTT() const;

    DNATranslation* getAminoTT() const;

    DNASequenceSelection* getSequenceSelection() const;

    qint64 getSequenceLength() const;

    virtual void addSequenceView(GSequenceLineView* v, QWidget* after = nullptr);

    virtual void removeSequenceView(GSequenceLineView* v, bool deleteView = true);

    U2SequenceObject* getSequenceObject() const;

    void buildPopupMenu(QMenu& m) override;

    bool isWidgetOnlyObject(GObject* o) const override;

    void addADVSequenceWidgetAction(ADVSequenceWidgetAction* action) override;

    void addADVSequenceWidgetActionToViewsToolbar(ADVSequenceWidgetAction* action);

    void setViewCollapsed(bool v);
    bool isViewCollapsed() const;
    void updateViewButtonState();

    void setPanViewCollapsed(bool collapsed);
    bool isPanViewCollapsed() const;

    void setDetViewCollapsed(bool collapsed);
    bool isDetViewCollapsed() const;

    void setOverviewCollapsed(bool collapsed);
    bool isOverviewCollapsed() const;

    void updateState(const QVariantMap& m) override;
    virtual void updateSelectionActions();
    QVector<U2Region> getSelectedAnnotationRegions(int max);

    void saveState(QVariantMap& m) override;

    void closeView();

    U2Region getVisibleRange() const override;

    void setVisibleRange(const U2Region& r) override;

    int getNumBasesVisible() const override;

    void setNumBasesVisible(qint64 n) override;

    QAction* getSelectRangeAction() const {
        return selectRangeAction1;
    }

    void onSequenceObjectRenamed(const QString& oldName) override;

signals:
    void si_titleClicked(ADVSequenceWidget*);

protected slots:
    void sl_onViewDestroyed(QObject*);

    void sl_toggleAllSubViews();
    void sl_togglePanView(bool checked);
    void sl_toggleDetView(bool checked);
    void sl_toggleOverview(bool checked);
    void sl_onSelectRange();
    void sl_onSelectInRange();
    void sl_onSelectOutRange();
    void sl_zoomToRange();
    void sl_onLocalCenteringRequest(qint64 pos);
    void sl_createCustomRuler();
    void sl_removeCustomRuler();

    void sl_onAnnotationSelectionChanged(AnnotationSelection* thiz, const QList<Annotation*>& added, const QList<Annotation*>& removed);

    // QT 4.5.0 bug workaround
public slots:
    void sl_closeView();

signals:
    void si_updateGraphView(const QStringList&, const QVariantMap&);
private slots:
    void sl_saveScreenshot();

protected:
    void init();
    bool eventFilter(QObject* o, QEvent* e) override;

private:
    virtual void updateMinMaxHeight();

    QToolButton* addButtonWithActionToToolbar(QAction* buttonAction, QToolBar* toolBar, int position = -1) const;
    void addRulersMenu(QMenu& m);
    void addSelectMenu(QMenu& m);

    /** Used by several other functions to set new selected region */
    void setSelectedRegion(const U2Region& region);

    virtual GSequenceLineView* findSequenceViewByPos(const QPoint& globalPos) const;

    virtual void addZoomMenu(const QPoint& globalPos, QMenu* m);

    DetView* detView;
    PanView* panView;
    Overview* overview;
    QList<GSequenceLineView*> lineViews;
    QVBoxLayout* linesLayout;
    QSplitter* linesSplitter;
    ADVSingleSequenceHeaderWidget* headerWidget;

    QAction* toggleViewAction;
    QAction* togglePanViewAction;
    QAction* toggleDetViewAction;
    QAction* toggleOverviewAction;
    QAction* selectRangeAction1;
    QAction* selectRangeAction2;
    QAction* selectInAnnotationRangeAction;
    QAction* selectOutAnnotationRangeAction;
    QAction* zoomToRangeAction;
    QAction* createNewRulerAction;
    QAction* shotScreenAction;
    QAction* closeViewAction;

    GAutoDeleteList<QAction> rulerActions;
    QList<QString>* buttonTabOrederedNames;

    PanView::ZoomUseObject zoomUseObject;

    static const QString SEQUENCE_SETTINGS;
    static const QString DET_VIEW_COLLAPSED;
    static const QString ZOOM_VIEW_COLLAPSED;
    static const QString OVERVIEW_COLLAPSED;

    friend class ADVSingleSequenceHeaderWidget;
};

class U2VIEW_EXPORT ADVSingleSequenceHeaderWidget : public QWidget {
    Q_OBJECT
public:
    ADVSingleSequenceHeaderWidget(ADVSingleSequenceWidget* p);

    QToolBar* getStandardToolBar() const {
        return standardToolBar;
    }
    QToolBar* getViewsToolBar() const {
        return viewsToolBar;
    }

    void setTitle(const QString& title) {
        nameLabel->setText(title);
    }
    void updateTitle();

protected:
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

protected slots:
    /** Toggles header active/inactive visual state if the old or the new widget is related to the header. */
    void sl_onActiveSequenceWidgetChanged(ADVSequenceWidget* oldActiveWidget, ADVSequenceWidget* newActiveWidget);
    void sl_actionTriggered(QAction* a);

private:
    void updateActiveState();
    QString getShortAlphabetName(const DNAAlphabet* al);

    ADVSingleSequenceWidget* ctx;
    QToolBar* standardToolBar;
    QToolBar* viewsToolBar;
    QLabel* pixLabel;
    QLabel* nameLabel;
};

}  // namespace U2
