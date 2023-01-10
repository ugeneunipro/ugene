/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_DISABLE_WARNINGS_H_
#define _U2_DISABLE_WARNINGS_H_

/***
 * Once included this file disables all listed warnings in the current compilation unit.
 * This utility should be used to suppress warnings in 3rd party code included into UGENE.
 *
 * Note: never include this file into files owned/created by UGENE (both headers and cpp) because
 *  it will disable warnings in our code too.
 */

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpragmas"  // Ignore all unknown (for example when an old GCC is used) pragmas below.
// All warnings in the list below should be sorted by name.
// Warnings that require GCC compiler > 5.4 should be enabled in qmake file with a GCC version check.
#    pragma GCC diagnostic ignored "-Wbool-compare"
#    pragma GCC diagnostic ignored "-Wclass-memaccess"
#    pragma GCC diagnostic ignored "-Wdeprecated"
#    pragma GCC diagnostic ignored "-Wdeprecated-copy"
#    pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#    pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#    pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#    pragma GCC diagnostic ignored "-Wimplicit-int-float-conversion"
#    pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#    pragma GCC diagnostic ignored "-Wmisleading-indentation"
#    pragma GCC diagnostic ignored "-Wparentheses"
#    pragma GCC diagnostic ignored "-Wpointer-compare"
#    pragma GCC diagnostic ignored "-Wreturn-type"
#    pragma GCC diagnostic ignored "-Wshadow=compatible-local"
#    pragma GCC diagnostic ignored "-Wshadow=local"
#    pragma GCC diagnostic ignored "-Wsign-compare"
#    pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
#    pragma GCC diagnostic ignored "-Wstrict-aliasing"
#    pragma GCC diagnostic ignored "-Wstringop-truncation"
#    pragma GCC diagnostic ignored "-Wswitch"
#    pragma GCC diagnostic ignored "-Wuninitialized"
#    pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#    pragma GCC diagnostic ignored "-Wunused-value"
#    pragma GCC diagnostic ignored "-Wunused-variable"
#    pragma GCC diagnostic ignored "-Wuse-after-free"
#
#    ifndef __cplusplus  // The macros below are not valid in C++ context but are needed for plain C files.
#        pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#        pragma GCC diagnostic ignored "-Wimplicit-int"
#    endif
#endif

/**
 * This macro does nothing today but helps to avoid "unused include" warning for #include <U2Core/disable-warnings.h>:
 *
 * Usage:
 * #include <U2Core/disable-warnings>
 * U2_DISABLE_WARNINGS
 */
#define U2_DISABLE_WARNINGS

#endif
