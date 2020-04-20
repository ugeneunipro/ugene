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

#include "ORFMarkerTests.h"

#include <U2Core/DNASequence.h>

#include <U2Core/DocumentModel.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/GObjectTypes.h>
#include <U2Core/DNASequenceObject.h>

/* TRANSLATOR U2::GTest */

namespace U2 {

#define SEQ_ATTR "seq"
#define STRAND_ATTR "strand"
#define MIN_LENGTH_ATTR "min_length"
#define INCLUDE_STOP_CODON_ATTR "include_stop_codon"
#define TERMINATES_ATTR "terminates_with_region"
#define START_WITH_INIT_ATTR "starts_with_init_codon"
#define ALT_INIT_ATTR "allow_alt_init_codons"
#define TRANSLATION_ID_ATTR "translation_id"
#define EXPECTED_RESULTS_ATTR  "expected_results"

Translator::Translator(const U2SequenceObject *s, const QString& tid) : seq(s), complTransl(NULL), aminoTransl(NULL) {
    const DNAAlphabet* al = seq->getAlphabet();
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    aminoTransl = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO, ("NCBI-GenBank #" + tid));
    assert(aminoTransl);
    DNATranslation* complT = tr->lookupComplementTranslation(al);
    if (complT != NULL) {
        complTransl = complT ;
    }
}

void GTest_ORFMarkerTask::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    seqName = el.attribute(SEQ_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQ_ATTR);
        return;
    }

    QString expected = el.attribute(EXPECTED_RESULTS_ATTR);
    if (!expected.isEmpty()) {
        QStringList expectedList = expected.split(QRegExp("\\,")); //may be QRegExp("\\,")
        foreach(QString region, expectedList) {
            QStringList bounds = region.split(QRegExp("\\.."));
            if (bounds.size() != 2) {
                stateInfo.setError(  QString("wrong value for %1").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            bool startOk, finishOk;
            int start = bounds.first().toInt(&startOk), finish = bounds.last().toInt(&finishOk);
            if (startOk && finishOk != true) {
                stateInfo.setError(  QString("wrong value for %1").arg(EXPECTED_RESULTS_ATTR) );
                return;
            }
            start--;
            expectedResults.append(U2Region(start, finish - start));
        }
    }

    QString strand = el.attribute(STRAND_ATTR);
    if(strand == "direct"){
        settings.strand = ORFAlgorithmStrand_Direct;
    }else if(strand == "compliment"){
        settings.strand = ORFAlgorithmStrand_Complement;
    }else if(strand == "both"){
        settings.strand = ORFAlgorithmStrand_Both;
    }else{
        stateInfo.setError(  QString("value not correct %1").arg(STRAND_ATTR) );
        return;
    }

    QString strLength = el.attribute(MIN_LENGTH_ATTR);
    if (strLength.isEmpty()) {
        failMissingValue(MIN_LENGTH_ATTR);
        return;
    }
    bool isOk;
    int length = strLength.toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(  QString("Unable to convert. Value wrong %1").arg(MIN_LENGTH_ATTR) );
        return;
    }
    settings.minLen = length;

    QString strTerminates = el.attribute(TERMINATES_ATTR);
    if (strTerminates.isEmpty()) {
        failMissingValue(TERMINATES_ATTR);
        return;
    }
    if (strTerminates == "true") {
        settings.mustFit = true;
    } else if (strTerminates == "false"){
        settings.mustFit = false;
    } else {
        stateInfo.setError(  QString("Unable to convert. Value wrong %1").arg(TERMINATES_ATTR) );
        return;
    }

    QString strStartWithCodon = el.attribute(START_WITH_INIT_ATTR);
    if (strStartWithCodon.isEmpty()) {
        failMissingValue(START_WITH_INIT_ATTR);
        return;
    }
    if (strStartWithCodon == "true") {
        settings.mustInit = true;
    } else if (strStartWithCodon == "false"){
        settings.mustInit = false;
    } else {
        stateInfo.setError(  QString("Unable to convert. Value wrong %1").arg(START_WITH_INIT_ATTR) );
        return;
    }

    QString strIncStopCodon = el.attribute(INCLUDE_STOP_CODON_ATTR);
    if (strIncStopCodon == "true") {
        settings.includeStopCodon = true;
    } else {
        settings.includeStopCodon = false;
    }

    QString strAltStart = el.attribute(ALT_INIT_ATTR);
    if (strAltStart.isEmpty()) {
        failMissingValue(ALT_INIT_ATTR);
        return;
    }
    if (strAltStart == "true") {
        settings.allowAltStart = true;
    } else if (strAltStart == "false"){
        settings.allowAltStart = false;
    } else {
        stateInfo.setError(  QString("Unable to convert. Value wrong %1").arg(ALT_INIT_ATTR) );
        return;
    }

    settings.maxResult2Search = INT_MAX;

    QString strTranslationId = el.attribute(TRANSLATION_ID_ATTR);
    if (strTranslationId.isEmpty()) {
        failMissingValue(TRANSLATION_ID_ATTR);
        return;
    }
    translationId = strTranslationId.toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(  QString("Unable to convert. Value wrong %1").arg(TRANSLATION_ID_ATTR) );
        return;
    }
    translationId = strTranslationId;
}

void GTest_ORFMarkerTask::prepare() {
    U2SequenceObject * mySequence = getContext<U2SequenceObject>(this, seqName);
    CHECK_EXT(mySequence != NULL, setError("Can't cast to sequence from GObject"), );

    Translator tr(mySequence, translationId);
    settings.complementTT = tr.getComplTranslation();
    settings.proteinTT = tr.getAminoTranslation();
    settings.searchRegion = U2Region(0, mySequence->getSequenceLength());
    settings.circularSearch = mySequence->isCircular();
    task = new ORFFindTask(settings, mySequence->getSequenceRef());
    addSubTask(task);
}

Task::ReportResult GTest_ORFMarkerTask::report() {
    QVector<U2Region> actualResults;
    foreach(ORFFindResult i, task->popResults()){
        actualResults.append(i.region);
        if(i.isJoined){
            actualResults.append(i.joinedRegion);
        }
    }
    int actualSize = actualResults.size(), expectedSize = expectedResults.size();
    if (actualSize != expectedSize) {
        stateInfo.setError(  QString("Expected and Actual lists of regions are different: %1 %2").arg(expectedSize).arg(actualSize) );
        return ReportResult_Finished;
    }
    qSort(actualResults); qSort(expectedResults);
    if (actualResults != expectedResults) {
        stateInfo.setError(  QString("One of the expected regions not found in results") );
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}

} //namespace

