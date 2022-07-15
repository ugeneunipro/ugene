/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_FILE_FILTERS_H_
#define _U2_FILE_FILTERS_H_

#include <QStringList>

#include <U2Core/global.h>

namespace U2 {

class DocumentFormatConstraints;
class DocumentFormat;

/**
 * Set of builders of file filters in FileDialog way.
 * Placed in U2Core but not U2Gui because there are some dependencies in U2Lang package.
 */
class U2CORE_EXPORT FileFilters {
public:
    /** Returns "All files" filter line. */
    static QString createAllFilesFilter();

    /**
     * Builds QT File Dialog compatible file filter string from the given list of filters
     * and adds 'All Files (*.*)' option with the correct order.
     * It is recommended to use this method instead of adding 'All Files' filter manually.
     */
    static QString withAllFilesFilter(const QStringList& filters);

    /** Creates a single filter. No 'All files' included. */
    static QString createSingleFileFilter(const QString& name, const QStringList& extensions, bool addGzipVariant);

    /** Creates a single filter with all document extensions. No 'All files' included. */
    static QString createSingleFileFilter(const DocumentFormat* documentFormat);

    /** Creates a single filter with all document extensions. No 'All files' included. */
    static QString createSingleFileFilterByDocumentFormatId(const QString& documentFormatId);

    /** Returns filters string with the given name and extension. 'All files' filter. */
    static QString createFileFilter(const QString& name, const QStringList& extensions, bool useGzipVariant);

    /** Returns filters string with all formats listed in 'formatExtensionsByName' plus 'All files' filter. */
    static QString createFileFilter(const QMap<QString, QStringList>& formatExtensionsByName, bool addGzipVariant);

    /** Returns a filter with the given name and file extensions plus 'All files' filter. */
    static QString createFileFilter(const QString& name, const QStringList& extensions);

    /** Returns a filter with all document format extensions plus 'All files' filter. */
    static QString createFileFilterByDocumentFormatId(const DocumentFormatId& documentFormatId);

    /** Returns a filter with all document formats supported by UGENE plus 'All files' filter. */
    static QString createAllSupportedFormatsFileFilter(const QMap<QString, QStringList>& extraFilters = {});

    /** Returns a filter with all document formats what support writing of any of the given object types plus 'All files' filter. */
    static QString createFileFilterByObjectTypes(const QList<GObjectType>& objectTypes, bool useWriteOnlyFormats = false);
};

}  // namespace U2

#endif  // _U2_FILE_FILTERS_H_
