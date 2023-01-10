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

#ifndef _U2_DET_VIEW_H_
#define _U2_DET_VIEW_H_

#include <QAction>
#include <QFont>

#include <U2Core/U2Location.h>

#include <U2View/SequenceObjectContext.h>

#include "GSequenceLineViewAnnotated.h"

class QActionGroup;

namespace U2 {

class Annotation;
class DNATranslation;
class DetViewRenderArea;
class DetViewRenderer;
class DetViewSequenceEditor;

class U2VIEW_EXPORT DetView : public GSequenceLineViewAnnotated {
    Q_OBJECT
    friend class DetViewSequenceEditor;

public:
    DetView(QWidget* p, SequenceObjectContext* ctx);
    ~DetView();

    DetViewSequenceEditor* getEditor() const {
        return editor;
    }

    DetViewRenderArea* getDetViewRenderArea() const;

    bool hasTranslations() const;
    bool hasComplementaryStrand() const;
    bool isWrapMode() const;
    bool isEditMode() const;

    void setStartPos(qint64 pos) override;
    void setCenterPos(qint64 pos) override;

    DNATranslation* getComplementTT() const;
    DNATranslation* getAminoTT() const;
    int getSymbolsPerLine() const;

    void setWrapSequence(bool v);
    void setShowComplement(bool t);
    void setShowTranslation(bool t);

    void setDisabledDetViewActions(bool t);

    int getVerticalScrollBarPosition();
    int getShift() const;
    void setSelectedTranslations();

    void ensurePositionVisible(qint64 pos);

protected slots:
    void sl_sequenceChanged() override;
    void sl_onDNASelectionChanged(LRegionsSelection* thiz, const QVector<U2Region>& added, const QVector<U2Region>& removed) override;
    void sl_onAminoTTChanged();
    void sl_translationRowsChanged();
    void sl_showComplementToggle(bool v);
    void sl_showTranslationToggle(bool v);
    void sl_wrapSequenceToggle(bool v);
    void sl_verticalScrollBarMoved(int position);
    void sl_doNotTranslate();
    void sl_translateAnnotationsOrSelection();
    void sl_setUpFramesManually();
    void sl_showAllFrames();

protected:
    virtual void pack() override;

    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;

    void mouseMoveEvent(QMouseEvent* me) override;
    void mouseReleaseEvent(QMouseEvent* me) override;
    void wheelEvent(QWheelEvent* we) override;
    void resizeEvent(QResizeEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

    void updateVisibleRange();
    void updateActions();
    void updateSize();
    void updateVerticalScrollBar();
    void updateVerticalScrollBarPosition();

    QAction* showComplementAction;
    QAction* showTranslationAction;
    QAction* wrapSequenceAction;
    QAction* doNotTranslateAction;
    QAction* translateAnnotationsOrSelectionAction;
    QAction* setUpFramesManuallyAction;
    QAction* showAllFramesAction;

    DetViewSequenceEditor* editor;

    GScrollBar* verticalScrollBar;

    int numShiftsInOneLine;
    int currentShiftsCounter;

private:
    void setupTranslationsMenu();
    void setupGeneticCodeMenu();
    QPoint getRenderAreaPointAfterAutoScroll(const QPoint& pos);
    void moveBorder(const QPoint& p);

    /** Returns whole rendering area height region for the normal mode & limited Y range for the wrap mode. */
    U2Region getCapturingRenderAreaYRegionForPos(qint64 pos) const override;

    void setDefaultState();

    void updateTranslationRowsVisibilityBySelectionState();
    void updateSelectedTranslations(const SequenceObjectContext::TranslationState& state);

    static const QString SEQUENCE_SETTINGS;
    static const QString SEQUENCE_WRAPPED;
    static const QString COMPLEMENTARY_STRAND_SHOWN;
    static const QString TRANSLATION_STATE;
};

class U2VIEW_EXPORT DetViewRenderArea : public GSequenceLineViewGridAnnotationRenderArea {
    Q_OBJECT
public:
    DetViewRenderArea(DetView* d);
    ~DetViewRenderArea();

    DetViewRenderer* getRenderer() {
        return renderer;
    }

    /** Returns all y regions covered by the given location of the annotation. */
    QList<U2Region> getAnnotationYRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings) const override;

    double getCurrentScale() const override;

    void setWrapSequence(bool v);

    qint64 coordToPos(const QPoint& coord) const override;

    int posToCoord(qint64 pos, bool useVirtualSpace = false) const override;

    DetView* getDetView() const;

    /** Returns number of bases in a single line. */
    int getSymbolsPerLine() const;

    /** Returns number of visible lines in the view. */
    qint64 getLinesCount() const;

    /** Returns number of bases in all visible lines.  */
    int getVisibleSymbolsCount() const;

    /** Returns index of the direct strand line. */
    int getDirectLine() const;

    /** Number of shifts (text rows) in a single line. */
    int getShiftsCount() const;

    /** Number of pixels in one shift. */
    int getShiftHeight() const;

    void updateSize();

    bool isOnTranslationsLine(const QPoint& p) const;

protected:
    void drawAll(QPaintDevice* pd) override;

private:
    DetViewRenderer* renderer;
};

}  // namespace U2

#endif
