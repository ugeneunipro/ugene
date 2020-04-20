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

#ifndef _SCHEME_HANDLE_H_
#define _SCHEME_HANDLE_H_

#include "globals.h"

typedef void * SchemeHandle;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function creates a new object representing a computational scheme for Workflow Designer
 * and places reference to it into the `scheme` parameter.
 *
 * The `pathToScheme` has to contain either path to an existing scheme or null pointer.
 * In the first case the `scheme` object is based on the existing scheme,
 * i.e. it will contain all definitions from the scheme. In the second one
 * the `scheme` object describes an empty scheme.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_FAILED_TO_CREATE_DIR -    failed to create a folder for temporary files
 * U2_FAILED_TO_CREATE_FILE -   failed to create temporary file describing scheme
 * U2_FAILED_TO_READ_FILE -     failed to open the file given by the `pathToScheme` parameter
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_PATH -            the `pathToScheme` contains the path that does not exist
 * U2_INVALID_SCHEME -          the scheme described in file given by the `pathToScheme` parameter
                                    cannot be used because it does not meet the format of UWL
 * U2_NOT_ENOUGH_MEMORY -       the constructing of the scheme object ran out of memory
 * U2_UNKNOWN_ELEMENT -         the scheme described in file given by the `pathToScheme` parameter
 *                                  contains element of unknown type
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     createScheme(                const wchar_t *     pathToScheme,
                                                             SchemeHandle *      scheme );

/*
 * This function creates a new object representing a Single Algorithm Scheme
 * for Workflow Designer and places reference to it into the `scheme` parameter.
 *
 * The `algorithmType` has to contain some Workflow Designer element type name
 * which is considered to suit for SAS
 *
 * The `inputPath` and `outputPath` may contain path to input file(s) for the required algorithm
 * and path for output file with results respectively. The `inputPath` may contain a few paths
 * separated by semicolon. These two parameters may also be null pointers,
 * then the resulting scheme object needs for these attributes to be set later.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_ELEMENT_NOT_SUIT_SAS -    the `algorithmType` contains an element type name
 *                                  that does not suit for SAS
 * U2_FAILED_TO_CREATE_DIR -    failed to create a folder for temporary files
 * U2_FAILED_TO_CREATE_FILE -   failed to create temporary file describing scheme
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_STRING -          `algorithmType` is an empty string
 * U2_NOT_ENOUGH_MEMORY -       the constructing of the scheme object ran out of memory
 * U2_UNKNOWN_ELEMENT -         the element with the type name `algorithmType`
 *                                  does not exist or it is unsupported
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     createSas(                   const wchar_t *    algorithmType,
                                                             const wchar_t *    inputPath,
                                                             const wchar_t *    outputPath,
                                                             SchemeHandle *     scheme );

/*
 * This function adds to the computational scheme described by the `scheme` object
 * a new element of the type `readerType`, sets its input file path to the `inputFilePath`,
 * then writes the name of the reader to the `name` string.
 *
 * The `maxExpectedNameLength` parameter has to contain the number of allocated symbols
 * in the `name` array. If the `maxExpectedNameLength` is greater than the actual `name`
 * allocation size this may cause memory violation error and program crash. If the actual
 * reader name is longer than `maxExpectedNameLength` then `name` stays unchanged. In
 * this case the reader's name may still be obtained by getLastFailedStringValue( ) function.
 *
 * The `inputFilePath` parameter may contain a few paths separated by semicolon. All the
 * specified files are treated as input files for the final reader.
 *
 * The file(s) pointed by the `inputFilePath` string may not exist before the invocation. But
 * it has to before the scheme launches. Otherwise scheme has a runtime error and
 * finishes execution.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer or the `inputFilePath`
 *                                  string is empty
 * U2_TOO_SMALL_BUFFER -        the actual reader's name requires more symbols to store it,
 *                                  than `maxExpectedNameLength`
 * U2_UNKNOWN_ELEMENT -         the element with the type name `algorithmType`
 *                                  does not exist or it is unsupported
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     addReaderToScheme(           SchemeHandle        scheme,
                                                             const wchar_t *     readerType,
                                                             const wchar_t *     inputFilePath,
                                                             int                 maxExpectedNameLength,
                                                             wchar_t *           name );

/*
 * This function adds to the computational scheme described by the `scheme` object
 * a new element of the type `writerType`, sets its output file path to the `outputFilePath`,
 * then writes the name of the writer to the `name` string.
 *
 * The `maxExpectedNameLength` parameter has to contain the number of allocated symbols
 * in the `name` array. If the `maxExpectedNameLength` is greater than the actual `name`
 * allocation size this may cause memory violation error and program crash. If the actual
 * writer's name is longer than `maxExpectedNameLength` then `name` stays unchanged. In
 * this case the writer's name may still be obtained by getLastFailedStringValue( ) function.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer
 *                                  or the `outputFilePath` string is empty
 * U2_TOO_SMALL_BUFFER -        the actual writer's name requires more symbols to store it,
 *                                  than `maxExpectedNameLength`
 * U2_UNKNOWN_ELEMENT -         the element with the type name `algorithmType`
 *                                  does not exist or it is unsupported
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     addWriterToScheme(           SchemeHandle        scheme,
                                                             const wchar_t *     writerType,
                                                             const wchar_t *     outputFilePath,
                                                             int                 maxExpectedNameLength,
                                                             wchar_t *           name );

/*
 * This function adds to the computational scheme described by the `scheme` object
 * a new element of the type `elementType`, then writes the name of the element
 * to the `name` string.
 *
 * The `maxExpectedNameLength` parameter has to contain the number of allocated symbols
 * in the `name` array. If the `maxExpectedNameLength` is greater than the actual `name`
 * allocation size this may cause memory violation error and program crash. If the actual
 * element's name is longer than `maxExpectedNameLength` then `name` stays unchanged. In
 * this case the element's name may still be obtained by getLastFailedStringValue( ) function.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer
 * U2_TOO_SMALL_BUFFER -        the actual element's name requires more symbols to store it,
 *                                  than `maxExpectedNameLength`
 * U2_UNKNOWN_ELEMENT -         the element with the type name `elementType` does not exist
 *                                  or it is unsupported
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     addElementToScheme(          SchemeHandle        scheme,
                                                             const wchar_t *     elementType,
                                                             int                 maxExpectedNameLength,
                                                             wchar_t *           name );

/*
 * This function sets attribute with name `attributeName` of the computational element
 * with name `elementName` of the computational scheme described by the `scheme` object
 * to the value of `attributeValue`. If the attribute does not exist before the invocation
 * it is created.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_ELEMENT_NOT_FOUND -       the element with the `elementName` name does not present
 *                                  on the scheme described by the `scheme` object
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_NAME -            the element with the `elementName` name does not have an attribute
 *                                  with the name `attributeName`
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     setSchemeElementAttribute(   SchemeHandle        scheme,
                                                             const wchar_t *     elementName,
                                                             const wchar_t *     attributeName,
                                                             const wchar_t *     attributeValue );

/*
 * This function writes the value of the attribute with name `attributeName` of the computational
 * element with name `elementName` belonging to the computational scheme described by the `scheme`
 * object to the `attributeValue` parameter. If the attribute does not exist before the invocation
 * the `attributeValue` parameter contains an empty string.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_ELEMENT_NOT_FOUND -       the element with the `elementName` name does not present
 *                                  on the scheme described by the `scheme` object
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_NAME -            the element with the `elementName` name does not have an attribute
 *                                  with the name `attributeName`
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     getSchemeElementAttribute(   SchemeHandle        scheme,
                                                             const wchar_t *     elementName,
                                                             const wchar_t *     attributeName,
                                                             int                 maxExpectedNameLength,
                                                             wchar_t *           attributeValue );

/*
 * This function sets binding between ports with `srcPortName` and `dstPortName` names
 * of the computational elements with `srcElementName` and `dstElementName` respectively.
 * These elements and their interaction occur in the computational scheme described
 * by the `scheme` object
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_ELEMENT_NOT_FOUND -       element with the `srcElementName` or `dstElementName` name
 *                                  does not present on the scheme described by the
 *                                  `scheme` object
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_NAME -            strings `srcPortName` or `dstPortName` does not match
 *                                  any port name of element with name `srcElementName`
 *                                  or `dstElementName` respectively
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     addFlowToScheme(             SchemeHandle        scheme,
                                                             const wchar_t *     srcElementName,
                                                             const wchar_t *     srcPortName,
                                                             const wchar_t *     dstElementName,
                                                             const wchar_t *     dstPortName );

/*
 * This function sets binding between slots with `srcSlotName` and `dstPortAndSlotNames` names
 * of the computational elements with `srcElementName` and `dstElementName` respectively.
 * These elements and their interaction occur in the computational scheme described
 * by the `scheme` object
 *
 * The `dstPortAndSlotNames` string
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_ELEMENT_NOT_FOUND -       element with the `srcElementName` or `dstElementName` name
 *                                  does not present on the scheme described by the
 *                                  `scheme` object
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_NAME -            strings `srcPortName` or `dstPortName` does not match
 *                                  any port name of element with name `srcElementName`
 *                                  or `dstElementName` respectively
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          at least one string argument is null pointer
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     addSchemeActorsBinding(      SchemeHandle        scheme,
                                                             const wchar_t *     srcElementName,
                                                             const wchar_t *     srcSlotName,
                                                             const wchar_t *     dstElementName,
                                                             const wchar_t *     dstPortAndSlotNames );

/*
 * This function saves the computational scheme described by the `scheme` object to file
 * in UWL format which path is specified by the `path` parameter
 *
 * The `path` parameter has to specify some file within existing folder.
 * The function does not succeed if these conditions are not satisfied.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_FAILED_TO_CREATE_FILE -   the function has not succeeded in creating file. This may be caused
 *                                  by restriction of the conditions mentioned above
 *                                  or some issues with access rights for given file system
 * U2_INVALID_SCHEME -          the `scheme` object is not properly initialized
 * U2_INVALID_STRING -          the `path`argument is null pointer
 *
 * Result file has the "uwl" extension.
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType     saveSchemeToFile(            SchemeHandle        scheme,
                                                             const wchar_t *     path );

/*
 * This function deallocates all the resources acquired by the `scheme` object.
 *
 * This function should be invoked after all custom interaction with the `scheme` object
 * has finished. Otherwise memory leak may occur. If an arbitrary pointer is passed
 * to the function memory violation error may occur and program crashes.
 *
 */
U2SCRIPT_EXPORT void            releaseScheme(               SchemeHandle        scheme );

/*
 * This function synchronously runs scheme described by the `scheme` object with Workflow Designer
 *
 * The `scheme` object should be completely initialized before the invocation.
 *
 * After the invocation `*outputFilesCount` contains a count of files produced by calculating
 * scheme and `outputFileNames` contains their paths.
 * IMPORTANT: `outputFileNames` is dynamically allocated array of C-strings. Its each item
 * and array at whole should be deallocated explicitly by `free( )` function.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_FAILED_TO_CREATE_FILE -   failed to create temporary file describing scheme
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_SCHEME -          the `scheme` object describes the scheme which validation failed
 *                                  or it is not properly initialized
 * U2_NOT_ENOUGH_MEMORY -       processing of computational scheme ran out of memory
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType         launchScheme(            SchemeHandle        scheme,
                                                             int  *              outputFilesCount,
                                                             wchar_t ***         outputFileNames );

/*
 * This function creates new Single Algorithm Scheme then runs it with Workflow Designer
 *
 * The `algorithmType` has to contain some Workflow Designer element type name
 * which is considered to suit for SAS
 *
 * `inputPath` and `outputPath` has to contain path to input file for the required algorithm
 * and path for output file with results respectively.
 *
 * After the invocation `*outputFilesCount` contains a count of files produced by calculating
 * scheme and `outputFileNames` contains their paths.
 * IMPORTANT: `outputFileNames` is dynamically allocated array of C-strings. Its each item
 * and array at whole should be deallocated explicitly by `free( )` function.
 *
 * Possible returning error codes:
 * U2_OK -                      success
 * U2_ELEMENT_NOT_SUIT_SAS -    the `algorithmType` contains an element type name
 *                                  that does not suit for SAS
 * U2_FAILED_TO_CREATE_DIR -    failed to create a folder for temporary files
 * U2_FAILED_TO_CREATE_FILE -   failed to create temporary file describing scheme
 * U2_INVALID_CALL -            UGENE environment has not been properly initialized
 * U2_INVALID_STRING -          at least one argument is null pointer
 * U2_NOT_ENOUGH_MEMORY -       processing of computational scheme ran out of memory
 * U2_UNKNOWN_ELEMENT -         the element with the type name `algorithmType`
 *                                  does not exist or it is unsupported
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType         launchSas(               const wchar_t *     algorithmType,
                                                             const wchar_t *     inputPath,
                                                             const wchar_t *     outputPath,
                                                             int  *              outputFilesCount,
                                                             wchar_t ***         outputFileNames );

#ifdef __cplusplus
}
#endif

#endif // _SCHEME_HANDLE_H_
