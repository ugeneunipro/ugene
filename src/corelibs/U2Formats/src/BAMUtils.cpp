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

#include <QFileInfo>
#include <QTemporaryFile>
#include <QUuid>

#include <U2Core/FileAndDirectoryUtils.h>

extern "C" {
#include <samtools_core/bam.h>
#include <samtools_core/bam_sort.c>

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4018)
#endif

#include <htslib/sam.h>

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

#include <htslib/faidx.h>
#include <htslib/kseq.h>
}

#include <3rdparty/zlib/zlib.h>

#include <SamtoolsAdapter.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AssemblyDbiUtils.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BAMUtils.h"

namespace U2 {

/** Safely opens gzip file. Supports unicode file names. */
static gzFile openGzipFile(const QString& fileUrl, const char* mode = "r") {
    gzFile fp = nullptr;
#ifdef Q_OS_WIN
    QScopedPointer<wchar_t> unicodeFileName(TextUtils::toWideCharsArray(fileUrl));
    fp = gzopen_w(unicodeFileName.data(), mode);
#else
    fp = gzopen(fileUrl.toLocal8Bit().constData(), mode);
#endif
    return fp;
}

static void closeFiles(samFile* in, samFile* out) {
    sam_close(in);
    sam_close(out);
}

static QString openFileError(const QString& file) {
    return QObject::tr("Fail to open \"%1\" for reading").arg(file);
}

static QString headerReadError(const QString& file) {
    return QObject::tr("Fail to read the header from the file, probably, not an assembly format: \"%1\"").arg(file);
}

static QString headerWriteError(const QString& file) {
    return QObject::tr("Fail to write the header to the file: \"%1\"").arg(file);
}

static QString truncatedError(const QString& file) {
    return QObject::tr("Truncated file: \"%1\"").arg(file);
}

static void checkFileReadState(int read, U2OpStatus& os, const QString& fileName) {
    if (read < -1) {
        os.setError(truncatedError(fileName) + ", code: " + QString::number(read));
    }
}

#define SAMTOOL_CHECK(cond, msg, ret) \
    if (!(cond)) { \
        os.setError(msg); \
        closeFiles(in, out); \
        return ret; \
    }


namespace {
static constexpr int BUFFER_SIZE = 1024 * 1024;  // 1 Mb
static constexpr int REFERENCE_NAME_COLUMN = 2;
static constexpr int FIRST_BASE_POS_COLUMN = 3;
static constexpr int READ_COLUMN = 9;

inline QByteArray readLine(IOAdapter* io, char* buffer, int maxLineLength) {
    QByteArray result;
    bool terminatorFound = false;
    do {
        qint64 length = io->readLine(buffer, maxLineLength, &terminatorFound);
        CHECK(length > 0, result);
        result += QByteArray(buffer, length);
    } while (!terminatorFound);
    return result;
}

/**
 * Saves the list of references to the file in the SAMtools fai format.
 */
void createReferenceDataFile(const GUrl& faiUrl, const QMap<QString, int>& referenceData, U2OpStatus& os) {
    QScopedPointer<IOAdapter> io(IOAdapterUtils::open(faiUrl, os, IOAdapterMode_Write));
    CHECK_OP(os, );

    const auto& references = referenceData.keys();
    // Example:
    // @SQ SN:ref LN:45
    for (const auto& reference : qAsConst(references)) {
        int length = referenceData[reference];
        QString line = "@SQ\tSN:" + reference + "\tLN:" + QString::number(length) + "\n";
        io->writeBlock(line.toLocal8Bit());
    }
}

/**
 * Returns the map, which contains reference names as keys and their alignments lengths as values.
 */
static QMap<QString, int> scanSamForReferenceInfo(const GUrl& samUrl, U2OpStatus& os) {
    QMap<QString, int> result;
    QScopedPointer<IOAdapter> io(IOAdapterUtils::open(samUrl, os));
    CHECK_OP(os, result);

    QByteArray buffer(BUFFER_SIZE, 0);
    char* bufferData = buffer.data();
    do {
        QByteArray line = readLine(io.data(), bufferData, BUFFER_SIZE);
        CHECK_CONTINUE(!line.isEmpty());
        CHECK_CONTINUE(!line.startsWith("@"));

        QList<QByteArray> columns = line.split('\t');
        if (columns.size() <= READ_COLUMN) {
            coreLog.error(BAMUtils::tr("Wrong line in a SAM file: \"%1\". Skipped").arg(QString(line)));
        }

        bool ok = false;
        // From the SAM specification: reference sequence length. Range: [1, 2^31 - 1]
        auto firstBasePos = columns[FIRST_BASE_POS_COLUMN].toInt(&ok);
        if (!ok) {
            coreLog.error(BAMUtils::tr("Wrong left base position format: \"%1\". Line has been skipped.")
                              .arg(QString(line)));
            continue;
        }
        // Positions start from 0 in UGENE, but SAM has 1 base first
        firstBasePos--;

        const auto& readLength = columns[READ_COLUMN].size();
        int alignmentLength = firstBasePos + readLength;
        const auto& referenceName = QString(columns[REFERENCE_NAME_COLUMN]);
        result[referenceName] = qMax(result.value(referenceName, 0), alignmentLength);

    } while (!io->isEof());
    return result;
}

QByteArray getFileFormatName(const QString& filePath) {
    FormatDetectionConfig cfg;
    // SAM/BAM formats are hidden
    cfg.excludeHiddenFormats = false;
    // To avoid VCF format confusion (e.g. when data are pasted from clipboard)
    // GUITest_common_scenarios_project_test_0058
    cfg.useExtensionBonus = false;
    auto formats = DocumentUtils::detectFormat(filePath, cfg);
    CHECK(!formats.empty(), QFileInfo(filePath).completeSuffix().toLocal8Bit());

    auto format = formats.first().format->getFormatId().toLocal8Bit();
    if (format == BaseDocumentFormats::SAM &&
        IOAdapterUtils::url2io(filePath) == BaseIOAdapters::GZIPPED_LOCAL_FILE) {
        format.append(".gz");
    }

    return format;
}


}  // namespace


#define SAMTOOL_READ_CHECK(cond, msg) \
    if (!(cond)) { \
        os.setError(msg); \
        sam_close(res); \
        return nullptr; \
    }

samFile* openForRead(const QString& path, U2OpStatus& os, const QString& referencePath = "") {
    auto format = getFileFormatName(path);
    samFile* res = sam_open(path.toLocal8Bit(), format  == BaseDocumentFormats::BAM ? "rb" : "r");
    SAMTOOL_READ_CHECK(res != nullptr, openFileError(path));

    QString faiPath = BAMUtils::hasValidFastaIndex(referencePath) ? referencePath + ".fai" : "";
    if (!faiPath.isEmpty()) {
        hts_set_fai_filename(res, faiPath.toLocal8Bit());
    }

    if (res->bam_header == nullptr) {
        hts_parse_format(&res->format, format);
    }
    res->bam_header = sam_hdr_read(res);
    SAMTOOL_READ_CHECK(res->bam_header != nullptr, headerReadError(path));

    if (res->bam_header->n_targets == 0) {
        coreLog.details(BAMUtils::tr("There is no header in the SAM file \"%1\". The header information will be generated automatically.").arg(path));

        auto referenceMap = scanSamForReferenceInfo(path, os);
        SAMTOOL_READ_CHECK(!os.isCoR(), os.getError());
        SAMTOOL_READ_CHECK(!referenceMap.empty(), BAMUtils::tr("No reference data in the file: %1").arg(path));

        QTemporaryFile referenceDataFile;
        referenceDataFile.open();
        QString referenceDataFileUrl = referenceDataFile.fileName();
        createReferenceDataFile(referenceDataFileUrl, referenceMap, os);
        SAMTOOL_READ_CHECK(!os.isCoR(), os.getError());

        samFile* tmp = sam_open(referenceDataFileUrl.toLocal8Bit(), "r");
        SAMTOOL_READ_CHECK(tmp != nullptr, openFileError(referenceDataFileUrl));

        hts_parse_format(&tmp->format, BaseDocumentFormats::SAM.toLocal8Bit());
        res->bam_header = sam_hdr_read(tmp);
        sam_close(tmp);
        SAMTOOL_READ_CHECK(res->bam_header != nullptr, headerReadError(referenceDataFileUrl));
        SAMTOOL_READ_CHECK(res->bam_header->n_targets != 0, headerReadError(referenceDataFileUrl));
    }

    return res;
}

void BAMUtils::convertBamToSam(U2OpStatus& os, const QString& bamPath, const QString& samPath) {
    samFile* in = nullptr;
    samFile* out = nullptr;

    in = openForRead(bamPath, os);
    CHECK_OP(os, );

    out = sam_open(samPath.toLocal8Bit(), "w");
    SAMTOOL_CHECK(out != nullptr, openFileError(samPath), );

    int r = 0;
    r = sam_hdr_write(out, in->bam_header);
    SAMTOOL_CHECK(r == 0, headerWriteError(samPath), );

    bam1_t* b = bam_init1();
    r = 0;
    while ((r = bam_read1(in->fp.bgzf, b)) >= 0) {  // read one alignment from `in'
        sam_write1(out, in->bam_header, b);  // write the alignment to `out'.
    }
    checkFileReadState(r, os, bamPath);
    bam_destroy1(b);
    closeFiles(in, out);
}

void BAMUtils::convertSamToBam(U2OpStatus& os, const QString& samPath, const QString& bamPath, const QString& referencePath) {
    samFile* in = nullptr;
    samFile* out = nullptr;

    in = openForRead(samPath, os, referencePath);
    CHECK_OP(os, );

    out = sam_open(bamPath.toLocal8Bit(), "wb");
    SAMTOOL_CHECK(out != nullptr, openFileError(bamPath), );

    int r = 0;
    r = bam_hdr_write(out->fp.bgzf, in->bam_header);
    SAMTOOL_CHECK(r == 0, headerWriteError(bamPath), );

    bam1_t* b = bam_init1();
    while ((r = sam_read1(in, in->bam_header, b) >= 0)) {  // read one alignment from `in'
        bam_write1(out->fp.bgzf, b);  // write the alignment to `out'
    }

    checkFileReadState(r, os, bamPath);
    bam_destroy1(b);
    closeFiles(in, out);
}

static bool isSorted(const QString& headerText) {
    QString text = headerText;
    text = text.replace('\r', QString(""));
    QStringList lines = text.split('\n');

    bool result = false;
    for (const auto& line : lines) {
        CHECK_CONTINUE(line.startsWith("@HD"));

        QStringList tokens = line.split('\t');
        bool found = false;
        for (const auto& token : tokens) {
            int colonIdx = token.indexOf(':');
            CHECK_CONTINUE(colonIdx != -1);

            QString fieldTag = token.mid(0, colonIdx);
            CHECK_CONTINUE(fieldTag == "SO");

            QString fieldValue = token.mid(colonIdx + 1);
            result = ("coordinate" == fieldValue);
            found = true;
            break;
        }
        CHECK_BREAK(!found);
    }

    return result;
}

bool BAMUtils::isSortedBam(const QString& bamUrl, U2OpStatus& os) {
    bam_hdr_t* header = nullptr;
    QString error;
    bool result = false;

    BGZF* bamHandler = bgzf_open(bamUrl.toLocal8Bit(), "r");
    if (bamHandler != nullptr) {
        header = bam_hdr_read(bamHandler);
        if (header != nullptr) {
            result = isSorted(header->text);
        } else {
            error = headerReadError(bamUrl);
        }
    } else {
        error = openFileError(bamUrl);
    }

    // deallocate resources
    {
        if (header != nullptr) {
            bam_hdr_destroy(header);
        }
        if (bamHandler != nullptr) {
            bgzf_close(bamHandler);
        }
    }

    CHECK_EXT(error.isEmpty(), os.setError(error), false);

    return result;
}

GUrl BAMUtils::sortBam(const QString& bamUrl, const QString& sortedBamFilePath, U2OpStatus& os) {
    QString sortedBamFilePathPrefix = sortedBamFilePath.endsWith(".bam")
                                          ? sortedBamFilePath.left(sortedBamFilePath.length() - 4)
                                          : sortedBamFilePath;
    coreLog.trace(BAMUtils::tr("Sorting \"%1\" and saving the result to \"%2\"").arg(bamUrl).arg(sortedBamFilePath));

    static constexpr size_t max_mem = 100 * 1000 * 1000;
    int ret = bam_sort_core(0, bamUrl.toLocal8Bit(), sortedBamFilePathPrefix.toLocal8Bit(), max_mem);
    CHECK_EXT(ret == 0, os.setError(BAMUtils::tr("Cannot sort \"%1\", abort").arg(bamUrl)), {})

    return sortedBamFilePathPrefix + ".bam";
}

GUrl BAMUtils::mergeBam(const QStringList& bamUrls, const QString& mergedBamTargetUrl, U2OpStatus& os) {
    coreLog.details(tr(R"(Merging BAM files: "%1". Resulting merged file is: "%2")")
                        .arg(QString(bamUrls.join(",")))
                        .arg(QString(mergedBamTargetUrl)));

    int buSize = bamUrls.size();
    char** files = new char*[buSize + 1];
    for (int i = 0; i < buSize; i++) {
        files[i] = strdup(bamUrls[i].toStdString().c_str());
    }
    files[buSize] = nullptr;

    int rc = bam_merge_core(0, mergedBamTargetUrl.toLocal8Bit(), nullptr, buSize, files, 0, nullptr);
    delete[] files;
    CHECK_EXT(rc >= 0, os.setError(tr("Failed to merge BAM files: %1 into %2").arg(bamUrls.join(",")).arg(mergedBamTargetUrl)), {});

    return mergedBamTargetUrl;
}

void* BAMUtils::loadIndex(const QString& filePath) {
    return hts_idx_load(filePath.toLocal8Bit(), HTS_FMT_BAI);
}

bool BAMUtils::hasValidBamIndex(const QString& bamUrl) {
    auto index = (hts_idx_t*)loadIndex(bamUrl);
    CHECK(index != nullptr, false);
    hts_idx_destroy(index);

    QFileInfo idxFileInfo(bamUrl + ".bai");
    if (!idxFileInfo.exists()) {
        QString indexUrl = bamUrl;
        indexUrl.chop(4);
        idxFileInfo.setFile(indexUrl + ".bai");
    }
    QFileInfo bamFileInfo(bamUrl);

    return idxFileInfo.lastModified() >= bamFileInfo.lastModified();
}

bool BAMUtils::hasValidFastaIndex(const QString& fastaUrl) {
    CHECK(!fastaUrl.isEmpty(), false);
    QFileInfo idxFileInfo(fastaUrl + ".fai");
    if (!idxFileInfo.exists() || !idxFileInfo.isReadable()) {
        return false;
    }
    QFileInfo fastaFileInfo(fastaUrl);
    return idxFileInfo.lastModified() >= fastaFileInfo.lastModified();
}

void BAMUtils::createBamIndex(const QString& bamUrl, U2OpStatus& os) {
    coreLog.details(BAMUtils::tr("Build index for bam file: \"%1\"").arg(bamUrl));
    int res = bam_index_build(bamUrl.toLocal8Bit(), 0);
    CHECK_EXT(res == 0, os.setError(tr("Can't build the index: %1").arg(bamUrl)), );
}

GUrl BAMUtils::getBamIndexUrl(const QString& bamUrl) {
    CHECK(hasValidBamIndex(bamUrl), {});

    QFileInfo fileInfo(bamUrl + ".bai");
    if (!fileInfo.exists()) {
        fileInfo.setFile(bamUrl.chopped(4) + ".bai");
    }
    SAFE_POINT(fileInfo.exists(), "Can't find the index file", GUrl());

    return GUrl(fileInfo.filePath());
}

static qint64 getSequenceLength(U2Dbi* dbi, const U2DataId& objectId, U2OpStatus& os) {
    qint64 seqLength = -1;

    U2AssemblyDbi* assemblyDbi = dbi->getAssemblyDbi();
    SAFE_POINT_EXT(assemblyDbi != nullptr, os.setError("NULL assembly DBI"), seqLength);

    U2AttributeDbi* attributeDbi = dbi->getAttributeDbi();
    bool useMaxPos = true;
    if (attributeDbi != nullptr) {
        U2IntegerAttribute attr = U2AttributeUtils::findIntegerAttribute(attributeDbi, objectId, U2BaseAttributeName::reference_length, os);
        CHECK_OP(os, seqLength);
        if (attr.hasValidId()) {
            seqLength = attr.value;
            useMaxPos = false;
        }
    }
    if (useMaxPos) {
        seqLength = assemblyDbi->getMaxEndPos(objectId, os) + 1;
        CHECK_OP(os, seqLength);
    }

    return seqLength;
}

static void createHeader(bam_hdr_t* header, const QList<GObject*>& objects, U2OpStatus& os) {
    CHECK_EXT(header != nullptr, os.setError("NULL header"), );

    header->n_targets = objects.size();
    header->target_name = new char*[header->n_targets];
    int objIdx = 0;
    for (GObject* obj : qAsConst(objects)) {
        QByteArray seqName = obj->getGObjectName().toLatin1();
        header->target_name[objIdx] = new char[seqName.length() + 1];
        {
            char* name = header->target_name[objIdx];
            qstrncpy(name, seqName.constData(), seqName.length() + 1);
            name[seqName.length()] = 0;
        }
        objIdx++;
    }
}

static void updateHeaderSeqLengthsAndText(bam_hdr_t* header, const QList<GObject*>& objects, QList<qint64> objectLengths, U2OpStatus& os) {
    CHECK_EXT(header != nullptr, os.setError("NULL header"), );
    header->target_len = new uint32_t[header->n_targets];

    QByteArray headerText;
    headerText += "@HD\tVN:1.4\tSO:coordinate\n";

    int objIdx = 0;
    for (GObject* obj : qAsConst(objects)) {
        const qint64 seqLength = objectLengths.at(objIdx);
        header->target_len[objIdx] = seqLength;
        const QByteArray seqName = obj->getGObjectName().toLatin1();
        headerText += QString("@SQ\tSN:%1\tLN:%2\n").arg(seqName.constData()).arg(seqLength).toUtf8();
        objIdx++;
    }

    header->text = new char[headerText.length() + 1];
    qstrncpy(header->text, headerText.constData(), headerText.length() + 1);
    header->text[headerText.length()] = 0;
    header->l_text = headerText.length();
}

static QMap<QString, int> getNumMap(const QList<GObject*>& objects, U2OpStatus& os) {
    QMap<QString, int> result;
    int i = 0;
    foreach (GObject* obj, objects) {
        auto assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(assemblyObj != nullptr, os.setError("NULL assembly object"), result);

        QString name = assemblyObj->getGObjectName();
        result[name] = i;
        i++;
    }
    return result;
}

static void writeObjectsWithSamtools(samFile* out, const QList<GObject*>& objects, U2OpStatus& os, const U2Region& desiredRegion, bool isBinary) {
    QList<qint64> leftShifts;
    QList<qint64> overrideLengths;
    for (auto obj : qAsConst(objects)) {
        auto assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(assemblyObj != nullptr, os.setError("NULL assembly object"), );

        DbiConnection con(assemblyObj->getEntityRef().dbiRef, os);
        CHECK_OP(os, );

        U2AssemblyDbi* dbi = con.dbi->getAssemblyDbi();
        SAFE_POINT_EXT(dbi != nullptr, os.setError("NULL assembly DBI"), );

        U2DataId assemblyId = assemblyObj->getEntityRef().entityId;
        U2Region region(0, dbi->getMaxEndPos(assemblyId, os) + 1);
        if (desiredRegion != U2_REGION_MAX) {
            region = desiredRegion;
            overrideLengths.append(region.length);
        } else {
            overrideLengths.append(getSequenceLength(con.dbi, obj->getEntityRef().entityId, os));
            CHECK_OP(os, );
        }
        const qint64 leftShift = U2AssemblyDbiUtils::calculateLeftShiftForReadsInRegion(dbi, assemblyId, desiredRegion, os);
        CHECK_OP(os, );
        leftShifts.append(leftShift);
    }

    bam_hdr_t* header = bam_hdr_init();
    createHeader(header, objects, os);
    CHECK_OP_EXT(os, bam_hdr_destroy(header), )

    out->bam_header = header;

    if (isBinary) {
        updateHeaderSeqLengthsAndText(header, objects, overrideLengths, os);
        out->is_bin = 1;
        bam_hdr_write(out->fp.bgzf, out->bam_header);
    }
    int idx = 0;
    for (auto obj : qAsConst(objects)) {
        auto assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(assemblyObj != nullptr, os.setError("NULL assembly object"), );

        ReadsContext ctx(assemblyObj->getGObjectName(), getNumMap(objects, os));
        CHECK_OP(os, );
        bam1_t* read = bam_init1();

        DbiConnection con(assemblyObj->getEntityRef().dbiRef, os);
        CHECK_OP(os, );

        U2AssemblyDbi* dbi = con.dbi->getAssemblyDbi();
        SAFE_POINT_EXT(dbi != nullptr, os.setError("NULL assembly DBI"), );

        U2DbiIterator<U2AssemblyRead>* iter = dbi->getReads(assemblyObj->getEntityRef().entityId, desiredRegion, os, true, true);
        CHECK_OP(os, );
        QScopedPointer<U2DbiIterator<U2AssemblyRead>> iterPtr(iter);
        qint64 maxAssemblyLength = 0;
        while (iter->hasNext()) {
            U2AssemblyRead r = iter->next();
            r->leftmostPos -= leftShifts[idx];
            SamtoolsAdapter::read2samtools(r, ctx, os, *read);
            CHECK_OP_EXT(os, bam_destroy1(read), );
            sam_write1(out, out->bam_header, read);
            maxAssemblyLength = qMax(maxAssemblyLength, r->leftmostPos + r->effectiveLen);
        }
        bam_destroy1(read);
        overrideLengths.append(maxAssemblyLength);
        idx++;
    }
    
    if (!isBinary) {
        updateHeaderSeqLengthsAndText(header, objects, overrideLengths, os);
        CHECK_OP_EXT(os, bam_hdr_destroy(header), )
        out->is_bin = 0;
        sam_hdr_write(out, out->bam_header);
    }    
}

void BAMUtils::writeDocument(Document* doc, U2OpStatus& os) {
    writeObjects(
        doc->findGObjectByType(GObjectTypes::ASSEMBLY),
        doc->getURL().getURLString(),
        doc->getDocumentFormatId(),
        os);
}

void BAMUtils::writeObjects(const QList<GObject*>& objects, const QString& url, const DocumentFormatId& formatId, U2OpStatus& os, const U2Region& desiredRegion) {
    CHECK_EXT(!objects.isEmpty(), os.setError("No assembly objects"), );
    CHECK_EXT(!url.isEmpty(), os.setError("Empty file url"), );

    QByteArray openMode("w");
    if (formatId == BaseDocumentFormats::BAM) {
        openMode += "b";  // BAM output
    } else if (formatId == BaseDocumentFormats::SAM) {
        openMode += "h";  // SAM only: write header
    } else {
        os.setError("Only BAM or SAM files could be written");
        return;
    }    
    samFile* out = sam_open(url.toLocal8Bit(), openMode.constData());
    CHECK_EXT(out != nullptr, os.setError(QString("Can not open file for writing: %1").arg(url)), );
    writeObjectsWithSamtools(out, objects, os, desiredRegion, openMode.contains('b'));
    sam_close(out);
}

// the function assumes the equal order of alignments in files
bool BAMUtils::isEqualByLength(const QString& fileUrl1, const QString& fileUrl2, U2OpStatus& os) {
    samFile* in = nullptr;
    samFile* out = nullptr;

    in = openForRead(fileUrl1, os);
    SAMTOOL_CHECK(in != nullptr, openFileError(fileUrl1), false);
    CHECK_OP(os, false);

    out = openForRead(fileUrl2, os);
    SAMTOOL_CHECK(out != nullptr, openFileError(fileUrl2), false);
    CHECK_OP(os, false);

    if (in->bam_header != out->bam_header) {
        SAMTOOL_CHECK(out->bam_header != nullptr, headerReadError(fileUrl2), false);
        SAMTOOL_CHECK(in->bam_header != nullptr, headerReadError(fileUrl1), false);
    }

    if (in->bam_header != nullptr && in->bam_header->target_len && out->bam_header->target_len) {
        // if there are headers
        if (*(in->bam_header->target_len) != *(out->bam_header->target_len)) {
            os.setError(QString("Different target length of files. %1 and %2").arg(qint64(in->bam_header->target_len)).arg(qint64(out->bam_header->target_len)));
            closeFiles(in, out);
            return false;
        }
    }

    bam1_t* b1 = bam_init1();
    bam1_t* b2 = bam_init1();

    QVector<int> length1;
    QVector<int> length2;
    int r1, r2;
    while ((r1 = sam_read1(in, in->bam_header, b1)) >= 0) {
        length1 << b1->l_data;
    }
    while ((r2 = sam_read1(out, out->bam_header, b2)) >= 0) {
        length2 << b2->l_data;
    }
    if (r1 != r2) {
        os.setError(QString("Different samread result codes at the end of files: %1 vs %2").arg(r1).arg(r2));
    }
    std::sort(length1.begin(), length1.end());
    std::sort(length2.begin(), length2.end());
    if (length1.size() != length2.size()) {
        os.setError(QString("Different number of reads in files: %1 vs %2").arg(length1.size()).arg(length2.size()));
    } else if (length1 != length2) {
        QString reads1;
        QString reads2;
        for (int i = 0; i < length1.size(); i++) {
            QString sep = i > 0 ? ", " : "";
            reads1 += sep + QString::number(length1[i]);
            reads2 += sep + QString::number(length2[i]);
        }
        os.setError(QString("Different reads in files: count: %1, 1:'%2' vs 2:'%3'").arg(length1.size()).arg(reads1).arg(reads2));
    }
    bam_destroy1(b1);
    bam_destroy1(b2);

    closeFiles(in, out);

    return !os.hasError();
}

/////////////////////////////////////////////////
// FASTQIterator

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4018)
#elif __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

KSEQ_INIT(gzFile, gzread)

#ifdef _MSC_VER
#    pragma warning(pop)
#elif __GNUC__
#    pragma GCC diagnostic pop
#endif

FASTQIterator::FASTQIterator(const QString& fileUrl, U2OpStatus& os)
    : seq(nullptr) {
    fp = openGzipFile(fileUrl, "r");
    if (fp == nullptr) {
        os.setError(QObject::tr("Can't open file with given url: %1.").arg(fileUrl));
        return;
    }
    seq = kseq_init(static_cast<gzFile>(fp));
    fetchNext();
}

FASTQIterator::~FASTQIterator() {
    kseq_destroy(static_cast<kseq_t*>(seq));
    gzclose(static_cast<gzFile>(fp));
}

DNASequence FASTQIterator::next() {
    if (hasNext()) {
        auto realSeq = static_cast<kseq_t*>(seq);
        QString name = realSeq->name.l != 0 ? QString::fromLatin1(realSeq->name.s) : "";
        QString comment = realSeq->comment.l != 0 ? QString::fromLatin1(realSeq->comment.s) : "";
        QString rseq = realSeq->seq.l != 0 ? QString::fromLatin1(realSeq->seq.s) : "";
        DNAQuality quality = (realSeq->qual.l) ? QString::fromLatin1(realSeq->qual.s).toLatin1() : QByteArray("");
        DNASequence res(name, rseq.toLatin1());
        res.quality = quality;
        res.info.insert(DNAInfo::FASTQ_COMMENT, comment);

        fetchNext();

        return res;
    }
    return DNASequence();
}

bool FASTQIterator::hasNext() const {
    return seq != nullptr;
}

void FASTQIterator::fetchNext() {
    if (kseq_read(static_cast<kseq_t*>(seq)) < 0) {
        seq = nullptr;
    }
}

}  // namespace U2
