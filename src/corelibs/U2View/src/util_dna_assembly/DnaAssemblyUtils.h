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

#ifndef _U2_DNA_ASSEMBLEY_UTILS_H_
#define _U2_DNA_ASSEMBLEY_UTILS_H_

#include <U2Algorithm/DnaAssemblyMultiTask.h>
#include <U2Core/ExternalToolRunTask.h>


namespace U2 {

class DnaAssemblyToRefTaskSettings;

class U2VIEW_EXPORT DnaAssemblySupport : public QObject {
    Q_OBJECT
public:
    DnaAssemblySupport();

    static QMap<QString, QString> toConvert(const DnaAssemblyToRefTaskSettings &settings, QList<GUrl> &unknownFormatFiles);
    static QString toConvertText(const QMap<QString, QString> &files);
    static QString unknownText(const QList<GUrl> &unknownFormatFiles);

private slots:
    void sl_showDnaAssemblyDialog();
    void sl_showGenomeAssemblyDialog();
    void sl_showBuildIndexDialog();
    void sl_showConvertToSamDialog();

};

class FilterUnpairedReadsTask : public Task {
    Q_OBJECT
public:
    FilterUnpairedReadsTask(const DnaAssemblyToRefTaskSettings &settings);
    void run();
    const QList<ShortReadSet>& getFilteredReadList() const { return filteredReads; }

private:
    QString getTmpFilePath(const GUrl& initialFile);
    void compareFiles(const GUrl& upstream, const GUrl& downstream,
                      const GUrl& upstreamFiltered, const GUrl& downstreamFiltered);

    DnaAssemblyToRefTaskSettings settings;
    QList<ShortReadSet> filteredReads;
    QString tmpDirPath;
};

class U2VIEW_EXPORT DnaAssemblyTaskWithConversions : public ExternalToolSupportTask {
    Q_OBJECT
public:
    DnaAssemblyTaskWithConversions(const DnaAssemblyToRefTaskSettings &settings, bool viewResult = false, bool justBuildIndex = false);

    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    const DnaAssemblyToRefTaskSettings& getSettings() const;
    ReportResult report();

private:
    DnaAssemblyToRefTaskSettings settings;
    bool viewResult;
    bool justBuildIndex;
    int conversionTasksCount;
    DnaAssemblyMultiTask *assemblyTask;
};

} // U2


#endif // _U2_DNA_ASSEMBLEY_TASK_H_
