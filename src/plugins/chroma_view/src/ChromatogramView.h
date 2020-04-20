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

#ifndef _U2_CHROMATOGRAM_VIEW_H_
#define _U2_CHROMATOGRAM_VIEW_H_

#include <U2Core/GObject.h>
#include <U2Core/DNAChromatogram.h>
#include <U2View/GSequenceLineView.h>
#include <U2Gui/ScaleBar.h>

namespace U2 {

class ADVSequenceObjectContext;
class ChromatogramViewRenderArea;
class AnnotatedDNAView;
class GObjectView;
class Task;


struct ChromatogramViewSettings {
    bool drawTraceA, drawTraceC, drawTraceG, drawTraceT;
    ChromatogramViewSettings()  {
        drawTraceA = true;
        drawTraceC = true;
        drawTraceG = true;
        drawTraceT = true;
    }
};

class ChromatogramView : public GSequenceLineView {
    Q_OBJECT
    friend class ChromatogramViewRenderArea;
public:
    ChromatogramView(QWidget* p, ADVSequenceObjectContext* v, GSequenceLineView* cv, const DNAChromatogram& chroma);

    virtual void buildPopupMenu(QMenu& m);

    virtual bool isWidgetOnlyObject(GObject* o) const;

    U2SequenceObject* getEditedSequence() const {return editDNASeq;}
    const ChromatogramViewSettings&  getSettings() const { return settings; }
    bool showQV() const {return showQVAction->isChecked();}

protected:
    virtual void pack();
    void mousePressEvent(QMouseEvent* me);

private slots:
    void setRenderAreaHeight(int k);
    void sl_onPopupMenuCkicked(QAction* a);
    void sl_addNewSequenceObject();
    void sl_onAddExistingSequenceObject();
    void sl_onSequenceObjectLoaded(Task*);
    void sl_clearEditableSequence();
    void sl_removeChanges();
    void sl_onObjectRemoved(GObjectView*, GObject*);
    void sl_showHideTrace();
    void sl_showAllTraces();

private:
    int getEditSeqIndex(int bcIndex);
    QAction* createToggleTraceAction(const QString& actionName);

    AnnotatedDNAView*           dnaView;
    U2SequenceObject*           editDNASeq;
    QByteArray                  currentBaseCalls;
    QSet<int>                   indexOfChangedChars;
    QList<int>                  gapIndexes;

    ScaleBar*                   scaleBar;
    ChromatogramViewSettings    settings;
    ChromatogramViewRenderArea* ra;
    QMenu*                      mP;
    int                         selIndex;
    QAction*                    addNewSeqAction;
    QAction*                    addExistSeqAction;
    QAction*                    clearEditableSequence;
    QAction*                    removeChanges;
    QAction*                    showQVAction;
    QAction*                    showAllTraces;
    QMenu*                      traceActionMenu;
};




class ChromatogramViewRenderArea : public GSequenceLineViewRenderArea {
    Q_OBJECT
public:
    ChromatogramViewRenderArea(ChromatogramView* p, const DNAChromatogram& chroma);
    ~ChromatogramViewRenderArea();

    int getHeightAreaBC() const {return heightAreaBC;}
    void setAreaHeight(int newH);
    virtual qint64 coordToPos(int x) const;
    virtual int posToCoord(qint64 p, bool useVirtualSpace = false) const;
    QRectF posToRect(int p) const;
    QRectF  selRect;
    bool    hasSel;
    qreal   charWidth;
    qreal   charHeight;
    qreal   addUpIfQVL;


protected:
    virtual void drawAll(QPaintDevice* pd);

private:
    QColor getBaseColor(char base);
    void drawChromatogramTrace(qreal x, qreal y, qreal w, qreal h, QPainter& p,
        const U2Region& visible, const ChromatogramViewSettings& settings);
    void drawOriginalBaseCalls(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba, bool is = true);
    void drawQualityValues(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba);
    void drawChromatogramBaseCallsLines(qreal x, qreal y, qreal w, qreal h, QPainter& p,
        const U2Region& visible, const QByteArray& ba, const ChromatogramViewSettings& settings);

    DNAChromatogram chroma;
    int             chromaMax;
    QPen            linePen;
    QFont           font;
    QFont           fontBold;
    int             heightPD;
    int             heightAreaBC;
    int             areaHeight;
    qreal           kLinearTransformTrace;
    qreal           bLinearTransformTrace;
    qreal           kLinearTransformBaseCallsOfEdited;
    qreal           bLinearTransformBaseCallsOfEdited;
};


} // namespace


#endif
