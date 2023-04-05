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

#include <QFileInfo>
#include <QTemporaryFile>

#include <U2Core/FileAndDirectoryUtils.h>

extern "C" {
#include <bam.h>
#include <bam_sort.c>

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4018)
#endif

#include <sam.h>

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

#include <kseq.h>
#include <sam_header.h>
}

#include <SamtoolsAdapter.h>

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BAMUtils.h"

namespace U2 {

NP<FILE> BAMUtils::openFile(const QString& fileUrl, const QString& mode) {
    return FileAndDirectoryUtils::openFile(fileUrl, mode);
}

/** Closes file descriptor if the file descriptor is defined and is open. */
void BAMUtils::closeFileIfOpen(FILE* file) {
    FileAndDirectoryUtils::closeFileIfOpen(file);
}

/** Version the original samopen() function with a correct handling of unicode in faiUrl and non-UGENE use cases removed. */
static samfile_t* samopen_ugene(const NP<FILE>& file, const char* mode, const QString& faiUrl, bam_header_t* bamHeader) {
    SAFE_POINT(file != nullptr, "samopen_ugene: file is null!", nullptr);
    int fd = fileno(file);
    SAFE_POINT(fd != -1, "samopen_ugene: file is closed", nullptr);
    int TYPE_BAM = 1;
    int TYPE_READ = 2;
    auto fp = (samfile_t*)calloc(1, sizeof(samfile_t));
    if (strchr(mode, 'r')) {  // read
        fp->type |= TYPE_READ;
        if (strchr(mode, 'b')) {  // binary
            fp->type |= TYPE_BAM;
            fp->x.bam = bgzf_fdopen(file, "r");
            if (fp->x.bam == nullptr) {
                free(fp);
                return nullptr;
            }
            fp->header = bam_header_read(fp->x.bam);
        } else {  // text
            fp->x.tamr = sam_dopen(fd);
            if (fp->x.tamr == nullptr) {
                free(fp);
                return nullptr;
            }
            fp->header = sam_header_read(fp->x.tamr);
            if (fp->header == nullptr) {
                free(fp);
                return nullptr;
            }
            if (fp->header->n_targets == 0) {  // no @SQ fields
                if (!faiUrl.isEmpty()) {
                    bam_header_t* textheader = fp->header;
                    NP<FILE> faiFile = BAMUtils::openFile(faiUrl, "r");
                    if (faiFile == nullptr) {
                        free(fp);
                        return nullptr;
                    }
                    fp->header = sam_header_read2_fd(fileno(faiFile));
                    if (fp->header == nullptr) {
                        free(fp);
                        return nullptr;
                    }
                    append_header_text(fp->header, textheader->text, textheader->l_text);
                    bam_header_destroy(textheader);
                }
                if (fp->header->n_targets == 0 && bam_verbose >= 1) {
                    fprintf(stderr, "[samopen] no @SQ lines in the header.\n");
                }
            } else if (bam_verbose >= 2) {
                fprintf(stderr, "[samopen] SAM header is present: %d sequences.\n", fp->header->n_targets);
            }
        }
    } else if (strchr(mode, 'w')) {  // write
        fp->header = bam_header_dup(bamHeader);
        if (strchr(mode, 'b')) {  // binary
            char bmode[3];
            int compress_level = -1;
            int i;
            for (i = 0; mode[i]; ++i) {
                if (mode[i] >= '0' && mode[i] <= '9') {
                    break;
                }
            }
            if (mode[i]) {
                compress_level = mode[i] - '0';
            }
            if (strchr(mode, 'u')) {
                compress_level = 0;
            }
            bmode[0] = 'w';
            bmode[1] = compress_level < 0 ? 0 : compress_level + '0';
            bmode[2] = 0;
            fp->type |= TYPE_BAM;
            fp->x.bam = bgzf_fdopen(file, bmode);
            if (fp->x.bam == nullptr) {
                free(fp);
                return nullptr;
            }
            bam_header_write(fp->x.bam, fp->header);
        } else {  // text
            // open file
            fp->x.tamw = fdopen(fd, "w");
            if (fp->x.tamr == nullptr) {
                free(fp);
                return nullptr;
            }
            if (strchr(mode, 'X')) {
                fp->type |= BAM_OFSTR << 2;
            } else if (strchr(mode, 'x')) {
                fp->type |= BAM_OFHEX << 2;
            } else {
                fp->type |= BAM_OFDEC << 2;
            }
            // write header
            if (strchr(mode, 'h')) {
                // parse the header text
                bam_header_t* alt = bam_header_init();
                alt->l_text = fp->header->l_text;
                alt->text = fp->header->text;
                sam_header_parse(alt);
                alt->l_text = 0;
                alt->text = nullptr;
                // check if there are @SQ lines in the header
                fwrite(fp->header->text, 1, fp->header->l_text, fp->x.tamw);  // FIXME: better to skip the trailing NULL
                if (alt->n_targets) {  // then write the header text without dumping ->target_{name,len}
                    if (alt->n_targets != fp->header->n_targets && bam_verbose >= 1) {
                        fprintf(stderr, "[samopen] inconsistent number of target sequences. Output the text header.\n");
                    }
                } else {  // then dump ->target_{name,len}
                    for (int i = 0; i < fp->header->n_targets; ++i) {
                        fprintf(fp->x.tamw, "@SQ\tSN:%s\tLN:%d\n", fp->header->target_name[i], fp->header->target_len[i]);
                    }
                }
                bam_header_destroy(alt);
            }
        }
    }
    return fp;
}

static samfile_t* samOpen(const QString& url, const char* samMode, const QString& faiUrl = "", bam_header_t* header = nullptr) {
    QString fileMode = samMode;
    fileMode.replace("h", "");
    NP<FILE> file = BAMUtils::openFile(url, fileMode);
    samfile_t* samfile = samopen_ugene(file, samMode, faiUrl, header);
    CHECK_EXT(samfile != nullptr, BAMUtils::closeFileIfOpen(file), nullptr);

    bool isBam = samfile->type == 1;
    if (isBam) {
        samfile->x.bam->owned_file = 1;
    }
    return samfile;
}

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

static void closeFiles(samfile_t* in, samfile_t* out) {
    samclose(in);
    samclose(out);
}

static samfile_t* openSamWithFai(const QString& samUrl, U2OpStatus& os) {
    QStringList references = BAMUtils::scanSamForReferenceNames(samUrl, os);
    CHECK_OP(os, nullptr);

    QTemporaryFile faiFile;
    faiFile.open();
    QString faiUrl = faiFile.fileName();
    BAMUtils::createFai(faiUrl, references, os);
    CHECK_OP(os, nullptr);

    return samOpen(samUrl, "r", faiUrl);
}

static QString openFileError(const QString& file) {
    return QObject::tr("Fail to open \"%1\" for reading").arg(file);
}

static QString headerError(const QString& file) {
    return QObject::tr("Fail to read the header from the file: \"%1\"").arg(file);
}

static QString readsError(const QString& file) {
    return QObject::tr("Error parsing the reads from the file: \"%1\"").arg(file);
}

static QString truncatedError(const QString& file) {
    return QObject::tr("Truncated file: \"%1\"").arg(file);
}

static void checkFileReadState(int read, U2OpStatus& os, const QString& fileName) {
    if (read == READ_ERROR_CODE) {
        os.setError(readsError(fileName));
    } else if (read < -1) {
        os.setError(truncatedError(fileName) + ", code: " + QString::number(read));
    }
}

#define SAMTOOL_CHECK(cond, msg, ret) \
    if (!(cond)) { \
        os.setError(msg); \
        closeFiles(in, out); \
        return ret; \
    }

void BAMUtils::convertBamToSam(U2OpStatus& os, const QString& bamPath, const QString& samPath) {
    samfile_t* in = samOpen(bamPath, "rb");
    samfile_t* out = nullptr;
    SAMTOOL_CHECK(in != nullptr, openFileError(bamPath), );
    SAMTOOL_CHECK(in->header != nullptr, headerError(bamPath), );
    out = samOpen(samPath, "wh", "", in->header);
    SAMTOOL_CHECK(out != nullptr, openFileError(samPath), );

    bam1_t* b = bam_init1();
    int r = 0;
    while ((r = samread(in, b)) >= 0) {  // read one alignment from `in'
        samwrite(out, b);  // write the alignment to `out'.
    }
    checkFileReadState(r, os, bamPath);
    bam_destroy1(b);
    closeFiles(in, out);
}

void BAMUtils::convertSamToBam(U2OpStatus& os, const QString& samPath, const QString& bamPath, const QString& referencePath) {
    samfile_t* in = nullptr;
    samfile_t* out = nullptr;
    QString faiPath = hasValidFastaIndex(referencePath) ? referencePath + ".fai" : "";
    in = samOpen(samPath, "r", faiPath);
    SAMTOOL_CHECK(in != nullptr, openFileError(samPath), );
    SAMTOOL_CHECK(in->header != nullptr, headerError(samPath), );
    if (in->header->n_targets == 0) {
        coreLog.details(tr("There is no header in the SAM file \"%1\". The header information will be generated automatically.").arg(samPath));
        samclose(in);
        in = openSamWithFai(samPath, os);
        SAMTOOL_CHECK(!os.hasError(), os.getError(), );
        SAMTOOL_CHECK(in != nullptr, openFileError(samPath), );
        SAMTOOL_CHECK(in->header != nullptr, headerError(samPath), );
    }

    out = samOpen(bamPath, "wb", "", in->header);
    SAMTOOL_CHECK(out != nullptr, openFileError(bamPath), );

    bam1_t* b = bam_init1();
    int r = 0;
    while ((r = samread(in, b)) >= 0) {  // read one alignment from `in'
        samwrite(out, b);  // write the alignment to `out'
    }
    checkFileReadState(r, os, samPath);
    bam_destroy1(b);
    closeFiles(in, out);
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

bool BAMUtils::isSortedBam(const QString& bamUrl, U2OpStatus& os) {
    bam_header_t* header = nullptr;
    QString error;
    bool result = false;

    NP<FILE> file = BAMUtils::openFile(bamUrl, "rb");
    bamFile bamHandler = bgzf_fdopen(file.getNullable(), "r");
    if (bamHandler != nullptr) {
        header = bam_header_read(bamHandler);
        if (header != nullptr) {
            result = isSorted(header->text);
        } else {
            error = QString("Can't read header from file '%1'").arg(bamUrl);
        }
    } else {
        error = QString("Can't open file '%1'").arg(bamUrl);
    }

    // deallocate resources
    {
        if (header != nullptr) {
            bam_header_destroy(header);
        }
        if (bamHandler != nullptr) {
            bgzf_close(bamHandler);
        } else {
            BAMUtils::closeFileIfOpen(file);
        }
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

static int bamMergeCore(const QString& outFileName, const QList<QString>& filesToMerge);

static QString createNumericSuffix(int n) {
    QString suffix = QString::number(n);
    return ("000" + suffix).right(qMin(suffix.length(), 4));
}

static void bamSortBlocks(int n, int k, bam1_p* buf, const QString& prefix, const bam_header_t* h) {
    QString sortedFileName = n < 0 ? prefix + ".bam" : prefix + "." + createNumericSuffix(n) + ".bam";
    coreLog.trace(QString("bamSortBlocks, n: %1, k: %2, prefix: %3, sorted file: %4").arg(n).arg(k).arg(prefix).arg(sortedFileName));
    ks_mergesort(sort, k, buf, nullptr);
    NP<FILE> file = BAMUtils::openFile(sortedFileName, "w");
    bamFile fp = bgzf_fdopen(file.getNullable(), "w");
    if (fp == nullptr) {
        coreLog.error(BAMUtils::tr("[sort_blocks] fail to create file %1").arg(sortedFileName));
        return;
    }
    bam_header_write(fp, h);
    for (int i = 0; i < k; ++i) {
        bam_write1_core(fp, &buf[i]->core, buf[i]->data_len, buf[i]->data);
    }
    bgzf_close(fp);
}

static void bamSortCore(U2OpStatus& os, const QString& bamFileToSort, const QString& prefix) {
    coreLog.trace("bamSortCore: " + bamFileToSort + ", result prefix: " + prefix);
    NP<FILE> file = BAMUtils::openFile(bamFileToSort, "rb");
    CHECK_EXT(file != nullptr, os.setError(BAMUtils::tr("Failed to open file: %1").arg(bamFileToSort)), );
    bamFile fp = bgzf_fdopen(file, "r");
    if (fp == nullptr) {
        BAMUtils::closeFileIfOpen(file);
        coreLog.error(BAMUtils::tr("[bam_sort_core] fail to open file"));
        return;
    }
    int n = 0;
    int k = 0;
    size_t max_mem = 100 * 1000 * 1000;
    size_t mem = 0;
    bam_header_t* header = bam_header_read(fp);
    change_SO(header, "coordinate");
    auto buf = (bam1_t**)calloc(max_mem / BAM_CORE_SIZE, sizeof(bam1_t*));
    // write sub files
    int ret = 0;
    for (;;) {
        if (buf[k] == nullptr) {
            buf[k] = (bam1_t*)calloc(1, sizeof(bam1_t));
        }
        bam1_t* b = buf[k];
        ret = bam_read1(fp, b);
        if (ret < 0) {
            break;
        }
        mem += ret;
        ++k;
        if (mem >= max_mem) {
            bamSortBlocks(n++, k, buf, prefix, header);
            mem = 0;
            k = 0;
        }
    }
    if (ret != -1) {
        coreLog.trace(QString("[bam_sort_core] truncated file. Continue anyway."));
    }
    if (n == 0) {
        bamSortBlocks(-1, k, buf, prefix, header);
    } else {  // then merge
        coreLog.trace(QString("[bam_sort_core] merging from %1 files...").arg(n + 1));
        bamSortBlocks(n++, k, buf, prefix, header);
        QString mergedBamPath = prefix + ".bam";
        QStringList filesToMerge;
        for (int i = 0; i < n; ++i) {
            filesToMerge.append(prefix + "." + createNumericSuffix(i) + ".bam");
        }
        bamMergeCore(mergedBamPath, filesToMerge);
    }
    for (k = 0; k < max_mem / BAM_CORE_SIZE; ++k) {
        if (buf[k]) {
            free(buf[k]->data);
            free(buf[k]);
        }
    }
    free(buf);
    bam_header_destroy(header);
    bgzf_close(fp);
}

GUrl BAMUtils::sortBam(const QString& bamUrl, const QString& sortedBamFilePath, U2OpStatus& os) {
    QString sortedBamFilePathPrefix = sortedBamFilePath.endsWith(".bam")
                                          ? sortedBamFilePath.left(sortedBamFilePath.length() - 4)
                                          : sortedBamFilePath;
    coreLog.trace(QString("BAMUtils::sortBam %1 to %2").arg(bamUrl).arg(sortedBamFilePath));
    bamSortCore(os, bamUrl, sortedBamFilePathPrefix);
    return sortedBamFilePathPrefix + ".bam";
}

/**
 * Merges multiple sorted BAM.
 * Copy of the 'bam_merge_core' but with Unicode strings and parameters limited to the current UGENE use-cases.
 */
static int bamMergeCore(const QString& outFileName, const QList<QString>& filesToMerge) {
    coreLog.trace("bamMergeCore: " + filesToMerge.join(",") + " to " + outFileName);
    bam_header_t* hout = nullptr;
    int n = filesToMerge.length();
    auto fp = (bamFile*)calloc(n, sizeof(bamFile));
    auto heap = (heap1_t*)calloc(n, sizeof(heap1_t));
    // read the first
    for (int i = 0; i != n; ++i) {
        NP<FILE> file = BAMUtils::openFile(filesToMerge[i], "r");
        fp[i] = bgzf_fdopen(file.getNullable(), "r");
        if (fp[i] == nullptr) {
            coreLog.error(BAMUtils::tr("[bam_merge_core] fail to open file %1").arg(filesToMerge[i]));
            BAMUtils::closeFileIfOpen(file);
            for (int j = 0; j < i; ++j) {
                bgzf_close(fp[j]);
            }
            free(fp);
            free(heap);
            return -1;
        }
        bam_header_t* hin = bam_header_read(fp[i]);
        if (i == 0) {  // the first BAM
            hout = hin;
        } else {  // validate multiple baf
            int min_n_targets = hout->n_targets;
            if (hin->n_targets < min_n_targets)
                min_n_targets = hin->n_targets;

            for (int j = 0; j < min_n_targets; ++j)
                if (strcmp(hout->target_name[j], hin->target_name[j]) != 0) {
                    coreLog.error(BAMUtils::tr("[bam_merge_core] different target sequence name: '%1' != '%2' in file '%3'\n")
                                      .arg(hout->target_name[j])
                                      .arg(hin->target_name[j])
                                      .arg(filesToMerge[i]));
                    for (int m = 0; m <= i; m++) {
                        bgzf_close(fp[m]);
                    }
                    free(fp);
                    free(heap);
                    return -1;
                }

            // If this input file has additional target reference sequences,
            // add them to the headers to be output
            if (hin->n_targets > hout->n_targets) {
                swap_header_targets(hout, hin);
            }
            bam_header_destroy(hin);
        }
    }

    uint64_t idx = 0;
    auto iter = (bam_iter_t*)calloc(n, sizeof(bam_iter_t));
    for (int i = 0; i < n; ++i) {
        heap1_t* h = heap + i;
        h->i = i;
        h->b = (bam1_t*)calloc(1, sizeof(bam1_t));
        if (bam_iter_read(fp[i], iter[i], h->b) >= 0) {
            h->pos = ((uint64_t)h->b->core.tid << 32) | (uint32_t)((int32_t)h->b->core.pos + 1) << 1 | bam1_strand(h->b);
            h->idx = idx++;
        } else {
            h->pos = HEAP_EMPTY;
        }
    }
    NP<FILE> outFile = BAMUtils::openFile(outFileName, "wb");
    bamFile fpout = bgzf_fdopen(outFile.getNullable(), "w");
    if (fpout == nullptr) {
        coreLog.error(BAMUtils::tr("Failed to create the output file: %1").arg(outFileName));
        BAMUtils::closeFileIfOpen(outFile);
        for (int i = 0; i < n; ++i) {
            bam_iter_destroy(iter[i]);
            bgzf_close(fp[i]);
        }
        free(fp);
        free(heap);
        return -1;
    }
    bam_header_write(fpout, hout);
    bam_header_destroy(hout);

    ks_heapmake(heap, n, heap);
    while (heap->pos != HEAP_EMPTY) {
        bam1_t* b = heap->b;
        bam_write1_core(fpout, &b->core, b->data_len, b->data);
        int j = bam_iter_read(fp[heap->i], iter[heap->i], b);
        if (j >= 0) {
            heap->pos = ((uint64_t)b->core.tid << 32) | (uint32_t)((int)b->core.pos + 1) << 1 | bam1_strand(b);
            heap->idx = idx++;
        } else if (j == -1) {
            heap->pos = HEAP_EMPTY;
            free(heap->b->data);
            free(heap->b);
            heap->b = nullptr;
        } else {
            coreLog.error(BAMUtils::tr("[bam_merge_core] '%1' is truncated. Continue anyway.").arg(filesToMerge[heap->i]));
        }
        ks_heapadjust(heap, 0, n, heap);
    }

    for (int i = 0; i != n; ++i) {
        bam_iter_destroy(iter[i]);
        bgzf_close(fp[i]);
    }
    bgzf_close(fpout);
    free(fp);
    free(heap);
    free(iter);
    return 0;
}

GUrl BAMUtils::mergeBam(const QStringList& bamUrls, const QString& mergedBamTargetUrl, U2OpStatus& os) {
    coreLog.details(tr(R"(Merging BAM files: "%1". Resulting merged file is: "%2")")
                        .arg(QString(bamUrls.join(",")))
                        .arg(QString(mergedBamTargetUrl)));

    int rc = bamMergeCore(mergedBamTargetUrl, bamUrls);
    CHECK_EXT(rc >= 0, os.setError(tr("Failed to merge BAM files: %1 into %2").arg(bamUrls.join(",")).arg(mergedBamTargetUrl)), {});
    return mergedBamTargetUrl;
}

void* BAMUtils::loadIndex(const QString& filePath) {
    // See bam_index_load_local.
    QString mode = "rb";
    NP<FILE> file = BAMUtils::openFile(filePath + ".bai", mode);
    if (file == nullptr && filePath.endsWith("bam")) {
        file = BAMUtils::openFile(filePath.chopped(4) + ".bai", mode);
    }
    CHECK(file != nullptr, nullptr);
    bam_index_t* idx = bam_index_load_core(file);
    closeFileIfOpen(file);
    return idx;
}

bool BAMUtils::hasValidBamIndex(const QString& bamUrl) {
    auto index = (bam_index_t*)loadIndex(bamUrl);
    CHECK(index != nullptr, false);
    bam_index_destroy(index);

    QFileInfo idxFileInfo(bamUrl + ".bai");
    if (!idxFileInfo.exists()) {
        QString indexUrl = bamUrl;
        indexUrl.chop(4);
        idxFileInfo.setFile(indexUrl + ".bai");
    }
    QFileInfo bamFileInfo(bamUrl);

    if (idxFileInfo.lastModified() < bamFileInfo.lastModified()) {
        return false;
    }

    return true;
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

/**
 * Builds and saves index for BAM file. Returns 0 if the index was created correctly.
 * Exact copy of 'bam_index_build2' with a correct unicode file names support.
 */
static int bamIndexBuild(const QString& bamFileName) {
    NP<FILE> file = BAMUtils::openFile(bamFileName, "rb");
    bamFile fp = bgzf_fdopen(file.getNullable(), "r");
    if (fp == nullptr) {
        BAMUtils::closeFileIfOpen(file);
        fprintf(stderr, "[bam_index_build2] fail to open the BAM file.\n");
        return -1;
    }
    fp->owned_file = 1;
    bam_index_t* idx = bam_index_core(fp);
    bgzf_close(fp);
    if (idx == nullptr) {
        fprintf(stderr, "[bam_index_build2] fail to index the BAM file.\n");
        return -1;
    }
    NP<FILE> fpidx = BAMUtils::openFile(bamFileName + ".bai", "wb");
    if (fpidx == nullptr) {
        fprintf(stderr, "[bam_index_build2] fail to create the index file.\n");
        return -1;
    }
    bam_index_save(idx, fpidx);
    bam_index_destroy(idx);
    fclose(fpidx);
    return 0;
}

void BAMUtils::createBamIndex(const QString& bamUrl, U2OpStatus& os) {
    coreLog.details(BAMUtils::tr("Build index for bam file: \"%1\"").arg(bamUrl));
    int error = bamIndexBuild(bamUrl);
    if (error == -1) {
        os.setError(tr("Can't build the index: %1").arg(bamUrl));
    }
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

static void createHeader(bam_header_t* header, const QList<GObject*>& objects, U2OpStatus& os) {
    CHECK_EXT(header != nullptr, os.setError("NULL header"), );

    header->n_targets = objects.size();
    header->target_name = new char*[header->n_targets];
    header->target_len = new uint32_t[header->n_targets];

    QByteArray headerText;
    headerText += "@HD\tVN:1.4\tSO:coordinate\n";

    int objIdx = 0;
    foreach (GObject* obj, objects) {
        auto assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(assemblyObj != nullptr, os.setError("NULL assembly object"), );

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
        auto assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(assemblyObj != nullptr, os.setError("NULL assembly object"), result);

        QString name = assemblyObj->getGObjectName();
        result[name] = i;
        i++;
    }
    return result;
}

static void writeObjectsWithSamtools(samfile_t* out, const QList<GObject*>& objects, U2OpStatus& os, const U2Region& desiredRegion) {
    foreach (GObject* obj, objects) {
        auto assemblyObj = dynamic_cast<AssemblyObject*>(obj);
        SAFE_POINT_EXT(assemblyObj != nullptr, os.setError("NULL assembly object"), );

        DbiConnection con(assemblyObj->getEntityRef().dbiRef, os);
        CHECK_OP(os, );

        U2AssemblyDbi* dbi = con.dbi->getAssemblyDbi();
        SAFE_POINT_EXT(dbi != nullptr, os.setError("NULL assembly DBI"), );

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

    bam_header_t* header = bam_header_init();
    createHeader(header, objects, os);
    if (os.isCoR()) {
        bam_header_destroy(header);
        return;
    }

    samfile_t* out = samOpen(url, openMode.constData(), "", header);
    bam_header_destroy(header);
    CHECK_EXT(out != nullptr, os.setError(QString("Can not open file for writing: %1").arg(url)), );

    writeObjectsWithSamtools(out, objects, os, desiredRegion);
    samclose(out);
}

// the function assumes the equal order of alignments in files
bool BAMUtils::isEqualByLength(const QString& fileUrl1, const QString& fileUrl2, U2OpStatus& os) {
    samfile_t* in = nullptr;
    samfile_t* out = nullptr;

    const char* readMode1 = fileUrl1.endsWith(".bam", Qt::CaseInsensitive) ? "rb" : "r";
    const char* readMode2 = fileUrl2.endsWith(".bam", Qt::CaseInsensitive) ? "rb" : "r";
    {
        in = samOpen(fileUrl1, readMode1);
        SAMTOOL_CHECK(in != nullptr, openFileError(fileUrl1), false);

        out = samOpen(fileUrl2, readMode2);
        SAMTOOL_CHECK(out != nullptr, openFileError(fileUrl2), false);

        if (in->header != out->header) {
            SAMTOOL_CHECK(out->header != nullptr, headerError(fileUrl2), false);
            SAMTOOL_CHECK(in->header != nullptr, headerError(fileUrl1), false);
        }
    }

    if (in->header != nullptr && in->header->target_len && out->header->target_len) {
        // if there are headers
        if (*(in->header->target_len) != *(out->header->target_len)) {
            os.setError(QString("Different target length of files. %1 and %2").arg(qint64(in->header->target_len)).arg(qint64(out->header->target_len)));
            closeFiles(in, out);
            return false;
        }
    }

    bam1_t* b1 = bam_init1();
    bam1_t* b2 = bam_init1();

    QVector<int> length1;
    QVector<int> length2;
    int r1, r2;
    while ((r1 = samread(in, b1)) >= 0) {
        length1 << b1->data_len;
    }
    while ((r2 = samread(out, b2)) >= 0) {
        length2 << b2->data_len;
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

namespace {
const int bufferSize = 1024 * 1024;  // 1 Mb
const int referenceColumn = 2;  // 5 Mb

inline QByteArray readLine(IOAdapter* io, char* buffer, int maxLineLength) {
    QByteArray result;
    bool terminatorFound = false;
    do {
        qint64 length = io->readLine(buffer, maxLineLength, &terminatorFound);
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
    for (const QString& reference : qAsConst(references)) {
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
