include (ugene_version.pri)

UGENE_GLOBALS_DEFINED=1

# Generate only 2 separate Makefiles: Debug & Release. Do not generate a common one that builds both targets.
CONFIG -= debug_and_release debug_and_release_target

DEFINES+=UGENE_VERSION=$${UGENE_VERSION}
# Separate minor/major version tokens are used in .rc resource.
win32:DEFINES+=UGENE_VER_MAJOR=$${UGENE_VER_MAJOR}
win32:DEFINES+=UGENE_VER_MINOR=$${UGENE_VER_MINOR}

# Use of any Qt API marked as deprecated before 5.7 will cause compile time errors.
# The goal is to increase this value gradually up to the current version used in UGENE
# and do not use any deprecated API.
DEFINES+=QT_DISABLE_DEPRECATED_BEFORE=0x050700

CONFIG += c++14

# Do not use library suffix names for files and ELF-dependency sections on Linux.
# Reason: we do not support multiple versions of UGENE in the same folder and
#  use -Wl,-rpath to locate dependencies for own libraries.
unix:!macx: CONFIG += unversioned_libname unversioned_soname

#win32 : CONFIG -= flat  #group the files within the source/header group depending on the directory they reside in file system
win32 : QMAKE_CXXFLAGS += /MP # use parallel build with nmake
win32 : DEFINES+= _WINDOWS
win32-msvc2013 : DEFINES += _SCL_SECURE_NO_WARNINGS
win32-msvc2015|greaterThan(QMAKE_MSC_VER, 1909) {
    DEFINES += _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS _XKEYCHECK_H
    QMAKE_CXXFLAGS-=-Zc:strictStrings
    QMAKE_CXXFLAGS-=Zc:strictStrings
    QMAKE_CFLAGS-=-Zc:strictStrings
    QMAKE_CFLAGS-=Zc:strictStrings
    QMAKE_CXXFLAGS-=-g
    QMAKE_CFLAGS-=-g
    DEFINES += __STDC_LIMIT_MACROS
}

greaterThan(QMAKE_MSC_VER, 1909) {
    DEFINES += _ALLOW_KEYWORD_MACROS
}

win32 : QMAKE_CFLAGS_RELEASE += -O2 -Oy- -MD -Zi
win32 : QMAKE_CXXFLAGS_RELEASE += -O2 -Oy- -MD -Zi
win32 : QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /MAP /MAPINFO:EXPORTS /DEBUG
win32 : LIBS += psapi.lib
win32 : DEFINES += "PSAPI_VERSION=1"

clang {
    CONFIG -= warn_on
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -Wno-ignored-attributes
    QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override
    QMAKE_CXXFLAGS += -Wno-unknown-warning-option
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -Wno-char-subscripts

    # Clang v9 on MacOS 10.13.
    QMAKE_CXXFLAGS += -Wno-invalid-constexpr
}

macx {
    LIBS += -framework CoreFoundation
}

linux-g++ {
    GCC_VERSION = $$system($$QMAKE_CXX " -dumpfullversion -dumpversion")
    # Enable all warnings. Every new version of GCC will provide new reasonable defaults.
    # See https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
    QMAKE_CXXFLAGS += -Wall

    # TODO: a lot of deprecated declarations after the migration to Qt 5.15.2
    QMAKE_CXXFLAGS +=-Wno-deprecated-declarations

    # To enable 'ugene-warnings-as-errors' block below add the following qmake params:
    # QMAKE_DEFAULT_INCDIRS+="<path-to>/gcc_64/include" QMAKE_PROJECT_DEPTH=0 CONFIG+=ugene-warnings-as-errors
    #
    # Where:
    # QMAKE_DEFAULT_INCDIRS: makes Qt paths to be included with -isystem. This way we have no warnings from QT sources.
    # QMAKE_PROJECT_DEPTH=0: forces qmake do not generate relative paths, so QMAKE_DEFAULT_INCDIRS is matched correctly.
    # CONFIG+=ugene-warnings-as-errors: enables the block below.
    #
    # Also add "CPLUS_INCLUDE_PATH=<path-to>/gcc_64/include" to the environment to let GCC know about new isystem paths.
    #
    # To work in this mode in QtCreator:
    # Add "CPATH=<path-to>/gcc_64/include" to the current 'Kit' environment to make QTCreator's code parser work.

    ugene-warnings-as-errors {
        # These warnings are processed as errors.
        # All entries must be added to "disable-warnings.h" to ignore problems in 3rd-party code.

        QMAKE_CXXFLAGS += -Werror=maybe-uninitialized
        QMAKE_CXXFLAGS += -Werror=parentheses
        QMAKE_CXXFLAGS += -Werror=return-type
        QMAKE_CXXFLAGS += -Werror=uninitialized
        QMAKE_CXXFLAGS += -Werror=unused-parameter
        QMAKE_CXXFLAGS += -Werror=unused-value
        QMAKE_CXXFLAGS += -Werror=unused-variable

        versionAtLeast(GCC_VERSION, 7.1) {
            QMAKE_CXXFLAGS += -Werror=shadow=local
        }
        versionAtLeast(GCC_VERSION, 9.1) {
            QMAKE_CXXFLAGS += -Werror=deprecated-copy
        }
    }

    # Build with coverage (gcov) support, now for Linux only.
    equals(UGENE_GCOV_ENABLE, 1) {
        message("Build with gcov support. See gcov/lcov doc for generating coverage info")
        QMAKE_CXXFLAGS += --coverage -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS += -lgcov --coverage
    }
    # Check for undefined symbols during the build.
    QMAKE_LFLAGS += "-Wl,--no-undefined"
}

isEmpty(PREFIX): PREFIX  = dist/ugene-$${UGENE_VERSION}


win32 : QMAKE_LFLAGS *= /MACHINE:X64

# new conditional function for case 'unix but not macx'
defineTest( unix_not_mac ) {
    unix : !macx {
        return (true)
    }
    return (false)
}


# By default, UGENE uses bundled zlib.
# To use system version on any platform set UGENE_USE_BUNDLED_ZLIB = 1
# Note: on Linux libpng depends on the current zlib version, so use of the system zlib is recommended.

defineTest( use_bundled_zlib ) {
    contains( UGENE_USE_BUNDLED_ZLIB, 1 ) : return (true)
    win32: return (true)
    return (false)
}

use_bundled_zlib() {
    DEFINES+=UGENE_USE_BUNDLED_ZLIB
}

# A function to add zlib library to the list of libraries
defineReplace(add_z_lib) {
    use_bundled_zlib() {
        RES = -lzlib$$D
    } else {
        RES = -lz
    }
    return ($$RES)
}


# By default, UGENE uses a bundled sqlite library built with special flags (see sqlite3.pri)
# To use an sqlite library from the OS use UGENE_USE_SYSTEM_SQLITE = 1

defineTest( use_system_sqlite ) {
    contains( UGENE_USE_SYSTEM_SQLITE, 1 ) : return (true)
    return (false)
}

use_system_sqlite() {
    DEFINES += UGENE_USE_SYSTEM_SQLITE
}

# A function to add SQLite library to the list of libraries
defineReplace(add_sqlite_lib) {
    use_system_sqlite() {
        RES = -lsqlite3
    } else {
        RES = -lugenedb$$D
    }
    return ($$RES)
}

# Returns active UGENE output dir name for core libs and executables used by build process: _debug or _release.
defineReplace(out_dir) {
    CONFIG(debug, debug|release) {
        RES = _debug
    } else {
        RES = _release
    }
    return ($$RES)
}

# Returns active UGENE output dir name for core libs and executables used by build process: _debug or _release.
defineTest(is_debug_build) {
    CONFIG(debug, debug|release) {
        RES = true
    } else {
        RES = false
    }
    return ($$RES)
}

# Common library suffix for all libraries that depends on build mode: 'd' for debug and '' for release.
# Example: 'libCore$$D.so' will result to the 'libCored.so' in debug mode and to the 'libCore.so' in release mode.
D=
is_debug_build() {
    D=d
}
