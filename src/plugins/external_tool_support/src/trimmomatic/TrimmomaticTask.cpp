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

#include <U2Core/Counter.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include "TrimmomaticLogParser.h"
#include "TrimmomaticSupport.h"
#include "TrimmomaticTask.h"

namespace U2 {

const QString TrimmomaticTaskSettings::SINGLE_END = "single-end";
const QString TrimmomaticTaskSettings::PAIRED_END = "paired-end";

TrimmomaticTaskSettings::TrimmomaticTaskSettings()
    : pairedReadsInput(false),
      generateLog(false),
      numberOfThreads(1)
{
}

TrimmomaticTask::TrimmomaticTask(const TrimmomaticTaskSettings &settings)
    : ExternalToolSupportTask(tr("Improve reads with Trimmomatic"), TaskFlags_NR_FOSE_COSC),
      settings(settings),
      trimmomaticToolRunTask(NULL)
{
    GCOUNTER(cvar, tvar, "TrimmomaticTask");

    if (settings.pairedReadsInput) {
        SAFE_POINT_EXT(!settings.pairedOutputUrl1.isEmpty() && !settings.pairedOutputUrl2.isEmpty() &&
                      !settings.unpairedOutputUrl1.isEmpty() && !settings.unpairedOutputUrl2.isEmpty(),
                      setError("At least one of the four output files is not set!"), );
    } else {
        SAFE_POINT_EXT(!settings.seOutputUrl.isEmpty(), setError("Output file is not set!"), );
    }

    SAFE_POINT_EXT(!(settings.generateLog && settings.logUrl.isEmpty()), setError("Log file is not set!"), );
}

void TrimmomaticTask::prepare() {
    trimmomaticToolRunTask = new ExternalToolRunTask(TrimmomaticSupport::ET_TRIMMOMATIC_ID, getArguments(), new TrimmomaticLogParser(), settings.workingDirectory);
    setListenerForTask(trimmomaticToolRunTask);
    addSubTask(trimmomaticToolRunTask);
}

QStringList TrimmomaticTask::getArguments() {
    QStringList arguments;

    if (!settings.pairedReadsInput) {
        arguments << "SE";
    } else {
        arguments << "PE";
    }

    arguments << "-threads" << QString::number(settings.numberOfThreads);

    if (settings.generateLog) {
        arguments << "-trimlog" << settings.logUrl;
        GUrlUtils::prepareFileLocation(settings.logUrl, stateInfo);
    }

    if (!settings.pairedReadsInput) {
        arguments << settings.inputUrl1;
        arguments << settings.seOutputUrl;
        GUrlUtils::prepareFileLocation(settings.seOutputUrl, stateInfo);
    } else {
        arguments << settings.inputUrl1;
        arguments << settings.inputUrl2;
        arguments << settings.pairedOutputUrl1;
        arguments << settings.unpairedOutputUrl1;
        arguments << settings.pairedOutputUrl2;
        arguments << settings.unpairedOutputUrl2;
        GUrlUtils::prepareFileLocation(settings.pairedOutputUrl1, stateInfo);
        GUrlUtils::prepareFileLocation(settings.pairedOutputUrl2, stateInfo);
        GUrlUtils::prepareFileLocation(settings.unpairedOutputUrl1, stateInfo);
        GUrlUtils::prepareFileLocation(settings.unpairedOutputUrl2, stateInfo);
    }

    foreach (QString step, settings.trimmingSteps) {
        step.remove('\'');
        arguments << step;
    }

    return arguments;
}

const QString &TrimmomaticTask::getInputUrl1() const {
    return settings.inputUrl1;
}

const QString &TrimmomaticTask::getSeOutputUrl() const {
    return settings.seOutputUrl;
}

const QString &TrimmomaticTask::getPairedOutputUrl1() const {
    return settings.pairedOutputUrl1;
}

const QString &TrimmomaticTask::getPairedOutputUrl2() const {
    return settings.pairedOutputUrl2;
}

const QString &TrimmomaticTask::getUnpairedOutputUrl1() const {
    return settings.unpairedOutputUrl1;
}

const QString &TrimmomaticTask::getUnpairedOutputUrl2() const {
    return settings.unpairedOutputUrl2;
}

const QString &TrimmomaticTask::getLogUrl() const {
    return settings.logUrl;
}

} // namespace U2

