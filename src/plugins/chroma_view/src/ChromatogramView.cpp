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

#include <QMessageBox>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
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

#include "ChromatogramView.h"

namespace U2 {

static const char GAP_CHAR = '-';

ChromatogramView::ChromatogramView(QWidget* p, ADVSequenceObjectContext* v, GSequenceLineView* cv, const DNAChromatogram& chroma)
: GSequenceLineView(p, v), editDNASeq(NULL)
{
    const QString objectName = "chromatogram_view_" + v->getSequenceGObject()->getGObjectName();
    setObjectName(objectName);

    dnaView = v->getAnnotatedDNAView();

    showQVAction = new QAction(tr("Show quality bars"), this);
    showQVAction->setIcon(QIcon(":chroma_view/images/bars.png"));
    showQVAction->setCheckable(true);
    showQVAction->setChecked(chroma.hasQV);
    showQVAction->setEnabled(chroma.hasQV);
    connect(showQVAction, SIGNAL(toggled(bool)), SLOT(completeUpdate()));

    showAllTraces = new QAction(tr("Show all"), this);
    connect(showAllTraces, SIGNAL(triggered()), SLOT(sl_showAllTraces()));

    traceActionMenu = new QMenu(tr("Show/hide trace"), this);
    traceActionMenu->addAction( createToggleTraceAction("A") );
    traceActionMenu->addAction( createToggleTraceAction("C") );
    traceActionMenu->addAction( createToggleTraceAction("G") );
    traceActionMenu->addAction( createToggleTraceAction("T") ) ;
    traceActionMenu->addSeparator();
    traceActionMenu->addAction(showAllTraces);

    renderArea = new ChromatogramViewRenderArea(this, chroma);

    scaleBar = new ScaleBar();
    scaleBar->setRange(100, 1000);
    scaleBar->setTickInterval(100);
    connect(scaleBar,SIGNAL(valueChanged(int)),SLOT(setRenderAreaHeight(int)));

    ra = static_cast<ChromatogramViewRenderArea *>(renderArea);
    scaleBar->setValue(int(ra->height() - ra->getHeightAreaBC() + ra->addUpIfQVL));

    setCoherentRangeView(cv);

    mP = new QMenu(this);

    mP->addAction(QString("A"));
    mP->addAction(QString("C"));
    mP->addAction(QString("G"));
    mP->addAction(QString("T"));
    mP->addAction(QString("N"));
    mP->addAction(QString(GAP_CHAR));
    connect(mP, SIGNAL(triggered(QAction*)),SLOT(sl_onPopupMenuCkicked(QAction*)));


    addNewSeqAction = new QAction(tr("Edit new sequence"), this);
    connect(addNewSeqAction, SIGNAL(triggered()), SLOT(sl_addNewSequenceObject()));

    addExistSeqAction = new QAction(tr("Edit existing sequence"),this);
    connect(addExistSeqAction, SIGNAL(triggered()), SLOT(sl_onAddExistingSequenceObject()));

    clearEditableSequence = new QAction(tr("Remove edited sequence"),this);
    connect(clearEditableSequence, SIGNAL(triggered()), SLOT(sl_clearEditableSequence()));

    removeChanges = new QAction(tr("Undo changes"),this);
    connect(removeChanges, SIGNAL(triggered()), SLOT(sl_removeChanges()));

    connect(dnaView, SIGNAL(si_objectRemoved(GObjectView*, GObject*)), SLOT(sl_onObjectRemoved(GObjectView*, GObject*)));
    pack();

    addActionToLocalToolbar(showQVAction);
    QToolButton* traceButton = addActionToLocalToolbar(traceActionMenu->menuAction());
    traceButton->setIcon(QIcon(":chroma_view/images/traces.png"));
    traceButton->setPopupMode(QToolButton::InstantPopup);
}

void ChromatogramView::pack() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->addWidget(renderArea);
    layout->addWidget(scaleBar);
    setContentLayout(layout);

    scrollBar->setHidden(true); //todo: support mode without scrollbar at all??

    setMinimumHeight(renderArea->minimumHeight());
}


void ChromatogramView::setRenderAreaHeight(int k)
{
    //k = chromaMax
    ChromatogramViewRenderArea* cvra = static_cast<ChromatogramViewRenderArea*>(renderArea);
    cvra->setAreaHeight(k);
    completeUpdate();
}

void ChromatogramView::buildPopupMenu(QMenu& m) {
    QPoint cpos = renderArea->mapFromGlobal(QCursor::pos());
    if (!renderArea->rect().contains(cpos)) {
        return;
    }
    //todo: move to submenus?
    QAction* before = GUIUtils::findActionAfter(m.actions(), ADV_MENU_ZOOM);

    m.insertAction(before, showQVAction);
    m.insertMenu(before, traceActionMenu);
    m.insertSeparator(before);
    if (editDNASeq != NULL) {
        m.insertAction(before, clearEditableSequence);
        m.insertAction(before, removeChanges);
    } else {
        m.insertAction(before, addNewSeqAction);
        m.insertAction(before, addExistSeqAction);
    }
    m.insertSeparator(before);

}

static const int MAX_DNA_LEN = 1000*1000*1000;

void ChromatogramView::mousePressEvent(QMouseEvent* me) {
    setFocus();
    if (me->button() == Qt::RightButton || editDNASeq == NULL) {
        GSequenceLineView::mousePressEvent(me);
        return;
    }
    if (editDNASeq != NULL && editDNASeq->getSequenceLength() > MAX_DNA_LEN) {
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
    int before=0;
    foreach(int gapIdx, gapIndexes) {
        if (gapIdx<bcIndex) {
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
            editDNASeq->replaceRegion(U2Region(editSeqIdx, 0), DNASequence(insData), os);//insert
        } else {
            if (newBase!=GAP_CHAR) {
                QByteArray insData(&newBase, 1);
                editDNASeq->replaceRegion(U2Region(editSeqIdx, 1), DNASequence(insData), os); //replace
            } else {
                editDNASeq->replaceRegion(U2Region(editSeqIdx, 1), DNASequence(), os); //remove
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

    assert(editDNASeq == NULL);

    AddNewDocumentDialogModel m;
    DocumentFormatConstraints c;

    GUrl seqUrl = ctx->getSequenceGObject()->getDocument()->getURL();
    m.url = GUrlUtils::rollFileName(seqUrl.dirPath() + "/" + seqUrl.baseFileName() + "_sequence.fa", DocumentUtils::getNewDocFileNameExcludesHint());
    c.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    c.addFlagToExclude(DocumentFormatFlag_Hidden);
    c.supportedObjectTypes += GObjectTypes::SEQUENCE;
    AddNewDocumentDialogController::run(NULL, m, c);
    if (!m.successful) {
        return;
    }
    Project* p = AppContext::getProject();

    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(m.format);
    SAFE_POINT(NULL != format, QString("Format is not registered: '%1'").arg(m.format), );
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

    assert(editDNASeq == NULL);

    ProjectTreeControllerModeSettings s;
    s.allowMultipleSelection = false;
    s.objectTypesToShow.insert(GObjectTypes::SEQUENCE);
    U2SequenceObjectConstraints ac;
    ac.sequenceSize = ctx->getSequenceLength();
    s.objectConstraints.insert(&ac);
    ac.alphabetType = ctx->getSequenceObject()->getAlphabet()->getType();
    s.groupMode = ProjectTreeGroupMode_ByDocument;
    s.ignoreRemoteObjects = true;
    foreach (GObject* o, dnaView->getObjects()) {
        s.excludeObjectList.append(o);
    }

    QList<GObject*> objs = ProjectTreeItemSelectorDialog::selectObjects(s, this);
    if (objs.size()!=0) {
        GObject* go = objs.first();
        if (go->getGObjectType() == GObjectTypes::SEQUENCE) {
            editDNASeq = qobject_cast<U2SequenceObject*>(go);
            QString err = dnaView->addObject(editDNASeq);
            assert(err.isEmpty());
            indexOfChangedChars.clear();
        } else if (go->getGObjectType() == GObjectTypes::UNLOADED) {
            LoadUnloadedDocumentTask* t = new LoadUnloadedDocumentTask(go->getDocument(),
                LoadDocumentTaskConfig(false, GObjectReference(go)));
            connect(new TaskSignalMapper(t), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_onSequenceObjectLoaded(Task*)));
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
        }
    }
}

void ChromatogramView::sl_onSequenceObjectLoaded(Task* t) {
    LoadUnloadedDocumentTask* lut = qobject_cast<LoadUnloadedDocumentTask*>(t);
    GObject* go = GObjectUtils::selectObjectByReference(lut->getConfig().checkObjRef,
        lut->getDocument()->getObjects(), UOF_LoadedOnly);
    if (go!=NULL) {
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
    if (editDNASeq == NULL) {
        return;
    }
    dnaView->removeObject(editDNASeq);
}

void ChromatogramView::sl_removeChanges()   {
    if (editDNASeq->isStateLocked()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("The sequence is locked"));
        return;
    }

    U2SequenceObject* seqObject = ctx->getSequenceObject();
    U2OpStatusImpl os;
    QByteArray sequence = seqObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    for (QSet<int>::const_iterator it = indexOfChangedChars.constBegin(); it != indexOfChangedChars.constEnd(); ++it)  {
        currentBaseCalls[*it] = sequence[*it];
    }
    editDNASeq->setWholeSequence(DNASequence(currentBaseCalls));
    indexOfChangedChars.clear();
}

void ChromatogramView::sl_onObjectRemoved(GObjectView* view, GObject* obj)  {
    Q_UNUSED(view);

    CHECK(obj == editDNASeq, );
    indexOfChangedChars.clear();
    editDNASeq = NULL;
    update();
}

QAction* ChromatogramView::createToggleTraceAction(const QString& actionName)
{
    QAction* showTraceAction = new QAction(actionName, this);
    showTraceAction->setCheckable(true);
    showTraceAction->setChecked(true);
    showTraceAction->setEnabled(true);
    connect(showTraceAction, SIGNAL(triggered(bool)), SLOT(sl_showHideTrace()));

    return showTraceAction;
}

void ChromatogramView::sl_showHideTrace()
{
    QAction* traceAction = qobject_cast<QAction*> (sender());

    if (!traceAction) {
        return;
    }

    if (traceAction->text() == "A") {
        settings.drawTraceA = traceAction->isChecked();
    } else if (traceAction->text() == "C") {
        settings.drawTraceC = traceAction->isChecked();
    } else if(traceAction->text() == "G") {
        settings.drawTraceG = traceAction->isChecked();
    } else if(traceAction->text() == "T") {
        settings.drawTraceT = traceAction->isChecked();
    } else {
        assert(0);
    }

    completeUpdate();


}

void ChromatogramView::sl_showAllTraces()
{
    settings.drawTraceA = true;
    settings.drawTraceC = true;
    settings.drawTraceG = true;
    settings.drawTraceT = true;
    QList<QAction*> actions = traceActionMenu->actions();
    foreach(QAction* action, actions) {
        action->setChecked(true);
    }
    completeUpdate();

}

//////////////////////////////////////
////render area
ChromatogramViewRenderArea::ChromatogramViewRenderArea(ChromatogramView* p, const DNAChromatogram& _chroma) :
    GSequenceLineViewRenderArea(p),
    linePen(Qt::gray, 1, Qt::DotLine),
    kLinearTransformTrace(0.0),
    bLinearTransformTrace(0.0)
{
    setMinimumHeight(200);
    font.setFamily("Courier");
    font.setPointSize(12);
    fontBold = font;
    fontBold.setBold(true);
    QFontMetricsF fm(font);
    charWidth = fm.width('W');
    charHeight = fm.ascent();
    heightPD = height();
    heightAreaBC = 50;
    areaHeight = height()-heightAreaBC;

    chroma = _chroma;
    chromaMax = 0;
    for (int i = 0; i < chroma.traceLength; i++)
    {
        if (chromaMax<chroma.A[i]) chromaMax=chroma.A[i];
        if (chromaMax<chroma.C[i]) chromaMax=chroma.C[i];
        if (chromaMax<chroma.G[i]) chromaMax=chroma.G[i];
        if (chromaMax<chroma.T[i]) chromaMax=chroma.T[i];
    }
    hasSel = false;
    if (chroma.hasQV && p->showQV()) {
        addUpIfQVL = 0;
    }
    else    {
        addUpIfQVL = heightAreaBC - 2*charHeight;
        setMinimumHeight(int(height() - addUpIfQVL));
        areaHeight = int(height() - heightAreaBC + addUpIfQVL);
    }
}

ChromatogramViewRenderArea::~ChromatogramViewRenderArea()
{
}


void ChromatogramViewRenderArea::drawAll(QPaintDevice* pd) {
    static const QColor colorForIds[4] = { Qt::darkGreen, Qt::blue, Qt::black, Qt::red};
    static const QString baseForIds[4] = { "A", "C", "G", "T" };
    static const qreal dividerTraceOrBaseCallsLines = 2;
    static const qreal dividerBoolShowBaseCallsChars = 1.5;

    ChromatogramView* chromaView = qobject_cast<ChromatogramView*>(view);

    const U2Region& visible = view->getVisibleRange();
    assert(!visible.isEmpty());

    SequenceObjectContext* seqCtx = view->getSequenceContext();
    U2OpStatusImpl os;
    QByteArray seq = seqCtx->getSequenceObject()->getWholeSequenceData(os);
    SAFE_POINT_OP(os, );

    GSLV_UpdateFlags uf = view->getUpdateFlags();
    bool completeRedraw = uf.testFlag(GSLV_UF_NeedCompleteRedraw) || uf.testFlag(GSLV_UF_ViewResized) ||
        uf.testFlag(GSLV_UF_VisibleRangeChanged);

    heightPD = height();

    if (completeRedraw) {
        QPainter p(cachedView);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setFont(font);
        p.setPen(Qt::black);
        p.fillRect(0, 0, pd->width(), heightPD, Qt::white);
        if (pd->width()/charWidth>visible.length/dividerBoolShowBaseCallsChars) {
            //draw basecalls
            drawOriginalBaseCalls(0, heightAreaBC-charHeight-addUpIfQVL, width(), charHeight, p, visible, seq);

            if (chroma.hasQV && chromaView->showQV()) {
                drawQualityValues(0, charHeight, width(), heightAreaBC - 2*charHeight, p, visible, seq);
            }
        } else {
            QRectF rect(charWidth, 0, width() - 2*charWidth, 2*charHeight);
            p.drawText(rect, Qt::AlignCenter, QString(tr("Chromatogram view (zoom in to see base calls)")));
            qreal curCP = width() - charWidth;
            for (int i = 0; i < 4; ++i) {
                curCP-= 2*charWidth;
                p.setPen(colorForIds[i]);
                p.drawRect(int(curCP+charWidth/6), int(heightAreaBC-charHeight), int(charWidth/2), -int(charHeight/2));
                p.setPen(Qt::black);
                p.drawText(int(curCP + charWidth), int(heightAreaBC - charHeight), baseForIds[i]);
            }
        }
        if (pd->width()/charWidth>visible.length/dividerTraceOrBaseCallsLines) {
            drawChromatogramTrace(0, heightAreaBC - addUpIfQVL, pd->width(), height() - heightAreaBC + addUpIfQVL,
                p, visible, chromaView->getSettings());
        } else {
            drawChromatogramBaseCallsLines(0, heightAreaBC, pd->width(), height() - heightAreaBC,
                p, visible, seq, chromaView->getSettings());
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

    if (pd->width() / charWidth > visible.length /dividerBoolShowBaseCallsChars && chromaView->editDNASeq!=NULL) {
        drawOriginalBaseCalls(0, 0, width(), charHeight, p, visible, chromaView->currentBaseCalls, false);
    }

    const QVector<U2Region>& sel=seqCtx->getSequenceSelection()->getSelectedRegions();
    if(!sel.isEmpty()) {
        //draw current selection
        //selection base on trace transform coef
        QPen linePenSelection(Qt::darkGray, 1, Qt::SolidLine);
        p.setPen(linePenSelection);
        p.setRenderHint(QPainter::Antialiasing, false);

        U2Region self=sel.first();
        int i1 = int(self.startPos), i2 = int(self.endPos() - 1);
        qreal startBaseCall = kLinearTransformTrace * chroma.baseCalls[i1];
        qreal endBaseCall = kLinearTransformTrace * chroma.baseCalls[i2];
        if (i1 != 0) {
            qreal prevBaseCall = kLinearTransformTrace * chroma.baseCalls[i1 - 1];
            int lineX = int((startBaseCall + prevBaseCall) / 2 + bLinearTransformTrace);
            p.drawLine(lineX, 0, lineX, pd->height());
        } else {
            int lineX = int(startBaseCall + bLinearTransformTrace - charWidth / 2);
            p.drawLine(lineX, 0, lineX, pd->height());
        }
        if (i2 != chroma.seqLength - 1) {
            int nextBaseCall = int(kLinearTransformTrace * chroma.baseCalls[i2 + 1]);
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

qint64 ChromatogramViewRenderArea::coordToPos(int c) const {
    const U2Region& visibleRange = view->getVisibleRange();
    qreal lastBaseCall = kLinearTransformTrace * chroma.baseCalls[chroma.seqLength-1] + bLinearTransformTrace;
    if (visibleRange.startPos + visibleRange.length == chroma.seqLength && c > lastBaseCall) {
        return chroma.seqLength;
    }
    qreal nearestPos = visibleRange.startPos;
    while (nearestPos < chroma.seqLength - 1) {
        qreal leftBaseCallPos = kLinearTransformTrace * chroma.baseCalls[nearestPos] + bLinearTransformTrace;
        qreal rightBaseCallPos = kLinearTransformTrace * chroma.baseCalls[nearestPos + 1] + bLinearTransformTrace;
        CHECK_BREAK((leftBaseCallPos + rightBaseCallPos) / 2 < c + (rightBaseCallPos - leftBaseCallPos) / 2);
        nearestPos++;
    }
    return qint64(nearestPos);
}

int ChromatogramViewRenderArea::posToCoord(qint64 p, bool useVirtualSpace) const {
    const U2Region& visibleRange = view->getVisibleRange();
    if (!useVirtualSpace && !visibleRange.contains(p) && p!=visibleRange.endPos()) {
        return -1;
    }
    qreal res = kLinearTransformTrace*chroma.baseCalls[visibleRange.startPos+p]+bLinearTransformTrace;
    assert(useVirtualSpace || res <= width());
    return int(res);
}

QRectF ChromatogramViewRenderArea::posToRect(int i) const {
    QRectF r(kLinearTransformBaseCallsOfEdited*chroma.baseCalls[i] + bLinearTransformBaseCallsOfEdited - charWidth/2, 0, charWidth, heightAreaBC-addUpIfQVL);
    return r;
}





//draw functions

void ChromatogramViewRenderArea::drawChromatogramTrace(qreal x, qreal y, qreal w, qreal h, QPainter& p,
                                                       const U2Region& visible, const ChromatogramViewSettings& settings)
{
    if (chromaMax == 0) {
        //nothing to draw
        return;
    }
    //founding problems

    //areaHeight how to define startValue?
    //colorForIds to private members
    static const QColor colorForIds[4] = {
        Qt::darkGreen, Qt::blue, Qt::black, Qt::red
    };
    p.setRenderHint(QPainter::Antialiasing, true);
    p.resetTransform();
    p.translate(x,y+h);

    //drawBoundingRect
    /*  p.drawLine(0,0,w,0);
    p.drawLine(0,-h,w,-h);
    p.drawLine(0,0,0,-h);
    p.drawLine(w,0,w,-h);*/


    int a1 = chroma.baseCalls[visible.startPos];
    int a2 = chroma.baseCalls[visible.endPos()-1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = charWidth;
    qreal k1 = w - leftMargin  - rightMargin;
    int k2 = a2 - a1;
    kLinearTransformTrace = qreal (k1) / k2;
    bLinearTransformTrace = leftMargin - kLinearTransformTrace*a1;
    int mk1 = qMin(static_cast<int>(leftMargin / kLinearTransformTrace), a1);
    int mk2 = qMin(static_cast<int>(rightMargin / kLinearTransformTrace), chroma.traceLength - a2 - 1);
    int polylineSize = a2-a1+mk1+mk2+1;
    QPolygonF polylineA(polylineSize), polylineC(polylineSize),
        polylineG(polylineSize), polylineT(polylineSize);
    qreal areaHeight = (heightPD - heightAreaBC + addUpIfQVL) * this->areaHeight / 100;
    for (int j = a1-mk1; j <= a2+mk2; ++j) {
        double lineX = kLinearTransformTrace*j+bLinearTransformTrace;
        qreal yA = -qMin(chroma.A[j] * areaHeight / chromaMax, h);
        qreal yC = -qMin(chroma.C[j] * areaHeight / chromaMax, h);
        qreal yG = -qMin(chroma.G[j] * areaHeight / chromaMax, h);
        qreal yT = -qMin(chroma.T[j] * areaHeight / chromaMax, h);
        polylineA[j-a1+mk1] = QPointF(lineX, yA);
        polylineC[j-a1+mk1] = QPointF(lineX, yC);
        polylineG[j-a1+mk1] = QPointF(lineX, yG);
        polylineT[j-a1+mk1] = QPointF(lineX, yT);
    }
    if (settings.drawTraceA) {
        p.setPen(colorForIds[0]);
        p.drawPolyline(polylineA);
    }
    if (settings.drawTraceC) {
        p.setPen(colorForIds[1]);
        p.drawPolyline(polylineC);
    }
    if (settings.drawTraceG) {
        p.setPen(colorForIds[2]);
        p.drawPolyline(polylineG);
    }
    if (settings.drawTraceT) {
        p.setPen(colorForIds[3]);
        p.drawPolyline(polylineT);
    }
    p.resetTransform();
}

void ChromatogramViewRenderArea::drawOriginalBaseCalls(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba, bool is)
{
    QRectF rect;


    p.setPen(Qt::black);
    p.resetTransform();
    p.translate(x,y+h);


    int a1 = chroma.baseCalls[visible.startPos];
    int a2 = chroma.baseCalls[visible.endPos()-1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = charWidth;
    qreal k1 = w - leftMargin  - rightMargin;
    int k2 = a2 - a1;
    qreal kLinearTransformBaseCalls = qreal (k1) / k2;
    qreal bLinearTransformBaseCalls = leftMargin - kLinearTransformBaseCalls*a1;

    if (!is)    {
        kLinearTransformBaseCallsOfEdited = kLinearTransformBaseCalls;
        bLinearTransformBaseCallsOfEdited = bLinearTransformBaseCalls;
    }
    ChromatogramView* cview = qobject_cast<ChromatogramView*>(view);
    for (int i = int(visible.startPos); i < visible.endPos(); i++) {
        QColor color = getBaseColor(ba[i]);
        p.setPen(color);

        if (cview->indexOfChangedChars.contains(i) && !is) {
            p.setFont(fontBold);
        } else {
            p.setFont(font);
        }
        qreal xP = kLinearTransformBaseCalls*chroma.baseCalls[i] + bLinearTransformBaseCalls;
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

void ChromatogramViewRenderArea::drawQualityValues(qreal x, qreal y, qreal w, qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba)
{
    QRectF rectangle;

    p.resetTransform();
    p.translate(x,y+h);

    //draw grid
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



     int a1 = chroma.baseCalls[visible.startPos];
     int a2 = chroma.baseCalls[visible.endPos()-1];
     qreal leftMargin, rightMargin;
     leftMargin = rightMargin = charWidth;
     qreal k1 = w - leftMargin  - rightMargin;
     int k2 = a2 - a1;
     qreal kLinearTransformQV = qreal (k1) / k2;
     qreal bLinearTransformQV = leftMargin - kLinearTransformQV*a1;

    for (int i = int(visible.startPos); i < visible.endPos(); i++) {
        qreal xP = kLinearTransformQV * chroma.baseCalls[i] + bLinearTransformQV - charWidth / 2 + linePen.width();
         switch (ba[i])  {
             case 'A':
                 rectangle.setCoords(xP, 0, xP+charWidth, -h/100*chroma.prob_A[i]);
                 break;
             case 'C':
                 rectangle.setCoords(xP, 0, xP+charWidth, -h/100*chroma.prob_C[i]);
                 break;
             case 'G':
                 rectangle.setCoords(xP, 0, xP+charWidth, -h/100*chroma.prob_G[i]);
                 break;
             case 'T':
                 rectangle.setCoords(xP, 0, xP+charWidth, -h/100*chroma.prob_T[i]);
                 break;
         }
         if (qAbs( rectangle.height() ) > h/100) {
            p.drawRoundedRect(rectangle, 1.0, 1.0);
         }

     }

     p.resetTransform();
}


void ChromatogramViewRenderArea::drawChromatogramBaseCallsLines(qreal x, qreal y, qreal w, qreal h, QPainter& p,
                                                                const U2Region& visible, const QByteArray& ba, const ChromatogramViewSettings& settings)
{
    static const QColor colorForIds[4] = {
        Qt::darkGreen, Qt::blue, Qt::black, Qt::red
    };
    p.setRenderHint(QPainter::Antialiasing, false);
    p.resetTransform();
    p.translate(x,y+h);

    /*  //drawBoundingRect
    p.drawLine(0,0,w,0);
    p.drawLine(0,-h,w,-h);
    p.drawLine(0,0,0,-h);
    p.drawLine(w,0,w,-h);*/


    int a1 = chroma.baseCalls[visible.startPos];
    int a2 = chroma.baseCalls[visible.endPos()-1];
    qreal leftMargin, rightMargin;
    leftMargin = rightMargin = linePen.width();
    qreal k1 = w - leftMargin  - rightMargin;
    int k2 = a2 - a1;
    kLinearTransformTrace = qreal (k1) / k2;
    bLinearTransformTrace = leftMargin - kLinearTransformTrace*a1;
    double yRes = 0;
    double areaHeight = (heightPD - heightAreaBC + addUpIfQVL) * this->areaHeight / 100;
    for (int j = int(visible.startPos); j < visible.startPos+visible.length; j++) {
        int temp = chroma.baseCalls[j];
        if (temp >= chroma.traceLength) {
            // damaged data - FIXME improve?
            break;
        }
        double lineX = kLinearTransformTrace*temp+bLinearTransformTrace;
        bool drawBase = true;
        switch (ba[j])  {
            case 'A':
                yRes = -qMin(chroma.A[temp] * areaHeight / chromaMax, h);
                p.setPen(colorForIds[0]);
                drawBase = settings.drawTraceA;
                break;
            case 'C':
                yRes = -qMin(chroma.C[temp] * areaHeight / chromaMax, h);
                p.setPen(colorForIds[1]);
                drawBase = settings.drawTraceC;
                break;
            case 'G':
                yRes = -qMin(chroma.G[temp] * areaHeight / chromaMax, h);
                p.setPen(colorForIds[2]);
                drawBase = settings.drawTraceG;
                break;
            case 'T':
                yRes = -qMin(chroma.T[temp] * areaHeight / chromaMax, h);
                p.setPen(colorForIds[3]);
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

QColor ChromatogramViewRenderArea::getBaseColor( char base )
{

    switch(base) {
        case 'A':
            return Qt::darkGreen;
        case 'C':
            return Qt::blue;
        case 'G':
            return Qt::black;
        case 'T':
            return Qt::red;
        default:
            return Qt::black;
    }

}




} // namespace
