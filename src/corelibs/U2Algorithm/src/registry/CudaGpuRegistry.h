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

#ifndef __CUDA_GPU_REGISTRY_H__
#define __CUDA_GPU_REGISTRY_H__

#include <QHash>
#include <U2Core/global.h>

namespace U2 {

typedef int CudaGpuId;

#define CUDA_GPU_REGISTRY_SETTINGS "/cuda_gpu_registry"
//stores settings for concrete GPU. The key for appending - textual representation of CudaGpuId
#define CUDA_GPU_REGISTRY_SETTINGS_GPU_SPECIFIC "/cuda_gpu_registry/gpu_specific"
#define CUDA_GPU_SETTINGS_ENABLED "/enabled"

class U2ALGORITHM_EXPORT CudaGpuModel {
public:
    CudaGpuModel( const QString & _name, const CudaGpuId & _id, quint64 _globalMemorySize, bool _enabled  = true ) :
      name(_name), id(_id), globalMemorySizeBytes(_globalMemorySize), enabled(_enabled), acquired(false) {};

    QString getName() const {return name;}
    CudaGpuId getId() const {return id;}
    quint64 getGlobalMemorySizeBytes() const {return globalMemorySizeBytes;}

    bool isEnabled() const { return enabled;}
    void setEnabled(bool b) {enabled = b;}

    bool isAcquired() const {return acquired;}
    void setAcquired( bool a ) {acquired = a;}

    bool isReady() {return !isAcquired() && isEnabled(); }
private:
    //TODO: add another cuda device properties from cudaDeviceProp
    QString name;
    CudaGpuId id;
    quint64 globalMemorySizeBytes;
    bool enabled;
    bool acquired;
};

class U2ALGORITHM_EXPORT CudaGpuRegistry {
public:
    ~CudaGpuRegistry();

    void registerCudaGpu( CudaGpuModel * gpu );
    CudaGpuModel * getGpuById( CudaGpuId id ) const;
    QList<CudaGpuModel*> getRegisteredGpus() const;

    CudaGpuModel * getAnyEnabledGpu() const;

    CudaGpuModel * acquireAnyReadyGpu();

    bool empty() const { return gpus.empty(); }

private:
    void saveGpusSettings() const;
    QHash< CudaGpuId, CudaGpuModel * > gpus;
};

} //namespace

#endif //__CUDA_GPU_REGISTRY_H__
