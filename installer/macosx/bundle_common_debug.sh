function add-plugin {
    plugin=$1
    echo "Registering plugin: ${plugin}"

    PLUGIN_LIB="lib${plugin}d.dylib"
    PLUGIN_DESC="${plugin}d.plugin"
    PLUGIN_LICENSE="${plugin}d.license"

    if [ ! -f ${DEBUG_DIR}/plugins/${PLUGIN_LIB} ] ;  
    then  
        echo "Plugin library file not found: ${PLUGIN_LIB} !"
        echo "Plugin is skipped"
        return
    fi

    if [ ! -f ${DEBUG_DIR}/plugins/${PLUGIN_DESC} ] ; 
    then
        echo "Plugin descriptor file not found: ${PLUGIN_DESC} !"
        echo "Plugin is skipped"
        return
    fi

    if [ ! -f ${DEBUG_DIR}/plugins/${PLUGIN_LICENSE} ] ; 
    then
        echo "Plugin descriptor file not found: ${PLUGIN_LICENSE} !"
        echo "Plugin is skipped"
        return
    fi

    cp "${DEBUG_DIR}/plugins/${PLUGIN_LIB}"  "${TARGET_EXE_DIR}/plugins/"
    cp "${DEBUG_DIR}/plugins/${PLUGIN_DESC}" "${TARGET_EXE_DIR}/plugins/"
    cp "${DEBUG_DIR}/plugins/${PLUGIN_LICENSE}" "${TARGET_EXE_DIR}/plugins/"
}

function add-library {
    lib=$1
    echo "Adding lib: ${lib}"

    LIB_FILE="lib${lib}d.1.dylib"

    if [ ! -f ${DEBUG_DIR}/${LIB_FILE} ] ;  
    then  
        echo "Library file not found: ${LIB_FILE} !"
        exit 1
    fi

    cp "${DEBUG_DIR}/${LIB_FILE}"  "${TARGET_EXE_DIR}/"
}
