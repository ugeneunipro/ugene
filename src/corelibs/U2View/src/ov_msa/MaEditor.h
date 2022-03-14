/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MA_EDITOR_H_
#define _U2_MA_EDITOR_H_

#include <U2Core/U2SafePoints.h>
#include <U2Gui/ObjectViewModel.h>

namespace U2 {

#define MSAE_SETTINGS_ROOT QString("msaeditor/")
#define MCAE_SETTINGS_ROOT QString("mcaeditor/")

#define MSAE_MENU_APPEARANCE "MSAE_MENU_APPEARANCE"
#define MSAE_MENU_NAVIGATION "MSAE_MENU_NAVIGATION"
#define MSAE_MENU_COPY "MSAE_MENU_COPY"
#define MSAE_MENU_EDIT "MSAE_MENU_EDIT"
#define MSAE_MENU_EXPORT "MSAE_MENU_EXPORT"
#define MSAE_MENU_SORT "MSAE_MENU_SORT"
#define MSAE_MENU_ALIGN "MSAE_MENU_ALIGN"
#define MSAE_MENU_TREES "MSAE_MENU_TREES"
#define MSAE_MENU_STATISTICS "MSAE_MENU_STATISTICS"
#define MSAE_MENU_ADVANCED "MSAE_MENU_ADVANCED"
#define MSAE_MENU_LOAD "MSAE_MENU_LOAD_SEQ"

#define MOBJECT_MIN_COLUMN_WIDTH 1

#define MOBJECT_SETTINGS_COLOR_NUCL "color_nucl"
#define MOBJECT_SETTINGS_COLOR_AMINO "color_amino"
#define MOBJECT_SETTINGS_FONT_FAMILY "font_family"
#define MOBJECT_SETTINGS_FONT_SIZE "font_size"
#define MOBJECT_SETTINGS_FONT_ITALIC "font_italic"
#define MOBJECT_SETTINGS_FONT_BOLD "font_bold"
#define MOBJECT_SETTINGS_ZOOM_FACTOR "zoom_factor"

#define MOBJECT_DEFAULT_FONT_FAMILY "Verdana"
#define MOBJECT_DEFAULT_FONT_SIZE 10
#define MOBJECT_DEFAULT_ZOOM_FACTOR 1.0

class MaCollapseModel;
class MaEditorWgt;
class MaEditorMultilineWgt;
class MaEditorOverviewArea;
class MultipleAlignmentObject;
class MaEditorSelection;
class MaEditorSelectionController;
class MaEditorWgt;
class MaModificationInfo;
class MaUndoRedoFramework;
class MultipleAlignment;
class MultipleAlignmentObject;

class SNPSettings {
public:
    SNPSettings();
    QPoint clickPoint;
    qint64 seqId;
    QVariantMap highlightSchemeSettings;
};

/** Set of row ordering modes supported by MaEditor. */
enum class MaEditorRowOrderMode {
    /**
     * The default order of sequences in the MA: same as in the original source file.
     * Note: there are no collapsible groups support in this mode.
     */
    Original,

    /**
     * Rows are ordered by sequence content similarity.
     * Sequences with the equal content are joined into collapsible groups.
     * This mode is managed/supported internally by MA editor: MA editor automatically rebuilds groups when MA content changes.
     * The order is not saved to the file and is kept in the view memory only.
     * This mode allows to re-order read-only MA objects.
     */
    Sequence,

    /**
     * Sequences are ordered by some external manager (e.g. tree-view) and can be re-ordered by user (e.g. drag & drop).
     * The order is not saved to the file and is kept in the view memory only.
     * This mode allows to re-order read-only MA objects.
     */
    Free
};

class U2VIEW_EXPORT MaEditor : public GObjectView {
    Q_OBJECT
    friend class OpenSavedMaEditorTask;
    friend class MaEditorState;

public:
    enum ResizeMode {
        ResizeMode_FontAndContent,
        ResizeMode_OnlyContent
    };
    static const float zoomMult;  // SANGER_TODO: should be dependable on the view

public:
    MaEditor(GObjectViewFactoryId factoryId, const QString& viewName, MultipleAlignmentObject* obj);

    virtual QVariantMap saveState();

    virtual Task* updateViewTask(const QString& stateName, const QVariantMap& stateData);

    virtual QString getSettingsRoot() const = 0;

    virtual MultipleAlignmentObject* getMaObject() const {
        return maObject;
    }

    QList<qint64> getMaRowIds() const;

    virtual QWidget *getUI() const {
        return ui;
    }

    virtual MaEditorWgt *getMaEditorWgt(uint index = 0) const {
        SAFE_POINT(false, "The function getMaEditorWgt(index) must be overrided", nullptr);
        Q_UNUSED(index);
        return nullptr;
    };

    virtual MaEditorMultilineWgt *getMaEditorMultilineWgt() const {
        SAFE_POINT(false, "The function getMaEditorMultilineWgt(index) must be overrided", nullptr);
        return nullptr;
    };

    virtual OptionsPanel* getOptionsPanel() {
        return optionsPanel;
    }

    const QFont& getFont() const {
        return font;
    }

    ResizeMode getResizeMode() const {
        return resizeMode;
    }

    int getAlignmentLen() const;

    int getNumSequences() const;

    bool isAlignmentEmpty() const;

    /* Returns current selection controller. */
    virtual MaEditorSelectionController* getSelectionController() const = 0;

    /* Returns current selection. */
    const MaEditorSelection& getSelection() const;

    virtual int getRowContentIndent(int rowId) const;
    int getSequenceRowHeight() const;  // SANGER_TODO: order the methods

    int getColumnWidth() const;

    QVariantMap getHighlightingSettings(const QString& highlightingFactoryId) const;

    void saveHighlightingSettings(const QString& highlightingFactoryId, const QVariantMap& settingsMap = QVariantMap());

    qint64 getReferenceRowId() const {
        return snp.seqId;
    }

    virtual QString getReferenceRowName() const = 0;

    virtual char getReferenceCharAt(int pos) const = 0;

    void setReference(qint64 sequenceId);

    void updateReference();

    /** Sets row ordering mode to 'Original' and resets collapse model to the original row order. */
    void resetCollapseModel();

    void exportHighlighted() {
        sl_exportHighlighted();
    }

    /** Returns current cursor position. */
    const QPoint& getCursorPosition() const;

    /** Sets new cursor position. Emits si_cursorPositionChanged() signal. */
    void setCursorPosition(const QPoint& cursorPosition);

    /** Sets selection to the given view rows. */
    virtual void selectRows(int firstViewRowIndex, int numberOfRows);

    /** Returns a unified bounding rect for a single sequence character for the given font. */
    QRect getUnifiedSequenceFontCharRect(const QFont& sequenceFont) const;

    /** Returns active row ordering mode. See docs for 'MaEditorRowOrderMode' enum for details. */
    MaEditorRowOrderMode getRowOrderMode() const;

    /**
     * Updates currently active row order mode.
     * This is a trivial method with no other actions/callbacks.
     */
    virtual void setRowOrderMode(MaEditorRowOrderMode mode);

    /** Returns collapse model instance. The returned value is never null. */
    MaCollapseModel* getCollapseModel() const;

    /** Returns undo-redo framework. The returned value is never null. */
    MaUndoRedoFramework *getUndoRedoFramework() const;

    virtual void initActionsAndSignals() {};
    virtual void initChildrenActionsAndSignals() {};

signals:
    void si_fontChanged(const QFont& f);
    void si_zoomOperationPerformed(bool resizeModeChanged);
    void si_referenceSeqChanged(qint64 referenceId);
    void si_sizeChanged(int newHeight, bool isMinimumSize, bool isMaximumSize);
    void si_completeUpdate();
    void si_updateActions();
    void si_cursorPositionChanged(const QPoint& cursorPosition);
    void si_showOffsets(bool);

protected slots:
    virtual void sl_onContextMenuRequested(const QPoint& pos) = 0;

    void sl_zoomIn();
    void sl_zoomOut();
    void sl_zoomToSelection();
    void sl_resetZoom();

    void sl_saveAlignment();
    void sl_saveAlignmentAs();
    void sl_changeFont();

    void sl_lockedStateChanged();

    void sl_exportHighlighted();
    void sl_onClearActionTriggered();

    /** The slot is called each time alignment is changed. By default calls 'updateActions'. */
    virtual void sl_onAlignmentChanged(const MultipleAlignment& ma, const MaModificationInfo& modInfo);

    /** The slot is called each time selection is changed. By default calls 'updateActions'. */
    virtual void sl_selectionChanged(const MaEditorSelection& ma, const MaEditorSelection& modInfo);

    /** Callback for the 'gotoSelectedReadAction' action. See docs for 'gotoSelectedReadAction'. */
    void sl_gotoSelectedRead();

    virtual void sl_multilineViewAction() {
        SAFE_POINT(false, "The function sl_multilineViewAction() must be overrided", );
        return;
    };

private slots:
    void sl_resetColumnWidthCache();

protected:
    virtual QWidget *createWidget() = 0;
    virtual void initActions();
    virtual void initZoom();
    virtual void initFont();
    void updateResizeMode();

    virtual void addCopyPasteMenu(QMenu *m, uint uiIndex);
    virtual void addEditMenu(QMenu *m) = 0;
    virtual void addExportMenu(QMenu *m);
    void addLoadMenu(QMenu *m);

    void setFont(const QFont& f);

    /** Updates font metrics like fontPixelToPointSize, minimum-font-size. Called on every font update. */
    void updateFontMetrics();

    void setFirstVisiblePosSeq(int firstPos, int firstSeq);
    void setZoomFactor(double newZoomFactor);

    virtual void updateActions();

    MultipleAlignmentObject *maObject;
    QWidget *ui = nullptr;

    QFont font;
    ResizeMode resizeMode;

    /** Minimum font size to render a sequence. Then zoomed-out below this value no sequence text is shown. */
    int minimumFontPointSize;

    /**
     * Maximum font size to render a sequence. Then zoomed-in above this value sequence text stops to grow.
     * Note: since MA editor does not allow a cell size to grow beyond the size defined by the maximumFontPointSize
     *  this value indirectly defines the maximum zoom level.
     */
    int maximumFontPointSize;

    SNPSettings snp;
    double zoomFactor;
    double fontPixelToPointSize;
    mutable int cachedColumnWidth;

    /** Current cursor position: 'x' is offset in alignment (0...len) and 'y' is a sequence index in the aligment. */
    QPoint cursorPosition;

    /** Active row ordering mode in the view. */
    MaEditorRowOrderMode rowOrderMode;

    /** Collapse model instance. Created in the constructor and is never changed. */
    MaCollapseModel* const collapseModel;

    /** Undo-redo support. */
    MaUndoRedoFramework* undoRedoFramework = nullptr;

public:
    QAction* saveAlignmentAction = nullptr;
    QAction* saveAlignmentAsAction = nullptr;
    QAction* zoomInAction = nullptr;
    QAction* zoomOutAction = nullptr;
    QAction* zoomToSelectionAction = nullptr;
    QAction* showOverviewAction = nullptr;
    QAction* changeFontAction = nullptr;
    QAction* resetZoomAction = nullptr;
    QAction* exportHighlightedAction = nullptr;
    QAction* multilineViewAction = nullptr;

    /** Clears selection in normal mode or exits from editing mode in the edit mode. */
    QAction* clearSelectionAction = nullptr;

    QAction* copyConsensusAction = nullptr;
    QAction* copyConsensusWithGapsAction = nullptr;

    /**
     * When activated MA editor moves start of the currently selected read into the view.
     * If the selection contains multiple reads - selects the first one.
     *
     * The start of the direct reads (reads located on 3'-5' strand) is the same with their visual start on the screen -
     *  direct reads are read from the left to the right direction.
     * The start of the complement reads is they visual end position: such read sequences are read from the right to the left.
     * This way the action works the same as annotation selection in the sequence view.
     *
     * When the action is triggered for the already selected read it tries to center the opposite side of the read: 'start' -> 'end', 'end' -> 'start'.
     */
    QAction* gotoSelectedReadAction = nullptr;

    /** Undo action in MA Editor. Never null after the initialization. */
    QAction* undoAction = nullptr;

    /** Redo action. Never null after the initialization.*/
    QAction* redoAction = nullptr;
};

}  // namespace U2

#endif  // _U2_MA_EDITOR_H_
