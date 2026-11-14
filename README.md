# UGENE

Download UGENE: [https://ugeneunipro.github.io/ugene/](https://ugeneunipro.github.io/ugene/)

## Building UGENE

### Prerequisites

- CMake 3.20 or later and a C++ toolchain compatible with the Qt installation.
- Qt 6 Desktop with Qt Core private development headers, Qt 5 Compatibility Module,
  Qt SVG, Qt Network Authorization, Qt WebSockets, and Qt Linguist Tools.
  Use fresh build directories when switching from Qt 5 to Qt 6, since old plugin
  binaries in `dist/plugins` can prevent UGENE from starting.
- On Ubuntu and Debian, install the system development dependencies:

  ```sh
  sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxtst-dev zlib1g-dev
  ```

### Linux and macOS

Run the commands from the repository root. Set `Qt6_DIR` to the directory
containing `Qt6Config.cmake` in your Qt installation.

```sh
export Qt6_DIR=/path/to/qt/lib/cmake/Qt6

cmake -S . -B cmake-build-debug -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DQt6_DIR="$Qt6_DIR"
cmake --build cmake-build-debug --parallel 4

cmake -S . -B cmake-build-release -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DQt6_DIR="$Qt6_DIR"
cmake --build cmake-build-release --parallel 4
```

Build outputs are in `cmake-build-debug/dist` and `cmake-build-release/dist`.
The Debug CLI and GUI executables are `ugenecld` and `ugeneuid`. The Release
executables are `ugenecl` and `ugeneui`.

### Windows

In a Visual Studio 2022 Developer Command Prompt, set `Qt6_DIR` to the directory
containing `Qt6Config.cmake` for the matching 64-bit MSVC Qt kit.

```bat
set "Qt6_DIR=C:\path\to\qt\lib\cmake\Qt6"

cmake -S . -B cmake-build-debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CONFIGURATION_TYPES=Debug -DQt6_DIR="%Qt6_DIR%"
cmake --build cmake-build-debug --config Debug --parallel 4

cmake -S . -B cmake-build-release -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DQt6_DIR="%Qt6_DIR%"
cmake --build cmake-build-release --config Release --parallel 4
```

See [`etc/script`](etc/script) for platform-specific packaging and test scripts.
