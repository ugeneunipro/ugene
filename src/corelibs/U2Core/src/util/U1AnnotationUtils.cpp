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

#include "U1AnnotationUtils.h"

#include <QStringBuilder>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

typedef QPair<QString, QString> QStrStrPair;

QString U1AnnotationUtils::lowerCaseAnnotationName("lower_case");
QString U1AnnotationUtils::upperCaseAnnotationName("upper_case");

AnnotatedRegion::AnnotatedRegion()
    : annotation(NULL), regionIdx(-1) {
}

AnnotatedRegion::AnnotatedRegion(Annotation *annotation, int regionIdx)
    : annotation(annotation), regionIdx(regionIdx) {
}

AnnotatedRegion::AnnotatedRegion(const AnnotatedRegion &annRegion)
    : annotation(annRegion.annotation), regionIdx(annRegion.regionIdx) {
}

QList<QVector<U2Region>> U1AnnotationUtils::fixLocationsForReplacedRegion(const U2Region &region2Remove, qint64 region2InsertLength, const QVector<U2Region> &original, AnnotationStrategyForResize s) {
    QList<QVector<U2Region>> res;
    const qint64 dLen = region2InsertLength - region2Remove.length;
    if (AnnotationStrategyForResize_Resize == s) {
        if (region2Remove.length == region2InsertLength) {
            res << original;
            return res;
        }
    }
    res << QVector<U2Region>();
    QVector<U2Region> &updated = res[0];

    foreach (U2Region r, original) {
        //if location ends before modification
        if (r.endPos() <= region2Remove.startPos) {
            updated << r;
            continue;
        }
        // if location starts after the modification
        if (r.startPos >= region2Remove.endPos()) {
            r.startPos += dLen;
            updated << r;
            continue;
        }
        if (AnnotationStrategyForResize_Remove == s) {
            continue;
        } else if (AnnotationStrategyForResize_Resize == s) {
            // if location is in the region to remove -> remove it
            if (region2Remove.contains(r)) {
                continue;
            }
            // if location contains modified region -> update it length
            if (r.contains(region2Remove)) {
                // if set A = set B - do nothing
                if (!(r.startPos == region2Remove.startPos && r.endPos() == region2Remove.endPos())) {
                    r.length += dLen;
                    updated << r;
                }
            }
            // if location partly contain (in the end) region2remove - update length
            else if (r.contains(U2Region(region2Remove.startPos, 0))) {
                if (dLen < 0) {
                    r.length -= (r.endPos() - region2Remove.startPos);
                }
                updated << r;
            } else if (r.contains(U2Region(region2Remove.endPos(), 0))) {
                if (dLen < 0) {
                    int diff = region2Remove.endPos() - r.startPos;
                    r.startPos += diff + dLen;
                    r.length -= diff;
                }
                updated << r;
            }
            continue;
        }
        SAFE_POINT(AnnotationStrategyForResize_Split_To_Joined == s || AnnotationStrategyForResize_Split_To_Separate == s,
                   "Unexpected resize strategy detected!",
                   res);
        //leave left part in original(updated) locations and push right into new one
        const bool join = (AnnotationStrategyForResize_Split_To_Joined == s);
        const U2Region interR = r.intersect(region2Remove);
        const U2Region leftR = r.startPos < interR.startPos ? U2Region(r.startPos, interR.startPos - r.startPos) : U2Region();
        const U2Region rightR = (r.endPos() > interR.endPos()) ? U2Region(interR.endPos() + dLen, r.endPos() - interR.endPos()) : U2Region();
        if (leftR.isEmpty()) {
            if (!rightR.isEmpty()) {
                updated << rightR;
            }
            continue;
        }
        updated << leftR;
        if (!rightR.isEmpty()) {
            if (join) {
                updated << rightR;
            } else {
                QVector<U2Region> extraAnnReg;
                extraAnnReg << rightR;
                res << extraAnnReg;
            }
        }
    }
    return res;
}

int U1AnnotationUtils::getRegionFrame(int sequenceLen, const U2Strand &strand, bool order, int region, const QVector<U2Region> &location) {
    int frame = 0;
    const U2Region &r = location.at(region);
    if (strand.isCompementary()) {
        frame = (sequenceLen - r.endPos()) % 3;
    } else {
        frame = r.startPos % 3;
    }
    if (!order) {    //join -> need to join region with prev regions to derive frame
        if (strand.isCompementary()) {
            int offset = 0;
            for (int i = location.size(); --i > region;) {
                const U2Region &rb = location.at(i);
                offset += rb.length;
            }
            int dFrame = offset % 3;
            frame = (frame + (3 - dFrame)) % 3;
        } else {
            int offset = 0;
            for (int i = 0; i < region; i++) {
                const U2Region &rb = location.at(i);
                offset += rb.length;
            }
            int dFrame = offset % 3;
            frame = (frame + (3 - dFrame)) % 3;
        }
    }
    return frame;
}

bool U1AnnotationUtils::isSplitted(const U2Location &location, const U2Region &seqRange) {
    QVector<U2Region> regions = location->regions;
    if (2 != regions.size()) {
        return false;
    } else if ((regions[0].endPos() == seqRange.endPos() && regions[1].startPos == seqRange.startPos) || (regions[1].endPos() == seqRange.endPos() && regions[0].startPos == seqRange.startPos)) {
        return true;
    }

    return false;
}

bool findCaseRegion(const char *data, int dataLen, int startFrom, int globalOffset, U2Region &reg, bool &unfinished, bool isLowerCaseSearching) {
    bool foundStart = false;
    int strIdx = startFrom;
    unfinished = false;

    // find start of lower case block
    while (!foundStart && strIdx < dataLen) {
        QChar c(data[strIdx]);
        if (isLowerCaseSearching) {
            foundStart = c.isLower();
        } else {
            foundStart = c.isUpper();
        }
        strIdx++;
    }

    if (foundStart) {
        int startPos = globalOffset + strIdx - 1;

        // find end of lower case block
        bool foundEnd = false;
        while (!foundEnd && strIdx < dataLen) {
            QChar c(data[strIdx]);
            if (isLowerCaseSearching) {
                foundEnd = c.isUpper();
            } else {
                foundEnd = c.isLower();
            }
            strIdx++;
        }
        int endPos = 0;
        if (foundEnd) {
            endPos = globalOffset + strIdx - 2;
        } else {
            endPos = globalOffset + strIdx - 1;
        }
        reg = U2Region(startPos, endPos - startPos + 1);
        unfinished = !foundEnd;
    }

    return foundStart;
}

QList<SharedAnnotationData> U1AnnotationUtils::getCaseAnnotations(const char *data, int dataLen, int globalOffset, bool &isUnfinishedRegion, U2Region &unfinishedRegion, bool isLowerCaseSearching) {
    QList<SharedAnnotationData> result;

    U2Region reg;
    int startPos = 0;
    bool unfinished = false;
    while (findCaseRegion(data, dataLen, startPos, globalOffset, reg, unfinished, isLowerCaseSearching)) {
        startPos = reg.endPos() - globalOffset;
        if (isUnfinishedRegion) {
            reg.startPos = unfinishedRegion.startPos;
            reg.length += unfinishedRegion.length;
            isUnfinishedRegion = false;
        }

        if (unfinished) {
            isUnfinishedRegion = true;
            unfinishedRegion = reg;
            break;
        } else {
            result << finalizeUnfinishedRegion(true, reg, isLowerCaseSearching);
        }
    }

    return result;
}

QList<SharedAnnotationData> U1AnnotationUtils::finalizeUnfinishedRegion(bool isUnfinishedRegion, U2Region &unfinishedRegion, bool isLowerCaseSearching) {
    QList<SharedAnnotationData> result;

    if (isUnfinishedRegion) {
        SharedAnnotationData annData(new AnnotationData);
        annData->name = isLowerCaseSearching ? lowerCaseAnnotationName : upperCaseAnnotationName;
        annData->location->regions.append(unfinishedRegion);
        annData->caseAnnotation = true;
        result.append(annData);
    }

    return result;
}

void U1AnnotationUtils::addAnnotations(QList<GObject *> &objects, const QList<SharedAnnotationData> &annList, const GObjectReference &sequenceRef, AnnotationTableObject *annotationsObject, const QVariantMap &hints) {
    U2OpStatusImpl os;
    if (!annList.isEmpty()) {
        if (NULL == annotationsObject) {
            U2DbiRef dbiRef;
            if (hints.contains(DocumentFormat::DBI_REF_HINT)) {
                dbiRef = hints.value(DocumentFormat::DBI_REF_HINT).value<U2DbiRef>();
            } else {
                dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
                SAFE_POINT_OP(os, );
            }

            QVariantMap objectHints;
            objectHints.insert(DocumentFormat::DBI_FOLDER_HINT, hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));

            annotationsObject = new AnnotationTableObject(sequenceRef.objName + " features", dbiRef, objectHints);
            annotationsObject->addObjectRelation(
                GObjectRelation(sequenceRef, ObjectRole_Sequence));
        }
        annotationsObject->addAnnotations(annList);
        if (!objects.contains(annotationsObject)) {
            objects.append(annotationsObject);
        }
    }
}

QList<U2Region> U1AnnotationUtils::getRelatedLowerCaseRegions(const U2SequenceObject *so,
                                                              const QList<GObject *> &anns) {
    QList<GObject *> aos;
    if (NULL != so->getDocument()) {
        aos = GObjectUtils::findObjectsRelatedToObjectByRole(so, GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence, anns, UOF_LoadedOnly);
    } else {
        aos = anns;
    }

    QList<U2Region> lowerCaseRegs;
    QList<U2Region> upperCaseRegs;
    foreach (GObject *o, aos) {
        AnnotationTableObject *ato = dynamic_cast<AnnotationTableObject *>(o);
        foreach (Annotation *a, ato->getAnnotations()) {
            if (a->getName() == lowerCaseAnnotationName) {
                lowerCaseRegs << a->getRegions().toList();
            } else if (a->getName() == upperCaseAnnotationName) {
                upperCaseRegs << a->getRegions().toList();
            }
        }
    }

    if (upperCaseRegs.isEmpty()) {
        return lowerCaseRegs;
    }

    // extract lower case annotations from upper case ones
    qStableSort(upperCaseRegs.begin(), upperCaseRegs.end());

    qint64 nextStartPos = 0;
    foreach (const U2Region &reg, upperCaseRegs) {
        if (reg.startPos > nextStartPos) {
            lowerCaseRegs.append(U2Region(nextStartPos, reg.startPos - nextStartPos));
        }
        nextStartPos = reg.endPos();
    }
    if (nextStartPos < so->getSequenceLength()) {
        lowerCaseRegs.append(U2Region(nextStartPos, so->getSequenceLength() - nextStartPos));
    }

    return lowerCaseRegs;
}

bool U1AnnotationUtils::isAnnotationContainsJunctionPoint(const Annotation *annotation,
                                                          const qint64 sequenceLength)
{
    const QList<RegionsPair> mergedRegions =  mergeAnnotatiedRegionsAroundJunctionPoint
                                             (annotation->getRegions(),
                                              sequenceLength);
    return isAnnotationContainsJunctionPoint(mergedRegions);
}

bool U1AnnotationUtils::isAnnotationContainsJunctionPoint(const QList<RegionsPair> &mergedRegions) {
    bool result = false;
    foreach (const RegionsPair &pair, mergedRegions) {
        CHECK_CONTINUE(!pair.second.isEmpty());
        result = true;
        break;
    }
    return result;
}

QList<RegionsPair> U1AnnotationUtils::mergeAnnotatiedRegionsAroundJunctionPoint(const QVector<U2Region> &regions,
                                                                             const qint64 sequenceLength)
{
    QList<RegionsPair> result;

    for (int i = 0; i < regions.size(); i++) {
        const U2Region reg = regions[i];
        if (reg.endPos() != sequenceLength) {
            result.append(RegionsPair(reg, U2Region()));
            continue;
        }
        if ((i + 1) >= regions.size()) {
            result.append(RegionsPair(reg, U2Region()));
            break;
        }

        const U2Region secondReg = regions.value(i + 1);
        if (secondReg.startPos != 0) {
            result.append(RegionsPair(reg, U2Region()));
            continue;
        }

        result.append(RegionsPair(reg, secondReg));
        i++;
    }

    return result;
}

char *U1AnnotationUtils::applyLowerCaseRegions(char *seq, qint64 first, qint64 len, qint64 globalOffset, const QList<U2Region> &regs) {
    const U2Region seqRegion(first + globalOffset, len);
    foreach (const U2Region &reg, regs) {
        const U2Region &intersection = seqRegion.intersect(reg);

        TextUtils::translate(TextUtils::LOWER_CASE_MAP, seq + intersection.startPos - globalOffset, intersection.length);
    }

    return seq;
}

QString U1AnnotationUtils::guessAminoTranslation(AnnotationTableObject *ao, const DNAAlphabet *al) {
    DNATranslation *res = NULL;
    DNATranslationRegistry *tr = AppContext::getDNATranslationRegistry();

    if (NULL != ao && NULL != al) {
        if (al->isNucleic()) {
            foreach (Annotation *ann, ao->getAnnotationsByName("CDS")) {
                QList<U2Qualifier> ql;
                ann->findQualifiers("transl_table", ql);
                if (!ql.isEmpty()) {
                    const QString guess = "NCBI-GenBank #" + ql.first().value;
                    res = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO, guess);
                    if (NULL != res) {
                        return guess;
                    }
                }
            }
        }
    }
    return "";
}

QList<AnnotatedRegion> U1AnnotationUtils::getAnnotatedRegionsByStartPos(QList<AnnotationTableObject *> annotationObjects, qint64 startPos) {
    QList<AnnotatedRegion> result;
    foreach (AnnotationTableObject *annObject, annotationObjects) {
        QList<Annotation *> annots = annObject->getAnnotationsByRegion(U2Region(startPos, 1));
        foreach (Annotation *a, annots) {
            QVector<U2Region> regions = a->getRegions();
            for (int i = 0; i < regions.size(); i++) {
                if (regions[i].startPos == startPos) {
                    AnnotatedRegion ar(a, i);
                    result.append(ar);
                }
            }
        }
    }
    return result;
}

void U1AnnotationUtils::addDescriptionQualifier(QList<SharedAnnotationData> &annotations, const QString &description) {
    for (int i = 0; i < annotations.size(); i++) {
        addDescriptionQualifier(annotations[i], description);
    }
}

void U1AnnotationUtils::addDescriptionQualifier(SharedAnnotationData &annotationData, const QString &description) {
    CHECK(!description.isEmpty(), );

    for (int i = 0; i < annotationData->qualifiers.size(); i++) {
        U2Qualifier &qualifier = annotationData->qualifiers[i];
        if (GBFeatureUtils::QUALIFIER_NOTE == qualifier.name) {
            qualifier.value = description;
            return;
        }
    }

    annotationData->qualifiers << U2Qualifier(GBFeatureUtils::QUALIFIER_NOTE, description);
}

bool U1AnnotationUtils::containsQualifier(const QList<U2Qualifier> &qualifiers, const QString &qualifierName) {
    foreach (const U2Qualifier &qualifier, qualifiers) {
        if (qualifier.name == qualifierName) {
            return true;
        }
    }
    return false;
}

void U1AnnotationUtils::removeAllQualifier(SharedAnnotationData &annotationData, const QString &qualifierName) {
    QMutableVectorIterator<U2Qualifier> i(annotationData->qualifiers);
    while (i.hasNext()) {
        if (i.next().name == qualifierName) {
            i.remove();
        }
    }
}

QString U1AnnotationUtils::buildLocationString(const U2LocationData &location) {
    bool complement = location.strand.isCompementary();
    bool multi = location.regions.size() > 1;
    QString locationStr = complement ? "complement(" : "";
    if (!location.regions.empty()) {
        if (multi) {
            locationStr += location.isOrder() ? "order(" : (location.isBond() ? "bond(" : "join(");
        }
        locationStr += buildLocationString(location.regions);
    }
    if (multi) {
        locationStr += ")";
    }
    if (complement) {
        locationStr.append(")");
    }
    return locationStr;
}

QString U1AnnotationUtils::buildLocationString(const SharedAnnotationData &d) {
    return buildLocationString(*d->location);
}

QString U1AnnotationUtils::buildLocationString(const QVector<U2Region> &regions) {
    QString locationStr;
    for (int i = 0, n = regions.size(); i < n; ++i) {
        const U2Region &r = regions[i];
        locationStr = locationStr % QString::number(r.startPos + 1) % ".." % QString::number(r.endPos()) % ",";
    }
    locationStr.chop(1);
    return locationStr;
}

U2Location U1AnnotationUtils::shiftLocation(const U2Location &location, qint64 shift, qint64 sequenceLength) {
    const QVector<U2Region> &oldRegions = location->regions;
    if (shift == 0 || oldRegions.isEmpty()) {
        return location;
    }
    U2Location newLocation(location);
    QVector<U2Region> &newRegions = newLocation->regions;
    newRegions.clear();

    // Check merge location either with the left or the right neighbour on the overflow.
    QVector<int> mergeIndexes;
    for (int i = 0; i < oldRegions.size(); i++) {
        const U2Region &oldRegion = oldRegions[i];
        U2Region shiftedRegion(oldRegion.startPos + (shift % sequenceLength), oldRegion.length);
        if (shiftedRegion.startPos >= 0 && shiftedRegion.endPos() <= sequenceLength) {    // no overflow.
            newRegions.append(shiftedRegion);
        } else if (shiftedRegion.endPos() <= 0) {    // start overflow with no split.
            U2Region newRegion(shiftedRegion.startPos + sequenceLength, shiftedRegion.length);
            newRegions.append(newRegion);
            bool merge = i > 0 && oldRegions[i - 1].endPos() == sequenceLength;
            if (merge) {
                mergeIndexes << newRegions.length() - 2;
            }
        } else if (shiftedRegion.startPos >= sequenceLength) {    // end overflow with no split.
            U2Region newRegion(shiftedRegion.startPos - sequenceLength, shiftedRegion.length);
            newRegions.append(newRegion);
            bool merge = i + 1 < oldRegions.size() && oldRegions[i + 1].startPos == 0;
            if (merge) {
                mergeIndexes << newRegions.length() - 1;
            }
        } else if (shiftedRegion.startPos < 0) {    // start overflow with split.
            U2Region newRegion1(shiftedRegion.startPos + sequenceLength, -shiftedRegion.startPos);
            U2Region newRegion2(0, oldRegion.length - newRegion1.length);
            newRegions.append(newRegion1);
            newRegions.append(newRegion2);
            newLocation->op = U2LocationOperator_Join;
            bool merge = i > 0 && oldRegions[i - 1].endPos() == sequenceLength;
            if (merge) {
                mergeIndexes << newRegions.length() - 3;
            }
        } else if (shiftedRegion.endPos() > sequenceLength) {    // end overflow with split.
            U2Region newRegion1(shiftedRegion.startPos, sequenceLength - shiftedRegion.startPos);
            U2Region newRegion2(0, oldRegion.length - newRegion1.length);
            newRegions.append(newRegion1);
            newRegions.append(newRegion2);
            newLocation->op = U2LocationOperator_Join;
            bool merge = i + 1 < oldRegions.size() && oldRegions[i + 1].startPos == 0;
            if (merge) {
                mergeIndexes << newRegions.length() - 1;
            }
        }
    }

    // If there was an overflow: try to merge regions around overflow point.
    for (int i = mergeIndexes.size(); --i >= 0;) {
        int mergeIndex = mergeIndexes[i];
        Q_ASSERT(mergeIndex + 1 < newRegions.size());
        U2Region &region0 = newRegions[mergeIndex];
        U2Region &region1 = newRegions[mergeIndex + 1];
        Q_ASSERT(region0.endPos() == region1.startPos);
        region0.length += region1.length;
        newRegions.removeAt(mergeIndex + 1);
    }
    return newLocation;
}

QMap<Annotation *, QList<QPair<QString, QString>>> FixAnnotationsUtils::fixAnnotations(U2OpStatus *os, U2SequenceObject *seqObj, const U2Region &regionToReplace, const DNASequence &sequence2Insert, bool recalculateQualifiers, U1AnnotationUtils::AnnotationStrategyForResize str, QList<Document *> docs) {
    FixAnnotationsUtils fixer(os, seqObj, regionToReplace, sequence2Insert, recalculateQualifiers, str, docs);
    fixer.fixAnnotations();
    return fixer.annotationForReport;
}

FixAnnotationsUtils::FixAnnotationsUtils(U2OpStatus *os, U2SequenceObject *seqObj, const U2Region &regionToReplace, const DNASequence &sequence2Insert, bool recalculateQualifiers, U1AnnotationUtils::AnnotationStrategyForResize str, QList<Document *> docs)
    : recalculateQualifiers(recalculateQualifiers),
      strat(str),
      docs(docs),
      seqObj(seqObj),
      regionToReplace(regionToReplace),
      sequence2Insert(sequence2Insert),
      stateInfo(os) {
}

void FixAnnotationsUtils::fixAnnotations() {
    QList<GObject *> annotationTablesList;
    if (AppContext::getProject() != NULL) {
        annotationTablesList = GObjectUtils::findObjectsRelatedToObjectByRole(seqObj, GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence, GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::ANNOTATION_TABLE), UOF_LoadedOnly);
    } else {
        foreach (Document *d, docs) {
            QList<GObject *> allAnnotationTables = d->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
            foreach (GObject *table, allAnnotationTables) {
                if (table->hasObjectRelation(seqObj, ObjectRole_Sequence)) {
                    annotationTablesList.append(table);
                }
            }
        }
    }

    foreach (GObject *table, annotationTablesList) {
        AnnotationTableObject *ato = qobject_cast<AnnotationTableObject *>(table);
        if (NULL != ato) {
            QMap<QString, QList<SharedAnnotationData>> group2AnnotationsToAdd;
            QList<Annotation *> annotationToRemove;
            foreach (Annotation *an, ato->getAnnotations()) {
                bool annIsToBeRemoved = false;
                QMap<QString, QList<SharedAnnotationData>> newAnnotations = fixAnnotation(an, annIsToBeRemoved);
                foreach (const QString &groupName, newAnnotations.keys()) {
                    group2AnnotationsToAdd[groupName].append(newAnnotations[groupName]);
                }
                if (annIsToBeRemoved) {
                    annotationToRemove.append(an);
                }
            }
            foreach (const QString &groupName, group2AnnotationsToAdd.keys()) {
                ato->addAnnotations(group2AnnotationsToAdd[groupName], groupName);
            }
            ato->removeAnnotations(annotationToRemove);
        } else {
            assert(false);
            coreLog.error(L10N::nullPointerError("Annotation table object"));
        }
    }
}

QMap<QString, QList<SharedAnnotationData>> FixAnnotationsUtils::fixAnnotation(Annotation *an, bool &annIsRemoved) {
    QMap<QString, QList<SharedAnnotationData>> result;
    SAFE_POINT(NULL != an, L10N::nullPointerError("Annotation"), result);
    AnnotationTableObject *ato = an->getGObject();
    SAFE_POINT(NULL != ato, L10N::nullPointerError("Annotation table object"), result);

    QList<QVector<U2Region>> newRegions = U1AnnotationUtils::fixLocationsForReplacedRegion(regionToReplace,
                                                                                           sequence2Insert.seq.length(),
                                                                                           an->getRegions(),
                                                                                           strat);

    if (newRegions[0].isEmpty()) {
        annIsRemoved = true;
    } else {
        fixAnnotationQualifiers(an);

        an->updateRegions(newRegions[0]);
        fixTranslationQualifier(an);
        for (int i = 1; i < newRegions.size(); i++) {
            SharedAnnotationData splittedAnnotation(new AnnotationData(*an->getData()));
            const QString groupName = an->getGroup()->getGroupPath();
            splittedAnnotation->location->regions = newRegions[i];
            fixTranslationQualifier(splittedAnnotation);
            result[groupName].append(splittedAnnotation);
        }
    }
    return result;
}

void FixAnnotationsUtils::fixAnnotationQualifiers(Annotation *an) {
    CHECK(recalculateQualifiers, );

    QRegExp locationMatcher("(\\d+)\\.\\.(\\d+)");
    foreach (const U2Qualifier &qual, an->getQualifiers()) {
        QString newQualifierValue = qual.value;

        int lastModifiedPos = 0;
        int lastFoundPos = 0;
        while ((lastFoundPos = locationMatcher.indexIn(qual.value, lastFoundPos)) != -1) {
            const QString matchedRegion = locationMatcher.cap();
            const qint64 start = locationMatcher.cap(1).toLongLong() - 1;    // position starts with 0
            const qint64 end = locationMatcher.cap(2).toLongLong() - 1;

            U2Region referencedRegion(start, end - start + 1);
            if (isRegionValid(referencedRegion)) {
                QList<QVector<U2Region>> newRegions = U1AnnotationUtils::fixLocationsForReplacedRegion(regionToReplace,
                                                                                                       sequence2Insert.seq.length(),
                                                                                                       QVector<U2Region>() << referencedRegion,
                                                                                                       U1AnnotationUtils::AnnotationStrategyForResize_Resize);

                if (!newRegions.isEmpty() && !newRegions[0].empty()) {
                    QString newRegionsStr;
                    foreach (const U2Region &region, newRegions[0]) {
                        newRegionsStr += QString("%1..%2,").arg(region.startPos + 1).arg(region.endPos());    // position starts with 1
                    }
                    newRegionsStr.chop(1);    // remove last comma

                    const int oldRegionPos = newQualifierValue.indexOf(matchedRegion, lastModifiedPos);
                    SAFE_POINT(oldRegionPos != -1, "Unexpected region matched", );

                    newQualifierValue.replace(oldRegionPos, matchedRegion.length(), newRegionsStr);
                    lastModifiedPos = oldRegionPos + newRegionsStr.length();
                } else {
                    annotationForReport[an].append(QStrStrPair(qual.name, matchedRegion));
                }
            }

            lastFoundPos += locationMatcher.matchedLength();
        }

        if (newQualifierValue != qual.value) {
            an->removeQualifier(qual);
            an->addQualifier(U2Qualifier(qual.name, newQualifierValue));
        }
    }
}

void FixAnnotationsUtils::fixTranslationQualifier(SharedAnnotationData &ad) {
    CHECK(recalculateQualifiers, );

    const U2Qualifier translQual = getFixedTranslationQualifier(ad);
    CHECK(translQual.isValid(), );

    const QString existingTranslation = ad->findFirstQualifierValue(GBFeatureUtils::QUALIFIER_TRANSLATION);
    const U2Qualifier existingTranslQual(GBFeatureUtils::QUALIFIER_TRANSLATION, existingTranslation);
    for (int i = 0, n = ad->qualifiers.size(); i < n; ++i) {
        if (ad->qualifiers[i] == existingTranslQual) {
            ad->qualifiers.remove(i);
            break;
        }
    }
    ad->qualifiers.append(translQual);
}

void FixAnnotationsUtils::fixTranslationQualifier(Annotation *an) {
    CHECK(recalculateQualifiers, );

    const U2Qualifier newTranslQual = getFixedTranslationQualifier(an->getData());
    CHECK(newTranslQual.isValid(), );

    QList<U2Qualifier> translationQuals;
    an->findQualifiers(GBFeatureUtils::QUALIFIER_TRANSLATION, translationQuals);
    an->removeQualifier(translationQuals.first());
    an->addQualifier(newTranslQual);
}

U2Qualifier FixAnnotationsUtils::getFixedTranslationQualifier(const SharedAnnotationData &ad) {
    QVector<U2Qualifier> translationQuals;
    ad->findQualifiers(GBFeatureUtils::QUALIFIER_TRANSLATION, translationQuals);
    CHECK(!translationQuals.empty(), U2Qualifier());

    DNATranslation *aminoTranslation = GObjectUtils::findAminoTT(seqObj, false);
    SAFE_POINT(NULL != aminoTranslation, L10N::nullPointerError("Amino translation"), U2Qualifier());

    QString completeTranslation;
    foreach (const U2Region &r, ad->getRegions()) {
        const QByteArray annotatedData = seqObj->getSequenceData(r, *stateInfo);
        CHECK(!stateInfo->isCoR(), U2Qualifier());

        const DNAAlphabet *dstAlphabet = aminoTranslation->getDstAlphabet();
        QByteArray transContent(annotatedData.size() / 3, dstAlphabet->getDefaultSymbol());

        aminoTranslation->translate(annotatedData.constData(), annotatedData.length(), transContent.data(), transContent.length());
        completeTranslation.append(transContent);
    }

    return (completeTranslation != translationQuals.first().value) ? U2Qualifier(GBFeatureUtils::QUALIFIER_TRANSLATION, completeTranslation) : U2Qualifier();
}

bool FixAnnotationsUtils::isRegionValid(const U2Region &region) const {
    return region.length > 0 && region.startPos < seqObj->getSequenceLength() - 1;
}

}    // namespace U2
