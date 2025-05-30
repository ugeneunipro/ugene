/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <assert.h>

#include <QList>
#include <QSet>
#include <QVariantMap>
#include <QtGlobal>

#include <U2Core/U2IdTypes.h>

#ifdef _DEBUG
#    define U2_PRODUCT_NAME "UGENED"
#else
#    define U2_PRODUCT_NAME "UGENE"
#endif

/** used to store settings with a project name in a key, etc. Same in debug and release */
#define U2_PRODUCT_KEY "ugene"
#define U2_APP_TITLE "UGENE"

#define U2_ORGANIZATION_NAME "Unipro"
#define U2_USER_INI "UGENE_USER_INI"
#define U2_SYSTEM_INI "UGENE_SYSTEM_INI"
#define U2_PRINT_TO_FILE "UGENE_PRINT_TO_FILE"

#define U2_VFS_URL_PREFIX "VFS"
#define U2_VFS_FILE_SEPARATOR "!@#$"

#ifdef BUILDING_U2CORE_DLL
#    define U2CORE_EXPORT Q_DECL_EXPORT
#else
#    define U2CORE_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2FORMATS_DLL
#    define U2FORMATS_EXPORT Q_DECL_EXPORT
#else
#    define U2FORMATS_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2ALGORITHM_DLL
#    define U2ALGORITHM_EXPORT Q_DECL_EXPORT
#else
#    define U2ALGORITHM_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2TEST_DLL
#    define U2TEST_EXPORT Q_DECL_EXPORT
#else
#    define U2TEST_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2LANG_DLL
#    define U2LANG_EXPORT Q_DECL_EXPORT
#else
#    define U2LANG_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2GUI_DLL
#    define U2GUI_EXPORT Q_DECL_EXPORT
#else
#    define U2GUI_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2VIEW_DLL
#    define U2VIEW_EXPORT Q_DECL_EXPORT
#else
#    define U2VIEW_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2DESIGNER_DLL
#    define U2DESIGNER_EXPORT Q_DECL_EXPORT
#else
#    define U2DESIGNER_EXPORT Q_DECL_IMPORT
#endif
#ifdef BUILDING_U2PRIVATE_DLL
#    define U2PRIVATE_EXPORT Q_DECL_EXPORT
#else
#    define U2PRIVATE_EXPORT Q_DECL_IMPORT
#endif

// global
#define GLOBAL_SETTINGS QString("global/")

#define ENV_USE_CRASH_HANDLER "UGENE_USE_CRASH_HANDLER"
#define ENV_TEST_CRASH_HANDLER "UGENE_TEST_CRASH_HANDLER"
#define ENV_SEND_CRASH_REPORTS "UGENE_SEND_CRASH_REPORTS"
#define ENV_TEST_NOTIFICATIONS "UGENE_TEST_NOTIFICATIONS"
#define ENV_UGENE_DEV "UGENE_DEV"
#define ENV_GUI_TEST "UGENE_GUI_TEST"
#define ENV_USE_NATIVE_DIALOGS "UGENE_USE_NATIVE_DIALOGS"

#ifdef __GNUC__
#    define ATTR_UNUSED __attribute__((unused))
#else
#    define ATTR_UNUSED
#endif

#define PATH_PREFIX_DATA "data"
#define PATH_PREFIX_SCRIPTS "scripts"

#define U2_STRINGIFY(x) #x
#define U2_TOSTRING(x) U2_STRINGIFY(x)

namespace U2 {

enum TriState {
    TriState_Unknown,
    TriState_Yes,
    TriState_No
};

enum UnloadedObjectFilter {  // used as a separate type but not 'bool' to improve readability
    UOF_LoadedAndUnloaded,
    UOF_LoadedOnly
};

enum NavigationDirection {
    Forward,
    Backward
};

}  // namespace U2

enum DNAAlphabetType {
    DNAAlphabet_UNDEFINED = 0x0,
    DNAAlphabet_RAW = 0x1,
    DNAAlphabet_NUCL = 0x2,
    DNAAlphabet_AMINO = 0x4
};

Q_DECLARE_FLAGS(AlphabetFlags, DNAAlphabetType)
Q_DECLARE_OPERATORS_FOR_FLAGS(AlphabetFlags)

#define UGENE_ARCH_X86_64 8664
#define UGENE_ARCH_X86_32 8632
#define UGENE_ARCH_UNKNOWN -1

/** Returns UGENE binary (build) architecture. */
inline int getUgeneBinaryArch() {
#if defined Q_PROCESSOR_X86_64
    return UGENE_ARCH_X86_64;
#elif defined Q_PROCESSOR_X86_32
    return UGENE_ARCH_X86_32;
#else
    return UGENE_ARCH_UNKNOWN;
#endif
}

/** Returns true if the current OS is Windows (Q_OS_WINDOWS). */
inline bool isOsWindows() {
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

/** Returns true if the current OS is MacOS (Q_OS_DARWIN). */
inline bool isOsMac() {
#ifdef Q_OS_DARWIN
    return true;
#else
    return false;
#endif
}

/** Returns true if the current OS is Linux (Q_OS_LINUX). */
inline bool isOsLinux() {
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

/** Returns true if the current OS is a variant of Unix (Q_OS_UNIX). */
inline bool isOsUnix() {
#ifdef Q_OS_UNIX
    return true;
#else
    return false;
#endif
}
