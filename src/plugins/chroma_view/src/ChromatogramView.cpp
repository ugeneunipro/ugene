/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ChromatogramView.h"

#include <QMessageBox>

#include <U2Core/ChromatogramObject.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/AddNewDocumentDialogController.h>
#include <U2Gui/GScrollBar.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/McaColors.h>

namespace U2 {

static const char GAP_CHAR = '-';

ChromatogramView::ChromatogramView(QWidget* p, ADVSequenceObjectContext* v, GSequenceLineView* cv, const Chromatogram& chromatogram)
    : GSequenceLineView(p, v), editDNASeq(nullptr) {
    const QString objectName = "chromatogram_view_" + v->getSequenceGObject()->getGObjectName();
    setObjectName(objectName);

    dnaView = v->getAnnotatedDNAView();

    showQVAction = new QAction(tr("Show quality bars"), this);
    showQVAction->setIcon(GUIUtils::getIconResource("chroma_view", "bars.png", false));
    showQVAction->setCheckable(true);
    showQVAction->setChecked(chromatogram->hasQV);
    showQVAction->setEnabled(chromatogram->hasQV);
    connect(showQVAction, SIGNAL(toggled(bool)), SLOT(completeUpdate()));

    showAllTraces = new QAction(tr("Show all"), this);
    connect(showAllTraces, SIGNAL(triggered()), SLOT(sl_showAllTraces()));

    traceActionMenu = new QMenu(tr("Show/hide trace"), this);
    traceActionMenu->addAction(createToggleTraceAction("A"));
    traceActionMenu->addAction(createToggleTraceAction("C"));
    traceActionMenu->addAction(createToggleTraceAction("G"));
    traceActionMenu->addAction(createToggleTraceAction("T"));
    traceActionMenu->addSeparator();
    traceActionMenu->addAction(showAllTraces);

    renderArea = new ChromatogramViewRenderArea(this, chromatogram);

    scaleBar = new ScaleBar();
    scaleBar->setRange(100, 1000);
    scaleBar->setTickInterval(100);
    connect(scaleBar, SIGNAL(valueChanged(int)), SLOT(setRenderAreaHeight(int)));

    ra = static_cast<ChromatogramViewRenderArea*>(renderArea);
    scaleBar->setValue(int(ra->height() - ra->getHeightAreaBC() + ra->addUpIfQVL));

    setCoherentRangeView(cv);

    mP = new QMenu(this);

    mP->addAction(QString("A"));
    mP->addAction(QString("C"));
    mP->addAction(QString("G"));
    mP->addAction(QString("T"));
    mP->addAction(QString("N"));
    mP->addAction(QString(GAP_CHAR));
    connect(mP, SIGNAL(triggered(QAction*)), SLOT(sl_onPopupMenuCkicked(QAction*)));

    addNewSeqAction = new QAction(tr("Edit new sequence"), this);
    connect(addNewSeqAction, SIGNAL(triggered()), SLOT(sl_addNewSequenceObject()));

    addExistSeqAction = new QAction(tr("Edit existing sequence"), this);
    connect(addExistSeqAction, SIGNAL(triggered()), SLOT(sl_onAddExistingSequenceObject()));

    clearEditableSequence = new QAction(tr("Remove edited sequence"), this);
    connect(clearEditableSequence, SIGNAL(triggered()), SLOT(sl_clearEditableSequence()));

    removeChanges = new QAction(tr("Undo changes"), this);
    connect(removeChanges, SIGNAL(triggered()), SLOT(sl_removeChanges()));

    connect(dnaView, &GObjectViewController::si_objectRemoved, this, &ChromatogramView::sl_onObjectRemoved);
    pack();

    addActionToLocalToolbar(showQVAction);
    QToolButton* traceButton = addActionToLocalToolbar(traceActionMenu->menuAction());
    traceButton->setIcon(GUIUtils::getIconResource("chroma_view", "traces.png", false));
    traceButton->setPopupMode(QToolButton::InstantPopup);
}

void ChromatogramView::pack() {
    auto layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(renderArea);
    layout->addWidget(scaleBar);
    setContentLayout(layout);

    scrollBar->setHidden(true);  // todo: support mode without scrollbar at all??

    setMinimumHeight(renderArea->minimumHeight());
}

void ChromatogramView::setRenderAreaHeight(int k) {
    // k = chromaMax
    auto cvra = static_cast<ChromatogramViewRenderArea*>(renderArea);
    cvra->setAreaHeight(k);
    completeUpdate();
}

void ChromatogramView::buildPopupMenu(QMenu& m) {
    QPoint cpos = renderArea->mapFromGlobal(QCursor::pos());
    if (!renderArea->rect().contains(cpos)) {
        return;
    }
    // todo: move to submenus?
    QAction* before = GUIUtils::findActionAfter(m.actions(), ADV_MENU_ZOOM);

    m.insertAction(before, showQVAction);
    m.insertMenu(before, traceActionMenu);
    m.insertSeparator(before);
    if (editDNASeq != nullptr) {
        m.insertAction(before, clearEditableSequence);
        m.insertAction(before, removeChanges);
    } else {
        m.insertAction(before, addNewSeqAction);
        m.insertAction(before, addExistSeqAction);
    }
    m.insertSeparator(before);
}

static const int MAX_DNA_LEN = 1000 * 1000 * 1000;

void ChromatogramView::mousePressEvent(QMouseEvent* me) {
    setFocus();
    if (me->button() == Qt::RightButton || editDNASeq == nullptr) {
        GSequenceLineView::mousePressEvent(me);
        return;
    }
    if (editDNASeq != nullptr && editDNASeq->getSequenceLength() > MAX_DNA_LEN) {
        GSequenceLineView::mousePressEvent(me);
        return;
    }
    QPoint renderAreaPos = toRenderAreaPoint(me->pos());

    const U2Region& visibleRange = getVisibleRange();
    QRectF rect;
    for (int i = int(visibleRange.startPos); i < visibleRange.endPos(); ++i) {
        rect = ra->posToRect(i);
        if (rect.contains(renderAreaPos)) {
            ra->hasSel = true;
            ra->selRect = rect;
            selIndex = i;
            update();
            mP->popup(mapToGlobal(rect.bottomRight().toPoint()));
            return;
        }
    }
    ra->hasSel = false;
    update();
    GSequenceLineView::mousePressEvent(me);
}

int ChromatogramView::getEditSeqIndex(int bcIndex) {
    int before = 0;
    foreach (int gapIdx, gapIndexes) {
        if (gapIdx < bcIndex) {
            ++before;
        }
    }
    return bcIndex - before;
}

void ChromatogramView::sl_onPopupMenuCkicked(QAction* a) {
    if (editDNASeq->isStateLocked()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("The sequence is locked"));
        return;
    }
    char newBase = a->text().at(0).toLatin1();
    char curBase = currentBaseCalls.at(selIndex);
    U2OpStatus2Log os;
    if (newBase != curBase) {
        int editSeqIdx = getEditSeqIndex(selIndex);
        if (curBase == GAP_CHAR) {
            bool ok = gapIndexes.removeOne(selIndex);
            assert(ok);
            Q_UNUSED(ok);
            QByteArray insData(&newBase, 1);
            editDNASeq->replaceRegion(U2Region(editSeqIdx, 0), DNASequence(insData), os);  // insert
        } else {
            if (newBase != GAP_CHAR) {
                QByteArray insData(&newBase, 1);
                editDNASeq->replaceRegion(U2Region(editSeqIdx, 1), DNASequence(insData), os);  // replace
            } else {
                editDNASeq->replaceRegion(U2Region(editSeqIdx, 1), DNASequence(), os);  // remove
                gapIndexes.append(selIndex);
            }
        }

        currentBaseCalls[selIndex] = newBase;

        indexOfChangedChars.insert(selIndex);
        QByteArray seqData = ctx->getSequenceData(U2Region(selIndex, 1), os);
        SAFE_POINT_OP(os, );
        char refBase = seqData.at(0);
        if (newBase == refBase) {
            indexOfChangedChars.remove(selIndex);
        }
    }
    ra->hasSel = false;
    update();
}

void ChromatogramView::sl_addNewSequenceObject() {
    sl_clearEditableSequence();

    assert(editDNASeq == nullptr);

    AddNewDocumentDialogModel m;
    DocumentFormatConstraints c;

    GUrl seqUrl = ctx->getSequenceGObject()->getDocument()->getURL();
    m.url = GUrlUtils::rollFileName(seqUrl.dirPath() + "/" + seqUrl.baseFileName() + "_sequence.fa", DocumentUtils::getNewDocFileNameExcludesHint());
    c.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    c.addFlagToExclude(DocumentFormatFlag_Hidden);
    c.supportedObjectTypes += GObjectTypes::SEQUENCE;
    AddNewDocumentDialogController::run(nullptr, m, c);
    if (!m.successful) {
        return;
    }
    Project* p = AppContext::getProject();

    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(m.format);
    SAFE_POINT(format != nullptr, QString("Format is not registered: '%1'").arg(m.format), );
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(m.io);
    U2OpStatus2Log os;
    Document* doc = format->createNewLoadedDocument(iof, m.url, os);
    CHECK_OP(os, );
    p->addDocument(doc);

    U2SequenceObject* so = ctx->getSequenceObject();
    editDNASeq = qobject_cast<U2SequenceObject*>(so->clone(doc->getDbiRef(), os));
    CHECK_OP(os, );
    currentBaseCalls = editDNASeq->getWholeSequenceData(os);
    CHECK_OP(os, );
    doc->addObject(editDNASeq);
    dnaView->addObject(editDNASeq);
    indexOfChangedChars.clear();
}

void ChromatogramView::sl_onAddExistingSequenceObject() {
    sl_clearEditableSequence();

    assert(editDNASeq == nullptr);

    ProjectTreeControllerModeSettings s;
    s.allowMultipleSelection = false;
    s.objectTypesToShow.insert(GObjectTypes::SEQUENCE);
    U2SequenceObjectConstraints ac;
    ac.sequenceSize = ctx->getSequenceLength();
    s.objectConstraints.insert(&ac);
    ac.alphabetType = ctx->getSequenceObject()->getAlphabet()->getType();
    s.groupMode = ProjectTreeGroupMode_ByDocument;
    foreach (GObject* o, dnaView->getObjects()) {
        s.excludeObjectList.append(o);
    }

    QList<GObject*> objs = ProjectTreeItemSelectorDialog::selectObjects(s, this);
    if (objs.size() != 0) {
        GObject* go = objs.first();
        if (go->getGObjectType() == GObjectTypes::SEQUENCE) {
            editDNASeq = qobject_cast<U2SequenceObject*>(go);
            QString err = dnaView->addObject(editDNASeq);
            assert(err.isEmpty());
            indexOfChangedChars.clear();
        } else if (go->getGObjectType() == GObjectTypes::UNLOADED) {
            auto t = new LoadUnloadedDocumentTask(go->getDocument(),
                                                  LoadDocumentTaskConfig(false, GObjectReference(go)));
            connect(new TaskSignalMapper(t), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_onSequenceObjectLoaded(Task*)));
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
        }
    }
}

void ChromatogramView::sl_onSequenceObjectLoaded(Task* t) {
    auto lut = qobject_cast<LoadUnloadedDocumentTask*>(t);
    GObject* go = GObjectUtils::selectObjectByReference(lut->getConfig().checkObjRef,
                                                        lut->getDocument()->getObjects(),
                                                        UOF_LoadedOnly);
    if (go != nullptr) {
        editDNASeq = qobject_cast<U2SequenceObject*>(go);
        QString err = dnaView->addObject(editDNASeq);
        assert(err.isEmpty());
        indexOfChangedChars.clear();
        update();
    } else {
        qCritical("object not found!");
        Q_ASSERT(false);
    }
}

bool ChromatogramView::isWidgetOnlyObject(GObject* o) const {
    return o == editDNASeq;
}

void ChromatogramView::sl_clearEditableSequence() {
    if (editDNASeq == nullptr) {
        return;
    }
    dnaView->removeObject(editDNASeq);
}

void ChromatogramView::sl_removeChanges() {
    if (editDNASeq->isStateLocked()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("The sequence is locked"));
        return;
    }

    U2SequenceObject* seqObject = ctx->getSequenceObject();
    U2OpStatusImpl os;
    QByteArray sequence = seqObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    for (QSet<int>::const_iterator it = indexOfChangedChars.constBegin(); it != indexOfChangedChars.constEnd(); ++it) {
        currentBaseCalls[*it] = sequence[*it];
    }
    editDNASeq->setWholeSequence(DNASequence(currentBaseCalls));
    indexOfChangedChars.clear();
}

void ChromatogramView::sl_onObjectRemoved(GObjectViewController* view, GObject* obj) {
    Q_UNUSED(view);

    CHECK(obj == editDNASeq, );
    indexOfChangedChars.clear();
    editDNASeq = nullptr;
    update();
}

QAction* ChromatogramView::createToggleTraceAction(const QString& actionName) {
    auto showTraceAction = new QAction(actionName, this);
    showTraceAction->setCheckable(true);
    showTraceAction->setChecked(true);
    showTraceAction->setEnabled(true);
    connect(showTraceAction, SIGNAL(triggered(bool)), SLOT(sl_showHideTrace()));

    return showTraceAction;
}

void ChromatogramView::sl_showHideTrace() {
    auto traceAction = qobject_cast<QAction*>(sender());

    if (!traceAction) {
        return;
    }

    if (traceAction->text() == "A") {
        settings.drawTraceA = traceAction->isChecked();
    } else if (traceAction->text() == "C") {
        settings.drawTraceC = traceAction->isChecked();
    } else if (traceAction->text() == "G") {
        settings.drawTraceG = traceAction->isChecked();
    } else if (traceAction->text() == "T") {
        settings.drawTraceT = traceAction->isChecked();
    } else {
        assert(0);
    }

    completeUpdate();
}

void ChromatogramView::sl_showAllTraces() {
    settings.drawTraceA = true;
    settings.drawTraceC = true;
    settings.drawTraceG = true;
    settings.drawTraceT = true;
    QList<QAction*> actions = traceActionMenu->actions();
    foreach (QAction* action, actions) {
        action->setChecked(true);
    }
    completeUpdate();
}

//////////////////////////////////////
////render area

ChromatogramViewRenderArea::ChromatogramViewRenderArea(ChromatogramView* p, const Chromatogram& _chroma)
    : GSequenceLineViewRenderArea(p),
      linePen(Qt::gray, 1, Qt::DotLine),
      kLinearTransformTrace(0.0),
      bLinearTransformTrace(0.0) {
    setMinimumHeight(200);
    font.setFamily("Courier");
    font.setPointSize(12);
    fontBold = font;
    fontBold.setBold(true);
    QFontMetricsF fm(font);
    charWidth = fm.horizontalAdvance('W');
    charHeight = fm.ascent();
    heightPD = height();
    areaHeight = height() - heightAreaBC;

    chromatogram = _chroma;
    for (int i = 0; i < chromatogram->traceLength; i++) {
        if (chromaMax < chromatogram->A[i]) {
            chromaMax = chromatogram->A[i];
        }
        if (chromaMax < chromatogram->C[i]) {
            chromaMax = chromatogram->C[i];
        }
        if (chromaMax < chromatogram->G[i]) {
            chromaMax = chromatogram->G[i];
        }
        if (chromaMax < chromatogram->T[i]) {
            chromaMax = chromatogram->T[i];
        }
    }
    hasSel = false;
    if (chromatogram->hasQV && p->showQV()) {
        addUpIfQVL = 0;
    } else {
        addUpIfQVL = heightAreaBC - 2 * charHeight;
        setMinimumHeight(int(height() - addUpIfQVL));
        areaHeight = int(height() - heightAreaBC + addUpIfQVL);
    }
}

static const QList<QColor> colors = {Qt::darkGreen, Qt::blue, Qt::black, Qt::red};
static const QList<QString> bases = {"A", "C", "G", "T"};

void ChromatogramViewRenderArea::drawAll(QPaintDevice* pd) {
    static const qreal dividerTraceOrBaseCallsLines = 2;
    static const qreal dividerBoolShowBaseCallsChars = 1.5;

    auto chromaView = qobject_cast<ChromatogramView*>(view);

    const U2Region& visible = view->getVisibleRange();
    SAFE_POINT(!visible.isEmpty(), "Visible region is empty", );

    SequenceObjectContext* seqCtx = view->getSequenceContext();
    U2OpStatusImpl os;
    QByteArray seq = seqCtx->getSequenceObject()->getWholeSequenceData(os);
    SAFE_POINT_OP(os, );

    GSLV_UpdateFlags uf = view->getUpdateFlags();
    bool completeRedraw = uf.testFlag(GSLV_UF_NeedCompleteRedraw) || uf.testFlag(GSLV_UF_ViewResized) ||
                          uf.testFlag(GSLV_UF_VisibleRangeChanged);

    heightPD = height();

    if (completeRedraw) {
        linePen = QPen(AppContext::getMainWindow()->isDarkMode() ? Qt::lightGray : Qt::gray, 1, Qt::DotLine);
        QPainter p(getCachedPixmap());
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setFont(font);
        p.setPen(QPalette().text().color());
        p.fillRect(0, 0, pd->width(), heightPD, QPalette().base().color());
        if (pd->width() / charWidth > visible.length / dividerBoolShowBaseCallsChars) {
            // draw basecalls
            drawOriginalBaseCalls(0, heightAreaBC - charHeight - addUpIfQVL, width(), charHeight, p, visible, seq);

            if (chromatogram->hasQV && chromaView->showQV()) {
                drawQualityValues(0, charHeight, width(), heightAreaBC - 2 * charHeight, p, visible, seq);
            }
        } else {
            // Draw legend.
            QRectF rect(charWidth, 0, width() - 2 * charWidth, 2 * charHeight);
            p.drawText(rect, Qt::AlignCenter, QString(tr("Chromatogram view (zoom in to see base calls)")));
            int legendRectHeight = int(0.75 * charHeight);
            int legendRectWidth = int(0.41 * charWidth);
            int legendCharToRectPadding = int(charWidth / 2);
            int legendCharToCharPadding = int(charWidth);
            int legendX = int(width() - 4 * charWidth - 4 * legendRectWidth - 4 * legendCharToRectPadding - 3 * legendCharToCharPadding - 20);
            int legendY = heightAreaBC - int(charHeight);
            p.setPen(QPalette().text().color());
            bool isDark = AppContext::getMainWindow()->isDarkMode();
            for (int i = 0; i < 4; ++i) {
                p.fillRect(legendX, legendY - (charHeight - legendRectHeight) / 2, legendRectWidth, -legendRectHeight, McaColors::getChromatogramColorById(isDark, 1));
                legendX += legendRectWidth + legendCharToRectPadding;
                p.drawText(legendX, legendY, charWidth, -charHeight, Qt::AlignCenter, bases[i]);
                legendX += charWidth;
                legendX += legendCharToCharPadding;
            }
        }
        if (pd->width() / charWidth > visible.length / dividerTraceOrBaseCallsLines) {
            drawChromatogramTrace(0, heightAreaBC - addUpIfQVL, pd->width(), height() - heightAreaBC + addUpIfQVL, p, visible, chromaView->getSettings());
        } else {
            drawChromatogramBaseCallsLines(0, heightAreaBC, pd->width(), height() - heightAreaBC, p, visible, seq, chromaView->getSettings());
        }
    }
    QPainter p(pd);
    p.setFont(font);
    p.drawPixmap(0, 0, *cachedView);

    if (hasSel) {
        p.setPen(linePen);
        p.drawRect(selRect);
        hasSel = false;
    }

    if (pd->width() / charWidth > visible.length / dividerBoolShowBaseCallsChars && chromaView->editDNASeq != nullptr) {
        drawOriginalBaseCalls(0, 0, width(), charHeight, p, visible, chromaView->currentBaseCalls, false);
    }

    const QVector<U2Region>& sel = seqCtx->getSequenceSelection()->getSelectedRegions();
    if (!sel.isEmpty()) {
        // draw current selection
        QPen linePenSelection(Qt::darkGray, 1, Qt::SolidLine);
        p.setPen(linePenSelection);
        p.setRenderHint(QPainter::Antialiasing, false);

        U2Region self = sel.first();
        int i1 = int(self.startPos), i2 = int(self.endPos() - 1);
        qreal startBaseCall = kLinearTransformTrace * chromatogram->baseCalls[i1];
        qreal endBaseCall = kLinearTransformTrace * chromatogram->baseCalls[i2];
        if (i1 != 0) {
            qreal prevBaseCall = kLinearTransformTrace * chromatogram->baseCalls[i1 - 1];
            int lineX = int((startBaseCall + prevBaseCall) / 2 + bLinearTransformTrace);
            p.drawLine(lineX, 0, lineX, pd->height());
        } else {
            int lineX = int(startBaseCall + bLinearTransformTrace - charWidth / 2);
            p.drawLine(lineX, 0, lineX, pd->height());
        }
        if (i2 != chromatogram->seqLength - 1) {
            int nextBaseCall = int(kLinearTransformTrace * chromatogram->baseCalls[i2 + 1]);
            int lineX = int((endBaseCall + nextBaseCall) / 2 + bLinearTransformTrace);
            p.drawLine(lineX, 0, lineX, pd->height());
        } else {
            int lineX = int(endBaseCall + bLinearTransformTrace + charWidth / 2);
            p.drawLine(lineX, 0, lineX, pd->height());
        }
    }
}

void ChromatogramViewRenderArea::setAreaHeight(int newH) {
    areaHeight = newH;
}

qint64 ChromatogramViewRenderArea::coordToPos(const QPoint& coord) const {
    int x = coord.x();
    const U2Region& visibleRange = view->getVisibleRange();
    qreal lastBaseCall = kLinearTransformTrace * chromatogram->baseCalls[chromatogram->seqLength - 1] + bLinearTransformTrace;
    if (visibleRange.startPos + visibleRange.length == chromatogram->seqLength && x > lastBaseCall) {
        return chromatogram->seqLength;
    }
    qreal nearestPos = visibleRange.startPos;
    while (nearestPos < chromatogram->seqLength - 1) {
        qreal leftBaseCallPos = kLinearTransformTrace * chromatogram->baseCalls[nearestPos] + bLinearTransformTrace;
        qreal rightBaseCallPos = kLinearTransformTrace * chromatogram->baseCalls[nearestPos + 1] + bLinearTransformTrace;
        CHECK_BREAK((leftBaseCallPos + rightBaseCallPos) / 2 < x + (rightBaseCallPos - leftBaseCallPos) / 2);
        nearestPos++;
    }
    return qint64(nearestPos);
}

int ChromatogramViewRenderArea::posToCoord(qint64 p, bool useVirtualSpace) const {
    const U2Region& visibleRange = view->getVisibleRange();
    if (!useVirtualSpace && !visibleRange.contains(p) && p != visibleRange.endPos()) {
        return -1;
    }
    qreal res = kLinearTransformTrace * chromatogram->baseCalls[visibleRange.startPos + p] + bLinearTransformTrace;
    assert(useVirtualSpace || res <= width());
    return int(res);
}

QRectF ChromatogramViewRenderArea::posToRect(int i) const {
    QRectF r(kLinearTransformBaseCallsOfEdited * chromatogram->baseCalls[i] + bLinearTransformBaseCallsOfEdited - charWidth / 2, 0, charWidth, heightAreaBC - addUpIfQVL);
    return r;
}

// draw functions

void ChromatogramViewRenderArea::drawChromatogramTrace(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const ChromatogramViewSettings& settings) {
    if (chromaMax == 0) {
        // nothing to draw
        return;
    }

    // areaHeight how to define startValue?
    // colorForIds to private members
    bool isDark = AppContext::getMainWindow()->isDarkMode();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.resetTransform();
    p.translate(x, y + h);

    int a1 = chromatogram->baseCalls[visible.startPos];
    int a2 = chromatogram->baseCalls[visible.endPos() - 1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = charWidth;
    qreal k1 = w - leftMargin - rightMargin;
    int k2 = a2 - a1;
    kLinearTransformTrace = qreal(k1) / k2;
    bLinearTransformTrace = leftMargin - kLinearTransformTrace * a1;
    int mk1 = qMin(static_cast<int>(leftMargin / kLinearTransformTrace), a1);
    int mk2 = qMin(static_cast<int>(rightMargin / kLinearTransformTrace), chromatogram->traceLength - a2 - 1);
    int polylineSize = a2 - a1 + mk1 + mk2 + 1;
    QPolygonF polylineA(polylineSize), polylineC(polylineSize),
        polylineG(polylineSize), polylineT(polylineSize);
    qreal areaHeight = (heightPD - heightAreaBC + addUpIfQVL) * this->areaHeight / 100;
    for (int j = a1 - mk1; j <= a2 + mk2; ++j) {
        double lineX = kLinearTransformTrace * j + bLinearTransformTrace;
        qreal yA = -qMin(chromatogram->A[j] * areaHeight / chromaMax, h);
        qreal yC = -qMin(chromatogram->C[j] * areaHeight / chromaMax, h);
        qreal yG = -qMin(chromatogram->G[j] * areaHeight / chromaMax, h);
        qreal yT = -qMin(chromatogram->T[j] * areaHeight / chromaMax, h);
        polylineA[j - a1 + mk1] = QPointF(lineX, yA);
        polylineC[j - a1 + mk1] = QPointF(lineX, yC);
        polylineG[j - a1 + mk1] = QPointF(lineX, yG);
        polylineT[j - a1 + mk1] = QPointF(lineX, yT);
    }
    if (settings.drawTraceA) {
        p.setPen(McaColors::getChromatogramColorById(isDark, 0));
        p.drawPolyline(polylineA);
    }
    if (settings.drawTraceC) {
        p.setPen(McaColors::getChromatogramColorById(isDark, 1));
        p.drawPolyline(polylineC);
    }
    if (settings.drawTraceG) {
        p.setPen(McaColors::getChromatogramColorById(isDark, 2));
        p.drawPolyline(polylineG);
    }
    if (settings.drawTraceT) {
        p.setPen(McaColors::getChromatogramColorById(isDark, 3));
        p.drawPolyline(polylineT);
    }
    p.resetTransform();
}

void ChromatogramViewRenderArea::drawOriginalBaseCalls(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba, bool is) {
    QRectF rect;

    p.setPen(QPalette().text().color());
    p.resetTransform();
    p.translate(x, y + h);

    int a1 = chromatogram->baseCalls[visible.startPos];
    int a2 = chromatogram->baseCalls[visible.endPos() - 1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = charWidth;
    qreal k1 = w - leftMargin - rightMargin;
    int k2 = a2 - a1;
    qreal kLinearTransformBaseCalls = qreal(k1) / k2;
    qreal bLinearTransformBaseCalls = leftMargin - kLinearTransformBaseCalls * a1;

    if (!is) {
        kLinearTransformBaseCallsOfEdited = kLinearTransformBaseCalls;
        bLinearTransformBaseCallsOfEdited = bLinearTransformBaseCalls;
    }
    auto cview = qobject_cast<ChromatogramView*>(view);
    bool isDark = AppContext::getMainWindow()->isDarkMode();
    for (int i = int(visible.startPos); i < visible.endPos(); i++) {
        QColor color = McaColors::getChromatogramColorByBase(isDark, ba[i]);
        p.setPen(color);

        if (cview->indexOfChangedChars.contains(i) && !is) {
            p.setFont(fontBold);
        } else {
            p.setFont(font);
        }
        qreal xP = kLinearTransformBaseCalls * chromatogram->baseCalls[i] + bLinearTransformBaseCalls;
        rect.setRect(int(xP - charWidth / 2 + linePen.width()), -h, charWidth, h);
        p.drawText(rect, Qt::AlignCenter, QString(ba[i]));

        if (is) {
            p.setPen(linePen);
            p.setRenderHint(QPainter::Antialiasing, false);
            p.drawLine(int(xP), 0, int(xP), int(height() - y));
        }
    }

    if (is) {
        p.setPen(linePen);
        p.setFont(QFont(QString("Courier New"), 8));
        p.drawText(int(charWidth * 1.3), int(charHeight / 2), QString(tr("original sequence")));
    }
    p.resetTransform();
}

void ChromatogramViewRenderArea::drawQualityValues(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba) {
    QRectF rectangle;

    p.resetTransform();
    p.translate(x, y + h);

    // draw grid
    p.setPen(linePen);
    p.setRenderHint(QPainter::Antialiasing, false);
    for (int i = 0; i < 5; ++i) {
        p.drawLine(0, -int(h * i / 4), int(w), -int(h * i / 4));
    }

    QLinearGradient gradient(10, 0, 10, -h);
    gradient.setColorAt(0, Qt::green);
    gradient.setColorAt(0.33, Qt::yellow);
    gradient.setColorAt(0.66, Qt::red);
    QBrush brush(gradient);

    p.setBrush(brush);
    p.setPen(Qt::black);
    p.setRenderHint(QPainter::Antialiasing, true);

    int a1 = chromatogram->baseCalls[visible.startPos];
    int a2 = chromatogram->baseCalls[visible.endPos() - 1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = charWidth;
    qreal k1 = w - leftMargin - rightMargin;
    int k2 = a2 - a1;
    qreal kLinearTransformQV = qreal(k1) / k2;
    qreal bLinearTransformQV = leftMargin - kLinearTransformQV * a1;

    for (int i = int(visible.startPos); i < visible.endPos(); i++) {
        qreal xP = kLinearTransformQV * chromatogram->baseCalls[i] + bLinearTransformQV - charWidth / 2 + linePen.width();
        switch (ba[i]) {
            case 'A':
                rectangle.setCoords(xP, 0, xP + charWidth, -h / 100 * chromatogram->prob_A[i]);
                break;
            case 'C':
                rectangle.setCoords(xP, 0, xP + charWidth, -h / 100 * chromatogram->prob_C[i]);
                break;
            case 'G':
                rectangle.setCoords(xP, 0, xP + charWidth, -h / 100 * chromatogram->prob_G[i]);
                break;
            case 'T':
                rectangle.setCoords(xP, 0, xP + charWidth, -h / 100 * chromatogram->prob_T[i]);
                break;
        }
        if (qAbs(rectangle.height()) > h / 100) {
            p.drawRoundedRect(rectangle, 1.0, 1.0);
        }
    }

    p.resetTransform();
}

void ChromatogramViewRenderArea::drawChromatogramBaseCallsLines(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba, const ChromatogramViewSettings& settings) {
    p.setRenderHint(QPainter::Antialiasing, false);
    p.resetTransform();
    p.translate(x, y + h);

    /*  //drawBoundingRect
    p.drawLine(0,0,w,0);
    p.drawLine(0,-h,w,-h);
    p.drawLine(0,0,0,-h);
    p.drawLine(w,0,w,-h);*/

    int a1 = chromatogram->baseCalls[visible.startPos];
    int a2 = chromatogram->baseCalls[visible.endPos() - 1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = linePen.width();
    qreal k1 = w - leftMargin - rightMargin;
    int k2 = a2 - a1;
    kLinearTransformTrace = qreal(k1) / k2;
    bLinearTransformTrace = leftMargin - kLinearTransformTrace * a1;
    double yRes = 0;
    double areaHeight = (heightPD - heightAreaBC + addUpIfQVL) * this->areaHeight / 100;
    bool isDark = AppContext::getMainWindow()->isDarkMode();
    for (int j = int(visible.startPos); j < visible.startPos + visible.length; j++) {
        int temp = chromatogram->baseCalls[j];
        if (temp >= chromatogram->traceLength) {
            // damaged data - FIXME improve?
            break;
        }
        double lineX = kLinearTransformTrace * temp + bLinearTransformTrace;
        bool drawBase = true;
        switch (ba[j]) {
            case 'A':
                yRes = -qMin(chromatogram->A[temp] * areaHeight / chromaMax, h);
                p.setPen(McaColors::getChromatogramColorById(isDark, 0));
                drawBase = settings.drawTraceA;
                break;
            case 'C':
                yRes = -qMin(chromatogram->C[temp] * areaHeight / chromaMax, h);
                p.setPen(McaColors::getChromatogramColorById(isDark, 1));
                drawBase = settings.drawTraceC;
                break;
            case 'G':
                yRes = -qMin(chromatogram->G[temp] * areaHeight / chromaMax, h);
                p.setPen(McaColors::getChromatogramColorById(isDark, 2));
                drawBase = settings.drawTraceG;
                break;
            case 'T':
                yRes = -qMin(chromatogram->T[temp] * areaHeight / chromaMax, h);
                p.setPen(McaColors::getChromatogramColorById(isDark, 3));
                drawBase = settings.drawTraceT;
                break;
            case 'N':
                continue;
        };
        if (drawBase) {
            p.drawLine(int(lineX), 0, int(lineX), int(yRes));
        }
    }
    p.resetTransform();
}


}  // namespace U2
