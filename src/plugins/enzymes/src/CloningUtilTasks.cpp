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

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/Version.h>

#include <U2Gui/OpenViewTask.h>

#include "CloningUtilTasks.h"
#include "FindEnzymesTask.h"

namespace U2 {


bool operator<(const GenomicPosition& left, const GenomicPosition& right) {
   return left.coord < right.coord;
}


DigestSequenceTask::DigestSequenceTask(U2SequenceObject* so, AnnotationTableObject *source,
                                       AnnotationTableObject *dest, const DigestSequenceTaskConfig& config)
                                       :   Task("DigestSequenceTask", TaskFlags_FOSCOE | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
                                        sourceObj(source), destObj(dest), dnaObj(so), cfg(config)
{
    GCOUNTER(cvar,tvar,"DigestSequenceIntoFragments");

    SAFE_POINT_EXT(sourceObj != NULL, setError(L10N::nullPointerError("source object")), );
    SAFE_POINT_EXT(destObj != NULL, setError(L10N::nullPointerError("destination object")), );
    SAFE_POINT_EXT(dnaObj != NULL, setError(L10N::nullPointerError("sequence object")), );
    isCircular = cfg.forceCircular;
}

void DigestSequenceTask::prepare() {
    seqRange = U2Region(0, dnaObj->getSequenceLength());

    if (cfg.searchForRestrictionSites) {
        assert(sourceObj == destObj);
        FindEnzymesTaskConfig feCfg;
        feCfg.circular = isCircular;
        feCfg.groupName = ANNOTATION_GROUP_ENZYME;
        Task* t = new FindEnzymesToAnnotationsTask(sourceObj, dnaObj->getSequenceRef(), cfg.enzymeData, feCfg);
        addSubTask(t);
    }
}

SharedAnnotationData DigestSequenceTask::createFragment(int pos1, const DNAFragmentTerm& leftTerm, int pos2, const DNAFragmentTerm& rightTerm) {
    SharedAnnotationData ad(new AnnotationData);
    if (pos1 < pos2) {
        U2Region reg(pos1, pos2 - pos1);
        assert(reg.length > 0);

        ad->location->regions.append(reg);
    } else {
        U2Region reg1(pos1, seqRange.endPos() - pos1);
        if (pos2 < 0) {
            reg1.length += pos2;
            pos2 = 0;
        }
        U2Region reg2(seqRange.startPos, pos2 - seqRange.startPos);
        assert(reg1.length >= 0 && reg2.length >= 0);
        assert(!reg1.isEmpty() || !reg2.isEmpty());

        if (!reg1.isEmpty()){
            ad->location->regions.append(reg1);
        }
        if (!reg2.isEmpty()) {
            ad->location->regions.append(reg2);
        }
    }

    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_TERM, leftTerm.enzymeId));
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_TERM, rightTerm.enzymeId));

    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_OVERHANG, leftTerm.overhang));
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_OVERHANG, rightTerm.overhang));

    QString leftOverhangStrand = leftTerm.isDirect ? OVERHANG_STRAND_DIRECT : OVERHANG_STRAND_COMPL;
    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_STRAND, leftOverhangStrand));
    QString rightOverhangStrand = rightTerm.isDirect ? OVERHANG_STRAND_DIRECT : OVERHANG_STRAND_COMPL;
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_STRAND, rightOverhangStrand));

    QString leftOverhangType = leftTerm.enzymeId.isEmpty() || leftTerm.overhang.isEmpty() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_TYPE, leftOverhangType));
    QString rightOverhangType = rightTerm.enzymeId.isEmpty() || rightTerm.overhang.isEmpty() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_TYPE, rightOverhangType));

    ad->qualifiers.append(U2Qualifier(QUALIFIER_SOURCE, dnaObj->getGObjectName()));

    U1AnnotationUtils::addDescriptionQualifier(ad, cfg.annDescription);

    return ad;
}

Task::ReportResult DigestSequenceTask::report()
{
    checkForConservedAnnotations();

    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    saveResults();

    return ReportResult_Finished;
}

void DigestSequenceTask::findCutSites()
{
    foreach (const SEnzymeData &enzyme, cfg.enzymeData) {
        if (enzyme->cutDirect == ENZYME_CUT_UNKNOWN || enzyme->cutComplement == ENZYME_CUT_UNKNOWN) {
            setError(tr("Can't use restriction site %1 for digestion,  cleavage site is unknown ").arg(enzyme->id));
            return;
        }

        QList<Annotation *> anns;
        foreach (Annotation *a, sourceObj->getAnnotations()) {
            if (a->getName() == enzyme->id) {
                anns.append(a);
            }
        }

        if (anns.isEmpty()) {
            stateInfo.setError( QString("Restriction site %1 is not found").arg(enzyme->id));
            continue;
        }

        foreach (Annotation *a, anns) {
            const QVector<U2Region> &location = a->getRegions();
            int cutPos = location.first().startPos;
            cutSiteMap.insertMulti(GenomicPosition(cutPos, a->getStrand().isDirect()), enzyme);
        }

    }
}

// TODO: add this function
/*
static void prepareLeftDnaTerm(TermData& leftTerm,
                               const GenomicPosition& enzymePos,
                               const EnzymeData& leftCutter)
{
    int lcLen = leftCutter->seq.length();
    bool lcStrandDirect = enzymePos.directStrand;
    int lcDirectStrandCut = lcStrandDirect ? left->cutDirect : lcLen - leftCutter->cutDirect;
    int lcComplementStrandCut = lcStrandDirect ? lcLen - leftCutter->cutComplement : leftCutter->cutComplement;
    leftTerm.cutPos = enzymePos.coord + qMax(lcDirectStrandCut, lcComplementStrandCut);
    leftTerm.overhangStart = enzymePos.coord + qMin(lcDirectStrandCut, lcComplementStrandCut);
    leftTerm.isDirect = lcStrandDirect ? lcDirectStrandCut < lcComplementStrandCut :
        lcDirectStrandCut > lcComplementStrandCut;

}
*/

bool compareAnnotationsbyLength(const SharedAnnotationData &a1, const SharedAnnotationData& a2) {
    int length1 = 0;
    foreach(const U2Region &reg, a1->getRegions()) {
        length1 += reg.length;
    }

    int length2 = 0;
    foreach(const U2Region &reg, a2->getRegions()) {
        length2 += reg.length;
    }
    return length1 > length2;
}

void DigestSequenceTask::run()
{
    CHECK_OP(stateInfo,);

    findCutSites();

    CHECK(!cutSiteMap.isEmpty(),);

    QMap<GenomicPosition,SEnzymeData>::const_iterator prev = cutSiteMap.constBegin(), current = cutSiteMap.constBegin();
    int count = 2;
    qint64 seqLen = dnaObj->getSequenceLength();

    while ((++current) != cutSiteMap.constEnd())  {
        int pos1 = prev.key().coord;
        int pos2 = current.key().coord;
        const SEnzymeData& enzyme1 = prev.value();
        const SEnzymeData& enzyme2 = current.value();
        int len1= enzyme1->seq.length();
        int len2 = enzyme2->seq.length();

        {

            U2Region region1(pos1, len1);
            U2Region region2(pos2, len2);

            if (region1.intersects(region2)) {
                setError(tr("Unable to digest into fragments: intersecting restriction sites %1 (%2..%3) and %4 (%5..%6)")
                    .arg(enzyme1->id).arg(region1.startPos).arg(region1.endPos())
                    .arg(enzyme2->id).arg(region2.startPos).arg(region2.endPos()));
                return;
            }

        }

        DNAFragmentTerm leftTerm;
        bool leftStrandDirect = prev.key().directStrand;
        int leftCutDirect = leftStrandDirect ? enzyme1->cutDirect : len1 - enzyme1->cutDirect;
        int leftCutCompl = leftStrandDirect ? len1 - enzyme1->cutComplement : enzyme1->cutComplement;
        int leftCutPos = correctPos(pos1 + qMax(leftCutDirect, leftCutCompl));
        int leftOverhangStart = correctPos(pos1 + qMin(leftCutDirect, leftCutCompl));
        leftTerm.overhang = getOverhang(U2Region(leftOverhangStart, leftCutPos - leftOverhangStart));
        leftTerm.enzymeId = enzyme1->id.toLatin1();
        leftTerm.isDirect = leftStrandDirect ? leftCutDirect < leftCutCompl : leftCutDirect > leftCutCompl;

        DNAFragmentTerm rightTerm;
        bool rightStrandDirect = current.key().directStrand;
        int rightCutDirect = rightStrandDirect ? enzyme2->cutDirect : len2 - enzyme2->cutDirect;
        int rightCutCompl = rightStrandDirect ? len2 - enzyme2->cutComplement : enzyme2->cutComplement;
        int rightCutPos = correctPos(pos2 + qMin(rightCutDirect, rightCutCompl));
        int rightOverhangStart = correctPos(pos2 + qMax(rightCutDirect, rightCutCompl));
        rightTerm.overhang = getOverhang(U2Region(rightCutPos, rightOverhangStart - rightCutPos));
        rightTerm.enzymeId = enzyme2->id.toLatin1();
        rightTerm.isDirect = rightStrandDirect ? rightCutDirect > rightCutCompl : rightCutDirect < rightCutCompl;
        if (rightOverhangStart > seqLen) {
            int leftCutPos = rightOverhangStart - seqLen;
            rightTerm.overhang  += getOverhang(U2Region(0, leftCutPos));
        }
        SharedAnnotationData ad = createFragment(leftCutPos, leftTerm, rightCutPos, rightTerm);
        results.append(ad);
        ++count;
        ++prev;

    }

    QMap<GenomicPosition,SEnzymeData>::const_iterator first = cutSiteMap.constBegin();

    const SEnzymeData& firstCutter = first.value();
    int fcLen = firstCutter->seq.length();
    bool fcStrandDirect = first.key().directStrand;
    int fcDirectStrandCut = fcStrandDirect ? firstCutter->cutDirect : fcLen - firstCutter->cutDirect;
    int fcComplementStrandCut = fcStrandDirect ? fcLen - firstCutter->cutComplement : firstCutter->cutComplement;
    int firstCutPos = correctPos(first.key().coord + qMin(fcDirectStrandCut, fcComplementStrandCut));
    int rightOverhangStart = correctPos(first.key().coord + qMax(fcDirectStrandCut, fcComplementStrandCut));
    bool rightOverhangIsDirect = fcStrandDirect ? fcDirectStrandCut > fcComplementStrandCut :
        fcDirectStrandCut < fcComplementStrandCut;
    QByteArray firstRightOverhang = getOverhang(U2Region(firstCutPos, rightOverhangStart - firstCutPos));

    const SEnzymeData& lastCutter = prev.value();
    int lcLen = lastCutter->seq.length();
    bool lcStrandDirect = prev.key().directStrand;
    int lcDirectStrandCut = lcStrandDirect ? lastCutter->cutDirect : lcLen - lastCutter->cutDirect;
    int lcComplementStrandCut = lcStrandDirect ? lcLen - lastCutter->cutComplement : lastCutter->cutComplement;
    int lastCutPos = correctPos(prev.key().coord + qMax(lcDirectStrandCut, lcComplementStrandCut));
    int leftOverhangStart = correctPos(prev.key().coord + qMin(lcDirectStrandCut, lcComplementStrandCut));
    bool leftOverhangIsDirect = lcStrandDirect ? lcDirectStrandCut < lcComplementStrandCut :
        lcDirectStrandCut > lcComplementStrandCut;

    if (lastCutPos > seqLen) {
        // last restriction site is situated between sequence start and end
        assert(isCircular);
        int leftCutPos = lastCutPos - seqLen;
        QByteArray leftOverhang = getOverhang(U2Region(leftOverhangStart, seqLen - leftOverhangStart))
            + getOverhang(U2Region(0, leftCutPos));
        QByteArray rightOverhang = first == prev ? leftOverhang : firstRightOverhang;
        SharedAnnotationData ad1 = createFragment(leftCutPos, DNAFragmentTerm(lastCutter->id, leftOverhang, leftOverhangIsDirect),
            firstCutPos, DNAFragmentTerm(firstCutter->id, rightOverhang, rightOverhangIsDirect));
        results.append(ad1);
    } else {
        QByteArray lastLeftOverhang = getOverhang(U2Region(leftOverhangStart, lastCutPos - leftOverhangStart));
        if (isCircular) {
            SharedAnnotationData ad = createFragment(lastCutPos, DNAFragmentTerm(lastCutter->id, lastLeftOverhang, leftOverhangIsDirect),
                firstCutPos, DNAFragmentTerm(firstCutter->id, firstRightOverhang,rightOverhangIsDirect));

            results.append(ad);
        } else {
            if (firstCutPos != 0) {
                SharedAnnotationData ad1 = createFragment(seqRange.startPos, DNAFragmentTerm(),
                    firstCutPos, DNAFragmentTerm(firstCutter->id, firstRightOverhang, rightOverhangIsDirect));
                results.append(ad1);
            }
            if (lastCutPos != dnaObj->getSequenceLength()) {
                SharedAnnotationData ad2 = createFragment(lastCutPos, DNAFragmentTerm(lastCutter->id, lastLeftOverhang, leftOverhangIsDirect),
                    seqRange.endPos(), DNAFragmentTerm());
                results.append(ad2);
            }
        }
    }
    qSort(results.begin(), results.end(), compareAnnotationsbyLength);

    for (int fragmentCounter = 0; fragmentCounter < results.size(); fragmentCounter++) {
        results[fragmentCounter]->name = QString("Fragment %1").arg(fragmentCounter + 1);
    }
}

void DigestSequenceTask::saveResults()
{
    destObj->addAnnotations(results, ANNOTATION_GROUP_FRAGMENTS);
}

QString DigestSequenceTask::generateReport() const
{
    QString res;

    if (hasError()) {
        return res;
    }

    QString topology = dnaObj->isCircular() ? tr("circular") : tr("linear");
    res+= tr("<h3><br>Digest into fragments %1 (%2)</h3>").arg(dnaObj->getDocument()->getName()).arg(topology);
    res+=tr("<br>Generated %1 fragments.").arg(results.count());
    int counter = 1;
    foreach (const SharedAnnotationData &sdata, results) {
        const int startPos = sdata->location->regions.first().startPos + 1;
        const int endPos = sdata->location->regions.last().endPos();
        int len = 0;
        foreach (const U2Region& r, sdata->location->regions) {
            len += r.endPos() - r.startPos;
        }
        res += tr("<br><br>&nbsp;&nbsp;&nbsp;&nbsp;%1:&nbsp;&nbsp;&nbsp;&nbsp;From %3 (%2) To %5 (%4) - %6 bp ").arg(counter)
            .arg(startPos).arg(sdata->findFirstQualifierValue(QUALIFIER_LEFT_TERM))
            .arg(endPos).arg(sdata->findFirstQualifierValue(QUALIFIER_RIGHT_TERM))
            .arg(len);
        ++counter;
    }

    return res;

}

void DigestSequenceTask::checkForConservedAnnotations()
{
    QMap<QString, U2Region>::const_iterator it = cfg.conservedRegions.constBegin();
    for(; it != cfg.conservedRegions.constEnd(); ++it) {
        bool found = false;
        U2Region annRegion = it.value();
        foreach (const SharedAnnotationData &data, results) {
            const U2Region resRegion = data->location->regions.first();
            if (resRegion.contains(annRegion)) {
                found = true;
                break;
            }
        }
        if (!found) {
            QString locationStr = QString("%1..%2").arg(annRegion.startPos + 1).arg(annRegion.endPos());
            setError(tr("Conserved annotation %1 (%2) is disrupted by the digestion. Try changing the restriction sites.")
                .arg(it.key()).arg(locationStr));
            return;
        }
    }
}

qint64 DigestSequenceTask::correctPos(const qint64 pos) const {
    qint64 res = pos;
    if (!isCircular) {
        res = qBound<qint64>(0, pos, dnaObj->getSequenceLength());
    }
    return res;
}

QByteArray DigestSequenceTask::getOverhang(const U2Region& region) const {
    QByteArray result;
    if (region.startPos < 0 && isCircular) {
        result = dnaObj->getSequenceData(U2Region(dnaObj->getSequenceLength() + region.startPos, qAbs(region.startPos)));
        result += dnaObj->getSequenceData(U2Region(0, region.length + region.startPos));
    } else {
        result = dnaObj->getSequenceData(region);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////

LigateFragmentsTask::LigateFragmentsTask(const QList<DNAFragment>& fragments, const LigateFragmentsTaskConfig& config)
: Task("LigateFragmentsTask", TaskFlags_NR_FOSCOE), fragmentList(fragments), cfg(config),
  resultDoc(NULL), resultAlphabet(NULL)
{
    GCOUNTER(cvar,tvar,"LigateFragments");
}

void LigateFragmentsTask::processOverhangs(const DNAFragment& prevFragment, const DNAFragment& curFragment, QByteArray& overhangAddition)
{
    const DNAFragmentTerm& prevTerm = prevFragment.getRightTerminus();
    const DNAFragmentTerm& curTerm = curFragment.getLeftTerminus();

    if (prevTerm.type != curTerm.type) {
        stateInfo.setError(tr("Fragments %1 and  %2 are inconsistent. Blunt and sticky ends incompatibility")
            .arg(prevFragment.getName()).arg(curFragment.getName()));
        return;
    }

    QByteArray prevOverhang = prevFragment.getRightTerminus().overhang;
    QByteArray curOverhang = curFragment.getLeftTerminus().overhang;

    if (prevTerm.type == OVERHANG_TYPE_STICKY) {
        if (!overhangsAreConsistent(prevFragment.getRightTerminus(), curFragment.getLeftTerminus())) {
            stateInfo.setError(tr("Right overhang from %1 and left overhang from %2 are inconsistent.")
                .arg(prevFragment.getName()).arg(curFragment.getName()));
            return;
        } else {
            overhangAddition += curOverhang;
        }
    } else if (prevTerm.type == OVERHANG_TYPE_BLUNT) {
        overhangAddition += prevOverhang + curOverhang;
    } else {
        assert(0);
    }
}

bool LigateFragmentsTask::overhangsAreConsistent(const DNAFragmentTerm& curTerm, const DNAFragmentTerm& prevTerm)
{
    QByteArray curOverhang = curTerm.overhang;
    QByteArray prevOverhang = prevTerm.overhang;

    bool curStrand = curTerm.isDirect;
    bool prevStrand = prevTerm.isDirect;
    if (curStrand == prevStrand) {
        return false;
    }

    return curOverhang == prevOverhang;
}

void LigateFragmentsTask::prepare()
{
    QByteArray resultSeq;
    QVector<U2Region> fragmentRegions;

    DNAFragment prevFragment;
    assert(prevFragment.isEmpty());


    if (!cfg.makeCircular && cfg.checkOverhangs) {
        const DNAFragment& first = fragmentList.first();
        QByteArray leftOverhangAddition = first.getLeftTerminus().overhang;
        resultSeq.append(leftOverhangAddition);
    }

    foreach (const DNAFragment& dnaFragment, fragmentList) {

        QVector<U2Region> location = dnaFragment.getFragmentRegions();
        assert(location.size() > 0);

        // check alphabet consistency
        const DNAAlphabet* fragmentAlphabet = dnaFragment.getAlphabet();
        if (resultAlphabet == NULL) {
            resultAlphabet = fragmentAlphabet;
        } else if (resultAlphabet != fragmentAlphabet) {
            if (fragmentAlphabet == NULL) {
                stateInfo.setError(tr("Unknown DNA alphabet in fragment %1 of %2")
                    .arg(dnaFragment.getName()).arg(dnaFragment.getSequenceName()));
                return;
            }
            resultAlphabet = U2AlphabetUtils::deriveCommonAlphabet(resultAlphabet,fragmentAlphabet);
        }

        // check if overhangs are compatible
        QByteArray overhangAddition;
        if (cfg.checkOverhangs) {
             if (!prevFragment.isEmpty()) {
                processOverhangs(prevFragment, dnaFragment, overhangAddition);
                if (stateInfo.hasError()) {
                    return;
                }
             }
             prevFragment = dnaFragment;
        }

        // handle fragment annotations
        int resultLen = resultSeq.length() + overhangAddition.length();
        foreach (AnnotationTableObject *aObj, dnaFragment.getRelatedAnnotations()) {
            QList<SharedAnnotationData> toSave = cloneAnnotationsInFragmentRegion(dnaFragment, aObj, resultLen);
            annotations.append(toSave);
        }

        if (cfg.annotateFragments) {
            SharedAnnotationData a = createFragmentAnnotation(dnaFragment, resultLen);
            annotations.append(a);
        }

        resultSeq.append(overhangAddition);
        resultSeq.append(dnaFragment.getSequence(stateInfo));
        CHECK_OP(stateInfo, );
    }

    if (cfg.checkOverhangs) {
        if (cfg.makeCircular) {
            const DNAFragment& first = fragmentList.first();
            const DNAFragment& last = fragmentList.last();
            QByteArray overhangAddition;
            processOverhangs(last, first, overhangAddition);
            if (stateInfo.hasError()) {
                return;
            }
            resultSeq.append(overhangAddition);
        } else {
            const DNAFragment& last = fragmentList.last();
            QByteArray rightOverhangAddition = last.getRightTerminus().overhang;
            resultSeq.append(rightOverhangAddition);
        }
    }

    // create comment
    SharedAnnotationData sourceAnnot = createSourceAnnotation(resultSeq.length());
    annotations.append(sourceAnnot);

    createDocument(resultSeq, annotations);

    if (!cfg.addDocToProject) {
        return;
    }

    QList<Task*> tasks;
    tasks.append(new AddDocumentTask(resultDoc));

    if (cfg.openView) {
        tasks.append(new OpenViewTask(resultDoc));
    }
    if (cfg.saveDoc) {
        tasks.append(new SaveDocumentTask(resultDoc));
    }

    Task* multiTask = new MultiTask(tr("Add constructed molecule"), tasks);
    addSubTask(multiTask);
}

SharedAnnotationData LigateFragmentsTask::createSourceAnnotation(int regLen)
{
    Version v = Version::appVersion();
    SharedAnnotationData d(new AnnotationData);
    d->name = "source";
    d->location->regions << U2Region(0, regLen);
    d->qualifiers.append(U2Qualifier("comment", QString("Molecule is created with Unipro UGENE v%1.%2").arg(v.major).arg(v.minor)));
    return d;
}

SharedAnnotationData LigateFragmentsTask::createFragmentAnnotation(const DNAFragment& fragment, int startPos)
{
    SharedAnnotationData d(new AnnotationData);
    d->name = QString("%1 %2").arg(fragment.getSequenceName()).arg(fragment.getName());
    d->location->regions << U2Region(startPos, fragment.getLength());
    d->qualifiers.append(U2Qualifier("source_doc", fragment.getSequenceDocName()));

    return d;
}

QList<SharedAnnotationData> LigateFragmentsTask::cloneAnnotationsInRegion(const U2Region& fragmentRegion, AnnotationTableObject* source, int globalOffset)
{
    QList<SharedAnnotationData> results;
    // TODO: allow to cut annotations
    // TODO: consider optimizing the code below using AnnotationTableObject::getAnnotationsByRegion()
    foreach (Annotation *a, source->getAnnotations()) {
        bool ok = true;
        const QVector<U2Region>& location = a->getRegions();
        foreach (const U2Region &region, location) {
            if (!fragmentRegion.contains(region) || fragmentRegion == region) {
                ok = false;
                break;
            }
        }
        if (ok) {
            int newPos = globalOffset + location.first().startPos - fragmentRegion.startPos;
            SharedAnnotationData cloned(new AnnotationData(*a->getData()));
            QVector<U2Region> newLocation;
            foreach (const U2Region &region, a->getRegions()) {
                U2Region newRegion(region);
                newRegion.startPos = newPos;
                newLocation.append(newRegion);
            }
            cloned->location->regions = newLocation;
            results.append(cloned);
        }
    }

    return results;
}

static bool fragmentContainsRegion(const DNAFragment& fragment, const U2Region region) {

    QVector<U2Region> fragmentRegions = fragment.getFragmentRegions();

    bool result = false;
    foreach (const U2Region& fR, fragmentRegions) {
        if (fR.contains(region)) {
            result = true;
            break;
        }
    }

    return result;
}

static int getRelativeStartPos(const DNAFragment& fragment, const U2Region region) {
    QVector<U2Region> fragmentRegions = fragment.getFragmentRegions();

    int offset = 0;
    foreach (const U2Region& fR, fragmentRegions) {
        if (fR.contains(region)) {
            return offset + region.startPos - fR.startPos;
        }
        offset += fR.length;
    }

    // the fragment doesn't contain the region
    return -1;
}

QList<SharedAnnotationData> LigateFragmentsTask::cloneAnnotationsInFragmentRegion(const DNAFragment& fragment,
    AnnotationTableObject *source, int globalOffset)
{
    QList<SharedAnnotationData> results;

    // TODO: allow to remove annotations

    foreach (Annotation *a, source->getAnnotations()) {
        QVector<U2Region> location = a->getRegions();
        if (a->getName().startsWith("Fragment")) {
            continue;
        }

        bool ok = true;
        foreach (const U2Region &r, location) {
            // sneaky annotations shall not pass!
            if (!fragmentContainsRegion(fragment, r)) {
                ok = false;
                break;
            }
        }

        if (ok) {
            SharedAnnotationData cloned(new AnnotationData(*a->getData()));
            QVector<U2Region> newLocation;
            foreach (const U2Region &region, location) {
                int startPos = getRelativeStartPos(fragment, region);
                if (fragment.isInverted()) {
                    startPos = fragment.getLength() - startPos - region.length;
                    U2Strand strand = cloned->getStrand();
                    if (strand.isDirect()) {
                        cloned->setStrand(U2Strand::Complementary);
                    } else {
                        cloned->setStrand(U2Strand::Direct);
                    }
                }
                assert(startPos != -1);
                int newPos = globalOffset + startPos;
                U2Region newRegion(region);
                newRegion.startPos = newPos;
                newLocation.append(newRegion);
            }

            cloned->location->regions = newLocation;
            results.append(cloned);
        }
    }

    return results;
}


void LigateFragmentsTask::createDocument(const QByteArray& seq, const QList<SharedAnnotationData> &annotations)
{
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QList<GObject*> objects;

    QString seqName = cfg.seqName.isEmpty() ? cfg.docUrl.baseFileName() : cfg.seqName;
    DNASequence dna(seqName, seq, resultAlphabet);
    dna.circular = cfg.makeCircular;

    // set Genbank header
    DNALocusInfo loi;
    loi.name = seqName;
    loi.topology = cfg.makeCircular ? "circular" : "linear";
    loi.molecule = "DNA";
    loi.division = "SYN";
    QDate date = QDate::currentDate();
    loi.date = QString("%1-%2-%3").arg(date.toString("dd")).arg(FormatUtils::getShortMonthName(date.month())).arg(date.toString("yyyy"));

    dna.info.insert(DNAInfo::LOCUS, qVariantFromValue<DNALocusInfo>(loi));

    resultDoc = df->createNewLoadedDocument(iof, cfg.docUrl, stateInfo);
    CHECK_OP(stateInfo,);

    U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, resultDoc->getDbiRef(), dna);
    CHECK_OP_EXT(stateInfo, delete resultDoc; resultDoc = NULL,);

    U2SequenceObject* dnaObj = new U2SequenceObject(seqName, seqRef);
    resultDoc->addObject(dnaObj);

    AnnotationTableObject *aObj = new AnnotationTableObject(QString("%1 annotations").arg(seqName), resultDoc->getDbiRef());
    aObj->addAnnotations(annotations);
    resultDoc->addObject(aObj);

    aObj->addObjectRelation(dnaObj,ObjectRole_Sequence);
}

} // U2

