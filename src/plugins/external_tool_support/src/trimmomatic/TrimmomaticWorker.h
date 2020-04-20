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

#ifndef _U2_TRIMMOMATIC_WORKER_H_
#define _U2_TRIMMOMATIC_WORKER_H_

#include <U2Lang/BaseDatasetWorker.h>
#include <U2Lang/LocalDomain.h>

#include "TrimmomaticTask.h"

namespace U2 {
namespace LocalWorkflow {

class TrimmomaticWorker : public BaseDatasetWorker {
    Q_OBJECT
public:
    TrimmomaticWorker(Actor *actor);

    void init() override;
    void cleanup() override;

protected:
    Task* createPrepareTask(U2OpStatus& os) const override;
    void onPrepared(Task* task, U2OpStatus& os) override;

    Task* createTask(const QList<Message> &messages) const override;
    QVariantMap getResult(Task* task, U2OpStatus& os) const override;
    MessageMetadata generateMetadata(const QString &datasetName) const override;

private:
    TrimmomaticTaskSettings getSettings(const Message& message, const QString& dirForResults) const;

    // Set a value of an URL parameter that can be "Auto":
    // use the specified value if available or, if it is empty,
    // generate the value autoamtically as follows:
    // working_dir/input_file_name+suffix.input_file_extension
    // Roll the file name, if required.
    QString setAutoUrl(const QString &paramId, const QString &inputFile, const QString &workingDir, const QString &fileNameSuffix) const;
    QPair<QString, QString> getAbsoluteAndCopiedPathFromStep(const QString& trimmingStep) const;
    void changeAdapters();
    void processMetadata(QList<Task*> tasks) const;

    mutable QStringList copiedAdapters;
    mutable QSet<QString> excludedUrls;
    mutable QString metaFileUrl;

    bool pairedReadsInput;
    bool generateLog;
    QStringList trimmingSteps;
    int numberOfThreads;

    static const QString TRIMMOMATIC_DIR;
    static const QString SE_OUTPUT_FILE_NAME_SUFFIX;
    static const QString PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX;
    static const QString PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX;
    static const QString LOG_FILE_NAME_ENDING;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_TRIMMOMATIC_WORKER_H_
