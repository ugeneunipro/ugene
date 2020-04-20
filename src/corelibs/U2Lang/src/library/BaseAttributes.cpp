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

#include <QVariant>

#include "BaseAttributes.h"

static const QString URL_LOCATION_ATTR_ID("url_location");
static const QString URL_OUT_ATTR_ID("url-out");
static const QString URL_SUFFIX_ATTR_ID("url-suffix");
static const QString URL_IN_ATTR_ID("url-in");
static const QString DATA_STORAGE_ATTR_ID("data-storage");
static const QString DATABASE_ATTR_ID("database");
static const QString DB_PATH_ID("db-path");
static const QString DOCUMENT_FORMAT_ATTR_ID("document-format");
static const QString APPEND_ATTR_ID("accumulate");
static const QString READ_BY_LINES_ATTR_ID("read-by-lines");
static const QString FILE_MODE_ATTR_ID("write-mode");

static const QString STRAND_ATTR_ID("strand");
static const QString AMINO_TRANSLATION_OFFSET_ATTR_ID("translation");
static const QString SPLIT_ATTR("split");

namespace U2 {
namespace Workflow {

const Descriptor BaseAttributes::URL_IN_ATTRIBUTE() {
    return Descriptor(URL_IN_ATTR_ID, tr("Input file(s)"), tr("Semicolon-separated list of paths to the input files."));
}

const Descriptor BaseAttributes::URL_OUT_ATTRIBUTE() {
    return Descriptor(URL_OUT_ATTR_ID, tr("Output file"), tr("Location of output data file. If this attribute is set,"
        " slot \"Location\" in port will not be used."));
}

const Descriptor BaseAttributes::URL_SUFFIX() {
    return Descriptor(URL_SUFFIX_ATTR_ID, tr("Output file suffix"), tr("This suffix will be used for generating the output file name."));
}

const Descriptor BaseAttributes::URL_LOCATION_ATTRIBUTE() {
    return Descriptor(URL_LOCATION_ATTR_ID, tr("Located on"), tr("Machine file(s) are located on"));
}

const Descriptor BaseAttributes::DATA_STORAGE_ATTRIBUTE() {
    return Descriptor(DATA_STORAGE_ATTR_ID, tr("Data storage"), tr("Place to store workflow results"));
}

const QString BaseAttributes::LOCAL_FS_DATA_STORAGE() {
    return "Local file system";
}

const QString BaseAttributes::SHARED_DB_DATA_STORAGE() {
    return "Shared UGENE database";
}

const QVariantMap BaseAttributes::DATA_STORAGE_ATTRIBUTE_VALUES_MAP() {
    QVariantMap resultMap;

    resultMap[LOCAL_FS_DATA_STORAGE()] = LOCAL_FS_DATA_STORAGE();
    resultMap[SHARED_DB_DATA_STORAGE()] = SHARED_DB_DATA_STORAGE();

    return resultMap;
}

const Descriptor BaseAttributes::DATABASE_ATTRIBUTE() {
    return Descriptor(DATABASE_ATTR_ID, tr("Database"), tr("The URL or name of a shared UGENE database"));
}

const Descriptor BaseAttributes::DB_PATH() {
    return Descriptor(DB_PATH_ID, tr("Output path"), tr("Location of output objects in a shared database"));
}

const Descriptor BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE() {
    return Descriptor(DOCUMENT_FORMAT_ATTR_ID, tr("Document format"), tr("Document format of output file."));
}

const Descriptor BaseAttributes::ACCUMULATE_OBJS_ATTRIBUTE() {
    return Descriptor(APPEND_ATTR_ID, tr("Accumulate objects"),
        tr("Accumulate all incoming data in one file or create separate files for each input."
        "In the latter case, an incremental numerical suffix is added to the file name."));
}

const Descriptor BaseAttributes::SPLIT_SEQ_ATTRIBUTE(){
    return Descriptor(SPLIT_ATTR, tr("Split sequence"), tr("Split each incoming sequence on several parts."));
}

const Descriptor BaseAttributes::READ_BY_LINES_ATTRIBUTE() {
    return Descriptor(READ_BY_LINES_ATTR_ID, tr("Read by lines"), tr("Reads the input file line by line."));
}

const Descriptor BaseAttributes::FILE_MODE_ATTRIBUTE() {
    return Descriptor(FILE_MODE_ATTR_ID, tr("Existing file"), tr("If a target file already exists, you can specify"
        " how it should be handled: either overwritten, renamed"
        " or appended (if supported by file format). If Rename option is chosen existing file will be renamed."));
}

const Descriptor BaseAttributes::STRAND_ATTRIBUTE() {
    return Descriptor(STRAND_ATTR_ID, tr("Search in"), tr("Which strands should be searched: direct, complement or both."));
}

const QString BaseAttributes::STRAND_BOTH() {
    return "both";
}

const QString BaseAttributes::STRAND_DIRECT() {
    return "direct";
}

const QString BaseAttributes::STRAND_COMPLEMENTARY() {
    return "complementary";
}

const QVariantMap BaseAttributes::STRAND_ATTRIBUTE_VALUES_MAP() {
    QVariantMap strandMap;

    // The constant strings are used to generate the translations properly.
    // When a string is used as an argument of the strandMap, nothing is generated.
    const QString bothStrands = tr("both strands");
    const QString directStrand = tr("direct strand");
    const QString complementaryStrand = tr("complementary strand");

    strandMap[bothStrands] = STRAND_BOTH();
    strandMap[directStrand] = STRAND_DIRECT();
    strandMap[complementaryStrand] = STRAND_COMPLEMENTARY();

    return strandMap;
}



} // Workflow
} // U2
