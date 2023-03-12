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

#include <QApplication>
#include <QClipboard>
#include <QHelpEvent>
#include <QMenu>
#include <QPainter>
#include <QToolTip>

#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MsaColorScheme.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>

#include "ov_msa/BaseWidthController.h"
#include "ov_msa/MSAEditor.h"
#include "ov_msa/MSAEditorConsensusArea.h"
#include "ov_msa/MSAEditorSequenceArea.h"
#include "ov_msa/MaConsensusAreaRenderer.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/ScrollController.h"
#include "ov_msa/general_tab/MSAGeneralTabFactory.h"

namespace U2 {

MaEditorConsensusArea::MaEditorConsensusArea(MaEditorWgt* _ui)
    : editor(_ui->getEditor()),
      ui(_ui),
      renderer(nullptr) {
    SAFE_POINT(editor->getMaObject(), "No MA object in MaEditorConsensusArea", );
    completeRedraw = true;
    curPos = -1;
    selecting = false;
    cachedView = new QPixmap();

    connect(editor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)));
    connect(ui->getEditor(), SIGNAL(si_zoomOperationPerformed(bool)), SLOT(sl_zoomOperationPerformed(bool)));
    connect(ui, SIGNAL(si_completeRedraw()), SLOT(sl_completeRedraw()));

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), SLOT(sl_alignmentChanged()));

    connect(editor->copyConsensusAction, SIGNAL(triggered()), SLOT(sl_copyConsensusSequence()));
    connect(editor->copyConsensusWithGapsAction, SIGNAL(triggered()), SLOT(sl_copyConsensusSequenceWithGaps()));

    configureConsensusAction = new QAction(tr("Consensus mode..."), this);
    configureConsensusAction->setObjectName("Consensus mode");
    connect(configureConsensusAction, SIGNAL(triggered()), SLOT(sl_configureConsensusAction()));

    connect(editor, SIGNAL(si_fontChanged(QFont)), SLOT(setupFontAndHeight()));

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    addAction(ui->copySelectionAction);
    addAction(ui->pasteAction);
    addAction(ui->pasteBeforeAction);

    setObjectName("consArea");
}

MaEditorConsensusArea::~MaEditorConsensusArea() {
    delete cachedView;
}

MaEditorWgt* MaEditorConsensusArea::getEditorWgt() const {
    return ui;
}

QSize MaEditorConsensusArea::getCanvasSize(const U2Region& region, const MaEditorConsElements& elements) const {
    return QSize(ui->getBaseWidthController()->getBasesWidth(region), renderer->getHeight(elements));
}

QSharedPointer<MSAEditorConsensusCache> MaEditorConsensusArea::getConsensusCache() {
    return consensusCache;
}

bool MaEditorConsensusArea::event(QEvent* e) {
    switch (e->type()) {
        case QEvent::ToolTip: {
            auto he = static_cast<QHelpEvent*>(e);
            QString tip = createToolTip(he);
            if (!tip.isEmpty()) {
                QToolTip::showText(he->globalPos(), tip);
            }
            return true;
        }
        case QEvent::FocusIn:
            ui->getSequenceArea()->setFocus(static_cast<QFocusEvent*>(e)->reason());
            break;
        case QEvent::Wheel:
            ui->getSequenceArea()->setFocus(Qt::MouseFocusReason);
            break;
        default:;  // skip other events
    }

    return QWidget::event(e);
}

void MaEditorConsensusArea::initCache() {
    MSAConsensusAlgorithmFactory* algo = getConsensusAlgorithmFactory();
    GCounter::increment(QString("'%1' consensus type is selected on view opening").arg(algo->getName()), editor->getFactoryId());
    consensusCache = QSharedPointer<MSAEditorConsensusCache>(new MSAEditorConsensusCache(nullptr, editor->getMaObject(), algo));
    connect(consensusCache->getConsensusAlgorithm(), SIGNAL(si_thresholdChanged(int)), SLOT(sl_onConsensusThresholdChanged(int)));
    restoreLastUsedConsensusThreshold();
}

QString MaEditorConsensusArea::createToolTip(QHelpEvent* he) const {
    const int x = he->pos().x();
    const int column = ui->getBaseWidthController()->screenXPositionToColumn(x);
    QString result;
    if (0 <= column && column <= editor->getAlignmentLen()) {
        assert(editor->getMaObject());
        // const MultipleAlignment ma = editor->getMaObject()->getMultipleAlignment();
        // result = MSAConsensusUtils::getConsensusPercentTip(ma, column, 0, 4);
        result = getConsensusPercentTip(column, 0, 4);
    }
    return result;
}

void MaEditorConsensusArea::resizeEvent(QResizeEvent* e) {
    completeRedraw = true;
    QWidget::resizeEvent(e);
}

void MaEditorConsensusArea::paintEvent(QPaintEvent* e) {
    QSize s = size() * devicePixelRatio();
    QSize sas = ui->getSequenceArea()->size() * devicePixelRatio();

    if (sas.width() != s.width()) {  // this can happen due to the manual layouting performed by MSAEditor -> just wait for the next resize+paint
        return;
    }

    assert(s.width() == sas.width());

    if (cachedView->size() != s) {
        delete cachedView;
        cachedView = new QPixmap(s);
        cachedView->setDevicePixelRatio(devicePixelRatio());
        completeRedraw = true;
    }

    if (completeRedraw) {
        QPainter pCached(cachedView);
        pCached.fillRect(cachedView->rect(), Qt::white);
        drawContent(pCached);
        completeRedraw = false;
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, *cachedView);

    QWidget::paintEvent(e);
}

void MaEditorConsensusArea::drawContent(QPainter& painter) {
    renderer->drawContent(painter);
}

void MaEditorConsensusArea::drawContent(QPainter& painter,
                                        const QList<int>& seqIdx,
                                        const U2Region& region,
                                        const MaEditorConsensusAreaSettings& consensusAreaSettings) {
    ConsensusRenderData consensusRenderData = renderer->getConsensusRenderData(seqIdx, region);
    ConsensusRenderSettings renderSettings = renderer->getRenderSettigns(region, consensusAreaSettings);
    renderer->drawContent(painter, consensusRenderData, consensusAreaSettings, renderSettings);
}

bool MaEditorConsensusArea::highlightConsensusChar(int /*pos*/) {
    return false;
}

MSAConsensusAlgorithmFactory* MaEditorConsensusArea::getConsensusAlgorithmFactory() {
    MSAConsensusAlgorithmRegistry* reg = AppContext::getMSAConsensusAlgorithmRegistry();
    SAFE_POINT(reg != nullptr, "Consensus algorithm registry is NULL.", nullptr);
    QString lastUsedAlgoKey = getLastUsedAlgoSettingsKey();
    QString lastUsedAlgo = AppContext::getSettings()->getValue(lastUsedAlgoKey).toString();
    MSAConsensusAlgorithmFactory* algo = reg->getAlgorithmFactory(lastUsedAlgo);

    const DNAAlphabet* al = editor->getMaObject()->getAlphabet();
    ConsensusAlgorithmFlags alphaFlags = MSAConsensusAlgorithmFactory::getAphabetFlags(al);
    if (algo == nullptr || (algo->getFlags() & alphaFlags) != alphaFlags) {
        algo = reg->getAlgorithmFactory(getDefaultAlgorithmId());
        if ((algo->getFlags() & alphaFlags) != alphaFlags) {
            QList<MSAConsensusAlgorithmFactory*> algorithms = reg->getAlgorithmFactories(MSAConsensusAlgorithmFactory::getAphabetFlags(al));
            SAFE_POINT(algorithms.count() > 0, "There are no consensus algorithms for the current alphabet.", nullptr);
            algo = algorithms.first();
        }
        AppContext::getSettings()->setValue(lastUsedAlgoKey, algo->getId());
    }
    return algo;
}

void MaEditorConsensusArea::updateConsensusAlgorithm() {
    MSAConsensusAlgorithmFactory* newAlgo = getConsensusAlgorithmFactory();
    CHECK(consensusCache != nullptr && newAlgo != nullptr, );
    ConsensusAlgorithmFlags cacheConsensusFlags = consensusCache->getConsensusAlgorithm()->getFactory()->getFlags();
    ConsensusAlgorithmFlags curFlags = newAlgo->getFlags();
    if ((curFlags & cacheConsensusFlags) != curFlags) {
        consensusCache->setConsensusAlgorithm(newAlgo);
    }
    emit si_consensusAlgorithmChanged(newAlgo->getId());
}

void MaEditorConsensusArea::sl_alignmentChanged() {
    updateConsensusAlgorithm();
    completeRedraw = true;
    emit si_mismatchRedrawRequired();
    update();
}

void MaEditorConsensusArea::setupFontAndHeight() {
    consensusSettings.font = ui->getEditor()->getFont();
    consensusSettings.setRulerFont(ui->getEditor()->getFont());
    setFixedHeight(renderer->getHeight());
    completeRedraw = true;
    update();
}

void MaEditorConsensusArea::sl_zoomOperationPerformed(bool resizeModeChanged) {
    if (!(editor->getResizeMode() == MSAEditor::ResizeMode_OnlyContent && !resizeModeChanged)) {
        setupFontAndHeight();
    }
    sl_completeRedraw();
}

void MaEditorConsensusArea::sl_completeRedraw() {
    completeRedraw = true;
    update();
}

void MaEditorConsensusArea::sl_selectionChanged(const MaEditorSelection& current, const MaEditorSelection& prev) {
    U2Region currentRegion = current.getColumnRegion();
    U2Region prevRegion = prev.getColumnRegion();
    if (currentRegion != prevRegion) {
        sl_completeRedraw();
    }
}

void MaEditorConsensusArea::sl_copyConsensusSequence() {
    QApplication::clipboard()->setText(consensusCache->getConsensusLine(false));
}

void MaEditorConsensusArea::sl_copyConsensusSequenceWithGaps() {
    QApplication::clipboard()->setText(consensusCache->getConsensusLine(true));
}

void MaEditorConsensusArea::sl_configureConsensusAction() {
    editor->getOptionsPanelController()->openGroupById(MSAGeneralTabFactory::getGroupId());
}

void MaEditorConsensusArea::sl_changeConsensusAlgorithm(const QString& algoId) {
    MSAConsensusAlgorithmFactory* algoFactory = AppContext::getMSAConsensusAlgorithmRegistry()->getAlgorithmFactory(algoId);
    if (getConsensusAlgorithm()->getFactory() != algoFactory) {
        assert(algoFactory != nullptr);
        setConsensusAlgorithm(algoFactory);
    }
    emit si_consensusAlgorithmChanged(algoId);
}

QString MaEditorConsensusArea::getThresholdSettingsKey(const QString& factoryId) const {
    return getLastUsedAlgoSettingsKey() + "_" + factoryId + "_threshold";
}

void MaEditorConsensusArea::setConsensusAlgorithm(MSAConsensusAlgorithmFactory* algoFactory) {
    MSAConsensusAlgorithm* oldAlgo = getConsensusAlgorithm();
    if (oldAlgo != nullptr && algoFactory == oldAlgo->getFactory()) {
        return;
    }
    GCounter::increment(QString("'%1' consensus algorithm is selected").arg(algoFactory->getName()), editor->getFactoryId());

    // store threshold for the active algo
    if (oldAlgo != nullptr && oldAlgo->supportsThreshold()) {
        AppContext::getSettings()->setValue(getThresholdSettingsKey(oldAlgo->getId()), oldAlgo->getThreshold());
    }

    // store current algorithm selection
    AppContext::getSettings()->setValue(getLastUsedAlgoSettingsKey(), algoFactory->getId());

    consensusCache->setConsensusAlgorithm(algoFactory);
    connect(consensusCache->getConsensusAlgorithm(), SIGNAL(si_thresholdChanged(int)), SLOT(sl_onConsensusThresholdChanged(int)));
    restoreLastUsedConsensusThreshold();
    completeRedraw = true;
    emit si_mismatchRedrawRequired();
    update();
}

void MaEditorConsensusArea::setConsensusAlgorithmConsensusThreshold(int val) {
    MSAConsensusAlgorithm* algo = getConsensusAlgorithm();
    if (val == algo->getThreshold()) {
        return;
    }
    // store threshold as the last value
    AppContext::getSettings()->setValue(getThresholdSettingsKey(algo->getId()), val);
    algo->setThreshold(val);
}

const MaEditorConsensusAreaSettings& MaEditorConsensusArea::getDrawSettings() const {
    return consensusSettings;
}

void MaEditorConsensusArea::setDrawSettings(const MaEditorConsensusAreaSettings& settings) {
    consensusSettings = settings;
    setFixedHeight(renderer->getHeight());
}

void MaEditorConsensusArea::sl_onConsensusThresholdChanged(int /*newValue*/) {
    completeRedraw = true;
    emit si_mismatchRedrawRequired();
    update();
}

void MaEditorConsensusArea::restoreLastUsedConsensusThreshold() {
    // restore last used threshold for new algorithm type if found
    MSAConsensusAlgorithm* algo = getConsensusAlgorithm();
    int threshold = AppContext::getSettings()->getValue(getThresholdSettingsKey(algo->getId()), algo->getDefaultThreshold()).toInt();
    getConsensusAlgorithm()->setThreshold(threshold);
}

MSAConsensusAlgorithm* MaEditorConsensusArea::getConsensusAlgorithm() const {
    return consensusCache->getConsensusAlgorithm();
}

void MaEditorConsensusArea::sl_changeConsensusThreshold(int val) {
    setConsensusAlgorithmConsensusThreshold(val);
    emit si_consensusThresholdChanged(val);
}

void MaEditorConsensusArea::mousePressEvent(QMouseEvent* e) {
    if (!(e->buttons() & Qt::LeftButton)) {
        QWidget::mousePressEvent(e);
        return;
    }
    selecting = true;
    curPos = qBound(0, ui->getBaseWidthController()->screenXPositionToColumn(e->x()), ui->getEditor()->getAlignmentLen() - 1);
    QPoint cursorPosition = editor->getCursorPosition();
    if (e->modifiers() == Qt::ShiftModifier && cursorPosition.x() != -1) {
        growSelectionUpTo(curPos);
    } else {
        int selectionHeight = ui->getSequenceArea()->getViewRowCount();
        QRect selection(curPos, 0, 1, selectionHeight);
        ui->getSequenceArea()->setSelectionRect(selection);
        editor->setCursorPosition(QPoint(curPos, 0));
    }
    QWidget::mousePressEvent(e);
}

void MaEditorConsensusArea::mouseMoveEvent(QMouseEvent* event) {
    if ((event->buttons() & Qt::LeftButton) && selecting) {
        int newPos = qBound(0, ui->getBaseWidthController()->screenXPositionToColumn(event->x()), ui->getEditor()->getAlignmentLen() - 1);
        growSelectionUpTo(newPos);
    }
    QWidget::mouseMoveEvent(event);
}

void MaEditorConsensusArea::mouseReleaseEvent(QMouseEvent* event) {
    if (ui->getSequenceArea()->isAlignmentEmpty()) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton && selecting) {
        int newPos = qBound(0, ui->getBaseWidthController()->screenXPositionToColumn(event->x()), editor->getAlignmentLen() - 1);
        growSelectionUpTo(newPos);
        selecting = false;
    }

    ui->getScrollController()->stopSmoothScrolling();
    QWidget::mouseReleaseEvent(event);
}

void MaEditorConsensusArea::growSelectionUpTo(int xPos) {
    CHECK(xPos >= 0 && xPos <= editor->getAlignmentLen(), );

    int cursorX = editor->getCursorPosition().x();
    int selectionHeight = ui->getSequenceArea()->getViewRowCount();
    QRect selection(qMin(cursorX, xPos), 0, abs(xPos - cursorX) + 1, selectionHeight);
    ui->getSequenceArea()->setSelectionRect(selection);
}

}  // namespace U2
