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

#ifndef _U2_SPADES_TASK_H_
#define _U2_SPADES_TASK_H_

#include <U2Algorithm/GenomeAssemblyRegistry.h>

#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

class SpadesTask : public GenomeAssemblyTask {
    Q_OBJECT
    GENOME_ASSEMBLEY_TASK_FACTORY(SpadesTask)
public:
    SpadesTask(const GenomeAssemblyTaskSettings &settings);

    void prepare();
    ReportResult report();
    QString getScaffoldsUrl() const;
    QString getContigsUrl() const;

protected slots:
    QList<Task *> onSubTaskFinished(Task *subTask);

public:
    static const QString OPTION_DATASET_TYPE;
    static const QString OPTION_RUNNING_MODE;
    static const QString OPTION_K_MER;
    static const QString OPTION_INPUT_DATA;
    static const QString OPTION_THREADS;
    static const QString OPTION_MEMLIMIT;

    static const QString YAML_FILE_NAME;
    static const QString CONTIGS_NAME;
    static const QString SCAFFOLDS_NAME;

private:
    ExternalToolRunTask *assemblyTask;
    QString contigsUrl;

private:
    void writeYamlReads();
};

class SpadesTaskFactory : public GenomeAssemblyTaskFactory {
public:
    GenomeAssemblyTask *createTaskInstance(const GenomeAssemblyTaskSettings &settings);
};

class SpadesLogParser : public ExternalToolLogParser {
public:
    SpadesLogParser();
    void parseOutput(const QString &partOfLog);
    void parseErrOutput(const QString &partOfLog);

private:
    QString lastLine;
    QString lastErrLine;
};



} // namespace U2

#endif // _U2_SPADES_TASK_H_

