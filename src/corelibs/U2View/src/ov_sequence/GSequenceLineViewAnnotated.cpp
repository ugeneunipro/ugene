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

#include "GSequenceLineViewAnnotated.h"

#include <QApplication>
#include <QMenu>
#include <QPainterPath>
#include <QToolTip>

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/Timer.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/GUIUtils.h>

#include "ADVSequenceObjectContext.h"

namespace U2 {

GSequenceLineViewAnnotated::GSequenceLineViewAnnotated(QWidget* p, SequenceObjectContext* ctx)
    : GSequenceLineView(p, ctx) {
    const QSet<AnnotationTableObject*> aObjs = ctx->getAnnotationObjects(true);
    foreach (const AnnotationTableObject* ao, aObjs) {
        connectAnnotationObject(ao);
    }
    connect(ctx->getAnnotationsSelection(),
            SIGNAL(si_selectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&)),
            SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&)));

    connect(ctx, SIGNAL(si_annotationObjectAdded(AnnotationTableObject*)), SLOT(sl_onAnnotationObjectAdded(AnnotationTableObject*)));
    connect(ctx, SIGNAL(si_annotationObjectRemoved(AnnotationTableObject*)), SLOT(sl_onAnnotationObjectRemoved(AnnotationTableObject*)));
    connect(ctx, SIGNAL(si_annotationActivated(Annotation*, int)), SLOT(sl_onAnnotationActivated(Annotation*, int)));

    connect(AppContext::getAnnotationsSettingsRegistry(), SIGNAL(si_annotationSettingsChanged(const QStringList&)), SLOT(sl_onAnnotationSettingsChanged(const QStringList&)));
}

void GSequenceLineViewAnnotated::connectAnnotationObject(const AnnotationTableObject* ao) {
    connect(ao, SIGNAL(si_onAnnotationsAdded(const QList<Annotation*>&)), SLOT(sl_onAnnotationsAdded(const QList<Annotation*>&)));
    connect(ao, SIGNAL(si_onAnnotationsRemoved(const QList<Annotation*>&)), SLOT(sl_onAnnotationsRemoved(const QList<Annotation*>&)));
    connect(ao, SIGNAL(si_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*)), SLOT(sl_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*)));
    connect(ao, SIGNAL(si_onAnnotationsModified(const QList<AnnotationModification>&)), SLOT(sl_onAnnotationsModified(const QList<AnnotationModification>&)));
}

void GSequenceLineViewAnnotated::sl_onAnnotationSettingsChanged(const QStringList&) {
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void GSequenceLineViewAnnotated::sl_onAnnotationObjectAdded(AnnotationTableObject* o) {
    connectAnnotationObject(o);
    sl_onAnnotationsAdded(o->getAnnotations());
}

void GSequenceLineViewAnnotated::sl_onAnnotationObjectRemoved(AnnotationTableObject* o) {
    o->disconnect(this);
    sl_onAnnotationsRemoved(o->getAnnotations());
}

void GSequenceLineViewAnnotated::sl_onAnnotationsAdded(const QList<Annotation*>& l) {
    GTIMER(c2, t2, "GSequenceLineViewAnnotated::sl_onAnnotationsAdded");
    registerAnnotations(l);
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void GSequenceLineViewAnnotated::sl_onAnnotationsRemoved(const QList<Annotation*>& l) {
    unregisterAnnotations(l);
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void GSequenceLineViewAnnotated::sl_onAnnotationsInGroupRemoved(const QList<Annotation*>& l, AnnotationGroup*) {
    ClearAnnotationsTask* task = new ClearAnnotationsTask(l, this);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

ClearAnnotationsTask::ClearAnnotationsTask(const QList<Annotation*>& list, GSequenceLineViewAnnotated* view)
    : Task("Clear annotations", TaskFlag_None), l(list), view(view) {
}

void ClearAnnotationsTask::run() {
    view->unregisterAnnotations(l);
}

Task::ReportResult ClearAnnotationsTask::report() {
    view->addUpdateFlags(GSLV_UF_AnnotationsChanged);
    view->update();
    return ReportResult_Finished;
}

void GSequenceLineViewAnnotated::sl_onAnnotationActivated(U2::Annotation* annotation, int regionIndex) {
    const QSet<AnnotationTableObject*> aos = ctx->getAnnotationObjects(true);
    if (aos.contains(annotation->getGObject())) {
        ensureVisible(annotation, regionIndex);
    }
}

void GSequenceLineViewAnnotated::sl_onAnnotationSelectionChanged(AnnotationSelection* /*as*/, const QList<Annotation*>& _added, const QList<Annotation*>& _removed) {
    const QSet<AnnotationTableObject*> aos = ctx->getAnnotationObjects(true);

    bool changed = false;
    const QList<Annotation*> added = ctx->selectRelatedAnnotations(_added);
    const QList<Annotation*> removed = ctx->selectRelatedAnnotations(_removed);

    if (!changed) {
        foreach (Annotation* a, added) {
            if (aos.contains(a->getGObject()) && isAnnotationVisible(a)) {
                changed = true;
                break;
            }
        }
        if (!changed) {
            foreach (Annotation* a, removed) {
                if (aos.contains(a->getGObject()) && isAnnotationVisible(a)) {
                    changed = true;
                    break;
                }
            }
        }
    }

    if (changed) {
        addUpdateFlags(GSLV_UF_SelectionChanged);
        update();
    }
}

bool GSequenceLineViewAnnotated::isAnnotationVisible(const Annotation* a) const {
    foreach (const U2Region& r, a->getRegions()) {
        if (visibleRange.intersects(r)) {
            return true;
        }
    }
    return false;
}

QList<Annotation*> GSequenceLineViewAnnotated::findAnnotationsByCoord(const QPoint& renderAreaPoint) const {
    auto renderAreaAnnotated = qobject_cast<GSequenceLineViewAnnotatedRenderArea*>(renderArea);
    SAFE_POINT(renderAreaAnnotated != nullptr, "GSequenceLineViewAnnotated must have GSequenceLineViewAnnotatedRenderArea!", {});
    return renderAreaAnnotated->findAnnotationsByCoord(renderAreaPoint);
}

void GSequenceLineViewAnnotated::mousePressEvent(QMouseEvent* me) {
    setFocus();
    const QPoint renderAreaPoint = toRenderAreaPoint(me->pos());
    const QPoint p = toRenderAreaPoint(me->pos());
    const Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
    const bool singleBaseSelectionMode = km.testFlag(Qt::AltModifier);
    bool annotationEvent = false;  // true if mouse pressed in some annotation area
    if (renderArea->rect().contains(p) && me->button() == Qt::LeftButton && !singleBaseSelectionMode) {
        const Qt::KeyboardModifiers mouseEventModifiers = me->modifiers();
        const bool controlOrShiftPressed = mouseEventModifiers.testFlag(Qt::ControlModifier) || mouseEventModifiers.testFlag(Qt::ShiftModifier);
        QList<Annotation*> annotations = findAnnotationsByCoord(p);
        annotationEvent = !annotations.isEmpty();
        if ((!controlOrShiftPressed || !annotationEvent) && cursor().shape() == Qt::ArrowCursor) {
            ctx->getAnnotationsSelection()->clear();
            ctx->getSequenceSelection()->clear();
            ctx->emitClearSelectedAnnotationRegions();
        }
        if (annotationEvent && cursor().shape() == Qt::ArrowCursor) {
            Annotation* annotation = annotations.first();
            if (annotations.size() > 1) {
                AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
                QMenu popup;
                foreach (const Annotation* as, annotations) {
                    const SharedAnnotationData& aData = as->getData();
                    const U2Region r = as->getRegions().first();
                    const QString text = aData->name + QString(" [%1, %2]").arg(r.startPos + 1).arg(r.endPos());
                    AnnotationSettings* asettings = asr->getAnnotationSettings(aData);
                    const QIcon icon = GUIUtils::createSquareIcon(asettings->color, 10);
                    popup.addAction(icon, text);
                }
                QAction* a = popup.exec(QCursor::pos());
                if (a == nullptr) {
                    annotation = nullptr;
                } else {
                    int idx = popup.actions().indexOf(a);
                    annotation = annotations[idx];
                }
            }
            if (nullptr != annotation) {
                QVector<U2Region> annotationRegions = annotation->getRegions();
                bool processAllRegions = U1AnnotationUtils::isAnnotationContainsJunctionPoint(annotation, seqLen);
                if (processAllRegions) {
                    ctx->emitAnnotationActivated(annotation, -1);
                } else {
                    qint64 mousePressPos = renderArea->coordToPos(renderAreaPoint);
                    for (int i = 0; i < annotationRegions.size(); i++) {
                        const U2Region& region = annotationRegions[i];
                        if (region.contains(mousePressPos)) {
                            ctx->emitAnnotationActivated(annotation, i);
                        }
                    }
                }
            }
        }
    }
    // a hint to parent class: if mouse action leads to annotation selection -> skip selection handling for mouse press
    ignoreMouseSelectionEvents = annotationEvent;
    GSequenceLineView::mousePressEvent(me);
    ignoreMouseSelectionEvents = false;
}

void GSequenceLineViewAnnotated::mouseDoubleClickEvent(QMouseEvent* me) {
    const QPoint renderAreaPoint = toRenderAreaPoint(me->pos());
    QList<Annotation*> selection = findAnnotationsByCoord(renderAreaPoint);
    if (selection.isEmpty()) {
        GSequenceLineView::mouseDoubleClickEvent(me);
        return;
    }
    Annotation* annotation = selection.first();
    // Using any of modifiers (compatibility with older UGENE behavior).
    bool expandSelection = me->modifiers() == Qt::ControlModifier || me->modifiers() == Qt::ShiftModifier;
    if (!expandSelection) {
        ctx->emitClearSelectedAnnotationRegions();
    }
    const QVector<U2Region> annotationRegions = annotation->getRegions();
    qint64 renderAreaPos = renderArea->coordToPos(renderAreaPoint);
    foreach (const U2Region& region, annotationRegions) {
        CHECK_CONTINUE(region.contains(renderAreaPos));

        ctx->emitAnnotationDoubleClicked(annotation, annotationRegions.indexOf(region));
        break;
    }
}

//! VIEW_RENDERER_REFACTORING: used only in CV, doubled in SequenceViewAnnotetedRenderer.
//! Apply renederer logic to CV and remove this method.
QString GSequenceLineViewAnnotated::prepareAnnotationText(const SharedAnnotationData& a, const AnnotationSettings* as) {
    if (!as->showNameQuals || as->nameQuals.isEmpty()) {
        return a->name;
    }
    QVector<U2Qualifier> qs;
    foreach (const QString& qn, as->nameQuals) {
        qs.clear();
        a->findQualifiers(qn, qs);
        if (!qs.isEmpty()) {
            QString res = qs[0].value;
            return res;
        }
    }
    return a->name;
}

QList<Annotation*> GSequenceLineViewAnnotated::findAnnotationsInRange(const U2Region& range) const {
    QList<Annotation*> result;
    const QSet<AnnotationTableObject*> aObjs = ctx->getAnnotationObjects(true);
    foreach (AnnotationTableObject* ao, aObjs) {
        result << ao->getAnnotationsByRegion(range);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
/// Renderer

GSequenceLineViewAnnotatedRenderArea::GSequenceLineViewAnnotatedRenderArea(GSequenceLineViewAnnotated* sequenceLineView)
    : GSequenceLineViewRenderArea(sequenceLineView), sequenceLineViewAnnotated(sequenceLineView) {
    afNormal = new QFont("Courier", 10);
    afSmall = new QFont("Arial", 8);

    afmNormal = new QFontMetrics(*afNormal, this);
    afmSmall = new QFontMetrics(*afSmall, this);

    afNormalCharWidth = afmNormal->width('w');
    afSmallCharWidth = afmSmall->width('w');

    QLinearGradient gradient(0, 0, 0, 1);  // vertical
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.00, QColor(255, 255, 255, 120));
    gradient.setColorAt(0.50, QColor(0, 0, 0, 0));
    gradient.setColorAt(0.70, QColor(0, 0, 0, 0));
    gradient.setColorAt(1.00, QColor(0, 0, 0, 70));
    gradientMaskBrush = QBrush(gradient);
}

GSequenceLineViewAnnotatedRenderArea::~GSequenceLineViewAnnotatedRenderArea() {
    delete afmNormal;
    delete afNormal;
    delete afmSmall;
    delete afSmall;
}

void GSequenceLineViewAnnotated::registerAnnotations(const QList<Annotation*>&) {
}

void GSequenceLineViewAnnotated::unregisterAnnotations(const QList<Annotation*>&) {
}

void GSequenceLineViewAnnotated::ensureVisible(Annotation* a, int locationIdx) {
    QVector<U2Region> location = a->getRegions();
    SAFE_POINT(locationIdx < location.size(), "Invalid annotation location on the widget!", );
    if (-1 == locationIdx) {
        foreach (const U2Region& r, location) {
            if (visibleRange.intersects(r)) {
                return;
            }
        }
    }
    const U2Region& region = location[qMax(0, locationIdx)];
    if (!visibleRange.intersects(region)) {
        const qint64 pos = a->getStrand().isComplementary() ? region.endPos() : region.startPos;
        setCenterPos(qBound(qint64(0), pos, seqLen - 1));
    }
}

bool GSequenceLineViewAnnotated::event(QEvent* e) {
    if (e->type() == QEvent::ToolTip) {
        auto helpEvent = static_cast<QHelpEvent*>(e);
        QPoint renderAreaPount = renderArea->mapFrom(this, helpEvent->pos());
        QString tip = createToolTip(renderAreaPount);
        if (!tip.isEmpty()) {
            QToolTip::showText(helpEvent->globalPos(), tip);
        }
        return true;
    }
    return GSequenceLineView::event(e);
}

QString GSequenceLineViewAnnotated::createToolTip(const QPoint& renderAreaPoint) {
    const int ROWS_LIMIT = 25;
    QList<Annotation*> la = findAnnotationsByCoord(renderAreaPoint);
    QList<SharedAnnotationData> annotationList;
    if (la.isEmpty()) {
        return QString();
    } else {
        // fetch annotation data before further processing in order to improve performance
        foreach (const Annotation* annotation, la) {
            annotationList << annotation->getData();
        }
    }
    QString tip = "<table>";
    int rows = 0;
    if (annotationList.size() > 1) {
        foreach (const SharedAnnotationData& ad, annotationList) {
            rows += ad->qualifiers.size() + 1;
        }
    }

    const bool skipDetails = (rows > ROWS_LIMIT);
    rows = 0;
    foreach (const SharedAnnotationData& ad, annotationList) {
        if (++rows > ROWS_LIMIT) {
            break;
        }
        AnnotationSettingsRegistry* registry = AppContext::getAnnotationsSettingsRegistry();
        const QColor acl = registry->getAnnotationSettings(ad->name)->color;
        tip += "<tr><td bgcolor=" + acl.name() + " bordercolor=black width=15></td><td><big>" + ad->name + "</big></td></tr>";

        if (skipDetails) {
            tip += "<tr><td/><td>...</td>";
            rows++;
        } else {
            tip += "<tr><td></td><td><b>Location</b> = " + U1AnnotationUtils::buildLocationString(ad) + "</td></tr>";
            tip += "<tr><td/><td>";
            tip += Annotation::getQualifiersTip(ad, ROWS_LIMIT - rows, getSequenceObject(), ctx->getComplementTT(), ctx->getAminoTT());
            tip += "</td></tr>";
            rows += ad->qualifiers.size();
        }
    }
    tip += "</table>";
    if (rows > ROWS_LIMIT) {
        tip += "<hr> <div align=center>" + tr("etc ...") + "</div>";
    }
    return tip;
}

void GSequenceLineViewAnnotated::sl_onAnnotationsModified(const QList<AnnotationModification>& annotationModifications) {
    foreach (const AnnotationModification& annotationModification, annotationModifications) {
        if (annotationModification.type == AnnotationModification_LocationChanged ||
            annotationModification.type == AnnotationModification_NameChanged ||
            annotationModification.type == AnnotationModification_TypeChanged) {
            addUpdateFlags(GSLV_UF_AnnotationsChanged);
            update();
            break;
        }
    }
}

bool GSequenceLineViewAnnotated::isAnnotationSelectionInVisibleRange() const {
    const QSet<AnnotationTableObject*> aos = ctx->getAnnotationObjects(true);
    AnnotationSelection* as = ctx->getAnnotationsSelection();
    foreach (const Annotation* annotation, as->getAnnotations()) {
        if (!aos.contains(annotation->getGObject())) {
            continue;
        }
        if (isAnnotationVisible(annotation)) {
            return true;
        }
    }
    return false;
}

GSequenceLineViewGridAnnotationRenderArea::GSequenceLineViewGridAnnotationRenderArea(GSequenceLineViewAnnotated* sequenceLineView)
    : GSequenceLineViewAnnotatedRenderArea(sequenceLineView) {
}

QList<Annotation*> GSequenceLineViewGridAnnotationRenderArea::findAnnotationsByCoord(const QPoint& coord) const {
    QList<Annotation*> resultAnnotationList;
    CHECK(rect().contains(coord), resultAnnotationList);

    AnnotationSettingsRegistry* annotationsSettingsRegistry = AppContext::getAnnotationsSettingsRegistry();
    const qint64 pos = coordToPos(coord);
    qint64 uncertaintyLength = 0;
    SequenceObjectContext* sequenceContext = sequenceLineViewAnnotated->getSequenceContext();
    qint64 sequenceLength = sequenceContext->getSequenceLength();
    U2Region visibleRange = view->getVisibleRange();
    if (visibleRange.length > width()) {
        double scale = getCurrentScale();
        uncertaintyLength = static_cast<qint64>(1 / scale);
        SAFE_POINT(uncertaintyLength < sequenceLength, "Invalid uncertaintyLength for the given seqLen!", resultAnnotationList);
    }
    U2Region pointRegion(pos - uncertaintyLength, 1 + 2 * uncertaintyLength);  // A region of sequence covered by the 'QPoint& coord'.
    const QSet<AnnotationTableObject*> annotationObjectSet = sequenceContext->getAnnotationObjects(true);
    for (const AnnotationTableObject* annotationObject : qAsConst(annotationObjectSet)) {
        for (Annotation* annotation : annotationObject->getAnnotationsByRegion(pointRegion)) {
            const SharedAnnotationData& aData = annotation->getData();
            const QVector<U2Region> locationRegionList = aData->getRegions();
            for (int i = 0, n = locationRegionList.size(); i < n; i++) {
                const U2Region& locationRegion = locationRegionList[i];
                if (locationRegion.intersects(pointRegion) || pointRegion.startPos == locationRegion.endPos()) {
                    // now check pixel precise coords for boundaries.
                    if (pos == locationRegion.startPos) {
                        int posStartX = posToCoord(locationRegion.startPos, true);
                        if (coord.x() < posStartX) {
                            continue;
                        }
                    } else if (pos == locationRegion.endPos()) {
                        // Use endPos() - 1 to avoid moving to the next line in multiline views.
                        int posEndX = posToCoord(locationRegion.endPos() - 1, true) + getCharWidth();
                        if (coord.x() >= posEndX) {
                            continue;
                        }
                    }
                    AnnotationSettings* annotationSettings = annotationsSettingsRegistry->getAnnotationSettings(aData);
                    if (annotationSettings->visible && checkAnnotationRegionContainsYPoint(coord.y(), annotation, i, annotationSettings)) {
                        resultAnnotationList.append(annotation);  // select whole annotation (all regions)
                        break;
                    }
                }
            }
        }
    }
    return resultAnnotationList;
}

bool GSequenceLineViewGridAnnotationRenderArea::checkAnnotationRegionContainsYPoint(int y, Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings) const {
    QList<U2Region> yRegionList = getAnnotationYRegions(annotation, locationRegionIndex, annotationSettings);
    for (const U2Region& region : qAsConst(yRegionList)) {
        if (region.contains(y)) {
            return true;
        }
    }
    return false;
}

}  // namespace U2
