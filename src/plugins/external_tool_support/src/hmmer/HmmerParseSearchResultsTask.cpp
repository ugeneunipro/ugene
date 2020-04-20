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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>

#include "HmmerParseSearchResultsTask.h"

namespace U2 {

const qint64 HmmerParseSearchResultsTask::BUFF_SIZE = 4096;

HmmerParseSearchResultsTask::HmmerParseSearchResultsTask(const QString &resultUrl, const AnnotationCreationPattern &pattern)
    : Task("Parse HMMER search result", TaskFlag_None),
      resultUrl(resultUrl),
      pattern(pattern)
{
    SAFE_POINT_EXT(!resultUrl.isEmpty(), setError("Result file URL is empty"), );
}

const QList<SharedAnnotationData> & HmmerParseSearchResultsTask::getAnnotations() const {
    return annotations;
}

void HmmerParseSearchResultsTask::run() {
    QScopedPointer<IOAdapter> ioAdapter(IOAdapterUtils::open(resultUrl, stateInfo));
    CHECK_OP(stateInfo, );

    QByteArray buff;
    buff.reserve(BUFF_SIZE + 1);
    qint64 lineNumber = 1;

    while (!ioAdapter->isEof()) {
        qint64 bytesRead = ioAdapter->readLine(buff.data(), BUFF_SIZE);
        assert(bytesRead < BUFF_SIZE);
        Q_UNUSED(bytesRead);

        QString readData(buff.data());

        if (isComment(readData)) {
            lineNumber++;
            continue;
        }

        processHit(readData.split(QRegExp("\\s+"), QString::SkipEmptyParts), lineNumber);
        CHECK_OP(stateInfo, );
        lineNumber++;
    }
}

bool HmmerParseSearchResultsTask::isComment(const QString &line) {
    return line.startsWith("#");
}

void HmmerParseSearchResultsTask::processHit(const QStringList &tokens, qint64 lineNumber) {
    CHECK_EXT(23 <= tokens.size(), stateInfo.addWarning(tr("Can't parse line %1").arg(lineNumber)), );
    SharedAnnotationData annotation(new AnnotationData);
    annotation->qualifiers << U2Qualifier("Accuracy_per_residue", tokens[ACC]);
    annotation->qualifiers << U2Qualifier("Bias", tokens[BIAS]);
    annotation->qualifiers << U2Qualifier("Conditional_e-value", tokens[C_EVALUE]);
    annotation->qualifiers << U2Qualifier("Env_of_domain_loc", tokens[ENV_FROM] + ".." + tokens[ENV_TO]);
    annotation->qualifiers << U2Qualifier("HMM_model", tokens[QUERY_NAME]);
    annotation->qualifiers << U2Qualifier("HMM_region", tokens[HMM_FROM] + ".." + tokens[HMM_TO]);
    annotation->qualifiers << U2Qualifier("Independent_e-value", tokens[I_EVALUE]);
    annotation->qualifiers << U2Qualifier("Score", tokens[SCORE]);
    U1AnnotationUtils::addDescriptionQualifier(annotation, pattern.description);

    qint64 start = tokens[ALI_FROM].toLongLong();
    qint64 end = tokens[ALI_TO].toLongLong();
    annotation->location->regions << U2Region(start, end - start + 1);
    annotation->name = pattern.annotationName;
    annotation->type = pattern.type;

    annotations << annotation;
}

}   // namespace U2
