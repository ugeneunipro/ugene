# UGENE

Download UGENE: [https://ugeneunipro.github.io/ugene/](https://ugeneunipro.github.io/ugene/)

## Building UGENE

### Prerequisites

Qt 5.15.x with the following components installed via the Qt installer:
- Desktop
- QtScript

### Build

1. Set the `Qt5_DIR` system environment variable to point to the Qt 5 installation directory.
2. Run the following commands based on your operating system:
    - **Linux & macOS**:
      ```
      cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -S . -B "../build-ugene-release"
      cmake --build "../build-ugene-release" -- -j 4
      ```
    - **Windows**:
      ```
      cmake -DCMAKE_CONFIGURATION_TYPES=Release -G "Visual Studio 16 2019" -S . -B "../build-ugene-release"
      cmake --build "../build-ugene-release" --parallel --config Release
      ```

For more details, please check the `build.sh` files in the `etc/scripts` directory.
