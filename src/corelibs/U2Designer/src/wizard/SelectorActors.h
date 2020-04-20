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

#ifndef _U2_SELECTORACTORS_H_
#define _U2_SELECTORACTORS_H_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/ElementSelectorWidget.h>

namespace U2 {

class U2DESIGNER_EXPORT SelectorActors {
public:
    SelectorActors();
    SelectorActors(ElementSelectorWidget *widget, const QList<Actor*> &allActors, U2OpStatus &os);
    virtual ~SelectorActors();

    /** Returns NULL if there is no actor for the @value */
    Actor * getActor(const QString &value) const;
    Actor * getSourceActor() const;
    QList<PortMapping> getMappings(const QString &value) const;

private:
    ElementSelectorWidget *widget;
    QMap<QString, Actor*> actors; // selectorValue <-> actor
    Actor *srcActor;
};

} // U2

#endif // _U2_SELECTORACTORS_H_
