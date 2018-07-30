/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_TRIMMOMATIC_TASK_H_
#define _U2_TRIMMOMATIC_TASK_H_

#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

struct TrimmomaticTaskSettings {
    TrimmomaticTaskSettings();

    QString inputUrl1;
    QString inputUrl2;
    bool readsArePaired;
    QStringList trimmingSteps;
    QString seOutputUrl;
    QString pairedOutputUrl1;
    QString pairedOutputUrl2;
    QString unpairedOutputUrl1;
    QString unpairedOutputUrl2;
    bool generateLog;
    QString logUrl;
    int numberOfThreads;
    QString workingDirectory;

    static const QString SINGLE_END;
    static const QString PAIRED_END;
};

class TrimmomaticTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    TrimmomaticTask(const TrimmomaticTaskSettings &settings);

    const QString &getSeOutputUrl() const;
    const QString &getPairedOutputUrl1() const;
    const QString &getPairedOutputUrl2() const;
    const QString &getUnpairedOutputUrl1() const;
    const QString &getUnpairedOutputUrl2() const;
    const QString &getLogUrl() const;

private:
    void prepare();

    QStringList getArguments();

    const TrimmomaticTaskSettings settings;

    ExternalToolRunTask *trimmomaticToolRunTask;
};

} // namespace U2

#endif // _U2_TRIMMOMATIC_TASK_H_
