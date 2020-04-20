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

#ifndef _U2_EXPORT_SEQUENCE_PLUGIN_TASKS_H_
#define _U2_EXPORT_SEQUENCE_PLUGIN_TASKS_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentProviderTask.h>

namespace U2 {

class DNAAlphabet;
class DNATranslation;
class U2Sequence;

/** An item to export sequence. Contains unprocessed sequence + annotations + info about required transformations */
class ExportSequenceItem {
public:
    ExportSequenceItem();
    ExportSequenceItem(const ExportSequenceItem &other);

    ~ExportSequenceItem();

    void setOwnershipOverSeq(const U2Sequence &seq, const U2DbiRef &dbiRef);
    void setSequenceInfo(U2SequenceObject *seqObj);

    // after calling this method the client code takes responsibility for correct release the sequence from the database
    U2SequenceObject * takeOwnedSeq();
    bool ownsSeq() const;
    bool isEmpty() const;

    ExportSequenceItem & operator =(const ExportSequenceItem &other);
    bool operator==(const ExportSequenceItem& other) const;

    U2EntityRef                 seqRef; // sequence to copy
    QString                     name;
    bool                        circular;
    const DNAAlphabet *         alphabet;
    qint64                      length;

    QList<SharedAnnotationData> annotations;// annotations to copy
    const DNATranslation *      complTT;    // complement translations for a sequence. Used only if 'strand' is 'compl' or 'both'
    const DNATranslation *      aminoTT;    // amino translation for a sequence. If not NULL -> sequence is translated
    const DNATranslation *      backTT;     // nucleic translation for a sequence. If not NULL -> sequence is back translated

private:
    void startSeqOwnership();
    int incrementSeqRefCount() const;
    int decrementSeqRefCount() const;
    void stopSeqOwnership();

    void releaseOwnedSeq();

    static QMap<U2EntityRef, int> sequencesRefCounts;
};

class ExportSequenceTaskSettings {
public:
    ExportSequenceTaskSettings();

    QList<ExportSequenceItem> items;            // sequences to export

    QString             fileName;               // result file name
    bool                merge;                  // if true -> multiple sequences are merged
    int                 mergeGap;               // the gap between sequences if merged

    TriState            strand;                 // Yes -> direct, No -> complement, Unknown -> Both
    bool                allAminoFrames;         // for every sequence and (every complement if needed) generates 3-frame amino translations

    bool                mostProbable;           // True - use most probable codon, False - use distribution according to frequency

    bool                saveAnnotations;        // Store available annotations for sequences into result file too

    DocumentFormatId    formatId;

    QString             sequenceName;           // custom sequence name

    qint64              sequenceLength;         // length of the sequence if there is the only source sequence
};

/** Exports sequences a file */
class ExportSequenceTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportSequenceTask(const ExportSequenceTaskSettings &s);

    void run();

    static ExportSequenceItem mergedCircularItem(const ExportSequenceItem &first,
                                                 const ExportSequenceItem &second,
                                                 U2OpStatus &os);

private:
    ExportSequenceTaskSettings config;
};

//////////////////////////////////////////////////////////////////////////
// Task to export sequences under annotations

class ExportSequenceAItem {
public:
    ExportSequenceAItem();

    QPointer<U2SequenceObject>  sequence;     // sequence
    QList<SharedAnnotationData> annotations;  // annotated regions to be exported
    const DNATranslation *      aminoTT;      // if not null -> sequence regions will be translated (0-frame only)
    const DNATranslation *      complTT;      // if not null & annotation location is on complement strand - it will be rev-complemented
};


class ExportAnnotationSequenceTaskSettings {
public:
    QList<ExportSequenceAItem>  items;                  // data to export
    ExportSequenceTaskSettings exportSequenceSettings; // extra configuration for ExportSequenceTask
};

class ExportAnnotationSequenceSubTask : public Task {
    Q_OBJECT
public:
    ExportAnnotationSequenceSubTask(ExportAnnotationSequenceTaskSettings &s);

    void run();

private:
    U2Sequence importAnnotatedSeq2Dbi(const SharedAnnotationData &ad, const ExportSequenceAItem &ei, const U2DbiRef &resultDbiRef, QVector<U2Region> &resultRegions, U2OpStatus &os);
    ExportAnnotationSequenceTaskSettings &config;
};

class ExportAnnotationSequenceTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportAnnotationSequenceTask(const ExportAnnotationSequenceTaskSettings &s);

    QList<Task *> onSubTaskFinished(Task *subTask);

private:
    ExportAnnotationSequenceTaskSettings    config;
    ExportAnnotationSequenceSubTask *       extractSubTask;
    ExportSequenceTask *                    exportSubTask;
};

} // namespace U2

#endif // _U2_EXPORT_SEQUENCE_PLUGIN_TASKS_H_
