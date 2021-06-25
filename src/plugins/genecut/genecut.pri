PLUGIN_ID=genecut
PLUGIN_NAME=GeneCut access
PLUGIN_VENDOR=Unipro

include( ../../ugene_plugin_common.pri )

use_opencl(){
    DEFINES += OPENCL_SUPPORT
}
