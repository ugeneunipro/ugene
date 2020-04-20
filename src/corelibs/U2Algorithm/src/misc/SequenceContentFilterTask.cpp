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

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>

#include "SequenceContentFilterTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// SequenceConentFilterTask
//////////////////////////////////////////////////////////////////////////

const qint64 SequenceContentFilterTask::SEQUENCE_CHUNK_SIZE = 4194304; // 4 MB

SequenceContentFilterTask::SequenceContentFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs)
    : AbstractProjectFilterTask(settings, tr("Sequence content"), docs), searchStopFlag(0)
{
    filteredObjCountPerIteration = 1;
}

bool SequenceContentFilterTask::filterAcceptsObject(GObject *obj) {
    U2SequenceObject *seqObject = qobject_cast<U2SequenceObject *>(obj);
    CHECK(NULL != seqObject, false);

    FindAlgorithmSettings findSettings;
    SAFE_POINT(initFindAlgorithmSettings(seqObject, findSettings), "Unable to prepare search algorithm", false);

    foreach (const QString &pattern, settings.tokensToShow) {
        if (patternFitsSequenceAlphabet(seqObject, pattern) && sequenceContainsPattern(seqObject, pattern, findSettings)) {
            return true;
        }
    }
    return false;
}

bool SequenceContentFilterTask::patternFitsSequenceAlphabet(U2SequenceObject *seqObject, const QString &pattern) {
    SAFE_POINT(NULL != seqObject, L10N::nullPointerError("Sequence object"), false);
    SAFE_POINT(!pattern.isEmpty(), "Empty pattern to search", false);

    const DNAAlphabet *alphabet = seqObject->getAlphabet();
    SAFE_POINT(NULL != alphabet, L10N::nullPointerError("Sequence alphabet"), false);

    const QByteArray searchStr = pattern.toUpper().toLatin1();
    return alphabet->containsAll(searchStr.constData(), searchStr.length());
}

bool SequenceContentFilterTask::sequenceContainsPattern(U2SequenceObject *seqObject, const QString &pattern,
    const FindAlgorithmSettings &findSettings)
{
    SAFE_POINT(NULL != seqObject, L10N::nullPointerError("Sequence object"), false);
    SAFE_POINT(!pattern.isEmpty(), "Empty pattern to search", false);

    const qint64 patternLength = pattern.length();

    searchStopFlag = 0;
    const qint64 seqLength = seqObject->getSequenceLength();

    for (qint64 pos = 0; pos < seqLength && !stateInfo.isCoR(); pos += SEQUENCE_CHUNK_SIZE) {
        const qint64 currentChunkSize = qMin(SEQUENCE_CHUNK_SIZE, seqLength - pos);
        const U2Region chunkRegion(pos, currentChunkSize);

        searchThroughRegion(seqObject, chunkRegion, pattern, findSettings);

        U2Region overlap;
        const qint64 chunkEndPos = chunkRegion.endPos();
        if (0 == searchStopFlag && chunkEndPos < seqLength) {
            const int overlapLength = qMin(patternLength * 2 - 2, seqLength - chunkEndPos);
            overlap = U2Region(chunkEndPos - patternLength + 1, overlapLength);
        } else if (0 == searchStopFlag && seqObject->isCircular()) {
            overlap = U2Region(seqLength - patternLength + 1, patternLength * 2 - 2);
        } else {
            continue; // we have a non-circular sequence and by this moment it has been searched completely
        }
        if (!overlap.isEmpty()) { // `overlap` can be empty, it's a degenerative case
            searchThroughRegion(seqObject, overlap, pattern, findSettings);
        }
    }

    return 0 != searchStopFlag;
}

void SequenceContentFilterTask::searchThroughRegion(U2SequenceObject *seqObject, const U2Region &searchRegion, const QString &pattern,
    const FindAlgorithmSettings &findSettings)
{
    SAFE_POINT(NULL != seqObject, L10N::nullPointerError("Sequence object"), );
    int searchProgressStub = 0; // it's unused in fact

    QByteArray regionContent;
    const qint64 seqLength = seqObject->getSequenceLength();
    if (seqObject->isCircular() && searchRegion.endPos() > seqLength) {
        regionContent = seqObject->getSequenceData(U2Region(searchRegion.startPos, seqLength - searchRegion.startPos), stateInfo);
        CHECK_OP(stateInfo, );
        regionContent += seqObject->getSequenceData(U2Region(0, searchRegion.endPos() - seqLength), stateInfo);
    } else {
        SAFE_POINT(searchRegion.endPos() <= seqLength, "Invalid sequence region specified", );
        regionContent = seqObject->getSequenceData(searchRegion, stateInfo);
    }
    CHECK_OP(stateInfo, );
    const char *rawRegionContent = regionContent.constData();

    FindAlgorithmSettings specialFindSettings(findSettings);
    specialFindSettings.searchRegion = U2Region(0, searchRegion.length);
    specialFindSettings.pattern = pattern.toUpper().toLatin1();

    FindAlgorithm::find(this, specialFindSettings, rawRegionContent, searchRegion.length, false, searchStopFlag, searchProgressStub);
}

bool SequenceContentFilterTask::initFindAlgorithmSettings(U2SequenceObject *seqObject, FindAlgorithmSettings &findSettings) {
    SAFE_POINT(NULL != seqObject, L10N::nullPointerError("Sequence object"), false);

    const DNAAlphabet *alphabet = seqObject->getAlphabet();
    SAFE_POINT(NULL != alphabet, L10N::nullPointerError("Sequence alphabet"), false);

    if (alphabet->isNucleic()) {
        DNATranslation *complTranslation = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(alphabet);
        SAFE_POINT(NULL != complTranslation, L10N::nullPointerError("Sequence translation"), false);

        findSettings.complementTT = complTranslation;
        findSettings.strand = FindAlgorithmStrand_Both;
    } else {
        findSettings.complementTT = NULL;
        findSettings.strand = FindAlgorithmStrand_Direct;
    }
    findSettings.patternSettings = FindAlgorithmPatternSettings_Exact;

    return true;
}

void SequenceContentFilterTask::onResult(const FindAlgorithmResult & /*r*/) {
    searchStopFlag = 1;
}

//////////////////////////////////////////////////////////////////////////
/// SequenceConentFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask * SequenceContentFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    return new SequenceContentFilterTask(settings, docs);
}

} // namespace U2
