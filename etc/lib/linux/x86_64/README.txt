The libraries were patched with "patchelf --set-rpath '$ORIGIN/.' lib-name.so" to prioritize app folder
version over the OS default (/usr/lib...).

################ Image export
- libpng16.so
The library is required by QT 5.12 to support image export in PNG format.

################# HTTPS network calls.
 - libcrypto.so.1.1
 - libssl.so.1.1

These libraries are loaded statically by Qt Network module.
