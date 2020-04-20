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

#ifndef _U2_DOT_PLOT_SPLITTER_H_
#define _U2_DOT_PLOT_SPLITTER_H_


#include <U2View/ADVSplitWidget.h>

#include <QIcon>

class QMenu;
class QToolButton;

namespace U2 {

class GObjectView;
class DotPlotWidget;
class ADVSequenceObjectContext;

class DotPlotSplitter : public ADVSplitWidget {
    Q_OBJECT

public:
    DotPlotSplitter(AnnotatedDNAView*);

    virtual bool acceptsGObject(GObject* objects) {Q_UNUSED(objects);return false;}
    virtual void updateState(const QVariantMap&){}
    virtual void saveState(QVariantMap&){}

    void addView(DotPlotWidget*);
    void removeView(DotPlotWidget*);
    bool isEmpty() const;
    void buildPopupMenu(QMenu *);

protected:
    virtual bool onCloseEvent();

private:
    QAction *createAction(const QString& iconPath, const QString& toolTip, const char *slot, bool checkable = true);
    QAction *createAction(const QIcon& ic, const QString& toolTip, const char *slot, bool checkable = true);

    QSplitter *splitter;
    QList<DotPlotWidget*> dotPlotList;

    bool locked;
    QAction*    syncLockAction;
    QAction*    filterAction;
    QAction*    zoomInAction;
    QAction*    zoomOutAction;
    QAction*    resetZoomingAction;
    QAction*    zoomToAction;
    QAction*    handAction;
    QAction*    selAction;

    void updateButtonState();
    void checkLockButtonState();

private slots:
    void sl_toggleSyncLock(bool);
    void sl_toggleFilter();
    void sl_toggleAspectRatio(bool);
    void sl_toggleSel();
    void sl_toggleHand();
    void sl_toggleZoomIn();
    void sl_toggleZoomOut();
    void sl_toggleZoomReset();

    void sl_dotPlotChanged(ADVSequenceObjectContext*, ADVSequenceObjectContext*, float, float, QPointF);
    void sl_dotPlotSelecting();
};

} // namespace

#endif // _U2_DOT_PLOT_SPLITTER_H_
