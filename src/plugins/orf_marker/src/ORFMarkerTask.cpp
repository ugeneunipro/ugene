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
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/Settings.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ORFMarkerTask.h"

namespace U2 {

const QString ORFSettingsKeys::STRAND("orf_finder/strand");
const QString ORFSettingsKeys::ALLOW_ALT_START("orf_finder/allow_alt_start");
const QString ORFSettingsKeys::ALLOW_OVERLAP("orf_finder/allow_overlap");
const QString ORFSettingsKeys::AMINO_TRANSL("orf_finder/amino_transl");
const QString ORFSettingsKeys::MIN_LEN("orf_finder/min_length");
const QString ORFSettingsKeys::MUST_FIT("orf_finder/must_fit");
const QString ORFSettingsKeys::MUST_INIT("orf_finder/must_init");
const QString ORFSettingsKeys::SEARCH_REGION("orf_finder/region");
const QString ORFSettingsKeys::INCLUDE_STOP_CODON("orf_finder/incldue_stop_codon");
const QString ORFSettingsKeys::MAX_RESULT("orf_finder/max_result");
const QString ORFSettingsKeys::IS_RESULT_LIMITED("orf_finder/is_result_limited");

void ORFSettingsKeys::save(const ORFAlgorithmSettings& cfg, Settings* s) {
    s->setValue(ORFSettingsKeys::AMINO_TRANSL, cfg.proteinTT->getTranslationId());
    s->setValue(ORFSettingsKeys::MUST_FIT, cfg.mustFit);
    s->setValue(ORFSettingsKeys::MUST_INIT, cfg.mustInit);
    s->setValue(ORFSettingsKeys::ALLOW_ALT_START, cfg.allowAltStart);
    s->setValue(ORFSettingsKeys::ALLOW_OVERLAP, cfg.allowOverlap);
    s->setValue(ORFSettingsKeys::MIN_LEN, cfg.minLen);
    s->setValue(ORFSettingsKeys::SEARCH_REGION, QVariant::fromValue(cfg.searchRegion));
    s->setValue(ORFSettingsKeys::STRAND, ORFAlgorithmSettings::getStrandStringId(cfg.strand));
    s->setValue(ORFSettingsKeys::INCLUDE_STOP_CODON, cfg.includeStopCodon);
    s->setValue(ORFSettingsKeys::MAX_RESULT,cfg.maxResult2Search);
    s->setValue(ORFSettingsKeys::IS_RESULT_LIMITED,cfg.isResultsLimited);
}

void ORFSettingsKeys::read(ORFAlgorithmSettings& cfg, const Settings* s) {
    cfg.mustFit = s->getValue(ORFSettingsKeys::MUST_FIT, false).toBool();
    cfg.mustInit = s->getValue(ORFSettingsKeys::MUST_INIT, true).toBool();
    cfg.allowAltStart = s->getValue(ORFSettingsKeys::ALLOW_ALT_START, false).toBool();
    cfg.allowOverlap = s->getValue(ORFSettingsKeys::ALLOW_OVERLAP, false).toBool();
    cfg.minLen = s->getValue(ORFSettingsKeys::MIN_LEN, 100).toInt();
    cfg.maxResult2Search =  s->getValue(ORFSettingsKeys::MAX_RESULT,200000).toInt();
    cfg.isResultsLimited = s->getValue(ORFSettingsKeys::IS_RESULT_LIMITED,true).toBool();

    QString strandId = s->getValue(ORFSettingsKeys::STRAND, ORFAlgorithmSettings::STRAND_BOTH).toString();
    cfg.strand = ORFAlgorithmSettings::getStrandByStringId(strandId);

    QString translId = s->getValue(ORFSettingsKeys::AMINO_TRANSL, "").toString();
    if (!translId.isEmpty()) {
        cfg.proteinTT = AppContext::getDNATranslationRegistry()->lookupTranslation(translId);
    }

    cfg.includeStopCodon = s->getValue(ORFSettingsKeys::INCLUDE_STOP_CODON, false).toBool();
    cfg.searchRegion = s->getValue(ORFSettingsKeys::SEARCH_REGION).value<U2Region>();
}


//////////////////////////////////////////////////////////////////////////
// find ORFS and save 2 annotations task

FindORFsToAnnotationsTask::FindORFsToAnnotationsTask( AnnotationTableObject* aobj,const U2EntityRef& _entityRef,
                                                     const ORFAlgorithmSettings& settings, const QString& gName,
                                                      const QString &annDescription)
    : Task(tr("Find ORFs and save to annotations"), TaskFlags_NR_FOSCOE),
      aObj(aobj),
      cfg(settings),
      groupName(gName),
      annDescription(annDescription),
      entityRef(_entityRef)
{
    SAFE_POINT_EXT( aobj != NULL, setError(tr("Annotation table object is NULL!")), );
    if (groupName.isEmpty()) {
        groupName = ORFAlgorithmSettings::ANNOTATION_GROUP_NAME;
    }

    fTask = new ORFFindTask(cfg, entityRef);
    addSubTask(fTask);
}

QList<Task *> FindORFsToAnnotationsTask::onSubTaskFinished(Task *subTask) {
    CHECK(subTask == fTask, QList<Task *>());

    const QList<ORFFindResult> results = fTask->popResults();
    QList<SharedAnnotationData> annotationList;
    foreach (const ORFFindResult &res, results) {
        CHECK_OP(stateInfo, QList<Task *>());
        annotationList << res.toAnnotation(ORFAlgorithmSettings::ANNOTATION_GROUP_NAME);
    }

    U1AnnotationUtils::addDescriptionQualifier(annotationList, annDescription);

    return QList<Task *>() << new CreateAnnotationsTask(aObj, annotationList, groupName);
}

//////////////////////////////////////////////////////////////////////////
// ORFs auto annotation updater

ORFAutoAnnotationsUpdater::ORFAutoAnnotationsUpdater()
: AutoAnnotationsUpdater(tr("ORFs"), ORFAlgorithmSettings::ANNOTATION_GROUP_NAME, false, true)
{

}

Task* ORFAutoAnnotationsUpdater::createAutoAnnotationsUpdateTask( const AutoAnnotationObject* aa )
{
    AnnotationTableObject *aObj = aa->getAnnotationObject();
    U2SequenceObject* dnaObj = aa->getSeqObject();

    ORFAlgorithmSettings cfg;
    ORFSettingsKeys::read(cfg, AppContext::getSettings());
    cfg.circularSearch = dnaObj->isCircular();

    cfg.complementTT = GObjectUtils::findComplementTT(dnaObj->getAlphabet());
    cfg.proteinTT = aa->getAminoTT();

    const U2Region wholeSequenceRegion = U2Region(0, dnaObj->getSequenceLength());
    cfg.searchRegion = cfg.searchRegion.intersect(wholeSequenceRegion);
    if (cfg.searchRegion.isEmpty()) {
        cfg.searchRegion = wholeSequenceRegion;
    }

    Task* task = new FindORFsToAnnotationsTask(aObj, dnaObj->getSequenceRef(), cfg );

    return task;
}

bool ORFAutoAnnotationsUpdater::checkConstraints( const AutoAnnotationConstraints& constraints ) {
    if (constraints.alphabet == NULL) {
        return false;
    }
    return constraints.alphabet->isNucleic();
}

} // U2 namespace
