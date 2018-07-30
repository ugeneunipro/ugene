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

#ifdef OPENCL_SUPPORT

#include <algorithm>
#include <functional>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include "OpenCLGpuRegistry.h"

namespace U2 {

OpenCLGpuRegistry::OpenCLGpuRegistry() : openCLHelper(NULL){
}

OpenCLGpuRegistry::~OpenCLGpuRegistry() {
    saveGpusSettings();
    qDeleteAll( gpus.values() );
}

void OpenCLGpuRegistry::registerOpenCLGpu( OpenCLGpuModel * gpu ) {
    assert( !gpus.contains(gpu->getId()) );
    gpus.insert( gpu->getId(), gpu );
}

void OpenCLGpuRegistry::unregisterOpenCLGpu(OpenCLGpuModel * gpu) {
    CHECK(gpus.contains(gpu->getId()), );
    delete gpus.take(gpu->getId());
}

OpenCLGpuModel * OpenCLGpuRegistry::getGpuById( OpenCLGpuId id ) const {
    return gpus.value( id, 0 );
}

QList<OpenCLGpuModel *> OpenCLGpuRegistry::getRegisteredGpus() const {
    return gpus.values();
}

QList<OpenCLGpuModel *> OpenCLGpuRegistry::getEnabledGpus() const {
    QList<OpenCLGpuModel*> registeredGpus = getRegisteredGpus();

    QList<OpenCLGpuModel*> enabledGpus;
    foreach (OpenCLGpuModel* m, registeredGpus) {
        if (m && m->isEnabled()) {
            enabledGpus.append(m);
        }
    }

    return enabledGpus;
}

OpenCLGpuModel * OpenCLGpuRegistry::getAnyEnabledGpu() const {
    QHash<OpenCLGpuId, OpenCLGpuModel*>::const_iterator it = std::find_if( gpus.begin(), gpus.end(), std::mem_fun(&OpenCLGpuModel::isEnabled) );
    if( gpus.end() != it ) {
        return *it;
    }
    return 0;
}

OpenCLGpuModel * OpenCLGpuRegistry::acquireAnyReadyGpu() {
    QHash<OpenCLGpuId, OpenCLGpuModel*>::iterator it = std::find_if( gpus.begin(), gpus.end(), std::mem_fun(&OpenCLGpuModel::isReady) );
    if( gpus.end() != it ) {
        (*it)->setAcquired(true);
        return *it;
    }
    return 0;
}

void OpenCLGpuRegistry::saveGpusSettings() const {
    Settings * s = AppContext::getSettings();
    foreach( OpenCLGpuModel * m, gpus ) {
        QString key = OPENCL_GPU_REGISTRY_SETTINGS_GPU_SPECIFIC + QString::number(m->getId()) + OPENCL_GPU_SETTINGS_ENABLED;
        s->setValue( key, QVariant::fromValue(m->isEnabled()) );
    }
}

} //namespace

#endif /*OPENCL_SUPPORT*/
