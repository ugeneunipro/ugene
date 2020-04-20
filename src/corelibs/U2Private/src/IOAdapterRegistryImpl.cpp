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

#include "IOAdapterRegistryImpl.h"

#include <U2Core/LocalFileAdapter.h>
#include <U2Core/HttpFileAdapter.h>
#include <U2Core/VFSAdapter.h>
#include <U2Core/StringAdapter.h>
#include <U2Core/DatabaseConnectionAdapter.h>

namespace U2 {

bool IOAdapterRegistryImpl::registerIOAdapter(IOAdapterFactory* io) {
    if (getIOAdapterFactoryById(io->getAdapterId())!=NULL) {
        return false;
    }
    adapters.push_back(io);
    return true;
}

bool IOAdapterRegistryImpl::unregisterIOAdapter(IOAdapterFactory* io)  {
    int n = adapters.removeAll(io);
    return n > 0;
}


IOAdapterFactory* IOAdapterRegistryImpl::getIOAdapterFactoryById(IOAdapterId id) const  {
    foreach (IOAdapterFactory* io, adapters) {
        if (io->getAdapterId() == id) {
            return io;
        }
    }
    return NULL;
}


void IOAdapterRegistryImpl::init() {
    registerIOAdapter(new LocalFileAdapterFactory(this));
    registerIOAdapter(new GzippedLocalFileAdapterFactory(this));
    registerIOAdapter( new HttpFileAdapterFactory(this) );
    registerIOAdapter( new GzippedHttpFileAdapterFactory(this) );
    registerIOAdapter( new VFSAdapterFactory(this) );
    registerIOAdapter( new StringAdapterFactory(this) );
    registerIOAdapter( new DatabaseConnectionAdapterFactory(this) );
}

}//namespace
