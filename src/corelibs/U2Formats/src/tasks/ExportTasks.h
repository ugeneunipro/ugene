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

#ifndef _U2_EXPORT_TASKS_H_
#define _U2_EXPORT_TASKS_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>

namespace U2 {

class DNATranslation;
class LoadDocumentTask;
class MultipleSequenceAlignmentObject;

/** Saves a copy of the alignment using the given document format. */
class U2FORMATS_EXPORT ExportAlignmentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportAlignmentTask(const MultipleSequenceAlignment& ma, const QString& url, const DocumentFormatId& documentFormatId);

    void run() override;

    /**
     * Maximum MSA size in bytes that can be loaded/stored by UGENE safely.
     * UGENE loads a full MSA model into a memory in a lot of places across the codebase.
     * This constant should be limit the maxim size of MSA produced by UGENE, so UGENE won't create objects it can't handle.
     */
    static constexpr qint64 MAX_SAFE_ALIGNMENT_SIZE_TO_EXPORT = 100 * 1000 * 1000;

private:
    MultipleSequenceAlignment ma;
    QString url;
    DocumentFormatId documentFormatId;
};

/** Saves a copy of the alignment using the given sequence document format. */
class U2FORMATS_EXPORT ExportMSA2SequencesTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportMSA2SequencesTask(const MultipleSequenceAlignment& ma, const QString& url, bool trimLeadingAndTrailingGaps, const DocumentFormatId& documentFormatId);

    void run() override;

private:
    MultipleSequenceAlignment ma;
    QString url;
    bool trimLeadingAndTrailingGaps;
    QString documentFormatId;
};

/** Saves a copy of the alignment. */
class U2FORMATS_EXPORT ExportMSA2MSATask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportMSA2MSATask(const MultipleSequenceAlignment& msa,
                      const QList<qint64>& rowIds,
                      const U2Region& columnRegion,
                      const QString& url,
                      const DNATranslation* aminoTranslation,
                      const DocumentFormatId& documentFormatId,
                      bool trimLeadingAndTrailingGaps,
                      bool convertUnknownToGap,
                      bool reverseComplement,
                      int translationFrame);

    void run() override;

private:
    /** Columns to export. */
    U2Region columnRegion;

    /** Alignment rows converted to sequences. */
    QList<DNASequence> sequenceList;

    QString url;
    QString documentFormatId;

    /** Amino translation for a sequences in alignment. If not NULL -> sequence is translated. */
    const DNATranslation* aminoTranslation = nullptr;

    /** Trim gaps before translation of not. */
    const bool trimLeadingAndTrailingGaps;

    /* If there are unknown amino bases, they are translated as "X" by default, if this value is true tey will be tranlated as "-". */
    const bool convertUnknownToGap;

    /* There is required to translate a reverse-complement strand. */
    const bool reverseComplement;

    /**
     * Indicates which frame to translate: 0, 1, 2.
     * If 'reverseComplement' is true the frame offset is applied after the sequence is reversed and & complemented.
     */
    const int translationFrame;
};

class DNAChromatogramObject;

/** A task settings to export chromatograms. */
class U2FORMATS_EXPORT ExportChromatogramTaskSettings {
public:
    ExportChromatogramTaskSettings()
        : reverse(false), complement(false), loadDocument(false) {
    }
    QString url;
    bool reverse;
    bool complement;
    bool loadDocument;
};

/** Export chromatograms to SCF format. */
class U2FORMATS_EXPORT ExportDNAChromatogramTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportDNAChromatogramTask(DNAChromatogramObject* chromaObj, const ExportChromatogramTaskSettings& url);
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    DNAChromatogramObject* chromaObject;
    ExportChromatogramTaskSettings settings;
    LoadDocumentTask* loadTask;
};

}  // namespace U2

#endif
