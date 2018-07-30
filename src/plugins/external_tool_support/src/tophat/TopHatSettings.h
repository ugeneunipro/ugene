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

#ifndef _U2_TOPHAT_SETTINGS_H_
#define _U2_TOPHAT_SETTINGS_H_

#include "../RnaSeqCommon.h"

#include <U2Core/DNASequenceObject.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>
#include <U2Lang/WorkflowContext.h>


namespace U2 {

class TopHatInputData {
public:
    TopHatInputData();

    int size() const;

    bool paired;
    bool fromFiles;
    QStringList urls;
    QStringList pairedUrls;

    QList<Workflow::SharedDbiDataHandler>   seqIds;
    QList<Workflow::SharedDbiDataHandler>   pairedSeqIds;

    Workflow::WorkflowContext*              workflowContext;

public:
    void cleanupReads();
};

class TopHatSettings
{
public:
    TopHatSettings();

    // Workflow element parameters
    QString             bowtieIndexPathAndBasename;
    int                 mateInnerDistance;
    int                 mateStandardDeviation;
    RnaSeqLibraryType   libraryType;
    bool                noNovelJunctions;
    QString             rawJunctions;
    QString             knownTranscript;
    int                 maxMultihits;
    int                 segmentLength;
    bool                fusionSearch;
    bool                transcriptomeOnly;
    int                 transcriptomeMaxHits;
    bool                prefilterMultihits;
    int                 minAnchorLength;
    int                 spliceMismatches;
    int                 readMismatches;
    int                 segmentMismatches;
    bool                solexa13quals;
    BowtieMode          bowtieMode;
    bool                useBowtie1;
    QString             bowtiePath;
    QString             samtoolsPath;
    QString             sample;
    /** Working folder for the TopHat tool */
    QString             outDir;

    TopHatInputData     data;

    QString             referenceInputType;
    QString             referenceGenome;
    QString             buildIndexPathAndBasename;

public:
    void cleanupReads();
    Workflow::WorkflowContext * workflowContext() const;
    Workflow::DbiDataStorage * storage() const;

    static uint getThreadsCount();

    static const QString INDEX;
    static const QString SEQUENCE;
};
}

#endif
