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

#ifndef _U2_DNA_SEQUENCE_GENERATOR_H_
#define _U2_DNA_SEQUENCE_GENERATOR_H_

#include <QRandomGenerator>

#include <U2Core/DNASequence.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>
#include <U2Core/U2Sequence.h>

namespace U2 {

class DNAAlphabet;
class Document;
class GObject;
class LoadDocumentTask;
class SaveDocumentTask;

class DNASequenceGeneratorConfig {
public:
    DNASequenceGeneratorConfig()
        : addToProj(false), saveDoc(true), alphabet(nullptr), length(0), numSeqs(1), useRef(true), window(0), seed(0) {
    }

    const DNAAlphabet* getAlphabet() const {
        assert(alphabet);
        return alphabet;
    }

    bool useReference() const {
        return useRef;
    }

    const QString& getReferenceUrl() const {
        return refUrl;
    }

    const QMap<char, qreal>& getContent() const {
        return content;
    }

    int getLength() const {
        return length;
    }

    int getNumberOfSequences() const {
        return numSeqs;
    }

    QString getSequenceName() const {
        return sequenceName;
    }

    QString getOutUrlString() const {
        return outUrl;
    }

    const DocumentFormatId& getDocumentFormatId() const {
        return formatId;
    }

    bool addToProj;
    bool saveDoc;
    // output url
    QString outUrl;
    // output sequence base name
    QString sequenceName;
    // output document format
    DocumentFormatId formatId;
    // output sequence alphabet
    const DNAAlphabet* alphabet;
    // output sequence length
    int length;
    // number of sequences to generate
    int numSeqs;
    // use content from reference or specified manually
    bool useRef;
    // reference file url
    QString refUrl;
    // char frequencies
    QMap<char, qreal> content;
    // window size
    int window;
    // seed to initialize qrand
    int seed;
};

class DNASequenceGenerator {
public:
    static const QString ID;

    static QString prepareReferenceFileFilter();

    /**
     * Generates random sequence with the given character frequencies and the given random generator.
     * Character frequencies are values from 0 to 1 that ideally should sum up to 1. (=100%)
     * This way if an 'A' character has frequency '0.4' the result sequence will have around 40% of 'A' characters.
     * The frequency values precision is supported up the 3rd fractional digit: 0.123 and rounded up after that.
     */
    static void generateSequence(const QMap<char, qreal>& charFreqs,
                                 int length,
                                 QByteArray& result,
                                 QRandomGenerator& randomGenerator,
                                 U2OpStatus& os);

    static void evaluateBaseContent(const DNASequence& sequence, QMap<char, qreal>& result);

    static void evaluateBaseContent(const MultipleSequenceAlignment& ma, QMap<char, qreal>& result);
};

class EvaluateBaseContentTask : public Task {
    Q_OBJECT
public:
    EvaluateBaseContentTask(GObject* obj);

    void run();

    QMap<char, qreal> getResult() const {
        return result;
    }

    const DNAAlphabet* getAlphabet() const {
        return alp;
    }

private:
    GObject* _obj;
    const DNAAlphabet* alp;
    QMap<char, qreal> result;
};

class GenerateDNASequenceTask : public Task {
    Q_OBJECT
public:
    /**
     * Initializes a new GenerateDNASequenceTask.
     * If the seed is < 0 a random seed will be generated per each window.
     * Otherwise the same seed will be used for all windows that will lead to the equal sequences in every window.
     */
    GenerateDNASequenceTask(const QMap<char, qreal>& baseContent, int length, int window, int count, int seed);

    void prepare() override;
    void run() override;

    QList<U2Sequence> getResults() const {
        return results;
    }
    U2DbiRef getDbiRef() const {
        return dbiRef;
    }

private:
    QMap<char, qreal> baseContent;
    int length;
    int window;
    int count;
    int seed;
    QList<U2Sequence> results;
    U2DbiRef dbiRef;
};

class DNASequenceGeneratorTask : public Task {
    Q_OBJECT
public:
    DNASequenceGeneratorTask(const DNASequenceGeneratorConfig& cfg_);
    QList<Task*> onSubTaskFinished(Task* subTask);
    QList<DNASequence> getSequences() const {
        return results;
    }

private:
    QList<Task*> onLoadRefTaskFinished();
    QList<Task*> onEvalTaskFinished();
    QList<Task*> onGenerateTaskFinished();
    QList<Task*> onSaveTaskFinished();

    void addSequencesToMsaDoc(Document* source);
    void addSequencesToSeqDoc(Document* source);

    static EvaluateBaseContentTask* createEvaluationTask(Document* doc, QString& err);

    DNASequenceGeneratorConfig cfg;
    LoadDocumentTask* loadRefTask;
    EvaluateBaseContentTask* evalTask;
    GenerateDNASequenceTask* generateTask;
    SaveDocumentTask* saveTask;
    QList<DNASequence> results;
};

}  // namespace U2

#endif
