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

#ifndef __OPENCL_SUPPORT_PLUGIN_H__
#define __OPENCL_SUPPORT_PLUGIN_H__

#include <U2Core/PluginModel.h>
#include <U2Algorithm/OpenCLGpuRegistry.h>
#include <U2Algorithm/OpenCLHelper.h>

namespace U2 {

#define ULOG_CAT_PLUGIN_OPENCL_SUPPORT "Plugin: OpenCLSupport"

class OpenCLSupportPlugin : public Plugin {
    Q_OBJECT
public:
    enum OpenCLSupportError {
        Error_NoError,
        Error_BadDriverLib,
        Error_OpenCLError
    };

    OpenCLSupportPlugin();
    ~OpenCLSupportPlugin();

    OpenCLSupportError getError() const;
private:
    OpenCLSupportError obtainGpusInfo( QString & err );
    void loadGpusSettings();
    void registerAvailableGpus();
    void unregisterAvailableGpus();
    bool hasOPENCLError(cl_int err, QString& errMessage);

//    static QString getCudaErrorString( CUresult code );
    static QString getSettingsErrorString( OpenCLSupportError err );

    QList<OpenCLGpuModel *> gpus;
    OpenCLHelper            openCLHelper;

    OpenCLSupportError      err;
};

}

#endif //__OPENCL_SUPPORT_PLUGIN_H__
