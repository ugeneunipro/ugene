/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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
#include "GSequenceLineViewAnnotated.h"


namespace U2 {

GSequenceLineViewAnnotated::GSequenceLineViewAnnotated(QWidget* p, SequenceObjectContext* ctx)
    : GSequenceLineView(p, ctx)
{
    const QSet<AnnotationTableObject*> aObjs = ctx->getAnnotationObjects(true);
    foreach (const AnnotationTableObject *ao, aObjs) {
        connectAnnotationObject(ao);
    }
    connect(ctx->getAnnotationsSelection(),
        SIGNAL(si_selectionChanged(AnnotationSelection*, const QList<Annotation *> &, const QList<Annotation *> &)),
        SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)));

    connect(ctx, SIGNAL(si_annotationObjectAdded(AnnotationTableObject *)), SLOT(sl_onAnnotationObjectAdded(AnnotationTableObject *)));
    connect(ctx, SIGNAL(si_annotationObjectRemoved(AnnotationTableObject *)), SLOT(sl_onAnnotationObjectRemoved(AnnotationTableObject *)));

    connect(AppContext::getAnnotationsSettingsRegistry(), SIGNAL(si_annotationSettingsChanged(const QStringList &)),
        SLOT(sl_onAnnotationSettingsChanged(const QStringList &)));

}

void GSequenceLineViewAnnotated::connectAnnotationObject(const AnnotationTableObject *ao) {
    connect(ao, SIGNAL(si_onAnnotationsAdded(const QList<Annotation *> &)), SLOT(sl_onAnnotationsAdded(const QList<Annotation *> &)));
    connect(ao, SIGNAL(si_onAnnotationsRemoved(const QList<Annotation *> &)), SLOT(sl_onAnnotationsRemoved(const QList<Annotation *> &)));
    connect(ao, SIGNAL(si_onAnnotationsInGroupRemoved(const QList<Annotation *> &, AnnotationGroup *)),
            SLOT(sl_onAnnotationsInGroupRemoved(const QList<Annotation *> &, AnnotationGroup *)));
    connect(ao, SIGNAL(si_onAnnotationsModified(const QList<AnnotationModification> &)),
            SLOT(sl_onAnnotationsModified(const QList<AnnotationModification> &)));
}

void GSequenceLineViewAnnotated::sl_onAnnotationSettingsChanged(const QStringList &) {
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void GSequenceLineViewAnnotated::sl_onAnnotationObjectAdded(AnnotationTableObject *o) {
    connectAnnotationObject(o);
    sl_onAnnotationsAdded(o->getAnnotations());
}

void GSequenceLineViewAnnotated::sl_onAnnotationObjectRemoved(AnnotationTableObject *o) {
    o->disconnect(this);
    sl_onAnnotationsRemoved(o->getAnnotations());
}

void GSequenceLineViewAnnotated::sl_onAnnotationsAdded(const QList<Annotation *> &l) {
    GTIMER(c2,t2,"GSequenceLineViewAnnotated::sl_onAnnotationsAdded");
    registerAnnotations(l);
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void GSequenceLineViewAnnotated::sl_onAnnotationsRemoved(const QList<Annotation *> &l) {
    unregisterAnnotations(l);
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void GSequenceLineViewAnnotated::sl_onAnnotationsInGroupRemoved(const QList<Annotation *> &l, AnnotationGroup *) {
    ClearAnnotationsTask *task = new ClearAnnotationsTask(l, this);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

ClearAnnotationsTask::ClearAnnotationsTask(const QList<Annotation *> &list, GSequenceLineViewAnnotated *view)
    : Task("Clear annotations", TaskFlag_None), l(list), view(view)
{

}

void ClearAnnotationsTask::run() {
    view->unregisterAnnotations(l);
}

Task::ReportResult ClearAnnotationsTask::report() {
    view->addUpdateFlags(GSLV_UF_AnnotationsChanged);
    view->update();
    return ReportResult_Finished;
}

void GSequenceLineViewAnnotated::sl_onAnnotationSelectionChanged(AnnotationSelection *as, const QList<Annotation *> &_added, const QList<Annotation *> &_removed) {
    const QSet<AnnotationTableObject *> aos = ctx->getAnnotationObjects(true);

    bool changed = false;
    const QList<Annotation *> added = ctx->selectRelatedAnnotations(_added);
    const QList<Annotation *> removed = ctx->selectRelatedAnnotations(_removed);

    if (1 == added.size()) {
        Annotation *a = added.first();
        if (aos.contains(a->getGObject())) {
            const AnnotationSelectionData *asd = as->getAnnotationData(a);
            SAFE_POINT(asd != NULL, "AnnotationSelectionData are NULL",);
            foreach (int loc, asd->locationIdxList) {
                ensureVisible(a, loc);
            }
            changed = true;
        }
    }

    if (!changed) {
        foreach (Annotation *a, added) {
            if (aos.contains(a->getGObject()) && isAnnotationVisible(a)) {
                changed = true;
                break;
            }
        }
        if (!changed) {
            foreach (Annotation *a, removed) {
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

bool GSequenceLineViewAnnotated::isAnnotationVisible(Annotation *a) const  {
    foreach (const U2Region &r, a->getRegions()) {
        if (visibleRange.intersects(r)) {
            return true;
        }
    }
    return false;
}

QList<AnnotationSelectionData> GSequenceLineViewAnnotated::selectAnnotationByCoord(const QPoint &p) const {
    QList<AnnotationSelectionData> res;
    GSequenceLineViewAnnotatedRenderArea *ra = static_cast<GSequenceLineViewAnnotatedRenderArea *>(renderArea);
    CHECK(ra->rect().contains(p), res);
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    const qint64 pos = ra->coordToPos(p);
    qint64 dPos = 0;
    if (visibleRange.length > renderArea->width()) {
        float scale = renderArea->getCurrentScale();
        dPos = static_cast<qint64>((1 / scale));
        SAFE_POINT(dPos < seqLen, "Invalid render region end position!", res);
    }
    U2Region reg(pos - dPos, 1 + 2 * dPos);
    const QSet<AnnotationTableObject *> aObjs = ctx->getAnnotationObjects(true);
    foreach (AnnotationTableObject *ao, aObjs) {
        foreach (Annotation *a, ao->getAnnotationsByRegion(reg)) {
            const SharedAnnotationData &aData = a->getData();
            const QVector<U2Region> location = aData->getRegions();
            for (int i = 0, n = location.size(); i < n; i++) {
                const U2Region &l = location[i];
                if (l.intersects(reg) || l.endPos() == reg.startPos) {
                    bool ok = true;
                    if (l.endPos() == pos || pos == l.startPos) { //now check pixel precise coords for boundaries
                        int x1 = ra->posToCoord(l.startPos, true);
                        int x2 = ra->posToCoord(l.endPos(), true);
                        ok = p.x() <= x2 && p.x() >= x1;
                    }
                    if (ok) {
                        AnnotationSettings *as = asr->getAnnotationSettings(aData);
                        if (as->visible) {
                            if (ra->isPosOnAnnotationYRange(p, a, i, as)) {
                                res.append(AnnotationSelectionData(a, i));
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}

void GSequenceLineViewAnnotated::mousePressEvent(QMouseEvent *me) {
    setFocus();
    const QPoint p = toRenderAreaPoint(me->pos());
    const Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
    const bool singleBaseSelectionMode = km.testFlag(Qt::AltModifier);
    bool annotationEvent = false; // true if mouse pressed in some annotation area
    if (renderArea->rect().contains(p) && me->button() == Qt::LeftButton && !singleBaseSelectionMode) {
        const Qt::KeyboardModifiers km = me->modifiers();
        const bool controlOrShiftPressed = km.testFlag(Qt::ControlModifier) || km.testFlag(Qt::ShiftModifier);
        QList<AnnotationSelectionData> selected = selectAnnotationByCoord(p);
        annotationEvent = !selected.isEmpty();
        if ((!controlOrShiftPressed || !annotationEvent) && cursor().shape() == Qt::ArrowCursor) {
            ctx->getAnnotationsSelection()->clear();
            ctx->getSequenceSelection()->clear();
            ctx->emitClearSelectedAnnotationRegions();
        }
        if (annotationEvent && cursor().shape() == Qt::ArrowCursor) {
            AnnotationSelectionData *asd = &selected.first();
            if (selected.size() > 1) {
                AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
                QMenu popup;
                foreach (const AnnotationSelectionData &as, selected) {
                    const SharedAnnotationData &aData = as.annotation->getData();
                    SAFE_POINT(as.getSelectedRegions().size() == 1, "Invalid selection: only one region is possible!",);
                    const U2Region r = as.getSelectedRegions().first();
                    const QString text = aData->name + QString(" [%1, %2]").arg(r.startPos + 1).arg(r.endPos());
                    AnnotationSettings *asettings = asr->getAnnotationSettings(aData);
                    const QIcon icon = GUIUtils::createSquareIcon(asettings->color, 10);
                    popup.addAction(icon, text);
                }
                QAction *a = popup.exec(QCursor::pos());
                if (NULL == a) {
                    asd = NULL;
                } else {
                    int idx = popup.actions().indexOf(a);
                    asd = &selected[idx];
                }
            }
            if (NULL != asd) {
                ctx->emitAnnotationSelection(asd);
            }
        }
    }
    // a hint to parent class: if mouse action leads to annotation selection -> skip selection handling for mouse press
    ignoreMouseSelectionEvents = annotationEvent;
    GSequenceLineView::mousePressEvent(me);
    ignoreMouseSelectionEvents = false;
}

void GSequenceLineViewAnnotated::mouseDoubleClickEvent(QMouseEvent* me) {
    const QPoint p = toRenderAreaPoint(me->pos());
    QList<AnnotationSelectionData> selected = selectAnnotationByCoord(p);
    if (!selected.isEmpty()) {
        AnnotationSelectionData *asd = &selected.first();
        CHECK(asd != NULL, );

        const QVector<U2Region> selRegions = asd->getSelectedRegions();
        CHECK(selRegions.size() == 1 || U1AnnotationUtils::isAnnotationAroundJunctionPoint(asd, seqLen), );

        const qint64 currentPos = renderArea->coordToPos(toRenderAreaPoint(me->pos()));
        bool containsCurrentPoint = false;
        foreach(const U2Region& reg, selRegions) {
            CHECK_CONTINUE(reg.contains(currentPos));

            containsCurrentPoint = true;
            break;
        }
        if (containsCurrentPoint) {
            ctx->emitAnnotationSequenceSelection(asd);
        }
        lastPressPos = currentPos;
    } else {
        GSequenceLineView::mouseDoubleClickEvent(me);
    }
}

//! VIEW_RENDERER_REFACTORING: used only in CV, doubled in SequenceViewAnnotetedRenderer.
//! Apply renederer logic to CV and remove this method.
QString GSequenceLineViewAnnotated::prepareAnnotationText(const SharedAnnotationData &a, const AnnotationSettings *as) {
    if (!as->showNameQuals || as->nameQuals.isEmpty()) {
        return a->name;
    }
    QVector<U2Qualifier> qs;
    foreach (const QString &qn, as->nameQuals) {
        qs.clear();
        a->findQualifiers(qn, qs);
        if (!qs.isEmpty()) {
            QString res = qs[0].value;
            return res;
        }
    }
    return a->name;
}

QList<Annotation *> GSequenceLineViewAnnotated::findAnnotationsInRange(const U2Region &range) const {
    QList<Annotation *> result;
    const QSet<AnnotationTableObject *> aObjs = ctx->getAnnotationObjects(true);
    foreach (AnnotationTableObject *ao, aObjs) {
        result << ao->getAnnotationsByRegion(range);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
/// Renderer

GSequenceLineViewAnnotatedRenderArea::GSequenceLineViewAnnotatedRenderArea(GSequenceLineViewAnnotated* d)
    : GSequenceLineViewRenderArea(d)
{
    afNormal = new QFont("Courier", 10);
    afSmall = new QFont("Arial", 8);

    afmNormal = new QFontMetrics(*afNormal, this);
    afmSmall = new QFontMetrics(*afSmall, this);

    afNormalCharWidth  = afmNormal->width('w');
    afSmallCharWidth   = afmSmall->width('w');

    QLinearGradient gradient(0, 0, 0, 1); //vertical
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.00, QColor(255, 255, 255, 120));
    gradient.setColorAt(0.50, QColor( 0,   0,   0,   0));
    gradient.setColorAt(0.70, QColor( 0,   0,   0,   0));
    gradient.setColorAt(1.00, QColor( 0,   0,   0,  70));
    gradientMaskBrush = QBrush(gradient);
}

GSequenceLineViewAnnotatedRenderArea::~GSequenceLineViewAnnotatedRenderArea() {
    delete afmNormal;
    delete afNormal;
    delete afmSmall;
    delete afSmall;
}

bool GSequenceLineViewAnnotatedRenderArea::isPosOnAnnotationYRange(const QPoint &p, Annotation *a, int region, const AnnotationSettings *as) const {
    return getAnnotationYRange(a, region, as).contains(p.y());
}

GSequenceLineViewAnnotated * GSequenceLineViewAnnotatedRenderArea::getGSequenceLineViewAnnotated() const {
    return static_cast<GSequenceLineViewAnnotated *>(view);
}

void GSequenceLineViewAnnotated::registerAnnotations(const QList<Annotation *> &) {

}
void GSequenceLineViewAnnotated::unregisterAnnotations(const QList<Annotation *> &) {

}

void GSequenceLineViewAnnotated::ensureVisible(Annotation *a, int locationIdx) {
    QVector<U2Region> location = a->getRegions();
    SAFE_POINT(locationIdx < location.size(), "Invalid annotation location on the widget!",);
    if (-1 == locationIdx) {
        foreach ( const U2Region &r, location) {
            if (visibleRange.intersects(r)) {
                return;
            }
        }
    }
    const U2Region &region = location[qMax(0, locationIdx)];
    if (!visibleRange.intersects(region)) {
        const qint64 pos = a->getStrand().isCompementary() ? region.endPos() : region.startPos;
        setCenterPos(qBound(qint64(0), pos, seqLen - 1));
    }
}

bool GSequenceLineViewAnnotated::event(QEvent *e) {
    if (e->type() == QEvent::ToolTip) {
        QHelpEvent* he = static_cast<QHelpEvent*>(e);
        QString tip = createToolTip(he);
        if (!tip.isEmpty()) {
            QToolTip::showText(he->globalPos(), tip);
        }
        return true;
    }
    return GSequenceLineView::event(e);
}

QString GSequenceLineViewAnnotated::createToolTip(QHelpEvent *e) {
    const int ROWS_LIMIT = 25;
    QList<AnnotationSelectionData> la = selectAnnotationByCoord(e->pos());
    QList<SharedAnnotationData> annotationList;
    if (la.isEmpty()) {
        return QString();
    } else {
        // fetch annotation data before further processing in order to improve performance
        foreach (const AnnotationSelectionData &ad, la) {
            annotationList << ad.annotation->getData();
        }
    }
    QString tip = "<table>";
    int rows = 0;
    if (annotationList.size() > 1) {
        foreach (const SharedAnnotationData &ad, annotationList) {
            rows += ad->qualifiers.size() + 1;
        }
    }

    const bool skipDetails = (rows > ROWS_LIMIT);
    rows = 0;
    foreach (const SharedAnnotationData &ad, annotationList) {
        if (++rows > ROWS_LIMIT) {
            break;
        }
        AnnotationSettingsRegistry *registry = AppContext::getAnnotationsSettingsRegistry();
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

void GSequenceLineViewAnnotated::sl_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications) {
    foreach (const AnnotationModification &annotationModification, annotationModifications) {
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
    foreach(const AnnotationSelectionData& asd, as->getSelection()) {
        if (!aos.contains(asd.annotation->getGObject())) {
            continue;
        }
        if (isAnnotationVisible(asd.annotation)) {
            return true;
        }
    }
    return false;
}

} // namespace
