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

#ifndef _U2_CDS_FACTORY_REGISTRY_H_
#define _U2_CDS_FACTORY_REGISTRY_H_

#include <U2Algorithm/CDSearchTaskFactory.h>


namespace U2 {

class U2ALGORITHM_EXPORT CDSearchFactoryRegistry : public QObject {
    Q_OBJECT
public:
    CDSearchFactoryRegistry() : localSearchFactory(NULL), remoteSearchFactory(NULL) {}
    ~CDSearchFactoryRegistry() {
        delete localSearchFactory;
        delete remoteSearchFactory;
    }

    enum SearchType { LocalSearch, RemoteSearch };

    void registerFactory(CDSearchFactory* factory, SearchType type) {
        if (type == LocalSearch) {
            assert(localSearchFactory == NULL);
            localSearchFactory = factory;
        } else if (type == RemoteSearch) {
            assert(remoteSearchFactory == NULL);
            remoteSearchFactory = factory;
        } else {
            assert(0);
        }
    }

    CDSearchFactory* getFactory(SearchType type) const {
        if (type == LocalSearch) {
            return localSearchFactory;
        } else if (type == RemoteSearch) {
            return remoteSearchFactory;
        } else {
            assert(0);
            return NULL;
        }
    }

private:
    CDSearchFactory* localSearchFactory;
    CDSearchFactory* remoteSearchFactory;
};

} //namespace

#endif
