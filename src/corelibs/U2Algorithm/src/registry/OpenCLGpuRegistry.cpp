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

OpenCLGpuModel *OpenCLGpuRegistry::getGpuByName(const QString &name) const {
    OpenCLGpuModel *gpu = nullptr;
    foreach (OpenCLGpuModel *m, gpus.values()) {
        CHECK_CONTINUE(m->getName() == name);

        gpu = m;
        break;
    }

    return gpu;
}

QList<OpenCLGpuModel *> OpenCLGpuRegistry::getRegisteredGpus() const {
    return gpus.values();
}

OpenCLGpuModel* OpenCLGpuRegistry::getEnabledGpu() const {
    QList<OpenCLGpuModel*> registeredGpus = getRegisteredGpus();

    OpenCLGpuModel *enabledGpu = nullptr;
    foreach (OpenCLGpuModel* m, registeredGpus) {
        if (m && m->isEnabled()) {
            enabledGpu = m;
            break;
        }
    }

    return enabledGpu;
}

QString OpenCLGpuRegistry::getEnabledGpuName() const {
    OpenCLGpuModel * enabledGpu = getEnabledGpu();
    CHECK(nullptr != enabledGpu, QString());

    return enabledGpu->getName();
}

OpenCLGpuModel *OpenCLGpuRegistry::acquireEnabledGpuIfReady() {
    OpenCLGpuModel *model = nullptr;
    foreach(OpenCLGpuModel * gpuModel, gpus.values()) {
        CHECK_CONTINUE(gpuModel->isEnabled());
        CHECK_BREAK(gpuModel->isReady());

        gpuModel->setAcquired(true);
        model = gpuModel;
    }

    return model;
}

void OpenCLGpuRegistry::saveGpusSettings() const {
    Settings* s = AppContext::getSettings();
    foreach(OpenCLGpuModel *m, gpus) {
        CHECK_CONTINUE(m->isEnabled());

        s->setValue(OPENCL_GPU_REGISTRY_SETTINGS_GPU_ENABLED, QVariant(m->getName()));
        break;
    }
}

} //namespace

#endif /*OPENCL_SUPPORT*/
