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

#ifndef _U2_SEQUENCE_UTILS_H_
#define _U2_SEQUENCE_UTILS_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/FormatSettings.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2Sequence.h>

namespace U2 {

class Annotation;
class DNASequence;
class DNAQuality;
class DNATranslation;
class U2SequenceObject;

/**
    U2Sequence and related structures utility functions
    */
class U2CORE_EXPORT U2SequenceUtils : public QObject {
    Q_OBJECT
public:
    /** Extract sequence parts marked by the regions
        Note: the order of complemented regions is also reversed
        */
    static QList<QByteArray> extractRegions(const U2EntityRef& seqRef, const QVector<U2Region>& regions, const DNATranslation* complTT, const DNATranslation* aminoTT, bool join, U2OpStatus& os);

    static DNAAlphabetType alphabetType(const U2EntityRef& ref, U2OpStatus& os);

    static qint64 length(const U2EntityRef& ref, U2OpStatus& os);

    // the two methods below don't use transactions so invalid objects can be created in DBI in case of an error.
    // client code, using these methods, must take care of transactions.
    static U2Sequence copySequence(const DNASequence& srcSeq, const U2DbiRef& dstDbi, const QString& dstFolder, U2OpStatus& os);
    static U2Sequence copySequence(const U2EntityRef& srcSeq, const U2DbiRef& dstDbi, const QString& dstFolder, U2OpStatus& os);

    /** Sets sequence name to 'newName'.*/
    static void updateSequenceName(const U2EntityRef& entityRef, const QString& newName, U2OpStatus& os);

    static U2EntityRef import(U2OpStatus& os, const U2DbiRef& dbiRef, const DNASequence& seq, const U2AlphabetId& alphabetId = U2AlphabetId());
    static U2EntityRef import(U2OpStatus& os, const U2DbiRef& dbiRef, const QString& folder, const DNASequence& seq, const U2AlphabetId& alphabetId = U2AlphabetId());

    static void setQuality(const U2EntityRef& entityRef, const DNAQuality& quality);
    static void setSequenceInfo(U2OpStatus& os, const U2EntityRef& entityRef, const QVariantMap& info);
    static QVariantMap getSequenceInfo(U2OpStatus& os, const U2EntityRef& entityRef, const QString& name);

    static U2Sequence getSequenceDbInfo(U2SequenceObject* seqObj);
};

/** Class used to efficiently import sequence into DBI */
class U2CORE_EXPORT U2SequenceImporter {
public:
    static const QString EMPTY_SEQUENCE_ERROR;
    U2SequenceImporter(const QVariantMap& fs = QVariantMap(), bool lazyMode = false, bool singleThread = true);
    U2SequenceImporter(qint64 _insertBlockSize, const QVariantMap& fs = QVariantMap(), bool lazyMode = false, bool singleThread = true);
    virtual ~U2SequenceImporter();

    /**
     * Enables amino translation of the input sequence.
     * When 'aminoTT' is set all content added with 'addBlock' methods will be automatically translated.
     */
    void enableAminoTranslation(const DNATranslation* aminoTT);

    /**
     * Enables reverse-complementary translation of input sequence blocks.
     * When 'complTT' is set all content added with 'addBlock' methods will be automatically reversed and complemented.
     */
    void enableReverseComplement(const DNATranslation* complTT);

    void startSequence(U2OpStatus& os, const U2DbiRef& dbiRef, const QString& folder, const QString& visualName, bool circular, const U2AlphabetId& alphabetId = U2AlphabetId());
    virtual void addBlock(const char* data, qint64 len, U2OpStatus& os);
    void addSequenceBlock(const U2EntityRef& seqId, const U2Region& r, U2OpStatus& os);
    virtual void addDefaultSymbolsBlock(int n, U2OpStatus& os);
    U2Sequence finalizeSequence(U2OpStatus& os);
    /** Checks that finalized sequence has the valid id */
    U2Sequence finalizeSequenceAndValidate(U2OpStatus& os);
    U2AlphabetId getAlphabet() const {
        return sequence.alphabet;
    }

    void setCaseAnnotationsMode(CaseAnnotationsMode mode);
    bool isCaseAnnotationsModeOn() const;
    QList<SharedAnnotationData>& getCaseAnnotations();

    virtual qint64 getCurrentLength() const;

protected:
    void _addBlock2Buffer(const char* data, qint64 len, U2OpStatus& os);
    void _addBlock2Db(const char* data, qint64 len, U2OpStatus& os);
    void _addBuffer2Db(U2OpStatus& os);

    DbiConnection con;
    QString folder;
    qint64 insertBlockSize;

    U2Sequence sequence;
    QByteArray sequenceBuffer;

    /** Buffer of the pending non translated DNA characters. Maximum size is 2 characters. */
    QByteArray aminoTranslationBuffer;

    // for lower case annotations
    qint64 currentLength;
    QList<SharedAnnotationData> annList;
    bool isUnfinishedRegion;
    U2Region unfinishedRegion;
    CaseAnnotationsMode caseAnnsMode;

    bool lazyMode;
    bool singleThread;

    bool sequenceCreated;
    qint64 committedLength;  // singleThread only

    const DNATranslation* aminoTT = nullptr;
    const DNATranslation* complTT = nullptr;
};

/** Class to read sequences when there is already readers which use U2SequenceImporter interface */
class U2CORE_EXPORT U2MemorySequenceImporter : public U2SequenceImporter {
public:
    U2MemorySequenceImporter(QByteArray& data)
        : sequenceData(data) {
    }
    virtual ~U2MemorySequenceImporter() {
    }

    virtual void addBlock(const char* data, qint64 len, U2OpStatus& os);
    virtual void addDefaultSymbolsBlock(int n, U2OpStatus& os);

    virtual qint64 getCurrentLength() const;

private:
    void startSequence(const U2DbiRef& dbiRef, const QString& folder, const QString& visualName, bool circular, U2OpStatus& os);
    void addSequenceBlock(const U2EntityRef& seqId, const U2Region& r, U2OpStatus& os);
    U2Sequence finalizeSequence(U2OpStatus& os);
    void setCaseAnnotationsMode(CaseAnnotationsMode mode);
    bool isCaseAnnotationsModeOn() const;
    QList<SharedAnnotationData>& getCaseAnnotations();

    QByteArray& sequenceData;
};

class U2CORE_EXPORT U2SequenceDbiHints {
public:
    static const QString UPDATE_SEQUENCE_LENGTH;
    static const QString EMPTY_SEQUENCE;
};

/**
 * Utility class to deal with circular sequences.
 * Supports mapping of the original sequence coordinates to a circular sequence coordinates with a specified 'overlap' range.
 */
class U2CORE_EXPORT U2PseudoCircularization {
    U2PseudoCircularization() = delete;

public:
    /**
     * Creates a sequence that has all linear regions in the original sequence with at least 'maxLinearRegionLength' length.
     * If 'maxLinearRegionLength' is < 0 the uses 'sequence.length' as the 'maxLinearRegionLength'.
     */
    static QByteArray createSequenceWithCircularOverlaps(const QByteArray& sequence, int maxLinearRegionLength = -1);

    /** Maps region of the sequence created by 'createSequenceWithCircularOverlaps' to the original sequence coordinates. */
    static QVector<U2Region> getOriginalSequenceCoordinates(const U2Region& circularRegion, qint64 originalSequenceLength);

    /**
     * Converts (in-place) location built for a sequence created with 'createSequenceWithCircularOverlaps'
     * into the original sequence coordinates.
     */
    static void convertToOriginalSequenceCoordinates(U2Location& location, qint64 originalSequenceLength);
};

}  // namespace U2

#endif
