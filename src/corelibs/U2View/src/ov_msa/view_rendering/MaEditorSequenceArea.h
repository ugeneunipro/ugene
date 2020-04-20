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

#ifndef _U2_MA_EDITOR_SEQUENCE_AREA_
#define _U2_MA_EDITOR_SEQUENCE_AREA_

#include <QColor>
#include <QTimer>
#include <QPainter>
#include <QWidget>

#include <U2Core/U2Region.h>
#include <U2Core/MultipleAlignment.h>

#include <U2Gui/GScrollBar.h>
#include <U2Gui/SelectionModificationHelper.h>

#include "MaEditorSelection.h"
#include "../MsaEditorUserModStepController.h"

class QRubberBand;

namespace U2 {

#define SETTINGS_ROOT QString("msaeditor/")
#define SETTINGS_COLOR_NUCL     "color_nucl"
#define SETTINGS_COLOR_AMINO    "color_amino"
#define SETTINGS_COLOR_RAW      "color_raw"
#define SETTINGS_HIGHLIGHT_NUCL     "highlight_nucl"
#define SETTINGS_HIGHLIGHT_AMINO    "highlight_amino"
#define SETTINGS_HIGHLIGHT_RAW      "highlight_raw"
#define SETTINGS_COPY_FORMATTED "copyformatted"

class GScrollBar;
class MaEditor;
class MaEditorWgt;
class SequenceAreaRenderer;

class MaModificationInfo;
class MsaColorScheme;
class MsaColorSchemeFactory;
class MsaHighlightingScheme;
class MsaHighlightingSchemeFactory;

class U2VIEW_EXPORT MaEditorSequenceArea : public QWidget {
    Q_OBJECT
    friend class SequenceAreaRenderer;

public:
    MaEditorSequenceArea(MaEditorWgt* ui, GScrollBar* hb, GScrollBar* vb);
    virtual ~MaEditorSequenceArea();

    MaEditor *getEditor() const;

    QSize getCanvasSize(const QList<int> &seqIdx, const U2Region &region) const;

    int getFirstVisibleBase() const;
    int getLastVisibleBase(bool countClipped) const;
    int getNumVisibleBases() const;

    /*
     * Returns count of sequences that are drawn on the widget by taking into account
     * collapsed rows.
     */
    int getViewRowCount() const;

    int getRowIndex(const int num) const;

    bool isAlignmentEmpty() const;

    bool isPosInRange(int position) const;
    bool isSeqInRange(int rowNumber) const;
    bool isInRange(const QPoint &point) const;
    QPoint boundWithVisibleRange(const QPoint &point) const;

    bool isVisible(const QPoint& p, bool countClipped) const;
    bool isPositionVisible(int pos, bool countClipped) const;
    bool isRowVisible(int rowNumber, bool countClipped) const;

    const MaEditorSelection &getSelection() const;

    virtual void setSelection(const MaEditorSelection& newSelection);

    virtual void moveSelection(int dx, int dy, bool allowSelectionResize = false);

    virtual void adjustReferenceLength(U2OpStatus& os) { Q_UNUSED(os); }

    /** Returns list of selected MA row indexes. */
    QList<int> getSelectedMaRowIndexes() const;

    /** Returns MA row index of the top-most selected view row or -1 if selection is empty. */
    int getTopSelectedMaRow() const;

    QString getCopyFormattedAlgorithmId() const;

    virtual void deleteCurrentSelection();

    /**
     * Shifts currently selected region to @shift.
     * If @shift > 0, the region is moved to the right and "true" is returned.
     * If @shift <= 0, the region is moved to the left only for the available number
     * of columns (i.e. the columns with gaps). The returned value specifies
     * whether the region was actually moved in this case.
     */
    bool shiftSelectedRegion(int shift);

    void centerPos(const QPoint &point);
    void centerPos(int pos);

    QFont getFont() const;

    void onVisibleRangeChanged();

    bool isAlignmentLocked() const;

    void drawVisibleContent(QPainter &painter);

    bool drawContent(QPainter& painter, const U2Region& columns, const QList<int>& maRows, int xStart, int yStart);

    MsaColorScheme *getCurrentColorScheme() const;
    MsaHighlightingScheme *getCurrentHighlightingScheme() const;
    bool getUseDotsCheckedState() const;

    QAction *getReplaceCharacterAction() const;

public slots:
    void sl_changeColorSchemeOutside(const QString &id);
    void sl_delCurrentSelection();
    void sl_cancelSelection();

protected slots:
    void sl_changeCopyFormat(const QString& alg);
    void sl_changeColorScheme();
    void sl_fillCurrentSelectionWithGaps();

    void sl_alignmentChanged(const MultipleAlignment &ma, const MaModificationInfo &modInfo);

    void sl_completeUpdate();
    void sl_completeRedraw();

    virtual void sl_updateActions() = 0;

    void sl_triggerUseDots();
    void sl_useDots();

    void sl_registerCustomColorSchemes();
    void sl_colorSchemeFactoryUpdated();
    void sl_setDefaultColorScheme();
    void sl_changeHighlightScheme();

    void sl_replaceSelectedCharacter();
    void sl_changeSelectionColor();
    virtual void sl_modelChanged();

private slots:
    void sl_hScrollBarActionPerformed();

private:
    void setBorderCursor(const QPoint& p);
    void moveBorder(const QPoint& p);

    int shiftRegion(int shift);
    QList<U2MsaGap> findRemovableGapColumns(int& shift);
    QList<U2MsaGap> findCommonGapColumns(int& numOfColumns);
    U2MsaGap addTrailingGapColumns(int count);
    QList<U2MsaGap> findRestorableGapColumns(const int shift);

    /**
     * Restores view selection using cached MA selection.
     * If the original selection can't be restored moves the selection to the top-left corner of the original.
     */
    void restoreViewSelectionFromMaSelection();

signals:
    void si_selectionChanged(const MaEditorSelection& current, const MaEditorSelection& prev);
    void si_selectionChanged(const QStringList& selectedRows);
    void si_highlightingChanged();
    void si_visibleRangeChanged(QStringList visibleSequences, int reqHeight);
    void si_visibleRangeChanged();
    void si_startMaChanging();
    void si_stopMaChanging(bool msaUpdated);
    void si_copyFormattedChanging(bool enabled);
    void si_collapsingModeChanged();

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    virtual void initRenderer() = 0;
    virtual void drawBackground(QPainter& p);

    /**
     * Inserts a region consisting of gaps only before the selection. The inserted region width
     * is specified by @countOfGaps parameter if 0 < @countOfGaps, its height is equal to the
     * current selection's height.
     *
     * If there is no selection in MSA then the method does nothing.
     *
     * If -1 == @countOfGaps then the inserting region width is equal to
     * the selection's width. If 1 > @countOfGaps and -1 != @countOfGaps then nothing happens.
     */
    void insertGapsBeforeSelection( int countOfGaps = -1 );

    /**
     * Reverse operation for @insertGapsBeforeSelection( ),
     * removes the region preceding the selection if it consists of gaps only.
     *
     * If there is no selection in MSA then the method does nothing.
     *
     * @countOfGaps specifies maximum width of the removed region.
     * If -1 == @countOfGaps then count of removed gap columns is equal to
     * the selection width. If 1 > @countOfGaps and -1 != @countOfGaps then nothing happens.
     */
    void removeGapsPrecedingSelection( int countOfGaps = -1 );

    /*
     * Interrupts the tracking of MSA modifications caused by a region shifting,
     * also stops shifting. The method is used to keep consistence of undo/redo stack.
     */
    void cancelShiftTracking( );

    void drawAll();

    void updateColorAndHighlightSchemes();

    void initColorSchemes(MsaColorSchemeFactory* defaultColorSchemeFactory);

    void registerCommonColorSchemes();

    void initHighlightSchemes(MsaHighlightingSchemeFactory* hsf);

    MsaColorSchemeFactory * getDefaultColorSchemeFactory() const;
    MsaHighlightingSchemeFactory * getDefaultHighlightingSchemeFactory() const;

    virtual void getColorAndHighlightingIds(QString &csid, QString &hsid);
    void applyColorScheme(const QString &id);

    void processCharacterInEditMode(QKeyEvent *e);
    void processCharacterInEditMode(char newCharacter);
    void replaceChar(char newCharacter);
    virtual void insertChar(char ) {}
    void exitFromEditCharacterMode();
    virtual bool isCharacterAcceptable(const QString &text) const;
    virtual const QString &getInacceptableCharacterErrorMessage() const;

    void deleteOldCustomSchemes();

    /*
     * Update collapse model on alignment modification.
     * Note, that we have collapse model regardless if collapsing mode is enabled or not.
     * In the disabled collapsing mode the collapse model is 'flat': 1 view row = 1 MA row.
     */
    virtual void updateCollapseModel(const MaModificationInfo& maModificationInfo);

protected:
    enum MaMode {
        ViewMode,
        ReplaceCharMode,
        InsertCharMode
    };

public:
    MaMode getModInfo();

protected:
    MaEditor*       editor;
    MaEditorWgt*    ui;

    MsaColorScheme*         colorScheme;
    MsaHighlightingScheme*  highlightingScheme;

    GScrollBar*     shBar;
    GScrollBar*     svBar;
    QRubberBand*    rubberBand;
    bool            showRubberBandOnSelection;

    SequenceAreaRenderer*   renderer;

    QPixmap*        cachedView;
    bool            completeRedraw;

    MaMode          maMode;
    QTimer          editModeAnimationTimer;
    QColor          selectionColor;

    bool                editingEnabled;
    bool                shifting;
    bool                selecting;
    Qt::MouseButton     prevPressedButton;

    /* Last mouse press point. Global window coordinates. */
    QPoint              mousePressEventPoint;
    /*
     * Last mouse press point in view rows/columns coordinates.
     * May be out of range if clicked out of the view/rows range.
     */
    QPoint              mousePressViewPos;

    /** Current selection with view rows/column coordinates. */
    MaEditorSelection   selection;

    /** Selected MA row ids within the current view selection. */
    QList<qint64> selectedMaRowIds;

    /** Selected MA row columns within the current view selection. */
    U2Region selectedColumns;

    int                 maVersionBeforeShifting;
    SelectionModificationHelper::MovableSide movableBorder;

    QList<U2MsaGap>     ctrlModeGapModel;
    bool                isCtrlPressed;
    qint64              lengthOnMousePress;


    QAction*            replaceCharacterAction;
    QAction*            fillWithGapsinsSymAction;

public:
    QAction*            useDotsAction;

    QList<QAction*>     colorSchemeMenuActions;
    QList<QAction*>     customColorSchemeMenuActions;
    QList<QAction*>     highlightingSchemeMenuActions;

protected:
    // The member is intended for tracking MSA changes (handling U2UseCommonUserModStep objects)
    // that does not fit into one method, e.g. shifting MSA region with mouse.
    // If the changing action fits within one method it's recommended using
    // the U2UseCommonUserModStep object explicitly.
    MsaEditorUserModStepController changeTracker;

    static const QChar emDash;
};

} // namespace

#endif // _U2_MA_EDITOR_SEQUENCE_AREA_

