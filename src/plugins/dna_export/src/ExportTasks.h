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

#ifndef _U2_EXPORT_PLUGIN_TASKS_H_
#define _U2_EXPORT_PLUGIN_TASKS_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>

namespace U2 {

class DNATranslation;
class LoadDocumentTask;
class MultipleSequenceAlignmentObject;

/** A task to adds exported document to project and open view*/
//TODO: make this task a general purpose routine
class AddExportedDocumentAndOpenViewTask : public Task {
    Q_OBJECT
public:
    AddExportedDocumentAndOpenViewTask(DocumentProviderTask *t);
    QList<Task *> onSubTaskFinished(Task *subTask);

private:
    DocumentProviderTask *exportTask;
    LoadDocumentTask *loadTask;
};

/** A task to save alignment to CLUSTAL */
class ExportAlignmentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportAlignmentTask(const MultipleSequenceAlignment &ma, const QString &fileName, DocumentFormatId f);

    void run();

private:
    MultipleSequenceAlignment ma;
    QString fileName;
    DocumentFormatId format;
};

/** A task to export alignment to FASTA */
class ExportMSA2SequencesTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportMSA2SequencesTask(const MultipleSequenceAlignment &ma, const QString &url, bool trimAli, DocumentFormatId format);

    void run();

private:
    MultipleSequenceAlignment ma;
    QString url;
    bool trimAli;
    QString format;
};

class ExportMSA2MSATask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportMSA2MSATask(const MultipleSequenceAlignment &ma,
                      int offset,
                      int len,
                      const QString &url,
                      const QList<DNATranslation *> &aminoTranslations,
                      DocumentFormatId format,
                      const bool trimGaps,
                      const bool convertUnknownToGap,
                      const bool reverseComplement,
                      const int baseOffset);

    void run();

private:
    MultipleSequenceAlignment ma;
    int offset;
    int len;
    QString url;
    QString format;
    /*!
     * Amino translation for a sequences in alignment. If not NULL -> sequence is translated
     */
    QList<DNATranslation *> aminoTranslations;
    /*!
     * Trim gaps before translation of not
     */
    const bool trimGaps;
    /*!
     * If there are unknown amino bases, they are translated as "X" by default, if this value is true tey will be tranlated as "-"
     */
    const bool convertUnknownToGap;
    /*!
     * There is required to translate a reverse-complement strand
     */
    const bool reverseComplement;
    /*!
     * The number of characters to skip based on a translation frame.
     * If the frame is "1" or "-1", than baseOffset is 0.
     * If the frame is "2" or "-2", than baseOffset is 1.
     * If the frame is "3" or "-3", than baseOffset is 2.
     */
    const int baseOffset;
};

class DNAChromatogramObject;

/** A task to export chromatogram to SCF */

struct ExportChromatogramTaskSettings {
    ExportChromatogramTaskSettings()
        : reverse(false), complement(false), loadDocument(false) {
    }
    QString url;
    bool reverse;
    bool complement;
    bool loadDocument;
};

class ExportDNAChromatogramTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportDNAChromatogramTask(DNAChromatogramObject *chromaObj, const ExportChromatogramTaskSettings &url);
    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);

private:
    DNAChromatogramObject *cObj;
    ExportChromatogramTaskSettings settings;
    LoadDocumentTask *loadTask;
};

}    // namespace U2

#endif
