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

#include <QFileInfo>
#include <QTemporaryFile>

extern "C" {
#include <bam.h>
#include <bam_sort.c>

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4018)
#endif

#include <bam_rmdup.c>

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

#include <bgzf.h>
#include <kseq.h>
#include <sam.h>
#include <sam_header.h>
}

#include <SamtoolsAdapter.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BAMUtils.h"

namespace U2 {

/** Converts QString to wchar_t*. Caller is responsible to deallocated the returned result memory. */
static wchar_t* toWideCharsArray(const QString& text) {
    wchar_t* wideCharsText = new wchar_t[text.length() + 1];
    int unicodeFileNameLength = text.toWCharArray(wideCharsText);
    wideCharsText[unicodeFileNameLength] = 0;
    return wideCharsText;
}

FILE* BAMUtils::openFile(const QString& fileUrl, const QString& mode) {
#ifdef Q_OS_WIN
    QScopedPointer<wchar_t> unicodeFileName(toWideCharsArray(fileUrl));
    QString modeWithBinaryFlag = mode;
    if (!modeWithBinaryFlag.contains("b")) {
        modeWithBinaryFlag += "b";  // Always open file in binary mode, so any kind of sam, sam.gz, bam, bai files are processed the same way.
    }
    QScopedPointer<wchar_t> unicodeMode(toWideCharsArray(modeWithBinaryFlag));
    return _wfopen(unicodeFileName.data(), unicodeMode.data());
#else
    return fopen(fileUrl.toLocal8Bit(), mode.toLatin1());
#endif
}

/** Closes file descriptor if the file descriptor is defined and is open. */
static void closeFileIfOpen(FILE* file) {
    if (file != nullptr && ftell(file) > 0) {
        fclose(file);
    }
}

BAMUtils::ConvertOption::ConvertOption(bool samToBam, const QString& referenceUrl)
    : samToBam(samToBam), referenceUrl(referenceUrl) {
}

static samfile_t* samOpen(const QString& url, const char* samMode, const void* aux) {
    QString fileMode = samMode;
    fileMode.replace("h", "");
    FILE* file = BAMUtils::openFile(url, fileMode);
    samfile_t* samfile = samopen_with_fd("", fileno(file), samMode, aux);
    if (samfile == nullptr) {
        closeFileIfOpen(file);
    }
    bool isBam = samfile->type == 1;;
    if (isBam) {
        samfile->x.bam->owned_file = 1;
    }
    return samfile;
}

/** Safely opens gzip file. Supports unicode file names. */
static gzFile openGzipFile(const QString& fileUrl, const char* mode = "r") {
    gzFile fp = nullptr;
#ifdef Q_OS_WIN
    QScopedPointer<wchar_t> unicodeFileName(toWideCharsArray(fileUrl));
    fp = gzopen_w(unicodeFileName.data(), mode);
#else
    fp = gzopen(fileUrl.toLocal8Bit().constData(), mode);
#endif
    return fp;
}

static void closeFiles(samfile_t* in, samfile_t* out) {
    if (in != nullptr) {
        samclose(in);
    }

    if (nullptr != out) {
        samclose(out);
    }
}

static samfile_t* openSamWithFai(const QString& samUrl, U2OpStatus& os) {
    QStringList references = BAMUtils::scanSamForReferenceNames(samUrl, os);
    CHECK_OP(os, nullptr);

    QTemporaryFile faiFile;
    faiFile.open();
    QString faiUrl = faiFile.fileName();
    BAMUtils::createFai(faiUrl, references, os);
    CHECK_OP(os, nullptr);

    QByteArray faiUrlData = faiUrl.toLocal8Bit();
    void* aux = (void*)faiUrlData.constData();
    return samOpen(samUrl, "r", aux);
}

static QString openFileError(const QString& file) {
    return QObject::tr("Fail to open \"%1\" for reading").arg(file);
}

static QString openFileError(const QByteArray& file) {
    return QObject::tr("Fail to open \"%1\" for reading").arg(file.constData());
}

static QString headerError(const QString& file) {
    return QObject::tr("Fail to read the header from the file: \"%1\"").arg(file);
}

static QString headerError(const QByteArray& file) {
    return QObject::tr("Fail to read the header from the file: \"%1\"").arg(file.constData());
}

static QString faiError(const QString& filePath) {
    return QObject::tr("Can not build the fasta index for the file: \"%1\"").arg(filePath);
}

static QString readsError(const QString& file) {
    return QObject::tr("Error parsing the reads from the file: \"%1\"").arg(file);
}

static QString readsError(const QByteArray& file) {
    return QObject::tr("Error parsing the reads from the file: \"%1\"").arg(file.constData());
}

static QString truncatedError(const QByteArray& file) {
    return QObject::tr("Truncated file: \"%1\"").arg(file.constData());
}

static QString truncatedError(const QString& file) {
    return QObject::tr("Truncated file: \"%1\"").arg(file);
}

template<typename T>
void samreadCheck(int read, U2OpStatus& os, const T& fileName) {
    if (READ_ERROR_CODE == read) {
        if (nullptr != SAMTOOLS_ERROR_MESSAGE) {
            os.setError(SAMTOOLS_ERROR_MESSAGE);
        } else {
            os.setError(readsError(fileName));
        }
    } else if (read < -1) {
        os.setError(truncatedError(fileName));
    }
}

static QList<QByteArray> convertStringList(const QStringList& sList) {
    QList<QByteArray> result;
    for (int i = 0; i < sList.size(); i++) {
        result.append(sList.at(i).toLocal8Bit());
    }
    return result;
}

static void convertByteArray(const QList<QByteArray>& byteArray, char** charArray) {
    for (int i = 0; i < byteArray.size(); i++) {
        charArray[i] = const_cast<char*>(byteArray[i].constData());
    }
}

#define SAMTOOL_CHECK(cond, msg, ret) \
    if (!(cond)) { \
        if (nullptr != SAMTOOLS_ERROR_MESSAGE) { \
            os.setError(SAMTOOLS_ERROR_MESSAGE); \
        } else { \
            os.setError(msg); \
        } \
        closeFiles(in, out); \
        return ret; \
    }

void BAMUtils::convertToSamOrBam(const GUrl& samUrl, const GUrl& bamUrl, const ConvertOption& options, U2OpStatus& os) {
    QString sourcePath = GUrl(options.samToBam ? samUrl : bamUrl).getURLString();
    QString targetPath = GUrl(options.samToBam ? bamUrl : samUrl).getURLString();

    samfile_t* in = nullptr;
    samfile_t* out = nullptr;

    // open files
    {
        QByteArray readMode = options.samToBam ? "r" : "rb";
        void* aux = nullptr;
        if (options.samToBam && !options.referenceUrl.isEmpty()) {
            aux = samfaipath(options.referenceUrl.toLocal8Bit().constData());
            SAMTOOL_CHECK(aux != nullptr, faiError(options.referenceUrl), );
        }

        in = samOpen(sourcePath, readMode, aux);
        SAMTOOL_CHECK(in != nullptr, openFileError(sourcePath), );
        SAMTOOL_CHECK(in->header != nullptr, headerError(sourcePath), );
        if (options.samToBam && in->header->n_targets == 0) {
            os.addWarning(tr("There is no header in the SAM file \"%1\". The header information will be generated automatically.").arg(sourcePath));
            samclose(in);
            in = openSamWithFai(sourcePath, os);
            CHECK_OP(os, );
            SAMTOOL_CHECK(nullptr != in, openFileError(sourcePath), );
            SAMTOOL_CHECK(nullptr != in->header, headerError(sourcePath), );
        }

        QByteArray writeMode = (options.samToBam) ? "wb" : "wh";
        out = samOpen(targetPath, writeMode, in->header);
        SAMTOOL_CHECK(out != nullptr, openFileError(targetPath), );
    }
    // convert files
    bam1_t* b = bam_init1();
    {
        int r = 0;
        while ((r = samread(in, b)) >= 0) {  // read one alignment from `in'
            samwrite(out, b);  // write the alignment to `out'
        }

        samreadCheck<QString>(r, os, sourcePath);
        bam_destroy1(b);
    }

    closeFiles(in, out);
    return;
}

static bool isSorted(const QString& headerText) {
    QString text = headerText;
    text = text.replace('\r', QString(""));
    QStringList lines = text.split('\n');

    bool result = false;
    foreach (const QString& line, lines) {
        if (!line.startsWith("@HD")) {
            continue;
        }

        QStringList tokens = line.split('\t');
        for (int i = 1; i < tokens.size(); i++) {
            QString& token = tokens[i];
            int colonIdx = token.indexOf(':');
            if (-1 == colonIdx) {
                continue;
            }
            QString fieldTag = token.mid(0, colonIdx);
            QString fieldValue = token.mid(colonIdx + 1);

            if ("SO" == fieldTag) {
                result = ("coordinate" == fieldValue);
                break;
            }
        }
    }

    return result;
}

bool BAMUtils::isSortedBam(const GUrl& bamUrl, U2OpStatus& os) {
    QString urlPath = bamUrl.getURLString();

    bam_header_t* header = nullptr;
    QString error;
    bool result = false;

    FILE* file = openFile(urlPath, "rb");
    bamFile bamHandler = bam_dopen(fileno(file), "rb");
    if (bamHandler != nullptr) {
        header = bam_header_read(bamHandler);
        if (header != nullptr) {
            result = isSorted(header->text);
        } else {
            error = QString("Can't read header from file '%1'").arg(bamUrl.getURLString());
        }
    } else {
        error = QString("Can't open file '%1'").arg(bamUrl.getURLString());
    }

    // deallocate resources
    {
        if (header != nullptr) {
            bam_header_destroy(header);
        }
        if (bamHandler != nullptr) {
            bam_close(bamHandler);
        }
        closeFileIfOpen(file);
    }

    if (!error.isEmpty()) {
        os.setError(error);
        return false;
    }

    /**
     * There is a bug in samtools. If you sort a BAM using samtools then
     * the header tag "SO" is not changed. So, if the check above is false
     * then maybe it is not true that the file is not sorted.
     */
    if (!result) {
        result = BAMUtils::hasValidBamIndex(bamUrl);
        if (!result) {
            U2OpStatusImpl idxOs;
            BAMUtils::createBamIndex(bamUrl, idxOs);
            CHECK_OP(idxOs, false);
        }
    }

    return true;
}

inline static int bytes2MB(qint64 bytes) {
    return (int)(bytes / (1024 * 1024)) + 1;
}

inline static qint64 mB2bytes(int mb) {
    return (qint64)mb * 1024 * 1024;
}

#define INITIAL_SAMTOOLS_MEM_SIZE_MB 500
#define SAMTOOLS_MEM_BOOST 5

GUrl BAMUtils::sortBam(const GUrl& bamUrl, const QString& sortedBamBaseName, U2OpStatus& os) {
    QString baseName = sortedBamBaseName;
    if (baseName.endsWith(".bam")) {
        baseName = baseName.left(baseName.size() - 4);
    }
    QString sortedFileName = baseName + ".bam";

    // get memory resource
    AppSettings* appSettings = AppContext::getAppSettings();
    AppResourcePool* resPool = appSettings->getAppResourcePool();
    AppResource* memory = resPool->getResource(RESOURCE_MEMORY);
    SAFE_POINT_EXT(memory != nullptr, os.setError("No memory resource"), QString());

    // calculate needed memory
    QString bamFileName = bamUrl.getURLString();
    QFileInfo info(bamFileName);
    qint64 fileSizeBytes = info.size();
    CHECK_EXT(fileSizeBytes >= 0, os.setError(QString("Unknown file size: %1").arg(bamFileName.constData())), QString());

    int maxMemMB = INITIAL_SAMTOOLS_MEM_SIZE_MB;
    int fileSizeMB = bytes2MB(fileSizeBytes);
    if (fileSizeMB < 10) {
        maxMemMB = fileSizeMB;
    } else if (fileSizeMB < 100) {
        maxMemMB = fileSizeMB / SAMTOOLS_MEM_BOOST;
    }
    maxMemMB = qMin(maxMemMB, INITIAL_SAMTOOLS_MEM_SIZE_MB);
    while (!memory->tryAcquire(maxMemMB)) {
        // reduce used memory
        maxMemMB = maxMemMB * 2 / 3;
        CHECK_EXT(maxMemMB > 0, os.setError("Failed to lock enough memory resource"), QString());
    }
    // sort bam
    {
        coreLog.details(BAMUtils::tr("Sort bam file: \"%1\" using %2 Mb of memory. Result sorted file is: \"%3\"")
                            .arg(bamFileName)
                            .arg(maxMemMB)
                            .arg(sortedFileName));
        size_t maxMemBytes = (size_t)(mB2bytes(maxMemMB));  // maxMemMB < 500 Mb, so the conversation is correct!
        QByteArray baseNameArray = baseName.toUtf8();
        FILE* file = openFile(bamFileName, "rb");
        bam_sort_core_ext(0, "", baseNameArray.constData(), maxMemBytes, false, fileno(file));  // maxMemBytes
    }
    memory->release(maxMemMB);

    return sortedFileName;
}

GUrl BAMUtils::mergeBam(const QStringList& bamUrls, const QString& mergetBamTargetUrl, U2OpStatus& /*os*/) {
    coreLog.details(BAMUtils::tr("Merging BAM files: \"%1\". Resulting merged file is: \"%2\"")
                        .arg(QString(bamUrls.join(",")))
                        .arg(QString(mergetBamTargetUrl)));

    int urlsSize = bamUrls.size();
    char** mergeArgv = new char*[urlsSize];
    QList<QByteArray> byteArray = convertStringList(bamUrls);
    convertByteArray(byteArray, mergeArgv);

    bam_merge_core(0, mergetBamTargetUrl.toLocal8Bit().constData(), 0, urlsSize, mergeArgv, 0, 0);

    delete[] mergeArgv;

    return QString(mergetBamTargetUrl);
}

GUrl BAMUtils::rmdupBam(const QString& bamUrl, const QString& rmdupBamTargetUrl, U2OpStatus& os, bool removeSingleEnd, bool treatReads) {
    coreLog.details(BAMUtils::tr("Remove PCR duplicate in BAM file: \"%1\". Resulting  file is: \"%2\"")
                        .arg(QString(bamUrl))
                        .arg(QString(rmdupBamTargetUrl)));

    int is_se = 0;
    int force_se = 0;
    if (removeSingleEnd) {
        is_se = 1;
    }
    if (treatReads) {
        is_se = 1;
        force_se = 1;
    }

    samfile_t* in = nullptr;
    samfile_t* out = nullptr;
    {
        in = samOpen(bamUrl, "rb", 0);
        SAMTOOL_CHECK(in != nullptr, openFileError(bamUrl), QString(""));
        SAMTOOL_CHECK(in->header != nullptr, headerError(bamUrl), QString(""));

        out = samOpen(rmdupBamTargetUrl, "wb", in->header);
        SAMTOOL_CHECK(out != nullptr, openFileError(rmdupBamTargetUrl), QString(""));
    }

    if (is_se) {
        bam_rmdupse_core(in, out, force_se);
    } else {
        bam_rmdup_core(in, out);
    }

    samclose(in);
    samclose(out);

    return QString(rmdupBamTargetUrl);
}

void* BAMUtils::loadIndex(const QString& filePath) {
    // See bam_index_load_local.
    QString mode = "rb";
    FILE* fp = openFile(filePath + ".bai", mode);
    if (fp == nullptr && filePath.endsWith("bam")) {
        fp = openFile(filePath.chopped(4) + ".bai", mode);
    }
    CHECK(fp != nullptr, nullptr);
    bam_index_t* idx = bam_index_load_core(fp);
    closeFileIfOpen(fp);
    return idx;
}

bool BAMUtils::hasValidBamIndex(const GUrl& bamUrl) {
    QString urlPath = bamUrl.getURLString();
    bam_index_t* index = (bam_index_t*)loadIndex(urlPath);
    CHECK(index != nullptr, false);
    bam_index_destroy(index);

    QFileInfo idxFileInfo(bamUrl.getURLString() + ".bai");
    if (!idxFileInfo.exists()) {
        QString indexUrl = bamUrl.getURLString();
        indexUrl.chop(4);
        idxFileInfo.setFile(indexUrl + ".bai");
    }
    QFileInfo bamFileInfo(bamUrl.getURLString());

    if (idxFileInfo.lastModified() < bamFileInfo.lastModified()) {
        return false;
    }

    return true;
}

bool BAMUtils::hasValidFastaIndex(const GUrl& fastaUrl) {
    const QByteArray fastaFileName = fastaUrl.getURLString().toLocal8Bit();

    char* index = samfaipath(fastaFileName.constData());

    if (nullptr == index) {
        return false;
    } else {
        free(index);

        QFileInfo idxFileInfo(fastaUrl.getURLString() + ".fai");
        QFileInfo fastaFileInfo(fastaUrl.getURLString());
        if (idxFileInfo.lastModified() < fastaFileInfo.lastModified()) {
            return false;
        }

        return true;
    }
}

/**
 * Builds and saves index for BAM file. Returns 0 if the index was created correctly.
 * Exact copy of 'bam_index_build2' with a correct unicode file names support.
 */
static int bam_index_build_unicode(const QString& bamFileName) {
    FILE* bFile = BAMUtils::openFile(bamFileName, "rb");
    CHECK(bFile != nullptr, -1);
    bamFile fp = bam_dopen(fileno(bFile), "rb");
    if (fp == nullptr) {
        closeFileIfOpen(bFile);
        fprintf(stderr, "[bam_index_build2] fail to open the BAM file.\n");
        return -1;
    }
    fp->owned_file = 1;
    bam_index_t* idx = bam_index_core(fp);
    bam_close(fp);
    if (idx == nullptr) {
        fprintf(stderr, "[bam_index_build2] fail to index the BAM file.\n");
        return -1;
    }
    FILE* fpidx = BAMUtils::openFile(bamFileName + ".bai", "wb");
    if (fpidx == nullptr) {
        fprintf(stderr, "[bam_index_build2] fail to create the index file.\n");
        return -1;
    }
    bam_index_save(idx, fpidx);
    bam_index_destroy(idx);
    fclose(fpidx);
    return 0;
}

void BAMUtils::createBamIndex(const GUrl& bamUrl, U2OpStatus& os) {
    QString path = bamUrl.getURLString();
    coreLog.details(BAMUtils::tr("Build index for bam file: \"%1\"").arg(path));

    int error = bam_index_build_unicode(path);
    if (error == -1) {
        os.setError(tr("Can't build the index: %1").arg(path));
    }
}

GUrl BAMUtils::getBamIndexUrl(const GUrl& bamUrl) {
    CHECK(hasValidBamIndex(bamUrl), GUrl());

    QString bamFileName = bamUrl.getURLString();
    QFileInfo fileInfo(bamFileName + ".bai");
    if (!fileInfo.exists()) {
        fileInfo.setFile(bamFileName.chopped(4) + ".bai");
    }
    SAFE_POINT(fileInfo.exists(), "Can't find the index file", GUrl());

    return GUrl(fileInfo.filePath());
}

static qint64 getSequenceLength(U2Dbi* dbi, const U2DataId& objectId, U2OpStatus& os) {
    qint64 seqLength = -1;

    U2AssemblyDbi* assemblyDbi = dbi->getAssemblyDbi();
    SAFE_POINT_EXT(nullptr != assemblyDbi, os.setError("NULL assembly DBI"), seqLength);

    U2AttributeDbi* attributeDbi = dbi->getAttributeDbi();
    bool useMaxPos = true;
    if (nullptr != attributeDbi) {
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

static void createHeader(bam_header_t* header, const QList<GObject*>& objects, U2OpStatus& os) {
    CHECK_EXT(nullptr != header, os.setError("NULL header"), );

    header->n_targets = objects.size();
    header->target_name = new char*[header->n_targets];
    header->target_len = new uint32_t[header->n_targets];

    QByteArray headerText;
    headerText += "@HD\tVN:1.4\tSO:coordinate\n";

    int objIdx = 0;
    foreach (GObject* obj, objects) {
        AssemblyObject* assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(nullptr != assemblyObj, os.setError("NULL assembly object"), );

        DbiConnection con(obj->getEntityRef().dbiRef, os);
        CHECK_OP(os, );

        qint64 seqLength = getSequenceLength(con.dbi, obj->getEntityRef().entityId, os);
        CHECK_OP(os, );

        QByteArray seqName = obj->getGObjectName().toLatin1();
        header->target_name[objIdx] = new char[seqName.length() + 1];
        {
            char* name = header->target_name[objIdx];
            qstrncpy(name, seqName.constData(), seqName.length() + 1);
            name[seqName.length()] = 0;
        }
        header->target_len[objIdx] = seqLength;

        headerText += QString("@SQ\tSN:%1\tLN:%2\n").arg(seqName.constData()).arg(seqLength);

        objIdx++;
    }

    if (headerText.length() > 0) {
        header->text = new char[headerText.length() + 1];
        qstrncpy(header->text, headerText.constData(), headerText.length() + 1);
        header->text[headerText.length()] = 0;
        header->l_text = headerText.length();
        header->dict = sam_header_parse2(header->text);
    }
}

static QMap<QString, int> getNumMap(const QList<GObject*>& objects, U2OpStatus& os) {
    QMap<QString, int> result;
    int i = 0;
    foreach (GObject* obj, objects) {
        AssemblyObject* assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(nullptr != assemblyObj, os.setError("NULL assembly object"), result);

        QString name = assemblyObj->getGObjectName();
        result[name] = i;
        i++;
    }
    return result;
}

static void writeObjectsWithSamtools(samfile_t* out, const QList<GObject*>& objects, U2OpStatus& os, const U2Region& desiredRegion) {
    foreach (GObject* obj, objects) {
        AssemblyObject* assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(nullptr != assemblyObj, os.setError("NULL assembly object"), );

        DbiConnection con(assemblyObj->getEntityRef().dbiRef, os);
        CHECK_OP(os, );

        U2AssemblyDbi* dbi = con.dbi->getAssemblyDbi();
        SAFE_POINT_EXT(nullptr != dbi, os.setError("NULL assembly DBI"), );

        U2DataId assemblyId = assemblyObj->getEntityRef().entityId;
        qint64 maxPos = dbi->getMaxEndPos(assemblyId, os);
        U2Region region(0, maxPos + 1);
        if (desiredRegion != U2_REGION_MAX) {
            region = desiredRegion;
        }
        QScopedPointer<U2DbiIterator<U2AssemblyRead>> reads(dbi->getReads(assemblyId, region, os, true));
        CHECK_OP(os, );

        ReadsContext ctx(assemblyObj->getGObjectName(), getNumMap(objects, os));
        CHECK_OP(os, );
        bam1_t* read = bam_init1();
        while (reads->hasNext()) {
            U2AssemblyRead r = reads->next();
            SamtoolsAdapter::read2samtools(r, ctx, os, *read);
            CHECK_OP_EXT(os, bam_destroy1(read), );
            samwrite(out, read);
        }
        bam_destroy1(read);
    }
}

void BAMUtils::writeDocument(Document* doc, U2OpStatus& os) {
    writeObjects(
        doc->findGObjectByType(GObjectTypes::ASSEMBLY),
        doc->getURL(),
        doc->getDocumentFormatId(),
        os);
}

void BAMUtils::writeObjects(const QList<GObject*>& objects, const GUrl& url, const DocumentFormatId& formatId, U2OpStatus& os, const U2Region& desiredRegion) {
    CHECK_EXT(!objects.isEmpty(), os.setError("No assembly objects"), );

    QString urlPath = url.getURLString();
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

    bam_header_t* header = bam_header_init();
    createHeader(header, objects, os);
    if (os.isCoR()) {
        bam_header_destroy(header);
        return;
    }

    samfile_t* out = samOpen(urlPath, openMode.constData(), header);
    bam_header_destroy(header);
    CHECK_EXT(nullptr != out, os.setError(QString("Can not open file for writing: %1").arg(urlPath.constData())), );

    writeObjectsWithSamtools(out, objects, os, desiredRegion);
    samclose(out);
}

// the function assumes the equal order of alignments in files
bool BAMUtils::isEqualByLength(const GUrl& fileUrl1, const GUrl& fileUrl2, U2OpStatus& os, bool isBAM) {
    QString fileName1 = fileUrl1.getURLString();
    QString fileName2 = fileUrl2.getURLString();

    samfile_t* in = nullptr;
    samfile_t* out = nullptr;

    const char* readMode = isBAM ? "rb" : "r";
    {
        void* aux = nullptr;
        in = samOpen(fileName1, readMode, aux);
        SAMTOOL_CHECK(nullptr != in, openFileError(fileName1), false);
        SAMTOOL_CHECK(nullptr != in->header, headerError(fileName1), false);

        out = samOpen(fileName2, readMode, aux);
        SAMTOOL_CHECK(nullptr != out, openFileError(fileName2), false);
        SAMTOOL_CHECK(nullptr != out->header, headerError(fileName2), false);
    }

    if (in->header->target_len && out->header->target_len) {
        // if there are headers
        if (*(in->header->target_len) != *(out->header->target_len)) {
            os.setError(QString("Different target length of files. %1 and %2").arg(qint64(in->header->target_len)).arg(qint64(out->header->target_len)));
            closeFiles(in, out);
            return false;
        }
    }

    bam1_t* b1 = bam_init1();
    bam1_t* b2 = bam_init1();
    {
        int r1 = 0;
        int r2 = 0;
        while ((r1 = samread(in, b1)) >= 0) {  // read one alignment from file1
            if ((r2 = samread(out, b2)) >= 0) {  // read one alignment from file2
                if (b1->data_len != b2->data_len) {
                    os.setError("Different alignment of reads");
                    break;
                }
            } else {
                samreadCheck(r2, os, fileName2);
                os.setError("Different number of reads in files");
                break;
            }
        }

        samreadCheck(r1, os, fileName1);
        if (!os.hasError() && (r2 = samread(out, b2)) >= 0) {
            os.setError("Different number of reads in files");
        }
        bam_destroy1(b1);
        bam_destroy1(b2);
    }

    closeFiles(in, out);

    if (os.hasError()) {
        return false;
    }

    return true;
}

namespace {
const int bufferSize = 1024 * 1024;  // 1 Mb
const int referenceColumn = 2;  // 5 Mb

inline QByteArray readLine(IOAdapter* io, char* buffer, int bufferSize) {
    QByteArray result;
    bool terminatorFound = false;
    do {
        qint64 length = io->readLine(buffer, bufferSize, &terminatorFound);
        CHECK(-1 != length, result);
        result += QByteArray(buffer, length);
    } while (!terminatorFound);
    return result;
}

inline QByteArray parseReferenceName(const QByteArray& line) {
    QList<QByteArray> columns = line.split('\t');
    if (columns.size() <= referenceColumn) {
        coreLog.error(BAMUtils::tr("Wrong line in a SAM file."));
        return "*";
    }
    return columns[referenceColumn];
}
}  // namespace

QStringList BAMUtils::scanSamForReferenceNames(const GUrl& samUrl, U2OpStatus& os) {
    QStringList result;
    QScopedPointer<IOAdapter> io(IOAdapterUtils::open(samUrl, os));
    CHECK_OP(os, result);

    QByteArray buffer(bufferSize, 0);
    char* bufferData = buffer.data();
    do {
        QByteArray line = readLine(io.data(), bufferData, bufferSize);
        if (line.isEmpty() || line.startsWith("@")) {
            continue;
        }
        QByteArray referenceName = parseReferenceName(line);
        if ("*" != referenceName && !result.contains(referenceName)) {
            result << referenceName;
        }
    } while (!io->isEof());
    return result;
}

void BAMUtils::createFai(const GUrl& faiUrl, const QStringList& references, U2OpStatus& os) {
    QScopedPointer<IOAdapter> io(IOAdapterUtils::open(faiUrl, os, IOAdapterMode_Write));
    CHECK_OP(os, );
    foreach (const QString& reference, references) {
        QString line = reference + "\n";
        io->writeBlock(line.toLocal8Bit());
    }
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
        kseq_t* realSeq = static_cast<kseq_t*>(seq);
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
