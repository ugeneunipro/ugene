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

#ifndef _U1_ANNOTATION_UTILS_H_
#define _U1_ANNOTATION_UTILS_H_

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationSelection.h>
#include <U2Core/DNASequence.h>
#include <U2Core/GUrl.h>

using RegionsPair = QPair<U2::U2Region, U2::U2Region>;

namespace U2 {
class DNAAlphabet;
class Document;
class AnnotationTableObject;
class GObject;
class GObjectReference;
class U2SequenceObject;
class U2OpStatus;

class U2CORE_EXPORT AnnotatedRegion {
public:
    AnnotatedRegion();
    AnnotatedRegion(Annotation *annotation, int regionIdx);
    AnnotatedRegion(const AnnotatedRegion &annRegion);

public:
    Annotation *annotation;
    int regionIdx;
};

/**
 * U2Annotation and related structures utility functions
 */
class U2CORE_EXPORT U1AnnotationUtils {
public:
    enum AnnotationStrategyForResize {
        AnnotationStrategyForResize_Resize = 0,
        AnnotationStrategyForResize_Remove = 1,
        AnnotationStrategyForResize_Split_To_Joined = 2,
        AnnotationStrategyForResize_Split_To_Separate = 3
    };
    Q_ENUMS(U2::U1AnnotationUtils::AnnotationStrategyForResize)

    /**
     * Corrects annotation locations for a sequence. The passed list is original locations
     * The returned list contains set of regions. Each set is per 1 annotation.
     * If specified strategy is 'remove', removes all locations which intersect the modified region or fall inside it.
     */
    static QList<QVector<U2Region>> fixLocationsForReplacedRegion(const U2Region &region2Remove, qint64 region2InsertLength, const QVector<U2Region> &originalLoc, AnnotationStrategyForResize s);
    /**
     * Returns translation frame[0,1,2] the region is placed on
     */
    static int getRegionFrame(int sequenceLen, const U2Strand &strand, bool order, int region, const QVector<U2Region> &location);
    /**
     * Returns true if annotation location is splitted by sequence "edges".
     * For example, location JOIN(N..SeqSize - 1, 0..M) is splitted.
     */
    static bool isSplitted(const U2Location &location, const U2Region &seqRange);
    /**
     * Return a list of lower/upper case annotations for @data sequence
     * If an annotation is placed from some symbol till the end of the sequence
     * then @isUnfinishedRegion == true and @unfinishedRegion keep this unfinished region
     */
    static QList<SharedAnnotationData> getCaseAnnotations(const char *data, int dataLen, int globalOffset, bool &isUnfinishedRegion, U2Region &unfinishedRegion, bool isLowerCaseSearching);

    static QList<SharedAnnotationData> finalizeUnfinishedRegion(bool isUnfinishedRegion, U2Region &unfinishedRegion, bool isLowerCaseSearching);
    /**
     * If @annotationsObject is NULL then it creates a new annotation object
     */
    static void addAnnotations(QList<GObject *> &objects, const QList<SharedAnnotationData> &annList, const GObjectReference &sequenceRef, AnnotationTableObject *annotationsObject, const QVariantMap &hints);

    static QList<U2Region> getRelatedLowerCaseRegions(const U2SequenceObject *so, const QList<GObject *> &anns);

    /**
    * Check if it's the selection of the circular view, which contains the junction point
    * Return true if the "Annotation Selection Data" argument contains two selected regions and two location IDs,
    * And if one of these regions has start point equals to zero, and another one has end pos equals to sequence length
    */
    static bool isAnnotationContainsJunctionPoint(const Annotation *annotation, const qint64 sequenceLength);
    static bool isAnnotationContainsJunctionPoint(const QList<RegionsPair> &mergedRegions);
    static QList<RegionsPair> mergeAnnotatiedRegionsAroundJunctionPoint(const QVector<U2Region> &regions, const qint64 sequenceLength);

    static char *applyLowerCaseRegions(char *seq, qint64 first, qint64 len, qint64 globalOffset, const QList<U2Region> &regs);

    static QString guessAminoTranslation(AnnotationTableObject *ao, const DNAAlphabet *al);

    static QList<AnnotatedRegion> getAnnotatedRegionsByStartPos(QList<AnnotationTableObject *> annotationObjects, qint64 startPos);

    /** Shifts annotation around the circular sequence and returns new location. */
    static U2Location shiftLocation(const U2Location &location, qint64 shift, qint64 sequenceLength);

    /**
     * Adds or replaces "/note" qualifier if description is not empty.
     */
    static void addDescriptionQualifier(QList<SharedAnnotationData> &annotations, const QString &description);
    static void addDescriptionQualifier(SharedAnnotationData &annotationData, const QString &description);

    static bool containsQualifier(const QList<U2Qualifier> &qualifiers, const QString &qualifierName);
    static void removeAllQualifier(SharedAnnotationData &annotationData, const QString &qualifierName);

    static QString buildLocationString(const SharedAnnotationData &a);
    static QString buildLocationString(const U2LocationData &location);
    static QString buildLocationString(const QVector<U2Region> &regions);

    static QString lowerCaseAnnotationName;
    static QString upperCaseAnnotationName;
};

class U2CORE_EXPORT FixAnnotationsUtils {
public:
    static QMap<Annotation *, QList<QPair<QString, QString>>> fixAnnotations(U2OpStatus *os, U2SequenceObject *seqObj, const U2Region &regionToReplace, const DNASequence &sequence2Insert, bool recalculateQualifiers = false, U1AnnotationUtils::AnnotationStrategyForResize str = U1AnnotationUtils::AnnotationStrategyForResize_Resize, QList<Document *> docs = QList<Document *>());

private:
    FixAnnotationsUtils(U2OpStatus *os, U2SequenceObject *seqObj, const U2Region &regionToReplace, const DNASequence &sequence2Insert, bool recalculateQualifiers = false, U1AnnotationUtils::AnnotationStrategyForResize str = U1AnnotationUtils::AnnotationStrategyForResize_Resize, QList<Document *> docs = QList<Document *>());
    void fixAnnotations();

    QMap<QString, QList<SharedAnnotationData>> fixAnnotation(Annotation *an, bool &annIsRemoved);
    void fixAnnotationQualifiers(Annotation *an);
    void fixTranslationQualifier(SharedAnnotationData &ad);
    void fixTranslationQualifier(Annotation *an);
    U2Qualifier getFixedTranslationQualifier(const SharedAnnotationData &ad);
    bool isRegionValid(const U2Region &region) const;

private:
    bool recalculateQualifiers;
    U1AnnotationUtils::AnnotationStrategyForResize strat;
    QList<Document *> docs;
    U2SequenceObject *seqObj;
    U2Region regionToReplace;
    DNASequence sequence2Insert;
    QMap<Annotation *, QList<QPair<QString, QString>>> annotationForReport;

    U2OpStatus *stateInfo;
};

}    // namespace U2

Q_DECLARE_METATYPE(U2::U1AnnotationUtils::AnnotationStrategyForResize)

#endif
