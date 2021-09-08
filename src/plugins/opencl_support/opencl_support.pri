include( ../../ugene_globals.pri )

use_opencl() {

PLUGIN_ID=opencl_support
PLUGIN_NAME=OpenCL support
PLUGIN_VENDOR=Unipro
DEFINES += OPENCL_SUPPORT

include( ../../ugene_plugin_common.pri )


} #use_opencl
