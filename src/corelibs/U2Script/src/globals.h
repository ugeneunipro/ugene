/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_SCRIPT_GLOBALS_H_
#define _U2_SCRIPT_GLOBALS_H_

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_U2SCRIPT_DLL
    #ifdef __GNUC__
      #define U2SCRIPT_EXPORT __attribute__ ((dllexport))
    #else
      #define U2SCRIPT_EXPORT __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define U2SCRIPT_EXPORT __attribute__ ((dllimport))
    #else
      #define U2SCRIPT_EXPORT __declspec(dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define U2SCRIPT_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define U2SCRIPT_EXPORT
  #endif
#endif

/*
 * List of possible error codes
 *
 */
typedef enum U2ErrorType {
    U2_OK,
    U2_ELEMENT_NOT_FOUND,
    U2_ELEMENT_NOT_SUIT_SAS,
    U2_FAILED_TO_CREATE_FILE,
    U2_FAILED_TO_CREATE_DIR,
    U2_FAILED_TO_READ_FILE,
    U2_FAILED_TO_REMOVE_TMP_FILE,
    U2_INVALID_CALL,
    U2_INVALID_NAME,
    U2_INVALID_PATH,
    U2_INVALID_SCHEME,
    U2_INVALID_STRING,
    U2_NOT_ENOUGH_MEMORY,
    U2_NUM_ARG_OUT_OF_RANGE,
    U2_TOO_SMALL_BUFFER,
    U2_UNKNOWN_ELEMENT
} U2ErrorType;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function converts the `type` value to human-readable form.
 *
 * If the `type` contains the value that is out of range of U2ErrorType enumeration
 * then corresponding string is returned.
 *
 */
U2SCRIPT_EXPORT const wchar_t *            getErrorString( U2ErrorType type );

/*
 * This function attempts to write last produced by U2Script string value that has not
 * been placed to custom string to the `destination` array.
 *
 * The `maxExpectedNameLength` parameter has to contain the number of allocated symbols
 * in the `destination` string. If the `maxExpectedNameLength` is larger than the actual
 * `destination` allocation size memory violation error may occur and program crashes.
 * If last string value has greater length than `maxExpectedLength` then `destination`
 * stays unchanged. In this case the string value may still be obtained by subsequent call
 * of this function.
 *
 * The `*requredSize` is returning value, it contains the actual size of last string value.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_INVALID_STRING -          the `destination` is null pointer
 * U2_NUM_ARG_OUT_OF_RANGE -    `requiredSize` is null pointer
 * U2_TOO_SMALL_BUFFER -        `maxExpectedLength` is smaller than `*requiredSize`
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType             getLastFailedStringValue( int maxExpectedLength,
                                            wchar_t *destination, int *requiredSize );

#ifdef __cplusplus
};
#endif

#endif // _U2_SCRIPT_GLOBALS_H_
