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

#ifndef _U2_SMITH_WATERMAN_REPORT_CALLBACK_H_
#define _U2_SMITH_WATERMAN_REPORT_CALLBACK_H_

#include <U2Algorithm/SmithWatermanResult.h>
#include <U2Core/DNAAlphabet.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

class U2ALGORITHM_EXPORT SmithWatermanReportCallback {
public:
    virtual                 ~SmithWatermanReportCallback();

    virtual QString         report(const QList<SmithWatermanResult> &) = 0;
};

class U2ALGORITHM_EXPORT SmithWatermanReportCallbackAnnotImpl : public QObject,
                                                                public SmithWatermanReportCallback
{
    Q_OBJECT
public:
    SmithWatermanReportCallbackAnnotImpl(AnnotationTableObject *aobj,
                                         U2FeatureType annotationType,
                                         const QString &annotationName,
                                         const QString &annotationGroup,
                                         const QString &annDescription,
                                         bool addPatternSubseqToQual,
                                         QObject *pOwn = NULL);

    QString                             report(const QList<SmithWatermanResult> &result);
    const QList<SharedAnnotationData> & getAnotations() const;

private:
    U2FeatureType                       annotationType;
    QString                             annotationName;
    QString                             annotationGroup;
    const QString                       annDescription;
    QPointer<AnnotationTableObject>     aObj;
    QList<SharedAnnotationData>         anns;
    bool                                autoReport;
    bool                                addPatternSubseqToQual;
};

class Project;

class U2ALGORITHM_EXPORT SmithWatermanReportCallbackMAImpl :    public QObject,
                                                                public SmithWatermanReportCallback
{
    Q_OBJECT
public:
    enum WhatDoYouWantFromMe {
        NotDefined = 0,
        SequenceView_Search = 1,
        MSA_Alignment_InNewWindow = 2,
        MSA_Alignment_InCurrentWindow = 3
    };

    struct TagExpansionPossibleData {
                                TagExpansionPossibleData();
                                TagExpansionPossibleData(const QString &_refSequenceName,
                                    const QString &_patternName);

        const QString           refSequenceName;
        const QString           patternName;
        U2Region *              curProcessingSubseq;
    };

                                //Smith-Waterman search in sequence viewer
                                SmithWatermanReportCallbackMAImpl(const QString &resultDirPath,
                                    const QString &mobjectNamesTemplate,
                                    const QString &refSubseqTemplate,
                                    const QString &ptrnSubseqTemplate,
                                    const QByteArray &refSequence,
                                    const QByteArray &pattern, const QString &refSeqName,
                                    const QString &patternName, const DNAAlphabet *alphabet,
                                    const DNATranslation* amitoTT = NULL,
                                    WhatDoYouWantFromMe plan = SequenceView_Search);

                                //Smith-Waterman alignment in MSA Editor (in new window)
                                SmithWatermanReportCallbackMAImpl(const QString &resultDirPath,
                                    const QString &mobjectName,
                                    const U2EntityRef &firstSequenceRef,
                                    const U2EntityRef &secondSequenceRef,
                                    const U2EntityRef &sourceMsaRef,
                                    WhatDoYouWantFromMe plan = MSA_Alignment_InNewWindow);

                                //Smith-Waterman alignment in MSA Editor (in current window)
                                SmithWatermanReportCallbackMAImpl(
                                    const U2EntityRef &firstSequenceRef,
                                    const U2EntityRef &secondSequenceRef,
                                    const U2EntityRef &sourceMsaRef,
                                    WhatDoYouWantFromMe plan = MSA_Alignment_InCurrentWindow);

    virtual QString             report(const QList<SmithWatermanResult> &results);
    static void                 alignSequences(QByteArray &refSequenceData,
                                    QByteArray &ptrnSequence,
                                    const QByteArray &pairwiseAlignment);
    static void                 alignSequences(QList<U2MsaGap> &refSequenceGapModel,
                                    QList<U2MsaGap> &ptrnSequenceGapModel,
                                    const QByteArray &pairwiseAlignment);
    static void                 changeGivenUrlIfDocumentExists(QString &givenUrl,
                                    const Project *curProject);

private:
    QString                     planFor_SequenceView_Search(
                                    const QList<SmithWatermanResult> &results);
    QString                     planFor_MSA_Alignment_InNewWindow(
                                    const QList<SmithWatermanResult> &results);
    QString                     planFor_MSA_Alignment_InCurrentWindow(
                                    const QList<SmithWatermanResult> &results);

private:
    WhatDoYouWantFromMe         plan;       //determine actions to be perfomed

    QString                     resultDirPath;
    QString                     mobjectNamesTemplate;
    QString                     refSubseqTemplate;
    QString                     ptrnSubseqTemplate;
    QByteArray                  refSequenceData;
    QByteArray                  ptrnSequenceData;
    const DNAAlphabet *         alphabet;
    const DNATranslation*       aminoTT; // not null if result must be translated.
    TagExpansionPossibleData    expansionInfo;

    U2EntityRef                 firstSequenceRef;
    U2EntityRef                 secondSequenceRef;
    U2EntityRef                 sourceMsaRef;
    QString                     mobjectName;
    DbiConnection               sourceMsaConnection;

    static const quint8         countOfSimultLoadedMADocs;
};

} // namespace

#endif
