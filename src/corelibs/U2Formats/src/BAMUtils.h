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

#ifndef _U2_BAM_UTILS_H_
#define _U2_BAM_UTILS_H_

#include <QFile>

#include <U2Core/DNASequence.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrl.h>
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

    static bool isSortedBam(const QString& bamUrl, U2OpStatus& os);

    /**
     * @sortedBamBaseName is the result file path without extension.
     * Returns @sortedBamBaseName.bam
     */
    static GUrl sortBam(const QString& bamUrl, const QString& sortedBamFilePath, U2OpStatus& os);

    static GUrl mergeBam(const QStringList& bamUrl, const QString& mergedBamTargetUrl, U2OpStatus& os);

    static bool hasValidBamIndex(const QString& bamUrl);

    static bool hasValidFastaIndex(const QString& fastaUrl);

    static void createBamIndex(const QString& bamUrl, U2OpStatus& os);

    static GUrl getBamIndexUrl(const QString& bamUrl);

    static void writeDocument(Document* doc, U2OpStatus& os);

    static void writeObjects(const QList<GObject*>& objects, const QString& url, const DocumentFormatId& formatId, U2OpStatus& os, const U2Region& desiredRegion = U2_REGION_MAX);

    static bool isEqualByLength(const QString& fileUrl1, const QString& fileUrl2, U2OpStatus& os);

    /**
     * Returns the list of names of references (despite "*") found among reads.
     */
    static QStringList scanSamForReferenceNames(const GUrl& samUrl, U2OpStatus& os);

    /**
     * Saves the list of references to the file in the SAMtools fai format.
     */
    static void createFai(const GUrl& faiUrl, const QStringList& references, U2OpStatus& os);

    /**
     * Calls fopen() correctly for files with Unicode
     * names and returns a FILE* structure for the the opened file.
     * For 'mode' see fopen() function description.
     * Caller is responsible to close the file.
     * If any error happens the method returns nullptr.
     */
    static FILE* openFile(const QString& path, const QString& mode);

    /** Loads BAM index from the file (bam_index_t*). Returns nullptr of error. */
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

#endif  // _U2_BAM_UTILS_H_
