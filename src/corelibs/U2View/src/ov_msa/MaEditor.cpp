/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MaEditor.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFontDialog>

#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MsaObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ExportDocumentDialogController.h>
#include <U2Gui/ExportObjectUtils.h>
#include <U2Gui/GUIUtils.h>

#include <U2View/McaEditor.h>
#include <U2View/MsaEditorOffsetsView.h>
#include <U2View/MsaEditorOverviewArea.h>
#include <U2View/MsaEditorSequenceArea.h>
#include <U2View/UndoRedoFramework.h>

#include "MaCollapseModel.h"
#include "MaEditorSelection.h"
#include "MaEditorState.h"
#include "MaEditorTasks.h"
#include "MultilineScrollController.h"
#include "ScrollController.h"

namespace U2 {

SNPSettings::SNPSettings()
    : seqId(U2MsaRow::INVALID_ROW_ID) {
}

const float MaEditor::zoomMult = 1.25;

const double MaEditor::FONT_BOX_TO_CELL_BOX_MULTIPLIER = 1.25;

MaEditor::MaEditor(const GObjectViewFactoryId& factoryId, const QString& viewName, MsaObject* obj)
    : GObjectViewController(factoryId, viewName),
      resizeMode(ResizeMode_FontAndContent),
      minimumFontPointSize(6),
      maximumFontPointSize(24),
      zoomFactor(0),
      cachedColumnWidth(0),
      cursorPosition(QPoint(0, 0)),
      rowOrderMode(MaEditorRowOrderMode::Original),
      collapseModel(new MaCollapseModel(this, obj->getRowIds())) {
    GCOUNTER(cvar, factoryId);

    maObject = obj;
    objects.append(maObject);

    onObjectAdded(maObject);

    requiredObjects.append(maObject);

    if (!U2DbiUtils::isDbiReadOnly(maObject->getEntityRef().dbiRef)) {
        U2OpStatus2Log os;
        maObject->setTrackMod(os, TrackOnUpdate);
    }

    undoRedoFramework = new MaUndoRedoFramework(this, obj);
    undoAction = undoRedoFramework->getUndoAction();
    redoAction = undoRedoFramework->getRedoAction();

    // SANGER_TODO: move to separate method
    // do that in createWidget along with initActions?
    saveAlignmentAction = new QAction(QIcon(":core/images/msa_save.png"), tr("Save alignment"), this);
    saveAlignmentAction->setObjectName("Save alignment");
    connect(saveAlignmentAction, SIGNAL(triggered()), SLOT(sl_saveAlignment()));

    saveAlignmentAsAction = new QAction(QIcon(":core/images/msa_save_as.png"), tr("Save alignment as"), this);
    saveAlignmentAsAction->setObjectName("Save alignment as");
    connect(saveAlignmentAsAction, SIGNAL(triggered()), SLOT(sl_saveAlignmentAs()));

    zoomInAction = new QAction(QIcon(":core/images/zoom_in.png"), tr("Zoom In"), this);
    zoomInAction->setObjectName("Zoom In");
    connect(zoomInAction, SIGNAL(triggered()), SLOT(sl_zoomIn()));

    zoomOutAction = new QAction(QIcon(":core/images/zoom_out.png"), tr("Zoom Out"), this);
    zoomOutAction->setObjectName("Zoom Out");
    connect(zoomOutAction, SIGNAL(triggered()), SLOT(sl_zoomOut()));

    zoomToSelectionAction = new QAction(QIcon(":core/images/zoom_reg.png"), tr("Zoom To Selection"), this);
    zoomToSelectionAction->setObjectName("Zoom To Selection");
    connect(zoomToSelectionAction, SIGNAL(triggered()), SLOT(sl_zoomToSelection()));

    resetZoomAction = new QAction(QIcon(":core/images/zoom_whole.png"), tr("Reset Zoom"), this);
    resetZoomAction->setObjectName("Reset Zoom");
    connect(resetZoomAction, SIGNAL(triggered()), SLOT(sl_resetZoom()));

    changeFontAction = new QAction(QIcon(":core/images/font.png"), tr("Change Font"), this);
    changeFontAction->setObjectName("Change Font");
    connect(changeFontAction, SIGNAL(triggered()), SLOT(sl_changeFont()));

    exportHighlightedAction = new QAction(tr("Export highlighted"), this);
    exportHighlightedAction->setObjectName("Export highlighted");
    connect(exportHighlightedAction, SIGNAL(triggered()), this, SLOT(sl_exportHighlighted()));
    exportHighlightedAction->setDisabled(true);

    copyConsensusAction = new QAction(tr("Copy consensus"), this);
    copyConsensusAction->setObjectName("Copy consensus");

    copyConsensusWithGapsAction = new QAction(tr("Copy consensus with gaps"), this);
    copyConsensusWithGapsAction->setObjectName("Copy consensus with gaps");

    gotoSelectedReadAction = new QAction(tr("Go to selected read"), this);
    gotoSelectedReadAction->setObjectName("center-read-start-end-action");
    gotoSelectedReadAction->setEnabled(false);
    connect(gotoSelectedReadAction, &QAction::triggered, this, &MaEditor::sl_gotoSelectedRead);

    multilineViewAction = new QAction(QIcon(":core/images/multiline_view.png"), tr("Wrap mode"), this);
    multilineViewAction->setObjectName("multilineView");
    multilineViewAction->setCheckable(true);
    multilineViewAction->setChecked(false);
    connect(multilineViewAction, SIGNAL(triggered()), SLOT(sl_multilineViewAction()));

    connect(maObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_lockedStateChanged()));
    connect(maObject,
            SIGNAL(si_alignmentChanged(const Msa&, const MaModificationInfo&)),
            SLOT(sl_onAlignmentChanged(const Msa&, const MaModificationInfo&)));
    connect(this, SIGNAL(si_fontChanged(QFont)), SLOT(resetColumnWidthCache()));
}

void MaEditor::sl_onAlignmentChanged(const Msa&, const MaModificationInfo&) {
    updateActions();
}

void MaEditor::sl_selectionChanged(const MaEditorSelection&, const MaEditorSelection&) {
    updateActions();
}

QVariantMap MaEditor::saveState() {
    return MaEditorState::saveState(this);
}

Task* MaEditor::updateViewTask(const QString& stateName, const QVariantMap& stateData) {
    return new UpdateMaEditorTask(this, stateName, stateData);
}

int MaEditor::getAlignmentLen() const {
    return maObject->getLength();
}

int MaEditor::getNumSequences() const {
    return maObject->getRowCount();
}

bool MaEditor::isAlignmentEmpty() const {
    return getAlignmentLen() == 0 || getNumSequences() == 0;
}

const MaEditorSelection& MaEditor::getSelection() const {
    return getSelectionController()->getSelection();
}

int MaEditor::getRowContentIndent(int) const {
    return 0;
}

int MaEditor::getRowHeight() const {
    QFontMetrics fm(font, viewWidget);
    return qRound(fm.height() * FONT_BOX_TO_CELL_BOX_MULTIPLIER);
}

int MaEditor::getColumnWidth() const {
    if (cachedColumnWidth == 0) {
        double columnWidth;
        if (resizeMode == ResizeMode_FontAndContent) {
            columnWidth = getUnifiedSequenceFontCharRect(font).width() * FONT_BOX_TO_CELL_BOX_MULTIPLIER * zoomFactor;
        } else {
            columnWidth = getUnifiedSequenceFontCharRect(font).width() * FONT_BOX_TO_CELL_BOX_MULTIPLIER;
            double rolledZoomFactor = zoomFactor;
            while (rolledZoomFactor <= 1 && columnWidth > MOBJECT_MIN_COLUMN_WIDTH) {
                rolledZoomFactor *= zoomMult;
                columnWidth = columnWidth / zoomMult;
            }
        }
        cachedColumnWidth = qMax((int)(columnWidth), MOBJECT_MIN_COLUMN_WIDTH);
    }
    return cachedColumnWidth;
}

QVariantMap MaEditor::getHighlightingSettings(const QString& highlightingFactoryId) const {
    const QVariant v = snp.highlightSchemeSettings.value(highlightingFactoryId);
    if (v.isNull()) {
        return QVariantMap();
    }
    CHECK(v.type() == QVariant::Map, QVariantMap());
    return v.toMap();
}

void MaEditor::saveHighlightingSettings(const QString& highlightingFactoryId, const QVariantMap& settingsMap /* = QVariant()*/) {
    snp.highlightSchemeSettings.insert(highlightingFactoryId, QVariant(settingsMap));
}

void MaEditor::setReference(qint64 sequenceId) {
    exportHighlightedAction->setEnabled(sequenceId != U2MsaRow::INVALID_ROW_ID);
    if (snp.seqId != sequenceId) {
        snp.seqId = sequenceId;
        emit si_referenceSeqChanged(sequenceId);
    }
    // REDRAW OTHER WIDGETS
}

void MaEditor::updateReference() {
    if (maObject->getRowPosById(snp.seqId) == -1) {
        setReference(U2MsaRow::INVALID_ROW_ID);
    }
}

void MaEditor::resetCollapseModel() {
    setRowOrderMode(MaEditorRowOrderMode::Original);
    collapseModel->reset(getMaRowIds());
}

void MaEditor::sl_zoomIn() {
    GCounter::increment("Zoom in", getFactoryId());

    ResizeMode oldResizeMode = resizeMode;
    if (resizeMode == ResizeMode_OnlyContent) {
        setZoomFactor(qMax(zoomFactor * zoomMult, 1.0));
    } else {
        int pSize = font.pointSize();
        if (pSize < maximumFontPointSize) {
            font.setPointSize(pSize + 1);
            setFont(font);
        }
        setZoomFactor(1);
    }
    updateActions();

    bool resizeModeChanged = resizeMode != oldResizeMode;
    emit si_zoomOperationPerformed(resizeModeChanged);
}

void MaEditor::sl_zoomOut() {
    GCounter::increment("Zoom out", getFactoryId());

    ResizeMode oldResizeMode = resizeMode;
    int fontPointSize = font.pointSize();
    if (fontPointSize > minimumFontPointSize) {
        font.setPointSize(fontPointSize - 1);
        setFont(font);
        setZoomFactor(1);
    } else {
        setZoomFactor(zoomFactor / zoomMult);
    }
    updateActions();

    bool resizeModeChanged = resizeMode != oldResizeMode;
    emit si_zoomOperationPerformed(resizeModeChanged);
}

void MaEditor::sl_zoomToSelection() {
    ResizeMode oldMode = resizeMode;
    QRect selectionRect = getSelection().toRect();
    CHECK(!selectionRect.isEmpty(), )

    MaEditorSequenceArea* sequenceArea = getLineWidget(0)->getSequenceArea();
    double viewWidth = sequenceArea->width();
    double viewHeight = sequenceArea->height();

    // Adjust selection width in bases with offsets view data:
    // Offsets view shares the same font and its width may grow in size if sequence view font is increased.
    // If offsets view grows the sequence view will reduce it width by the same amount of characters.
    MSAEditorOffsetsViewController* offsetsViewController = getLineWidget(0)->getOffsetsViewController();
    int basesInOffsetsView = offsetsViewController->leftWidget->getWidthInBases() + offsetsViewController->rightWidget->getWidthInBases();
    int adjustedSelectionWidth = selectionRect.width() + basesInOffsetsView;

    // Expected pixelsPerBase to fit the selection. Using int because getColumnWidth() returns int.
    int targetPixelsPerBaseX = (int)(viewWidth / (adjustedSelectionWidth * FONT_BOX_TO_CELL_BOX_MULTIPLIER));
    int targetPixelsPerBaseY = (int)(viewHeight / (selectionRect.height() * FONT_BOX_TO_CELL_BOX_MULTIPLIER));

    QDesktopWidget* desktopWidget = QApplication::desktop();
    double pixelToPointX = 72.0 / desktopWidget->logicalDpiX();  // 72 points == 1 inch (https://en.wikipedia.org/wiki/Point_(typography)).
    double pixelToPointY = 72.0 / desktopWidget->logicalDpiY();
    int targetFontPointSize = qMin(
        // Prefer the min value, so the font size will be smaller and selection will always fit.
        (int)qMin(targetPixelsPerBaseX * pixelToPointX, targetPixelsPerBaseY * pixelToPointY),
        maximumFontPointSize);
    if (targetFontPointSize >= minimumFontPointSize) {
        font.setPointSize(targetFontPointSize);
        setFont(font);
        setZoomFactor(1);
    } else {
        if (font.pointSize() != minimumFontPointSize) {
            font.setPointSize(minimumFontPointSize);
            setFont(font);
        }
        // Rendering mode with no characters shown because column width becomes too narrow.
        // Column height remains at minimum font height.
        double newZoomFactor = 1;
        double selectedAreaWidth = selectionRect.width() * minimumFontPointSize / pixelToPointX;
        while (selectedAreaWidth > viewWidth && selectedAreaWidth / selectionRect.width() > MOBJECT_MIN_COLUMN_WIDTH) {
            newZoomFactor /= zoomMult;
            selectedAreaWidth /= zoomMult;
        }
        setZoomFactor(newZoomFactor);
    }
    updateActions();
    emit si_zoomOperationPerformed(resizeMode != oldMode);

    // Wait util UI restructuring is finished and sequence view gets it final dims.
    // UI is restructured due to the font/zoom change.
    // Center the zoomed region next.
    QTimer::singleShot(200, this, [&]() { scrollSelectionIntoView(); });
}

void MaEditor::scrollSelectionIntoView() {
    QRect selectionRect = getSelection().toRect();
    CHECK(!selectionRect.isEmpty(), );
    MaEditorSequenceArea* sequenceArea = getLineWidget(0)->getSequenceArea();

    double viewWidth = sequenceArea->width();
    double viewHeight = sequenceArea->height();
    double pixelsPerCellX = getColumnWidth();
    double pixelsPerCellY = getRowHeight();
    SAFE_POINT(pixelsPerCellX > 0 && pixelsPerCellY > 0, "Invalid pixels per base/row", );

    int basesPerViewWidth = (int)(viewWidth / (double)pixelsPerCellX);
    int rowsPerViewHeight = (int)(viewHeight / (double)pixelsPerCellY);
    int basesOffset = 0;
    int rowsOffset = 0;
    if (basesPerViewWidth > selectionRect.width() && rowsPerViewHeight > selectionRect.height()) {
        basesOffset = -(basesPerViewWidth - selectionRect.width()) / 2;
        rowsOffset = -(rowsPerViewHeight - selectionRect.height()) / 2;
    }
    int firstVisibleBaseIndex = selectionRect.x() + basesOffset;
    int firstVisibleRowIndex = selectionRect.y() + rowsOffset;
    if (auto msaEditor = qobject_cast<MsaEditor*>(this)) {
        auto scrollController = msaEditor->getMainWidget()->getScrollController();
        scrollController->setFirstVisibleBase(firstVisibleBaseIndex);
        scrollController->setFirstVisibleViewRow(firstVisibleRowIndex);
    }
    updateActions();
}

void MaEditor::sl_resetZoom() {
    GCounter::increment("Reset zoom", getFactoryId());
    QFont f = getFont();
    CHECK(f.pointSize() != MOBJECT_DEFAULT_FONT_SIZE || zoomFactor != MOBJECT_DEFAULT_ZOOM_FACTOR, );

    ResizeMode oldResizeMode = resizeMode;
    if (f.pointSize() != MOBJECT_DEFAULT_FONT_SIZE) {
        f.setPointSize(MOBJECT_DEFAULT_FONT_SIZE);
        setFont(f);
    }
    setZoomFactor(MOBJECT_DEFAULT_ZOOM_FACTOR);

    resizeMode = ResizeMode_FontAndContent;
    emit si_zoomOperationPerformed(resizeMode != oldResizeMode);

    updateActions();
}

void MaEditor::sl_saveAlignment() {
    AppContext::getTaskScheduler()->registerTopLevelTask(new SaveDocumentTask(maObject->getDocument()));
}

void MaEditor::sl_saveAlignmentAs() {
    Document* srcDoc = maObject->getDocument();
    if (srcDoc == nullptr) {
        return;
    }
    if (!srcDoc->isLoaded()) {
        return;
    }

    QObjectScopedPointer<ExportDocumentDialogController> dialog = new ExportDocumentDialogController(srcDoc, getWidget());
    dialog->setAddToProjectFlag(true);
    dialog->setWindowTitle(tr("Save Alignment"));
    ExportObjectUtils::export2Document(dialog);
}

void MaEditor::sl_changeFont() {
    bool ok = false;
    GCounter::increment("Change of the characters font", getFactoryId());
    // QFontDialog::DontUseNativeDialog - no color selector, affects only Mac OS
    QFont f = QFontDialog::getFont(&ok, font, viewWidget, tr("Characters Font"), QFontDialog::DontUseNativeDialog);
    if (!ok) {
        return;
    }
    setFont(f);
    updateActions();
    emit si_completeUpdate();
}

void MaEditor::sl_lockedStateChanged() {
    updateActions();
}

void MaEditor::sl_exportHighlighted() {
    QObjectScopedPointer<ExportHighligtingDialogController> d = new ExportHighligtingDialogController(getLineWidget(0), (QWidget*)AppContext::getMainWindow()->getQMainWindow());
    d->exec();
    CHECK(!d.isNull(), );

    if (d->result() == QDialog::Accepted) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new ExportHighlightingTask(d.data(), this));
    }
}

void MaEditor::resetColumnWidthCache() {
    cachedColumnWidth = 0;
}

void MaEditor::initActions() {
    showOverviewAction = new QAction(QIcon(":/core/images/msa_show_overview.png"), tr("Overview"), this);
    showOverviewAction->setObjectName("Show overview");
    showOverviewAction->setCheckable(true);
    showOverviewAction->setChecked(true);
    connect(showOverviewAction, &QAction::triggered, getLineWidget(0)->getOverviewArea(), &QWidget::setVisible);
    getWidget()->addAction(showOverviewAction);
    clearSelectionAction = new QAction(tr("Clear selection"), this);
    clearSelectionAction->setShortcut(Qt::Key_Escape);
    clearSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(clearSelectionAction, SIGNAL(triggered()), SLOT(sl_onClearActionTriggered()));
    getWidget()->addAction(clearSelectionAction);

    connect(getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)));

    connect(undoAction, &QAction::triggered, [this]() { GCounter::increment("Undo", factoryId); });
    connect(redoAction, &QAction::triggered, [this]() { GCounter::increment("Redo", factoryId); });
    getWidget()->addAction(undoAction);
    getWidget()->addAction(redoAction);
}

void MaEditor::initZoom() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != nullptr, "AppContext is NULL", );
    zoomFactor = s->getValue(getSettingsRoot() + MOBJECT_SETTINGS_ZOOM_FACTOR, MOBJECT_DEFAULT_ZOOM_FACTOR).toFloat();
    updateResizeMode();
}

void MaEditor::initFont() {
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != nullptr, "AppContext is NULL", );
    font.setFamily(s->getValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_FAMILY, MOBJECT_DEFAULT_FONT_FAMILY).toString());
    font.setPointSize(s->getValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_SIZE, MOBJECT_DEFAULT_FONT_SIZE).toInt());
    font.setItalic(s->getValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_ITALIC, false).toBool());
    font.setBold(s->getValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_BOLD, false).toBool());

    updateFontMetrics();
}

void MaEditor::updateResizeMode() {
    SAFE_POINT(font.pointSize() >= minimumFontPointSize, "Illegal font point size", );
    resizeMode = zoomFactor < 1.0f ? ResizeMode_OnlyContent : ResizeMode_FontAndContent;
}

void MaEditor::addCopyPasteMenu(QMenu* m, int) {
    QMenu* cm = m->addMenu(tr("Copy/Paste"));
    cm->menuAction()->setObjectName(MSAE_MENU_COPY);
}

void MaEditor::addExportMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Export"));
    em->menuAction()->setObjectName(MSAE_MENU_EXPORT);
    em->addAction(exportHighlightedAction);
    if (!getLineWidget(0)->getSequenceArea()->getCurrentHighlightingScheme()->getFactory()->isRefFree() &&
        getReferenceRowId() != U2MsaRow::INVALID_ROW_ID) {
        exportHighlightedAction->setEnabled(true);
    } else {
        exportHighlightedAction->setDisabled(true);
    }
}

void MaEditor::addLoadMenu(QMenu* m) {
    QMenu* lsm = m->addMenu(tr("Add"));
    lsm->menuAction()->setObjectName(MSAE_MENU_LOAD);
}

void MaEditor::setFont(const QFont& f) {
    int pSize = f.pointSize();
    font = f;
    resetColumnWidthCache();
    updateFontMetrics();
    font.setPointSize(qBound(minimumFontPointSize, pSize, maximumFontPointSize));
    updateResizeMode();
    if (auto w = qobject_cast<McaEditorWgt*>(getWidget())) {
        w->getScrollController()->updateScrollBarsOnFontOrZoomChange();
    }
    emit si_fontChanged(font);

    Settings* s = AppContext::getSettings();
    s->setValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_FAMILY, f.family());
    s->setValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_SIZE, f.pointSize());
    s->setValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_ITALIC, f.italic());
    s->setValue(getSettingsRoot() + MOBJECT_SETTINGS_FONT_BOLD, f.bold());
    viewWidget->update();
}

void MaEditor::updateFontMetrics() {
    // Re-calculate the minimumFontPointSize based on the current font.
    // The minimum font size is computed using the similar logic with Assembly Browser:
    // it is equal to maximum font size value that can safely be rendered in the pre-defined on-screen cell of width = 7px.
    // This approach is good, because UGENE continues to draw sequence characters until there is enough pixels in the cell to draw a
    // meaningful shape regardless of the current device settings.
    // TODO: this logic should be refactored and centralized for all sequence views in UGENE with zooming support.
    const int minimumCellWidthToShowText = 7;
    const int minimumFontCharWidthInsideCell = minimumCellWidthToShowText - 2;  // Keep 1px left & right padding inside the cell.
    const int minimumSafeFontPointSize = 8;  // This value was historically used in UGENE as minimum with no known issues.
    QFont fontToEstimate = font;
    int estimatedMinimumFontPointSize = minimumSafeFontPointSize;  // Start with a safe value and estimate smaller values.
    while (fontToEstimate.pointSize() > 1) {
        int charWidth = getUnifiedSequenceFontCharRect(fontToEstimate).width();
        if (charWidth < minimumFontCharWidthInsideCell) {
            // The estimated char size is too small. Stop on the previous value.
            break;
        }
        estimatedMinimumFontPointSize = fontToEstimate.pointSize();
        fontToEstimate.setPointSize(fontToEstimate.pointSize() - 1);
    }
    minimumFontPointSize = estimatedMinimumFontPointSize;
}

void MaEditor::setFirstVisiblePosSeq(int firstPos, int firstSeq) const {
    CHECK(getLineWidget(0)->getSequenceArea()->isPosInRange(firstPos), );

    if (auto msaEditor = qobject_cast<const MsaEditor*>(this)) {
        msaEditor->getMainWidget()->getScrollController()->setFirstVisibleBase(firstPos);
        msaEditor->getMainWidget()->getScrollController()->setFirstVisibleMaRow(firstSeq);
    } else {
        getLineWidget(0)->getScrollController()->setFirstVisibleBase(firstPos);
        getLineWidget(0)->getScrollController()->setFirstVisibleMaRow(firstSeq);
    }
}

void MaEditor::setZoomFactor(double newZoomFactor) {
    CHECK(zoomFactor != newZoomFactor, );
    zoomFactor = newZoomFactor;
    updateResizeMode();
    Settings* s = AppContext::getSettings();
    s->setValue(getSettingsRoot() + MOBJECT_SETTINGS_ZOOM_FACTOR, zoomFactor);
    resetColumnWidthCache();
}

void MaEditor::updateActions() {
    zoomInAction->setEnabled(font.pointSize() < maximumFontPointSize);
    zoomOutAction->setEnabled(getColumnWidth() > MOBJECT_MIN_COLUMN_WIDTH);
    zoomToSelectionAction->setEnabled(font.pointSize() < maximumFontPointSize);

    bool isDefaultZoomAndFontSize = getFont().pointSize() == MOBJECT_DEFAULT_FONT_SIZE && zoomFactor == MOBJECT_DEFAULT_ZOOM_FACTOR;
    resetZoomAction->setEnabled(!isDefaultZoomAndFontSize);

    changeFontAction->setEnabled(resizeMode == ResizeMode_FontAndContent);

    MaEditorSelection selection = getSelection();
    gotoSelectedReadAction->setEnabled(!selection.isEmpty());

    emit si_updateActions();
}

const QPoint& MaEditor::getCursorPosition() const {
    return cursorPosition;
}

void MaEditor::setCursorPosition(const QPoint& newCursorPosition) {
    CHECK(cursorPosition != newCursorPosition, );
    int x = newCursorPosition.x(), y = newCursorPosition.y();
    CHECK(x >= 0 && y >= 0 && x < getAlignmentLen() && y < getNumSequences(), );
    cursorPosition = newCursorPosition;
    emit si_cursorPositionChanged(cursorPosition);
}

QList<qint64> MaEditor::getMaRowIds() const {
    return maObject->getAlignment()->getRowsIds();
}

void MaEditor::selectRows(int firstViewRowIndex, int numberOfRows) {
    getLineWidget(0)->getSequenceArea()->setSelectionRect(QRect(0, firstViewRowIndex, getAlignmentLen(), numberOfRows));
}

QRect MaEditor::getUnifiedSequenceFontCharRect(const QFont& sequenceFont) const {
    QFontMetrics fontMetrics(sequenceFont, viewWidget);
    return fontMetrics.boundingRect('W');
}

MaEditorRowOrderMode MaEditor::getRowOrderMode() const {
    return rowOrderMode;
}

void MaEditor::setRowOrderMode(MaEditorRowOrderMode mode) {
    rowOrderMode = mode;
}

void MaEditor::sl_onClearActionTriggered() {
    MaEditorSequenceArea* sequenceArea = getLineWidget(0)->getSequenceArea();
    if (sequenceArea->getMode() != MaEditorSequenceArea::ViewMode) {
        sequenceArea->exitFromEditCharacterMode();
        return;
    }
    getSelectionController()->clearSelection();
}

MaCollapseModel* MaEditor::getCollapseModel() const {
    return collapseModel;
}

MaUndoRedoFramework* MaEditor::getUndoRedoFramework() const {
    return undoRedoFramework;
}

bool MaEditor::isMultilineMode() const {
    return multilineMode;
}

bool MaEditor::setMultilineMode(bool newmode) {
    Q_UNUSED(newmode);
    return false;
}

}  // namespace U2
