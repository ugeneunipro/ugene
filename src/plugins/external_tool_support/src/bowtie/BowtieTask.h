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

#ifndef _U2_BOWTIE_TASK_H_
#define _U2_BOWTIE_TASK_H_

#include <U2Algorithm/DnaAssemblyTask.h>
#include <U2Core/ExternalToolRunTask.h>

#include <QTemporaryFile>

namespace U2 {

class BowtieBuildIndexTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BowtieBuildIndexTask(const QString &referencePath, const QString &indexPath, bool colorspace);

    void prepare();
private:
    class LogParser : public ExternalToolLogParser {
    public:
        enum Stage {
            PREPARE,
            FORWARD_INDEX,
            MIRROR_INDEX
        };
        enum Substage {
            UNKNOWN,
            BUCKET_SORT,
            GET_BLOCKS
        };

        LogParser();
        void parseOutput(const QString &partOfLog);
        void parseErrOutput(const QString &partOfLog);
        int getProgress();
    private:
        Stage stage;
        Substage substage;
        int bucketSortIteration;
        int blockIndex;
        int blockCount;
        int substageProgress;
        int progress;
    };

    QString referencePath;
    QString indexPath;
    bool colorspace;
};

class BowtieAssembleTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BowtieAssembleTask(const DnaAssemblyToRefTaskSettings &settings);

    bool hasResult()const;

    void prepare();
private:
    class LogParser : public ExternalToolLogParser {
    public:
        LogParser();

        void parseOutput(const QString &partOfLog);
        void parseErrOutput(const QString &partOfLog);

        bool hasResult()const;
    private:
        bool hasResults;
    };

    LogParser *logParser;
    DnaAssemblyToRefTaskSettings settings;
};

class BowtieTask : public DnaAssemblyToReferenceTask {
    Q_OBJECT
    DNA_ASSEMBLEY_TO_REF_TASK_FACTORY(BowtieTask)
public:
    BowtieTask(const DnaAssemblyToRefTaskSettings &settings, bool justBuildIndex = false);

    void prepare();
    ReportResult report();
protected slots:
    QList<Task *> onSubTaskFinished(Task *subTask);
public:
    static const QString OPTION_N_MISMATCHES;
    static const QString OPTION_V_MISMATCHES;
    static const QString OPTION_MAQERR;
    static const QString OPTION_SEED_LEN;
    static const QString OPTION_NOFW;
    static const QString OPTION_NORC;
    static const QString OPTION_MAXBTS;
    static const QString OPTION_TRYHARD;
    static const QString OPTION_CHUNKMBS;
    static const QString OPTION_NOMAQROUND;
    static const QString OPTION_SEED;
    static const QString OPTION_BEST;
    static const QString OPTION_ALL;
    static const QString OPTION_COLORSPACE;
    static const QString OPTION_THREADS;

    static const QStringList indexSuffixes;
    static const QStringList largeIndexSuffixes;
private:
    BowtieBuildIndexTask *buildIndexTask;
    BowtieAssembleTask *assembleTask;

    Task* unzipTask;
    QTemporaryFile temp;
};

class BowtieTaskFactory : public DnaAssemblyToRefTaskFactory {
public:
    DnaAssemblyToReferenceTask *createTaskInstance(const DnaAssemblyToRefTaskSettings &settings, bool justBuildIndex = false);
protected:
};

} // namespace U2

#endif // _U2_BOWTIE_TASK_H_
