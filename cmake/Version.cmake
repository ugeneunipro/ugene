# UGENE version definition

set(UGENE_VER_MAJOR 53)
set(UGENE_VER_MINOR 0)
set(UGENE_VER_SUFFIX "-dev") # optional, like "-dev" or ""

set(UGENE_VERSION "${UGENE_VER_MAJOR}.${UGENE_VER_MINOR}${UGENE_VER_SUFFIX}")

# Function to add version definitions to a target.
function(add_ugene_rc_file target)
    set(rc_file "${target}.rc")
    target_sources(${target} PRIVATE ${rc_file})
    set_source_files_properties(
            ${rc_file} PROPERTIES
            COMPILE_DEFINITIONS
            "UGENE_VERSION=${UGENE_VERSION};UGENE_VER_MAJOR=${UGENE_VER_MAJOR};UGENE_VER_MINOR=${UGENE_VER_MINOR}"
    )
endfunction()
