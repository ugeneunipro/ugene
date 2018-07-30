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

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>

#include "KrakenSupport.h"
#include "KrakenClassifyLogParser.h"
#include "KrakenClassifyTask.h"
#include "KrakenTranslateLogParser.h"

namespace U2 {

const QString KrakenClassifyTaskSettings::SINGLE_END = "single-end";
const QString KrakenClassifyTaskSettings::PAIRED_END = "paired-end";

KrakenClassifyTaskSettings::KrakenClassifyTaskSettings()
    : quickOperation(false),
      minNumberOfHits(1),
      numberOfThreads(1),
      preloadDatabase(true),
      pairedReads(false)
{

}

KrakenClassifyTask::KrakenClassifyTask(const KrakenClassifyTaskSettings &settings)
    : ExternalToolSupportTask(tr("Classify reads with Kraken"), TaskFlags_NR_FOSE_COSC),
      settings(settings),
      classifyTask(NULL)
{
    GCOUNTER(cvar, tvar, "KrakenClassifyTask");

    SAFE_POINT_EXT(!settings.readsUrl.isEmpty(), setError("Reads URL is empty"), );
    SAFE_POINT_EXT(!settings.pairedReads || !settings.readsUrl.isEmpty(), setError("Paired reads URL is empty, but the 'paired reads' option is set"), );
    SAFE_POINT_EXT(!settings.databaseUrl.isEmpty(), setError("Kraken database URL is empty"), );
    SAFE_POINT_EXT(!settings.classificationUrl.isEmpty(), setError("Kraken classification URL is empty"), );
}

const QString &KrakenClassifyTask::getClassificationUrl() const {
    return settings.classificationUrl;
}

void KrakenClassifyTask::prepare() {
    classifyTask = new ExternalToolRunTask(KrakenSupport::CLASSIFY_TOOL, getArguments(), new KrakenClassifyLogParser());
    setListenerForTask(classifyTask);
    addSubTask(classifyTask);
}

QStringList KrakenClassifyTask::getArguments() {
    QStringList arguments;
    arguments << "--db" << settings.databaseUrl;
    arguments << "--threads" << QString::number(settings.numberOfThreads);

    if (settings.quickOperation) {
        arguments << "--quick";
        arguments << "--min-hits" << QString::number(settings.minNumberOfHits);
    }

    arguments << "--output" << settings.classificationUrl;
    if (settings.preloadDatabase) {
        arguments << "--preload";
    }

    if (settings.pairedReads) {
        arguments << "--paired";
        arguments << "--check-names";
    }

    arguments << settings.readsUrl;
    if (settings.pairedReads) {
        arguments << settings.pairedReadsUrl;
    }

    return arguments;
}

}   // namespace U2
