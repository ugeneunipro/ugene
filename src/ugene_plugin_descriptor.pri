# This file generates plugin descriptor and puts it to the DESTDIR

#
# Plugin descriptor related fields that must/can be passed by caller
#
# PLUGIN_ID       (required)  - Unique plugin id
#
# PLUGIN_NAME     (required)  - Textual name, used only during the loading process for visualization
#
# PLUGIN_VENDOR   (required)  - Vendor of the plugin. For example: Unipro
#
# PLUGIN_VERSION  (optional)  - Version of the plugin. By default = UGENE_VERSION
#                               Default value = UGENE_VERSION
#
# TARGET          (optional)  - Plugin binary file name. For example: libofmarker.so, orfmaker.dll
#                               Default value is derived from PLUGIN_ID
#
# PLUGIN_DEPENDS  (optional)  - Depends line for the plugin: plugin_id1:version1;plugin_id2:version2
#                               For example: orf_marker:1.8.0;dna_export:1.7.2
#                               Default value = empty (no dependencies)
#
# PLUGIN_MODE   (optional)    - Valid values: 'ui', 'console'. Can be combined in a list with spaces or commas
#                               Default value: 'console,ui'
#        

# Unique plugin ID
isEmpty(PLUGIN_ID)  {
    error("PLUGIN_ID is not set!");    
}

# Visual name of the plugin. Not localized. Shown to user only if some error found during the loading stage
isEmpty(PLUGIN_NAME)  {
    error("PLUGIN_NAME is not set!");    
}


# Version of the plugin. If not set, UGENE version is used
isEmpty(PLUGIN_VERSION)  {
    PLUGIN_VERSION=$${UGENE_VERSION}
}

# Vendor of the plugin. Must be set
isEmpty(PLUGIN_VENDOR)  {
    error("PLUGIN_VENDOR is not set!");    
}

# Library of the plugin
isEmpty(TARGET)  {
    error("TARGET is not set")
}
PLUGIN_LIBRARY=$${TARGET}
win32: PLUGIN_LIBRARY=$$join(PLUGIN_LIBRARY,"", "", ".dll")
unix:!macx: PLUGIN_LIBRARY=$$join(PLUGIN_LIBRARY, "", "lib", ".so")
macx: PLUGIN_LIBRARY=$$join(PLUGIN_LIBRARY, "", "lib", ".dylib")


# Set platform info
win32: PLATFORM_NAME="win"
unix:!macx: PLATFORM_NAME="unix"
macx:  PLATFORM_NAME="macx"


!contains(QMAKE_HOST.arch, x86_64) : PLATFORM_ARCH=32
contains (QMAKE_HOST.arch, x86_64) : PLATFORM_ARCH=64
    contains(DEFINES, UGENE_X86_64) {
    PLATFORM_ARCH=64
}

# Set plugin mode
isEmpty(PLUGIN_MODE)  {
    PLUGIN_MODE="console,ui"
}


# Now generate the descriptor
PLUGIN_DESC_FILE=$$OUT_PWD/../../$$out_dir()/plugins/$${PLUGIN_ID}.plugin

defineTest(write){
    val = $$1
    op = $$2

    win32: val ~= s/>/^>
    win32: val ~= s/</^<

    win32: system (echo $$val $$op $${PLUGIN_DESC_FILE})
    else:  system (echo \"$$val\" $$op $${PLUGIN_DESC_FILE})

    return (true)
}

win32: QQ=\"
unix: QQ=\\\"

write("<?xml version=$${QQ}1.0$${QQ} encoding=$${QQ}iso-8859-1$${QQ}?>", >)
write("<ugene-plugin id=$${QQ}$${PLUGIN_ID}$${QQ} version=$${QQ}$${UGENE_VERSION}$${QQ} ugene-version=$${QQ}$${UGENE_VERSION}$${QQ} qt-version=$${QQ}$${QT_VERSION}$${QQ}    >", >>)
write("    <name>$${PLUGIN_NAME}</name>", >>)
write("    <plugin-vendor>$${PLUGIN_VENDOR}</plugin-vendor>", >>)
write("    <plugin-mode>$${PLUGIN_MODE}</plugin-mode>", >>)
write("    <library>$${PLUGIN_LIBRARY}</library>", >>)

!isEmpty(PLUGIN_DEPENDS) {
    write("    <depends>$${PLUGIN_DEPENDS}</depends>", >>)
}

write("    <platform name=$${QQ}$${PLATFORM_NAME}$${QQ} arch=$${QQ}$${PLATFORM_ARCH}$${QQ}/>", >>)


!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        write("    <debug-build>true</debug-build>", >>)
    }
}

write("</ugene-plugin>",  >>)


# Add license files

PLUGIN_LICENSE_FILE=$$_PRO_FILE_PWD_/*.license
PLUGIN_LICENSE_FILE_REP=$$replace(PLUGIN_LICENSE_FILE, "/","\\")
unix: system (cat $$PLUGIN_LICENSE_FILE > $$OUT_PWD/../../$$out_dir()/plugins/$${PLUGIN_ID}.license)
win32: system (copy /B $$PLUGIN_LICENSE_FILE_REP $$system_path($$OUT_PWD)\\..\\..\\$$out_dir()\\plugins\\$${PLUGIN_ID}.license)
