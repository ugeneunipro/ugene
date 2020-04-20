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

#include "QueryViewAdapter.h"
#include "QueryViewItems.h"


namespace U2 {

#define FN_MIN_LEN 30
QueryViewAdapter::QueryViewAdapter(QDScheme* scheme, const QPointF& topLeftCorner)
: scheme(scheme) {
    Q_UNUSED(this->scheme);
    QMap<QDSchemeUnit*, QDElement*> unitMap;
    QList<QDConstraint*> constraints = scheme->getConstraints();
    foreach(QDActor const* a, scheme->getActors()) {
        foreach(QDSchemeUnit* su, a->getSchemeUnits()) {
            QDElement* uv = new QDElement(su);
            uv->moveBy(topLeftCorner.x(), topLeftCorner.y());
            createdElements.append(uv);
            unitMap.insertMulti(su, uv);
        }
        constraints << a->getParamConstraints();
    }
    foreach(QDConstraint* c, constraints) {
        QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(c);
        if (dc) {
            QDElement* src = unitMap.value(dc->getSource());
            QDElement* dst = unitMap.value(dc->getDestination());
            createdFootnotes << new Footnote(src, dst, dc->distanceType(), dc);
        }
    }
}

void QueryViewAdapter::placeElement(QDElement* uv) {
    //rearrange other adjacent units positions if necessary
    foreach(Footnote* fn, uv->getFootnotes()) {
        const QPointF& srcPt = fn->getSrcPoint();
        const QPointF& dstPt = fn->getDstPoint();
        qreal fnLen = dstPt.x() - srcPt.x();
        qreal dx = fnLen - FN_MIN_LEN;
        if (dx<0) {
            if (fn->getSrc()==uv) {
                moveElement(fn->getDst(), -dx);
            }
            else {
                assert(fn->getDst()==uv);
                moveElement(uv, -dx);
            }
        }
    }
}

void QueryViewAdapter::moveElement(QDElement* uv, int dx) {
    assert(currentPath.isEmpty());
    QList<QDElement*> affectedUnits;
    if (needToMove(uv, dx, affectedUnits)) {
        foreach(QDElement* u, affectedUnits) {
            u->moveBy(dx, 0);
        }
    }
}

bool QueryViewAdapter::needToMove(QDElement* current, int dx, QList<QDElement*>& res) {
    //check recursion
    if (currentPath.contains(current)) {
        currentPath.clear();
        return false;
    }
    currentPath.append(current);
    if (!res.contains(current)) {
        res.append(current);
    }
    if (dx>0) {
        foreach(Footnote* fn, current->getFootnotes()) {
            if (fn->getSrc()==current) {
                if (!needToMove(fn->getDst(), dx, res)) {
                    return false;
                }
            }
        }
    }
    else {
        foreach(Footnote* fn, current->getFootnotes()) {
            if (fn->getDst()==current) {
                if (!needToMove(fn->getSrc(), dx, res)) {
                    return false;
                }
            }
        }
    }
    currentPath.removeAll(current);
    return true;
}

QRectF getUnitLocation(QDElement* uv) {
    QRectF res = uv->boundingRect();
    res.moveTopLeft(uv->scenePos());
    return res;
}

bool intersects(QDElement* uv, const QList<QDElement*>& others) {
    foreach(QDElement* other, others) {
        if (uv!=other) {
            const QRectF& uvRect = getUnitLocation(uv);
            const QRectF& otherRect = getUnitLocation(other);
            if (uvRect.intersects(otherRect)) {
                return true;
            }
        }
    }
    return false;
}

void QueryViewAdapter::sortVertically() {
    foreach(QDElement* uv, createdElements) {
        while (intersects(uv, createdElements)) {
            uv->moveBy(0, GRID_STEP);
        }
    }
}

QList<Footnote*> QueryViewAdapter::getSharedFootnotes(QDElement* uv1, QDElement* uv2) const {
    QList<Footnote*> uv1Fns = uv1->getFootnotes();
    const QList<Footnote*>& uv2Fns = uv2->getFootnotes();
    foreach(Footnote* uv2fn, uv2Fns) {
        if (!uv1Fns.contains(uv2fn)) {
            uv1Fns.removeAll(uv2fn);
        }
    }
    return uv1Fns;
}

//enum Border{Left,Right};
//
//class QDEdge;
//class QDVertex {
//public:
//    QDVertex(QDElement* _parent, Border _border, int _xPos=0)
//        : parent(_parent), border(_border), xPos(_xPos) {}
//    void setXPos(int x) { xPos=x; }
//    int getXPos() const { return xPos; }
//private:
//    QDElement* parent;
//    Border border;
//    QList<QDEdge*> in, out;
//    int xPos;
//    friend class QDEdge;
//};
//
//class QDEdge {
//public:
//    QDEdge(QDVertex* _src, QDVertex* _dst) : src(_src), dst(_dst) {
//        src->out.append(this);
//        dst->in.append(this);
//    }
//    QDVertex* getSrc() const { return src; }
//    QDVertex* getDst() const { return dst; }
//private:
//    QDVertex* src;
//    QDVertex* dst;
//};
//
//void QueryViewAdapter::scheme2graph() {
//    QList<QDVertex> graph;
//    foreach(QDElement* el, createdElements) {
//        graph << QDVertex(el, Left) << QDVertex(el, Right);
//    }
//    foreach(Footnote* fn, createdFootnotes) {
//        QDElement* srcEl = fn->getSrc();
//        QDElement* dstEl = fn->getDst();
//    }
//}

}//namespace
