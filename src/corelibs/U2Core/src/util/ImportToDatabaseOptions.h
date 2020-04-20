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

#ifndef _U2_IMPORT_TO_DATABASE_OPTIONS_H_
#define _U2_IMPORT_TO_DATABASE_OPTIONS_H_

#include <QStringList>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT ImportToDatabaseOptions {
public:
    ImportToDatabaseOptions();

    enum MultiSequencePolicy {
        SEPARATE,
        MERGE,
        MALIGNMENT
    };

    bool                    createSubfolderForEachDocument;
    bool                    createSubfolderForEachFile;
    bool                    createSubfolderForTopLevelFolder;
    bool                    importUnknownAsUdr;
    bool                    keepFileExtension;
    bool                    keepFoldersStructure;
    int                     mergeMultiSequencePolicySeparatorSize;
    MultiSequencePolicy     multiSequencePolicy;
    QStringList             preferredFormats;
    bool                    processFoldersRecursively;

    bool operator == (const ImportToDatabaseOptions& other) const;
    bool operator != (const ImportToDatabaseOptions& other) const;

    static const QString    DESTINATION_FOLDER;
    static const QString    KEEP_FOLDERS_STRUCTURE;
    static const QString    PROCESS_FOLDERS_RECUSIVELY;
    static const QString    CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER;
    static const QString    CREATE_SUBFOLDER_FOR_EACH_FILE;
    static const QString    IMPORT_UNKNOWN_AS_UDR;
    static const QString    MULTI_SEQUENCE_POLICY;
    static const QString    PREFERRED_FORMATS;
    static const QString    CREATE_SUBFOLDER_FOR_DOCUMENT;
    static const QString    MERGE_MULTI_SEQUENCE_POLICY_SEPARATOR_SIZE;
};

}   // namespace U2

#endif // _U2_IMPORT_TO_DATABASE_OPTIONS_H_
