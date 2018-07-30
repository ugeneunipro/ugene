include (../../ugene_globals.pri)


PLUGIN_ID=smith_waterman
PLUGIN_NAME=Smith-Waterman
PLUGIN_VENDOR=Unipro

include( ../../ugene_plugin_common.pri )

INCLUDEPATH += ../../corelibs/U2View/_tmp

#adding SSE2 gcc compiler flag if building on SSE2 capable CPU

use_sse2() {
    !win32 {
        QMAKE_CXXFLAGS += -msse2
        QMAKE_CFLAGS_DEBUG += -msse2
        QMAKE_CFLAGS_RELEASE += -msse2
    }
    DEFINES += SW2_BUILD_WITH_SSE2
}

#adding CUDA specific parameters
use_cuda() {

    LIBS += -L$$UGENE_CUDA_LIB_DIR -lcudart
    INCLUDEPATH += $$UGENE_CUDA_INC_DIR

    win32 {
        CONFIG(debug, debug|release) {
            SW2_NVCC_LIBS_TYPE_FLAG = -MDd
        }

        CONFIG(release, debug|release) {
            SW2_NVCC_LIBS_TYPE_FLAG = -MD
        }
        XCOMPILER_OPT = "-Xcompiler -Zi,$${SW2_NVCC_LIBS_TYPE_FLAG}"
        XLINKER_OPT = "-Xlinker /DEBUG"
    }

    !win32 {
        COMPILER_OPT_EXT = "--compiler-options -fPIC"
    }

    CONFIG(x64) {
        SW2_NVCC_ARCH_FLAG = -m 64
    } else {
        SW2_NVCC_ARCH_FLAG = -m 32
    }

    SW2_NVCC_FLAGS = $${SW2_NVCC_ARCH_FLAG} -g -G "$${XCOMPILER_OPT}" "$${XLINKER_OPT}"

    win32 {
        #libcmt conflicts with msvcrt
        QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
    }

    SW2_CUDA_LIBS += -lcudart
    SW2_CUDA_FILES += src/sw_cuda.cu

#manually convert INCLUDEPATH:
    SW2_CUDA_INCLUDEPATH =
    for(path, INCLUDEPATH) {
        SW2_CUDA_INCLUDEPATH += -I$${path}
    }
    for(path, QMAKE_INCDIR) {
        SW2_CUDA_INCLUDEPATH += -I$${path}
    }

    nvzz.input = SW2_CUDA_FILES
    nvzz.output = $$OBJECTS_DIR/${QMAKE_FILE_BASE}$$QMAKE_EXT_OBJ
    nvzz.name = CUDA compiler
    nvzz.commands = $$UGENE_NVCC $$SW2_NVCC_FLAGS -I$$UGENE_CUDA_INC_DIR $$SW2_CUDA_INCLUDEPATH \
                    -L$$UGENE_CUDA_LIB_DIR $$SW2_CUDA_LIBS \
                    -c $${COMPILER_OPT_EXT} ${QMAKE_FILE_IN} \
                    -o ${QMAKE_FILE_OUT}

    QMAKE_EXTRA_COMPILERS += nvzz

    DEFINES += SW2_BUILD_WITH_CUDA
}

#adding OPENCL specific parameters
use_opencl() {
    DEFINES += SW2_BUILD_WITH_OPENCL
    DEFINES += OPENCL_SUPPORT
}
