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

