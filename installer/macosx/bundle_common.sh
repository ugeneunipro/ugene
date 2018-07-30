function add-binary {
    BINARY=$1
    echo "Adding binary: ${BINARY}"

    BINARY_PATH="${RELEASE_DIR}/${BINARY}"
    if [ ! -f ${BINARY_PATH} ]; then
        BINARY_PATH="${RELEASE_DIR}/${BINARY}.app/Contents/MacOS/${BINARY}"
    fi

    if [ ! -f ${BINARY_PATH} ] ;
    then
        echo "Error: binary file not found: ${BINARY}"
        exit 1
    fi

    cp -f "${BINARY_PATH}" "${TARGET_EXE_DIR}"
    dump_symbols "${TARGET_EXE_DIR}/${BINARY}"
}

function add-plugin {
    plugin=$1
    echo "Registering plugin: ${plugin}"

    PLUGIN_LIB="lib${plugin}.dylib"
    PLUGIN_DESC="${plugin}.plugin"
    PLUGIN_LICENSE="${plugin}.license"

    if [ ! -f ${RELEASE_DIR}/plugins/${PLUGIN_LIB} ] ;  
    then  
        echo "Plugin library file not found: ${PLUGIN_LIB} !"
        exit 1
    fi

    if [ ! -f ${RELEASE_DIR}/plugins/${PLUGIN_DESC} ] ; 
    then
        echo "Plugin descriptor file not found: ${PLUGIN_DESC} !"
        exit 1
    fi

    if [ ! -f ${RELEASE_DIR}/plugins/${PLUGIN_LICENSE} ] ; 
    then
        echo "Plugin descriptor file not found: ${PLUGIN_LICENSE} !"
        exit 1
    fi

    cp "${RELEASE_DIR}/plugins/${PLUGIN_LIB}"  "${TARGET_EXE_DIR}/plugins/"
    cp "${RELEASE_DIR}/plugins/${PLUGIN_DESC}" "${TARGET_EXE_DIR}/plugins/"
    cp "${RELEASE_DIR}/plugins/${PLUGIN_LICENSE}" "${TARGET_EXE_DIR}/plugins/"

    echo Extracting debug symbols for "plugins/${PLUGIN_LIB}"
    dump_symbols "${TARGET_EXE_DIR}/plugins/${PLUGIN_LIB}"
}

function add-library {
    lib=$1
    echo "Adding lib: ${lib}"

    LIB_FILE="lib${lib}.1.dylib"

    if [ ! -f ${RELEASE_DIR}/${LIB_FILE} ] ;  
    then  
        echo "Library file not found: ${LIB_FILE} !"
        exit 1
    fi

    cp "${RELEASE_DIR}/${LIB_FILE}"  "${TARGET_EXE_DIR}/"

    echo Extracting debug symbols for "${LIB_FILE}"
    dump_symbols "${TARGET_EXE_DIR}/${LIB_FILE}"
}

# This function replaces @loader_path with @executable_path for UGENE plugins
# for the specified Qt library
# Supposed that Qt is build as frameworks
restorePluginsQtInstallName () {
    if [ "$1" ] && [ "$2" ]
    then
        install_name_tool -change @loader_path/../../Frameworks/$1.framework/Versions/5/$1 @executable_path/../Frameworks/$1.framework/Versions/5/$1 "$TARGET_EXE_DIR"/plugins/$2
    else
        echo "restorePluginsQtInstallName: not enough parameters"
    fi

    return 0;
}

# This function replaces @loader_path with @executable_path for UGENE plugins
# Supposed that Qt is build as frameworks
restorePluginsQtInstallNames () {
   if [ "$1" ]
   then
        echo "Restore qt install names for plugin $1"
        PLUGIN_LIB="lib$1.dylib"

        restorePluginsQtInstallName QtCore $PLUGIN_LIB
        restorePluginsQtInstallName QtGui $PLUGIN_LIB
        restorePluginsQtInstallName QtMultimedia $PLUGIN_LIB
        restorePluginsQtInstallName QtMultimediaWidgets $PLUGIN_LIB
        restorePluginsQtInstallName QtNetwork $PLUGIN_LIB
        restorePluginsQtInstallName QtOpenGL $PLUGIN_LIB
        restorePluginsQtInstallName QtPositioning $PLUGIN_LIB
        restorePluginsQtInstallName QtPrintSupport $PLUGIN_LIB
        restorePluginsQtInstallName QtQml $PLUGIN_LIB
        restorePluginsQtInstallName QtQuick $PLUGIN_LIB
        restorePluginsQtInstallName QtScript $PLUGIN_LIB
        restorePluginsQtInstallName QtScriptTools $PLUGIN_LIB
        restorePluginsQtInstallName QtSensors $PLUGIN_LIB
        restorePluginsQtInstallName QtSql $PLUGIN_LIB
        restorePluginsQtInstallName QtSvg $PLUGIN_LIB
        restorePluginsQtInstallName QtTest $PLUGIN_LIB
        restorePluginsQtInstallName QtWebChannel $PLUGIN_LIB
        restorePluginsQtInstallName QtWebKit $PLUGIN_LIB
        restorePluginsQtInstallName QtWebKitWidgets $PLUGIN_LIB
        restorePluginsQtInstallName QtWidgets $PLUGIN_LIB
        restorePluginsQtInstallName QtXml $PLUGIN_LIB
        restorePluginsQtInstallName QtXmlPatterns $PLUGIN_LIB

   else
       echo "restorePluginsQtInstallNames: no parameter passed."
   fi

   return 0
}

dump_symbols() {
    filename=`basename "${1}"`
    SYMBOL_FILE="${SYMBOLS_DIR}/$filename.sym";

    DUMP_SYMS=dump_syms_${ARCHITECTURE}
    ./${DUMP_SYMS} -a ${ARCHITECTURE} "$1" > "${SYMBOLS_DIR}/$filename.sym"

    FILE_HEAD=`head -n 1 "${SYMBOL_FILE}"`
    FILE_HASH=`echo ${FILE_HEAD} | awk '{ print $4 }'`
    FILE_NAME=`echo ${FILE_HEAD} | awk '{ print $5 }'`

    DEST_PATH="${SYMBOLS_DIR}/${FILE_NAME}/${FILE_HASH}";
    mkdir -p "${DEST_PATH}"
    mv "${SYMBOL_FILE}" "${DEST_PATH}/${FILE_NAME}.sym"
}
