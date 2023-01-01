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

#ifndef _U2_BLAST_SUPPORT_TASK_H
#define _U2_BLAST_SUPPORT_TASK_H

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>

#include "BlastTaskSettings.h"

class QDomNode;

namespace U2 {

class SaveDocumentTask;
class U2PseudoCircularization;

class BlastCommonTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BlastCommonTask(const BlastTaskSettings& settings);

    void prepare() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    ReportResult report() override;

    QString generateReport() const override;

    /** Maps current 'resultsPerQuerySequence' to a list of annotations. */
    QList<SharedAnnotationData> getResultAnnotations() const;

    /** Returns result found for the query sequence with the given index. */
    QList<SharedAnnotationData> getResultPerQuerySequence(int querySequenceIndex) const;

    BlastTaskSettings getSettings() const;

    virtual ExternalToolRunTask* createBlastTask() = 0;

protected:
    BlastTaskSettings settings;
    QString url;

private:
    SaveDocumentTask* saveTemporaryDocumentTask = nullptr;
    ExternalToolRunTask* blastTask = nullptr;
    U2SequenceObject* sequenceObject = nullptr;
    Document* tmpDoc = nullptr;

    /** Results by sequence index in settings.querySequences list. */
    QHash<int, QList<SharedAnnotationData>> resultsPerQuerySequence;

    /** Pre-processed and 'circularized' query sequences. */
    QList<QByteArray> querySequences;

    void parseTabularResult();
    void parseTabularLine(const QByteArray& line);

    void parseXMLResult();
    void parseXMLHit(const QDomNode& xml, int querySequenceIndex);
    void parseXMLHsp(const QDomNode& xml, int querySequenceIndex, const QString& id, const QString& def, const QString& accession);
    QString getAcceptableTempDir() const;

    /**
     * Parses query sequence index from the given BLAST query sequence name.
     * Triggers SAFE_POINT & returns -1 in case of parsing error or invalid sequence index.
     */
    int parseQuerySequenceIndex(const QString& querySequenceName) const;
};

class BlastMultiTask : public Task {
    Q_OBJECT
public:
    BlastMultiTask(QList<BlastTaskSettings>& settingsList, QString& url);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    Task::ReportResult report() override;
    QString generateReport() const;

private:
    QList<BlastTaskSettings> settingsList;
    Document* doc;
    QString url;
};

}  // namespace U2
#endif  // _U2_BLAST_SUPPORT_TASK_H
