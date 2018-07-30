/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_COREAPI_ID_REGISTRY_H_
#define _U2_COREAPI_ID_REGISTRY_H_

#include <QMap>

namespace U2 {

/*************************************
 * template class for default registry
 *************************************/
template <class T> class IdRegistry {
public:
    virtual T* getById(const QString& id) {return registry.value(id, NULL);}
    virtual bool registerEntry(T* t) {
        if (registry.contains(t->getId())) {
            return false;
        } else {
            registry.insert(t->getId(), t);
            return true;
        }
    }
    virtual T* unregisterEntry(const QString& id) {return registry.contains(id) ? registry.take(id) : NULL;}
    virtual ~IdRegistry() { qDeleteAll(registry.values());}

    virtual QList<T*> getAllEntries() const {return registry.values();}
    virtual QList<QString> getAllIds() const {return registry.uniqueKeys();}

protected:
    QMap<QString, T*> registry;

}; // IdRegistry

} // U2

#endif // _U2_COREAPI_ID_REGISTRY_H_
