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

#ifndef _U2_STRINGTIE_TASK_H_
#define _U2_STRINGTIE_TASK_H_

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrl.h>

namespace U2 {

class StringTieTaskSettings {
public:
    StringTieTaskSettings();

    QString inputBam;

    QString referenceAnnotations;   // 1
    QString readOrientation;        // 2 --fr or --rf values, enum?
    QString label;                  // 3
    double  minIsoformFraction;     // 4
    int     minTransciptLen;        // 5

    int     minAnchorLen;           // 6
    double  minJunctionCoverage;    // 7
    bool    trimTranscript;         // 8
    double  minCoverage;            // 9
    int     minLocusSeparation;     // 10

    double  multiHitFraction;       // 11
    QString skipSequences;          // 12 - a list of sequence names comma separated
    bool    refOnlyAbudance;        // 13
    bool    multiMappingCorrection; // 14
    bool    verboseLog;             // 15

    int threadNum;                  // 16

    QString primaryOutputFile;      // 17
    bool    geneAbundanceOutput;    // 18
    QString geneAbundanceOutputFile;// 19
    bool    coveredRefOutput;       // 20
    QString coveredRefOutputFile;   // 21
    bool    ballgownOutput;         // 22
    QString ballgowmOutputFolder;   // 23
};

class StringTieTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(StringTieTask)
public:
    StringTieTask(const StringTieTaskSettings& settings);

    void prepare();
    const StringTieTaskSettings& getSettings() const;

private:
    QStringList getArguments() const;

private:
    ExternalToolRunTask* stringTieTask;
    StringTieTaskSettings settings;
};

} // namespace
#endif // _U2_STRINGTIE_TASK_H_
