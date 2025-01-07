# Ensure necessary variables are defined.
# Ensure necessary variables are defined.
if (NOT UGENE_PLUGIN_ID)
    message(FATAL_ERROR "UGENE_PLUGIN_ID is not set!")
endif ()

if (NOT UGENE_PLUGIN_NAME)
    message(FATAL_ERROR "UGENE_PLUGIN_NAME is not set!")
endif ()

if (NOT UGENE_PLUGIN_VENDOR)
    set(UGENE_PLUGIN_VENDOR "Unipro")
endif ()

set(UGENE_PLUGIN_VERSION "${UGENE_VERSION}" CACHE STRING "Version of the plugin")
set(UGENE_PLUGIN_LIBRARY "${UGENE_PLUGIN_ID}${UGENE_BUILD_MODE_SUFFIX}")

if (WIN32)
    set(UGENE_PLUGIN_LIBRARY "${UGENE_PLUGIN_LIBRARY}.dll")
    elif(APPLE)
    set(UGENE_PLUGIN_LIBRARY "lib${UGENE_PLUGIN_LIBRARY}.dylib")
else ()
    set(UGENE_PLUGIN_LIBRARY "lib${UGENE_PLUGIN_LIBRARY}.so")
endif ()

# Determine platform and architecture
if (WIN32)
    set(PLATFORM_NAME "win")
    elif(APPLE)
    set(PLATFORM_NAME "macx")
else ()
    set(PLATFORM_NAME "unix")
endif ()

set(PLATFORM_ARCH "64")

# Plugin mode
# TODO!!!
if (NOT UGENE_PLUGIN_MODE)
    set(UGENE_PLUGIN_MODE "console,ui")
endif ()

# Plugin descriptor file
set(UGENE_PLUGIN_DESC_FILE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${UGENE_PLUGIN_ID}${UGENE_BUILD_MODE_SUFFIX}.plugin")

set(QT_VERSION "${Qt5Core_VERSION}")

# Generate descriptor file.
file(WRITE ${UGENE_PLUGIN_DESC_FILE} "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n")
file(APPEND ${UGENE_PLUGIN_DESC_FILE} "<ugene-plugin id=\"${UGENE_PLUGIN_ID}${UGENE_BUILD_MODE_SUFFIX}\" version=\"${UGENE_PLUGIN_VERSION}\" ugene-version=\"${UGENE_VERSION}\" qt-version=\"${QT_VERSION}\">\n")
file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <name>${UGENE_PLUGIN_NAME}</name>\n")
file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <plugin-vendor>${UGENE_PLUGIN_VENDOR}</plugin-vendor>\n")
file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <plugin-mode>${UGENE_PLUGIN_MODE}</plugin-mode>\n")
file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <library>${UGENE_PLUGIN_LIBRARY}</library>\n")

if (UGENE_PLUGIN_DEPENDS)
    file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <depends>${PLUGIN_DEPENDS}</depends>\n")
endif ()

file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <platform name=\"${PLATFORM_NAME}\" arch=\"${PLATFORM_ARCH}\"/>\n")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(APPEND ${UGENE_PLUGIN_DESC_FILE} "    <debug-build>true</debug-build>\n")
endif ()


file(APPEND ${UGENE_PLUGIN_DESC_FILE} "</ugene-plugin>\n")

# Handle license files.
file(GLOB UGENE_PLUGIN_LICENSE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.license")
foreach (UGENE_LICENSE_FILE ${UGENE_PLUGIN_LICENSE_FILES})
    file(COPY "${UGENE_LICENSE_FILE}" DESTINATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
endforeach ()
