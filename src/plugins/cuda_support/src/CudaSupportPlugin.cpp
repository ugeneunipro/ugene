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

#include <cuda.h>

#include <QLibrary>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>

#include "CudaSupportPlugin.h"
#include "CudaSupportSettingsController.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin * U2_PLUGIN_INIT_FUNC() {
    CudaSupportPlugin * plug = new CudaSupportPlugin();
    return plug;
}

const static char * RESOURCE_CUDA_GPU_NAME = "CudaGpu";

CudaSupportPlugin::CudaSupportPlugin() : Plugin( tr("CUDA Support"), tr("Utility plugin for CUDA-enabled GPUs support") ) {
    QString err_str;
    Error err = obtainGpusInfo( err_str );
    if( err_str.isEmpty() && gpus.empty() ) {
        err_str = "No CUDA-enabled GPUs found.";
    }
    if( Error_NoError == err ) {
        loadGpusSettings();
        registerAvailableGpus();
    } else {
        coreLog.details( err_str );
    }

    //adding settings page
    if (AppContext::getMainWindow()) {
        QString settingsPageMsg = getSettingsErrorString(err);
        AppContext::getAppSettingsGUI()->registerPage( new CudaSupportSettingsPageController(settingsPageMsg) );
    }

    //registering gpu resource
    if( !gpus.empty() ) {
        AppResource * gpuResource = new AppResourceSemaphore( RESOURCE_CUDA_GPU, gpus.size(), RESOURCE_CUDA_GPU_NAME );
        AppResourcePool::instance()->registerResource( gpuResource );
    }
}

QString CudaSupportPlugin::getCudaErrorString( CUresult code ) {
    switch(code) {
        case CUDA_SUCCESS:              return QString();
        case CUDA_ERROR_INVALID_DEVICE: return tr( "Invalid device" );
        case CUDA_ERROR_NO_DEVICE:      return tr( "No cuda-enabled devices found" );
        default:                        return tr( "Unknown error" ); //TODO
    }
}

QString CudaSupportPlugin::getSettingsErrorString( Error err ) {
    switch(err) {
        case Error_NoError: return QString("");

        case Error_NoDriverLib: return tr( "\
Cannot load CUDA driver dynamic library.<p>\
It is necessary to install latest Nvidia GPU driver for running<br>\
GPU-accelerated algorithms on Nvidia hardware." );

        case Error_BadDriverLib: return tr( "\
Cannot obtain needed info about Nvidia GPU.<p>\
Consider reinstallation of GPU driver.<br>\
See CUDA Support plugin log for details." );

        case Error_CudaError: return tr( "\
An error occurred while obtaining information about installed Nvidia GPUs.<br>\
See CUDA Support plugin log for details" );
        default: assert(false); return QString();
    }
}

#ifdef Q_OS_WIN
#define CUDA_DRIVER_LIB "nvcuda"
#else
#define CUDA_DRIVER_LIB "cuda"
#endif

#ifdef Q_OS_WIN
#define CALLING_CONVENTION __stdcall
#else
#define CALLING_CONVENTION
#endif

typedef CUresult ( CALLING_CONVENTION *cuinit_f)(int);
const static char * cuinit_n = "cuInit";

typedef CUresult ( CALLING_CONVENTION *cu_device_get_count_f)(int*);
const static char * cu_device_get_count_n = "cuDeviceGetCount";

typedef CUresult ( CALLING_CONVENTION *cu_device_get_f)(CUdevice*, int);
const static char * cu_device_get_n = "cuDeviceGet";

typedef CUresult ( CALLING_CONVENTION *cu_device_get_name_f)(char *, int, CUdevice );
const static char * cu_device_get_name_n = "cuDeviceGetName";

typedef CUresult ( CALLING_CONVENTION *cu_device_total_mem_f)(unsigned int*, CUdevice );
const static char * cu_device_total_mem_n = "cuDeviceTotalMem";

typedef CUresult ( CALLING_CONVENTION *cu_device_get_properties_f)(CUdevprop * prop, CUdevice dev);
const static char * cu_device_get_properties_n = "cuDeviceGetProperties";

CudaSupportPlugin::Error CudaSupportPlugin::obtainGpusInfo( QString & err )
{
    //load driver library
    coreLog.details( tr("Loading CUDA driver library") );
    QLibrary cudaLib( CUDA_DRIVER_LIB );
    cudaLib.load();
    if( !cudaLib.isLoaded() ) {
        QString message = tr( "Cannot load driver library. Error while loading %1: " ).arg(cudaLib.fileName()) + cudaLib.errorString();
        coreLog.details(message);
        return Error_NoDriverLib;
    }

    CUresult cudaRetCode = CUDA_SUCCESS;

    //call cuInit()
    coreLog.details( tr("Initializing CUDA") );
    cuinit_f c_i = cuinit_f( cudaLib.resolve(cuinit_n) );
    if( !c_i ) {
        err = tr("Cannot resolve symbol ") + cuinit_n;
        return Error_BadDriverLib;
    }
    cudaRetCode = c_i(0); // 0 is required by reference manual
    if( CUDA_SUCCESS != cudaRetCode ) {
        err = getCudaErrorString(cudaRetCode);
        return Error_CudaError;
    }

    //call cuDeviceGetCount()
    coreLog.details( tr("Obtaining number of CUDA-enabled devices") );
    cu_device_get_count_f c_dgc = cu_device_get_count_f( cudaLib.resolve(cu_device_get_count_n) );
    if( !c_dgc ) {
        err = tr("Cannot resolve symbol ") + cu_device_get_count_n;
        return Error_BadDriverLib;
    }
    int num_devices = 0;
    cudaRetCode = c_dgc( &num_devices );
    if( CUDA_SUCCESS != cudaRetCode ) {
        err =  getCudaErrorString(cudaRetCode);
        return Error_CudaError;
    }

    //for each device - get it's parameters
    for( int i = 0; i < num_devices; ++i ) {
        CUdevice cuDevice;
        //call cuDeviceGet()
        cu_device_get_f c_dg = cu_device_get_f( cudaLib.resolve(cu_device_get_n) );
        if( !c_dg ) {
            err =  tr("Cannot resolve symbol ") + cu_device_get_n;
            return Error_BadDriverLib;
        }
        cudaRetCode = c_dg( &cuDevice, i );
        if( CUDA_SUCCESS != cudaRetCode ) {
            err =  getCudaErrorString(cudaRetCode);
            return Error_CudaError;
        }

        //obtain device name
        const int maxname = 256;
        QByteArray name(maxname, 0);

        cu_device_get_name_f c_dgn = cu_device_get_name_f( cudaLib.resolve(cu_device_get_name_n) );
        if( !c_dgn ) {
            err =  tr( "Cannot resolve symbol " ) + cu_device_get_name_n;
            return Error_BadDriverLib;
        }
        cudaRetCode = c_dgn( name.data(), maxname, cuDevice );
        if( CUDA_SUCCESS != cudaRetCode ) {
            err =  getCudaErrorString(cudaRetCode);
            return Error_CudaError;
        }

        //obtain device global memory size
        unsigned int mem = 0;
        cu_device_total_mem_f c_dtm = cu_device_total_mem_f( cudaLib.resolve(cu_device_total_mem_n) );
        if( !c_dtm ) {
            err =  tr( "Cannot resolve symbol" ) + cu_device_total_mem_n;
            return Error_BadDriverLib;
        }
        cudaRetCode = c_dtm( &mem, cuDevice );
        if( CUDA_SUCCESS != cudaRetCode ) {
            err =  getCudaErrorString(cudaRetCode);
            return Error_CudaError;
        }

        //obtain device shared memory size
        cu_device_get_properties_f c_dgp = cu_device_get_properties_f( cudaLib.resolve(cu_device_get_properties_n) );
        if( !c_dgp ) {
            err =  tr( "Cannot resolve symbol" ) + cu_device_get_properties_n;
            return Error_BadDriverLib;
        }
        CUdevprop props;
        cudaRetCode = c_dgp( &props, cuDevice );
        if( CUDA_SUCCESS != cudaRetCode ) {
            err =  getCudaErrorString(cudaRetCode);
            return Error_CudaError;
        }

//         log.info("props.sharedMemPerBlock " + QString::number(props.sharedMemPerBlock));
//         log.info("props.clockRate) " + QString::number(props.clockRate));
//         log.info("props.maxGridSize[0] " + QString::number(props.maxGridSize[0]));
//         log.info("props.maxThreadsDim[0] " + QString::number(props.maxThreadsDim[0]));
//         log.info("props.maxThreadsPerBlock " + QString::number(props.maxThreadsPerBlock));
//         log.info("props.memPitch " + QString::number(props.memPitch));
//         log.info("props.regsPerBlock " + QString::number(props.regsPerBlock));
//         log.info("props.SIMDWidth " + QString::number(props.SIMDWidth));
//         log.info("props.textureAlign " + QString::number(props.textureAlign));
//         log.info("props.totalConstantMemory " + QString::number(props.totalConstantMemory));

        CudaGpuModel * m = new CudaGpuModel( QString(name), CudaGpuId(cuDevice), mem );
        gpus.push_back(m);
        coreLog.details( tr("Registering CUDA-enabled GPU: %1, id: %2").arg(m->getName(), QString::number(m->getId())) );
    }

    return Error_NoError;
}

void CudaSupportPlugin::registerAvailableGpus() {
    foreach( CudaGpuModel * m, gpus ) {
        AppContext::getCudaGpuRegistry()->registerCudaGpu(m);
    }
}

void CudaSupportPlugin::loadGpusSettings() {
    Settings * s = AppContext::getSettings();
    foreach( CudaGpuModel * m, gpus ) {
        QString key = CUDA_GPU_REGISTRY_SETTINGS_GPU_SPECIFIC + QString::number(m->getId()) + CUDA_GPU_SETTINGS_ENABLED;
        QVariant enabled_v = s->getValue( key );
        if( !enabled_v.isNull() ) {
            m->setEnabled( enabled_v.toBool() );
        } else {
            m->setEnabled( true );
        }
    }
}

} //namespace
