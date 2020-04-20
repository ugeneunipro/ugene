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

#include <algorithm>
#include <functional>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include "CudaGpuRegistry.h"

namespace U2 {

CudaGpuRegistry::~CudaGpuRegistry() {
    saveGpusSettings();
    qDeleteAll( gpus.values() );
}

void CudaGpuRegistry::registerCudaGpu( CudaGpuModel * gpu ) {
    assert( !gpus.contains(gpu->getId()) );
    gpus.insert( gpu->getId(), gpu );
}

CudaGpuModel * CudaGpuRegistry::getGpuById( CudaGpuId id ) const {
    return gpus.value( id, 0 );
}

QList<CudaGpuModel *> CudaGpuRegistry::getRegisteredGpus() const {
    return gpus.values();
}

CudaGpuModel * CudaGpuRegistry::getAnyEnabledGpu() const {
    QHash<CudaGpuId, CudaGpuModel*>::const_iterator it = std::find_if( gpus.begin(), gpus.end(), std::mem_fun(&CudaGpuModel::isEnabled) );
    if( gpus.end() != it ) {
        return *it;
    }
    return 0;
}

CudaGpuModel * CudaGpuRegistry::acquireAnyReadyGpu() {
    QHash<CudaGpuId, CudaGpuModel*>::iterator it = std::find_if( gpus.begin(), gpus.end(), std::mem_fun(&CudaGpuModel::isReady) );
    if( gpus.end() != it ) {
        (*it)->setAcquired(true);
        return *it;
    }
    return 0;
}

void CudaGpuRegistry::saveGpusSettings() const {
    Settings * s = AppContext::getSettings();
    foreach( CudaGpuModel * m, gpus ) {
        QString key = CUDA_GPU_REGISTRY_SETTINGS_GPU_SPECIFIC + QString::number(m->getId()) + CUDA_GPU_SETTINGS_ENABLED;
        s->setValue( key, QVariant::fromValue(m->isEnabled()) );
    }
}

} //namespace
