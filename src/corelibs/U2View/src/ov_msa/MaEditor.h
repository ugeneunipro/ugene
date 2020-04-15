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

#ifndef _U2_MA_EDITOR_H_
#define _U2_MA_EDITOR_H_

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

#define MSAE_SETTINGS_ROOT QString("msaeditor/")
#define MCAE_SETTINGS_ROOT QString("mcaeditor/")

#define MSAE_MENU_APPEARANCE    "MSAE_MENU_APPEARANCE"
#define MSAE_MENU_NAVIGATION    "MSAE_MENU_NAVIGATION"
#define MSAE_MENU_COPY          "MSAE_MENU_COPY"
#define MSAE_MENU_EDIT          "MSAE_MENU_EDIT"
#define MSAE_MENU_EXPORT        "MSAE_MENU_EXPORT"
#define MSAE_MENU_VIEW          "MSAE_MENU_VIEW"
#define MSAE_MENU_ALIGN         "MSAE_MENU_ALIGN"
#define MSAE_MENU_TREES         "MSAE_MENU_TREES"
#define MSAE_MENU_STATISTICS    "MSAE_MENU_STATISTICS"
#define MSAE_MENU_ADVANCED      "MSAE_MENU_ADVANCED"
#define MSAE_MENU_LOAD          "MSAE_MENU_LOAD_SEQ"

#define MOBJECT_MIN_FONT_SIZE 8
#define MOBJECT_MAX_FONT_SIZE 18
#define MOBJECT_MIN_COLUMN_WIDTH 1

#define MOBJECT_SETTINGS_COLOR_NUCL     "color_nucl"
#define MOBJECT_SETTINGS_COLOR_AMINO    "color_amino"
#define MOBJECT_SETTINGS_FONT_FAMILY    "font_family"
#define MOBJECT_SETTINGS_FONT_SIZE      "font_size"
#define MOBJECT_SETTINGS_FONT_ITALIC    "font_italic"
#define MOBJECT_SETTINGS_FONT_BOLD      "font_bold"
#define MOBJECT_SETTINGS_ZOOM_FACTOR    "zoom_factor"

#define MOBJECT_DEFAULT_FONT_FAMILY "Verdana"
#define MOBJECT_DEFAULT_FONT_SIZE 10
#define MOBJECT_DEFAULT_ZOOM_FACTOR 1.0

class MaEditorWgt;
class MultipleAlignmentObject;
class MaEditorSelection;

class SNPSettings {
public:
    SNPSettings();
    QPoint clickPoint;
    qint64 seqId;
    QVariantMap highlightSchemeSettings;
};

class U2VIEW_EXPORT MaEditor : public GObjectView {
    Q_OBJECT
    friend class OpenSavedMaEditorTask;
    friend class MaEditorState;
public:
    enum ResizeMode {
        ResizeMode_FontAndContent, ResizeMode_OnlyContent
    };
    static const float zoomMult; // SANGER_TODO: should be dependable on the view

public:
    MaEditor(GObjectViewFactoryId factoryId, const QString& viewName, GObject* obj);

    virtual QVariantMap saveState();

    virtual Task* updateViewTask(const QString& stateName, const QVariantMap& stateData);

    virtual QString getSettingsRoot() const = 0;

    virtual MultipleAlignmentObject* getMaObject() const { return maObject; }

    QList<qint64> getMaRowIds() const;

    virtual MaEditorWgt* getUI() const { return ui; }

    virtual OptionsPanel* getOptionsPanel() { return optionsPanel; }

    const QFont& getFont() const { return font; }

    ResizeMode getResizeMode() const { return resizeMode; }

    int getAlignmentLen() const;

    int getNumSequences() const;

    bool isAlignmentEmpty() const;

    /* Returns current selection. */
    const MaEditorSelection& getSelection() const;

    /*
     * Shortcut for getSelection().toRect().
     * Note: this method is useful because we have no "MaEditorSelection" type available outside of the U2View today.
     */
    QRect getSelectionRect() const;

    virtual int getRowContentIndent(int rowId) const;
    int getSequenceRowHeight() const; // SANGER_TODO: order the methods

    int getColumnWidth() const;

    QVariantMap getHighlightingSettings(const QString &highlightingFactoryId) const;

    void saveHighlightingSettings(const QString &highlightingFactoryId, const QVariantMap &settingsMap = QVariantMap());

    qint64 getReferenceRowId() const { return snp.seqId; }

    virtual QString getReferenceRowName() const = 0;

    virtual char getReferenceCharAt(int pos) const = 0;

    void setReference(qint64 sequenceId);

    void updateReference();

    void resetCollapsibleModel(); // SANGER_TODO: collapsible shouldn't be here

    void exportHighlighted(){ sl_exportHighlighted(); }

    /** Returns current cursor position. */
    const QPoint& getCursorPosition() const;

    /** Sets new cursor position. Emits si_cursorPositionChanged() signal. */
    void setCursorPosition(const QPoint& cursorPosition);

    QAction *getClearSelectionAction() const;

signals:
    void si_fontChanged(const QFont& f);
    void si_zoomOperationPerformed(bool resizeModeChanged);
    void si_referenceSeqChanged(qint64 referenceId);
    void si_sizeChanged(int newHeight, bool isMinimumSize, bool isMaximumSize);
    void si_completeUpdate();
    void si_updateActions();
    void si_cursorPositionChanged(const QPoint& cursorPosition);
    void si_clearSelection();

protected slots:
    virtual void sl_onContextMenuRequested(const QPoint & pos) = 0;

    void sl_zoomIn();
    void sl_zoomOut();
    void sl_zoomToSelection();
    void sl_resetZoom();

    void sl_saveAlignment();
    void sl_saveAlignmentAs();
    void sl_changeFont();

    void sl_lockedStateChanged();

    void sl_exportHighlighted();

private slots:
    void sl_resetColumnWidthCache();

protected:
    virtual QWidget* createWidget() = 0;
    virtual void initActions();
    virtual void initZoom();
    virtual void initFont();
    void updateResizeMode();

    void addCopyMenu(QMenu* m);
    virtual void addEditMenu(QMenu* m);
    virtual void addExportMenu(QMenu* m);
    void addSortMenu(QMenu* m);
    void addLoadMenu(QMenu* m);
    void addAlignMenu(QMenu* m); // SANGER_TODO: should the align menu exist in MCA?

    void setFont(const QFont& f);
    void calcFontPixelToPointSizeCoef();

    void setFirstVisiblePosSeq(int firstPos, int firstSeq);
    void setZoomFactor(double newZoomFactor);

    virtual void updateActions();

    MultipleAlignmentObject*    maObject;
    MaEditorWgt*                ui;

    QFont       font;
    ResizeMode  resizeMode;
    SNPSettings snp;
    double      zoomFactor;
    double      fontPixelToPointSize;
    mutable int cachedColumnWidth;

    /** Current cursor position: 'x' is offset in alignment (0...len) and 'y' is a sequence index in the aligment. */
    QPoint cursorPosition;

    QAction* saveAlignmentAction;
    QAction* saveAlignmentAsAction;
    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* zoomToSelectionAction;
    QAction* showOverviewAction;
    QAction* changeFontAction;
    QAction* resetZoomAction;
    QAction* saveScreenshotAction;
    QAction* exportHighlightedAction;
    QAction* clearSelectionAction;

};

} // namespace

#endif // _U2_MA_EDITOR_H_
