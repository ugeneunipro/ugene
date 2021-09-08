# UGENE
Download UGENE: [https://ugeneunipro.github.io/ugene/](https://ugeneunipro.github.io/ugene/)

## Building UGENE

### Prerequisites

Qt (>= 5.12.0 and <= 5.15.x) with the following components installed with Qt installer:
* Desktop
* QtScript

### For Windows users:

To build with devenv (Visual Studio):

1. `qmake -r -tp vc ugene.pro`
2. open ugene.sln from Visual Studio and build or run `devenv.exe ugene.sln /Build` from MSVC command line

To build with nmake:

1. `qmake -r ugene.pro`
2. run `nmake`, `nmake debug` or `nmake release` to build UGENE

### For *nix users:

To build and install UGENE on *nix:

1. `qmake -r PREFIX=/opt/ugene-${VERSION}`.
2. `make -j 4`
3. `sudo make install`
4. `sudo ln -s /opt/ugene-${VERSION}/ugene /usr/bin`
5. `ugene -ui`

> Note: you do not need 'sudo' if you select your own folder for the installation.

### Build with CUDA

1. Download and install required software from http://www.nvidia.com/object/cuda_get.html for your OS
2. Make sure that some system variable are set:
   ```
   CUDA_LIB_PATH=/path_where_cuda_installed/lib
   CUDA_INC_PATH=/path_where_cuda_installed/include
   PATH=$PATH:/path_where_cuda_installed/bin
   ```

   for *nix: `LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUDA_LIB_PATH`

3. cd ./src and open ugene_globals.pri, find and set variable UGENE_CUDA_DETECTED = 1

### Build with OpenCL

1. Download and install video driver with OpenCL support.
2. Download OpenCL headers from http://www.khronos.org/registry/cl/
   or find them in video vendor SDK directory.
3. Build & run UGENE as usual. During startup UGENE will try to dynamically 
   load 'OpenCL' library (See OpenCLHelper.cpp). 
