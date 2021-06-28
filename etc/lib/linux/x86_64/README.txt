The libraries were patched with "patchelf --set-rpath '$ORIGIN/.' lib-name.so" to prioritize app folder
version over the OS default (/usr/lib...).

################ MySQL
- libmysqlclient.so.21
The library is loaded by libqsqlmysql.so (QT) and was checked to work correctly with Qt 5.12.10

################ Image export
- libpng16.so
The library is required by QT 5.12 to support image export in PNG format.

################# HTTPS network calls.
 - libcrypto.so.1.1
 - libssl.so.1.1

These libraries are loaded statically by Qt Network module.
