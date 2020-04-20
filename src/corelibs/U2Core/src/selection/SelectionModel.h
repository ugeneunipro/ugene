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

#ifndef _U2_SELECTION_MODEL_H_
#define _U2_SELECTION_MODEL_H_

#include <QObject>

#include <U2Core/global.h>

namespace U2 {

typedef QString GSelectionType;

class U2CORE_EXPORT GSelection : public QObject {
    Q_OBJECT
public:
    GSelection(GSelectionType _type, QObject* p = NULL) : QObject(p), type(_type) {}

    GSelectionType getSelectionType() const {return type;}

    virtual bool isEmpty() const = 0;

    virtual void clear() = 0;

protected slots:
    void sl_selectionChanged();

signals:
    void si_onSelectionChanged(GSelection*);

private:
    GSelectionType type;
};

typedef QList<GSelection*> GSelections;

class U2CORE_EXPORT MultiGSelection {
public:
    MultiGSelection(){}
    //TODO: deallocation! -> use shared data

    void addSelection(const GSelection* s);

    void removeSelection(const GSelection* s);

    bool contains(GSelectionType t) const {return findSelectionByType(t) !=NULL;}

    const GSelection* findSelectionByType(GSelectionType t) const;

    const QList<const GSelection*>& getSelections() const {return selections;}

private:
    QList<const GSelection*> selections;
};

}//namespace
#endif
