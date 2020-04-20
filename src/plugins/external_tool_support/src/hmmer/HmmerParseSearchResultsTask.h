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

#ifndef _U2_HMMER_PARSE_SEARCH_RESULTS_TASK_H_
#define _U2_HMMER_PARSE_SEARCH_RESULTS_TASK_H_

#include <U2Core/Annotation.h>
#include <U2Core/Task.h>

#include "HmmerSearchTask.h"

namespace U2 {

class HmmerParseSearchResultsTask : public Task {
    Q_OBJECT
public:
    HmmerParseSearchResultsTask(const QString &resultUrl, const AnnotationCreationPattern &pattern);

    const QList<SharedAnnotationData> & getAnnotations() const;

private:
    enum TOKENS {
        TARGET_NAME = 0,
        TARGET_ACCESSION = 1,
        TLEN = 2,
        QUERY_NAME = 3,
        QUERY_ACCESSION = 4,
        QLEN = 5,
        FULL_SEQ_E_VALUE = 6,
        FULL_SEQ_SCORE = 7,
        FULL_SEQ_BIAS = 8,
        NUMBER = 9,
        TOTAL_COUNT = 10,
        C_EVALUE = 11,
        I_EVALUE = 12,
        SCORE = 13,
        BIAS = 14,
        HMM_FROM = 15,
        HMM_TO = 16,
        ALI_FROM = 17,
        ALI_TO = 18,
        ENV_FROM = 19,
        ENV_TO = 20,
        ACC = 21,
        DESCRIPTION = 22
    };

    void run();

    static bool isComment(const QString &line);
    void processHit(const QStringList &tokens, qint64 lineNumber);

    const QString resultUrl;
    const AnnotationCreationPattern pattern;
    QList<SharedAnnotationData> annotations;

    static const qint64 BUFF_SIZE;
};

}   // namespace U2

#endif // _U2_HMMER_PARSE_SEARCH_RESULTS_TASK_H_
