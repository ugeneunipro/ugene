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

#include <QFile>

#include <U2Core/DNASequence.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrl.h>
#include <U2Core/Nullable.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Region.h>

namespace U2 {

class Document;
class GObject;
class AssemblyObject;

class U2FORMATS_EXPORT BAMUtils : public QObject {
    Q_OBJECT
public:
    /** Converts BAM file to SAM file. */
    static void convertBamToSam(U2OpStatus& os, const QString& bamPath, const QString& samPath);

    /** Converts SAM file to BAM file. */
    static void convertSamToBam(U2OpStatus& os, const QString& samPath, const QString& bamPath, const QString& referencePath = "");

    /** Return true if file is sorted. */
    static bool isSortedBam(const QString& bamUrl, U2OpStatus& os);

    /**
     * @sortedBamBaseName is the result file path without extension.
     * Returns @sortedBamBaseName.bam
     */
    static GUrl sortBam(const QString& bamUrl, const QString& sortedBamFilePath, U2OpStatus& os);

    /**
     * Merges multiple sorted BAM.
     * Returns name of the merget file.
     */
    static GUrl mergeBam(const QStringList& bamUrl, const QString& mergedBamTargetUrl, U2OpStatus& os);

    /** Check if a BAM file ha correct index. */
    static bool hasValidBamIndex(const QString& bamUrl);

    /** Check that a FASTA file has correct index in a .fai file. */
    static bool hasValidFastaIndex(const QString& fastaUrl);

    /**
     * Builds and saves index for BAM file.
     * Index file will be saved to "@bamUrl + .bai".
     */
    static void createBamIndex(const QString& bamUrl, U2OpStatus& os);

    /**
     * Gets ".bai" file path, if index is valid.
     */
    static GUrl getBamIndexUrl(const QString& bamUrl);

    /**
     * Writes current document.
     */
    static void writeDocument(Document* doc, U2OpStatus& os);

    /**
     * Writes assembly objects to the set URL. Result assembly will be resized to fit reads without empty areas.
     */
    static void writeObjects(const QList<GObject*>& objects, const QString& url, const DocumentFormatId& formatId, U2OpStatus& os, const U2Region& desiredRegion = U2_REGION_MAX);

    /**
     * Check that two assemblies have equal length.
     */
    static bool isEqualByLength(const QString& fileUrl1, const QString& fileUrl2, U2OpStatus& os);

    /** Loads HTS index from the file (hts_idx_t*). Returns nullptr of error. */
    static void* loadIndex(const QString& path);
};

// iterates over a FASTQ file (including zipped) with kseq from samtools
class U2FORMATS_EXPORT FASTQIterator {
public:
    FASTQIterator(const QString& fileUrl, U2OpStatus& os);
    virtual ~FASTQIterator();

    DNASequence next();
    bool hasNext() const;

private:
    void fetchNext();

    void* fp;
    void* seq;
};

}  // namespace U2
