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

#include <QColor>

#include <U2Core/GUrl.h>
#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT L10N : public QObject {
    Q_OBJECT
public:
    // general purpose messages
    static QString errorTitle() {
        return tr("Error!");
    }
    static QString warningTitle() {
        return tr("Warning!");
    }
    static QString badArgument(const QString& name) {
        return tr("Internal error, bad argument: %1").arg(name);
    }
    static QString outOfMemory() {
        return tr("Out of memory!");
    }
    static QString internalError() {
        return tr("Internal error!");
    }
    static QString internalError(const QString& message) {
        return internalError() + " " + message;
    }
    static QString nullPointerError(const QString& objectName) {
        return tr("Internal error! The object unexpectedly does not exist: ") + objectName;
    }

    // messages to work with files
    static QString errorOpeningFileRead(const GUrl& url) {
        return tr("Error opening file for reading: '%1'").arg(url.getURLString());
    }
    static QString errorOpeningFileWrite(const GUrl& url) {
        return tr("Error opening file for writing: '%1'").arg(url.getURLString());
    }
    static QString errorWritingFile(const GUrl& url) {
        return tr("Write error: '%1'").arg(url.getURLString());
    }
    static QString errorReadingFile(const GUrl& url) {
        return tr("Read error: %1").arg(url.getURLString());
    }
    static QString errorFileTooLarge(const GUrl& url) {
        return tr("File is too large: '%1'").arg(url.getURLString());
    }
    static QString errorFileNotFound(const GUrl& url) {
        return tr("File not found: '%1'").arg(url.getURLString());
    }
    static QString errorIsNotAFile(const GUrl& url) {
        return tr("'%1' is not a file").arg(url.getURLString());
    }
    static QString errorDirNotFound(const GUrl& url) {
        return tr("Folder not found: '%1'").arg(url.getURLString());
    }
    static QString errorOpeningDir(const GUrl& url) {
        return tr("Error opening folder: '%1'").arg(url.getURLString());
    }
    static QString errorIsNotADir(const GUrl& url) {
        return tr("'%1' is not a folder").arg(url.getURLString());
    }
    // messages to work with a shared database
    static QString errorWrongDbObjUrlFormat(const QString& url) {
        return tr("'%1' does not match database object URL format").arg(url);
    }
    static QString errorWrongDbFolderUrlFormat(const QString& url) {
        return tr("'%1' does not match database folder URL format").arg(url);
    }
    static QString errorDbInacsessible(const QString& url) {
        return tr("Unable to connect to the database '%1'. Check your connection settings in File -> Connect to shared database").arg(url);
    }
    static QString errorDbObjectInaccessible(const QString& url, const QString& name) {
        return tr("Unable to find the object '%1' in the database '%2'").arg(name).arg(url);
    }
    static QString errorDbFolderInacsessible(const QString& url, const QString& path) {
        return tr("Unable to find the folder '%1' with the specified path in the database '%2'").arg(path).arg(url);
    }
    static QString errorDbWritePermissons(const QString& url) {
        return tr("You do not have write permissions to the database '%1'").arg(url);
    }

    static QString notValidFileFormat(const QString& type, const GUrl& url) {
        return tr("Invalid file format. Expected file format: %1,  File: %2").arg(type).arg(url.getURLString());
    }
    static QString notSupportedFileFormat(const GUrl& url) {
        return tr("Invalid file format! File: %1").arg(url.getURLString());
    }

    // working with qt plugins: images, scripts, styles
    static QString errorImageSave(const QString& file, const QString& format) {
        return tr("Can't save image to file %1, format %2").arg(file).arg(format);
    }
    static QString errorDocumentNotFound(const GUrl& url) {
        return tr("Document not found: %1").arg(url.getURLString());
    }

    static QString errorObjectNotFound(const QString& objectName) {
        return tr("Object not found: %1").arg(objectName);
    }

    static QString errorObjectIsReadOnly(const QString& objectName) {
        return tr("Object is read only: %1").arg(objectName);
    }

    static QString suffixBp() {
        return tr(" bp");
    }

    static QString defaultStr() {
        return "default";
    }  // It is used in WD, do not translate this without testing

    static QString directStrandStr() {
        return tr("Direct");
    }
    static QString complementStrandStr() {
        return tr("Reverse-complementary");
    }

    static QString required() {
        return tr("Required");
    }

    /**
     * Returns active language code: 'en', 'ru', 'tr'.
     * The string 'ugene-active-translation-language-code' must be correctly translated in the .ts file.
     * If the string is not translated, 'en' is returned as the default.
     */
    static QString getActiveLanguageCode() {
        QString code = tr("ugene-active-translation-language-code");
        return code.isEmpty() || code == "ugene-active-translation-language-code" ? "en" : code;
    }
};

}  // namespace U2
