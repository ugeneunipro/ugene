/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ProcessPrimer3ResultsToAnnotationsTask.h"

#include "CheckComplementTask.h"
#include "FindExonRegionsTask.h"
#include "Primer3Task.h"
#include "Primer3TaskSettings.h"
#include "PrimerPair.h"
#include "PrimerSingle.h"

#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include "U2Core/L10n.h"
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <cmath>

namespace U2 {

ProcessPrimer3ResultsToAnnotationsTask::ProcessPrimer3ResultsToAnnotationsTask(const QSharedPointer<Primer3TaskSettings>& _settings,
                                                   const QList<QSharedPointer<PrimerPair>>& _bestPairs,
                                                   const QList<QSharedPointer<PrimerPair>>& _filteredPairs,
                                                   const QList<QSharedPointer<PrimerSingle>>& _singlePrimers,
                                                   const QString& _groupName,
                                                   const QString& _annName,
                                                   const QString& _annDescription,
                                                   qint64 _sequenceLength)
    : Task(tr("Search primers to annotations"), TaskFlags_FOSE_COSC),
      settings(_settings),
      bestPairs(_bestPairs),
      filteredPairs(_filteredPairs),
      singlePrimers(_singlePrimers),
      groupName(_groupName),
      annName(_annName),
      annDescription(_annDescription),
      sequenceLength(_sequenceLength) {}

void ProcessPrimer3ResultsToAnnotationsTask::run() {
    int bestPairsSize = bestPairs.size();
    for (int i = 0; i < bestPairsSize; i++) {
        const auto& pair = bestPairs.at(i);
        CHECK_CONTINUE(!filteredPairs.contains(pair));

        QList<SharedAnnotationData> annotations;
        int productSize = pair->getProductSize() + settings->getOverhangLeft().size() + settings->getOverhangRight().size();
        if (pair->getLeftPrimer() != nullptr) {
            annotations.append(oligoToAnnotation(annName, pair->getLeftPrimer(), productSize, U2Strand::Direct));
        }
        if (pair->getInternalOligo() != nullptr) {
            annotations.append(oligoToAnnotation("internalOligo", pair->getInternalOligo(), productSize, U2Strand::Direct));
        }
        if (pair->getRightPrimer() != nullptr) {
            annotations.append(oligoToAnnotation(annName, pair->getRightPrimer(), productSize, U2Strand::Complementary));
        }
        int digitsNumberInBestPairsSize = (int)std::log10(bestPairsSize) + 1;
        QString number = QStringLiteral("%1").arg(i + 1, digitsNumberInBestPairsSize, 10, QLatin1Char('0'));
        resultAnnotations[groupName + "/pair " + number].append(annotations);
    }

    if (!singlePrimers.isEmpty()) {
        QList<SharedAnnotationData> annotations;
        for (const auto& primer : singlePrimers) {
            auto type = primer->getType();
            U2Strand s = type == OT_RIGHT ? U2Strand::Complementary : U2Strand::Direct;
            QString annotationName = type == OT_INTL ? "internalOligo" : annName;
            annotations.append(oligoToAnnotation(annotationName, primer, 0, s));
        }
        U1AnnotationUtils::addDescriptionQualifier(annotations, annDescription);

        if (!annotations.isEmpty()) {
            resultAnnotations[groupName].append(annotations);
        }
    }

}

const QMap<QString, QList<SharedAnnotationData>>& ProcessPrimer3ResultsToAnnotationsTask::getResultAnnotations() const {
    return resultAnnotations;
}

SharedAnnotationData ProcessPrimer3ResultsToAnnotationsTask::oligoToAnnotation(const QString& title, const QSharedPointer<PrimerSingle>& primer, int productSize, U2Strand strand) const {
    SharedAnnotationData annotationData(new AnnotationData);
    annotationData->name = title;
    annotationData->type = U2FeatureTypes::Primer;
    // primer can be found on circular extension of the sequence
    annotationData->location->regions = primer->getSequenceRegions(sequenceLength);
    if (annotationData->location->regions.size() > 1) {
        annotationData->location.data()->op = U2LocationOperator_Join;
    }

    annotationData->setStrand(strand);

    annotationData->qualifiers.append(U2Qualifier("product_size", QString::number(productSize)));
    annotationData->qualifiers.append(U2Qualifier("tm", QString::number(primer->getMeltingTemperature())));
    annotationData->qualifiers.append(U2Qualifier("gc%", QString::number(primer->getGcContent())));
    annotationData->qualifiers.append(U2Qualifier("any", QString::number(primer->getSelfAny())));
    annotationData->qualifiers.append(U2Qualifier("end", QString::number(primer->getSelfEnd())));
    annotationData->qualifiers.append(U2Qualifier("3'", QString::number(primer->getEndStability())));
    annotationData->qualifiers.append(U2Qualifier("penalty", QString::number(primer->getQuality())));

    auto areDoubleValuesEqual = [](double val, double reference) -> bool {
        return qAbs(reference - val) > 0.1;
    };
    if (areDoubleValuesEqual(primer->getBound(), OLIGOTM_ERROR)) {
        annotationData->qualifiers.append(U2Qualifier("bound%", QString::number(primer->getBound())));
    }
    if (areDoubleValuesEqual(primer->getTemplateMispriming(), ALIGN_SCORE_UNDEF)) {
        annotationData->qualifiers.append(U2Qualifier("template_mispriming", QString::number(primer->getTemplateMispriming())));
    }
    if (areDoubleValuesEqual(primer->getHairpin(), ALIGN_SCORE_UNDEF)) {
        annotationData->qualifiers.append(U2Qualifier("hairpin", QString::number(primer->getHairpin())));
    }
    auto primerType = primer->getType();
    if (primerType == oligo_type::OT_LEFT) {
        auto overhangLeft = settings->getOverhangLeft();
        if (!overhangLeft.isEmpty()) {
            annotationData->qualifiers.append(U2Qualifier("left_end_strand", "direct"));
            annotationData->qualifiers.append(U2Qualifier("left_end_type", "sticky"));
            annotationData->qualifiers.append(U2Qualifier("left_end_seq", overhangLeft));
        }
    } else if (primerType == oligo_type::OT_RIGHT) {
        auto overhangRight = settings->getOverhangRight();
        if (!overhangRight.isEmpty()) {
            annotationData->qualifiers.append(U2Qualifier("right_end_strand", "rev-compl"));
            annotationData->qualifiers.append(U2Qualifier("right_end_type", "sticky"));
            annotationData->qualifiers.append(U2Qualifier("right_end_seq", overhangRight));
        }
    }

    return annotationData;
}



}
