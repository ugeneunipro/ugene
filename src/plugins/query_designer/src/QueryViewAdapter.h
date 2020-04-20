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

#ifndef _U2_QUERY_VIEW_ADAPTER_H_
#define _U2_QUERY_VIEW_ADAPTER_H_

#include <QList>
#include <QPointF>


namespace U2 {

class QDScheme;
class QDSchemeUnit;
class QDElement;
class Footnote;
class QueryViewAdapter {
public:
    QueryViewAdapter(QDScheme* scheme, const QPointF& topLeftCorner = QPointF(0,0));
    const QList<QDElement*>& getElements() const { return createdElements; }
    const QList<Footnote*>& getFootnotes() const { return createdFootnotes; }
    void placeElement(QDElement* uv);
    void sortVertically();

    //void scheme2graph();

private:
    void moveElement(QDElement* uv, int dx);
    //returns false if recursive
    bool needToMove(QDElement* current, int dx, QList<QDElement*>& res);

    QList<Footnote*> getSharedFootnotes(QDElement* uv1, QDElement* uv2) const;
private:
    QDScheme* scheme;
    QList<QDElement*> createdElements;
    QList<Footnote*> createdFootnotes;
    QList<QDElement*> currentPath;
};

}//namespace

#endif
