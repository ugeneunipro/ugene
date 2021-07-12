# UGENE
Download UGENE: [https://ugeneunipro.github.io/ugene/](https://ugeneunipro.github.io/ugene/)

## Building UGENE

### Prerequisites

Make sure the Qt (>= 5.4.2 and <= 5.15) development libraries are installed:

We highly recommend you to install Qt to any operating system with Qt online-installer. This is the main way for Windows and macOS operating systems, also available  for Linux. To install Qt, download the online-installer from Qt official site (http://www.qt.io/download/). Make sure, that you have installed the following components:
* Qt Prebuild components
* QtScript

Other options to install Qt:
* Ubuntu >=14.04: `sudo apt-get install qt5-default qttools5-dev-tools qtscript5-dev libqt5svg5-dev`
* Ubuntu 12.04:
    * Download and install Qt 5.5.1: www.qt.io/download-open-source/
    * Set the system variable: export PATH=$PATH:~/Qt5.5.1/5.5/gcc_64/bin
    * `sudo apt-get install g++ libgl1-mesa-dev libglu1-mesa-dev`
* Fedora:       `sudo yum install qt5-qtscript-devel qt5-qtbase-devel qt5-qtsvg-devel qt5-linguist gcc-c++ redhat-rpm-config mesa-libGLU-devel`
* Arch Linux:   `sudo pacman -S qt`available

Also add "path/to/Qt/<component>/bin" to the PATH variable.

### For Windows users:

To build with devenv (Visual Studio)

1. `qmake -r -tp vc ugene.pro`
2. open ugene.sln from Visual Studio and build or run `devenv.exe ugene.sln /Build` from MSVC command line

To build with nmake.exe:

1. `qmake -r ugene.pro`
2. run `nmake`, `nmake debug` or `nmake release` to build UGENE

Also you need to add "libeay.dll" and "ssleay.dll" from OpenSSL ToolKit. Put these libs to the directory which is avaliable with the PATH variable.

### For *nix users:

0. installation paths may be set up in ugene_globals.pri
1. `qmake -r` (Fedora: `qmake-qt5 -r`)
2. `make -j 4`
3. `sudo make install`
4. `ugene -ui`

> Note: usually, `make` builds a release version of UGENE.
   However, on certain platforms default target is debug.
   To enforce release build use `make release` or `make all`.

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

1. Download and install video driver wich supports OpenCL
2. Download OpenCL headers from http://www.khronos.org/registry/cl/
   or find them in video vendor SDK directory.
3. make sure that you have system variable is set correctly:
          `OPENCL_INC_PATH=/path_where_open_cl_installed`
4. cd ./src and open ugene_globals.pri, find and set variable `UGENE_OPENCL_DETECTED = 1`

### Other notes:
 2) UGENE is be build for x86_64 arch by default, add CONFIG+=x86 to the qmake command to build 32-bit version of UGENE.
    E.g. for Windows:
    `qmake -r -tp vc CONFIG+=x86 ugene.pro`
    > ATTENTION: 32-bit version is not officially supported since UGENE v. 34.0
