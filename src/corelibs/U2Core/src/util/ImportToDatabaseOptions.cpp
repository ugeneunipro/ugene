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

#include <QFileInfo>

#include "ImportToDatabaseOptions.h"

namespace U2 {

const QString ImportToDatabaseOptions::DESTINATION_FOLDER = "destination_folder";
const QString ImportToDatabaseOptions::KEEP_FOLDERS_STRUCTURE = "keep_folders_structure";
const QString ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY = "process_folders_recusively";
const QString ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER = "create_subfolder_for_top_level_folder";
const QString ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_EACH_FILE = "create_subfolder_for_each_file";
const QString ImportToDatabaseOptions::IMPORT_UNKNOWN_AS_UDR = "import_unknown_as_udr";
const QString ImportToDatabaseOptions::MULTI_SEQUENCE_POLICY = "multi_sequence_policy";
const QString ImportToDatabaseOptions::PREFERRED_FORMATS = "preferred_formats";
const QString ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_DOCUMENT = "create_subfolder_for_document";
const QString ImportToDatabaseOptions::MERGE_MULTI_SEQUENCE_POLICY_SEPARATOR_SIZE = "merge_multi_sequence_policy_separator_size";

ImportToDatabaseOptions::ImportToDatabaseOptions() :
createSubfolderForEachDocument(true),
createSubfolderForEachFile(true),
createSubfolderForTopLevelFolder(false),
importUnknownAsUdr(false),
keepFileExtension(false),
keepFoldersStructure(true),
mergeMultiSequencePolicySeparatorSize(10),
multiSequencePolicy(SEPARATE),
processFoldersRecursively(true) {}

bool ImportToDatabaseOptions::operator == (const ImportToDatabaseOptions &other) const {
    return createSubfolderForEachDocument == other.createSubfolderForEachDocument &&
        createSubfolderForEachFile == other.createSubfolderForEachFile &&
        createSubfolderForTopLevelFolder == other.createSubfolderForTopLevelFolder &&
        importUnknownAsUdr == other.importUnknownAsUdr &&
        keepFileExtension == other.keepFileExtension &&
        keepFoldersStructure == other.keepFoldersStructure &&
        mergeMultiSequencePolicySeparatorSize == other.mergeMultiSequencePolicySeparatorSize &&
        multiSequencePolicy == other.multiSequencePolicy &&
        preferredFormats == other.preferredFormats &&
        processFoldersRecursively == other.processFoldersRecursively;
}

bool ImportToDatabaseOptions::operator !=(const ImportToDatabaseOptions &other) const {
    return !operator ==(other);
}

}   // namespace U2
