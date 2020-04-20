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

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"

namespace U2 {

static int getIntSettings(const QVariantMap& fs, const char* sName, int defVal) {
    QVariant v = fs.value(sName);
    if (v.type()!= QVariant::Int) {
        return defVal;
    }
    return v.toInt();
}

AnnotationTableObject * DocumentFormatUtils::addAnnotationsForMergedU2Sequence(const GObjectReference& mergedSequenceRef,
                                                                               const U2DbiRef& dbiRef,
                                                                               const QStringList &contigNames,
                                                                               const QVector<U2Region> &mergedMapping,
                                                                               const QVariantMap& hints)
{
    QVariantMap objectHints;
    objectHints.insert(DocumentFormat::DBI_FOLDER_HINT, hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
    AnnotationTableObject *ao = new AnnotationTableObject("Contigs", dbiRef, objectHints);

    // save relation if mergedSequenceRef is valid
    if (mergedSequenceRef.isValid()) {
        ao->addObjectRelation(GObjectRelation(mergedSequenceRef, ObjectRole_Sequence));
    }

    //save mapping info as annotations
    QStringList::const_iterator it = contigNames.begin();
    QList<SharedAnnotationData> resultData;
    for (int i = 0; it != contigNames.end(); i++, it++) {
        SharedAnnotationData d(new AnnotationData);
        d->name = QString("contig");
        d->location->regions << mergedMapping[i];
        d->qualifiers << U2Qualifier("name", *it);
        d->qualifiers << U2Qualifier("number", QString("%1").arg(i));
        resultData.append(d);
    }
    ao->addAnnotations(resultData);
    return ao;
}

class ExtComparator {
public:
    ExtComparator(const QString& _ext) : ext(_ext) {}

    bool operator()(const DocumentFormat* f1, const DocumentFormat* f2) const {
        int v1 = f1->getSupportedDocumentFileExtensions().contains(ext) ? 1 : 0;
        int v2 = f2->getSupportedDocumentFileExtensions().contains(ext) ? 1 : 0;
        return  v2 < v1; // reverse sort -> make higher vals on the top
    }
    QString ext;
};

QList<DocumentFormatId> DocumentFormatUtils::toIds(const QList<DocumentFormat*>& formats) {
    QList<DocumentFormatId> result;
    foreach(DocumentFormat* f, formats) {
        result.append(f->getFormatId());
    }
    return result;
}

QList<AnnotationSettings*> DocumentFormatUtils::predefinedSettings() {
    QList<AnnotationSettings*> predefined;
    foreach(GBFeatureKeyInfo fi, GBFeatureUtils::allKeys()) {
        AnnotationSettings* as = new AnnotationSettings();
        as->name = fi.text;
        as->amino = fi.showOnaminoFrame;
        as->color = fi.color;
        as->visible = as->name!="source";
        as->nameQuals = fi.namingQuals;
        if (!fi.namingQuals.isEmpty()) {
            as->showNameQuals = true;
        }
        predefined.append(as);
    }
    AnnotationSettings* secStructAnnotationSettings = new  AnnotationSettings(BioStruct3D::SecStructAnnotationTag, true, QColor(102,255, 0), true);
    secStructAnnotationSettings->nameQuals.append(BioStruct3D::SecStructTypeQualifierName);
    secStructAnnotationSettings->showNameQuals = true;
    predefined.append(secStructAnnotationSettings);
    predefined.append(new AnnotationSettings(BioStruct3D::AlphaHelixAnnotationTag, true, QColor(102,255, 0), true));
    predefined.append(new AnnotationSettings(BioStruct3D::BetaStrandAnnotationTag, true, QColor(255,255,153), true));
    predefined.append(new AnnotationSettings(BioStruct3D::TurnAnnotationTag, true, QColor(255,85,127), true));
    predefined.append(new AnnotationSettings(BioStruct3D::MoleculeAnnotationTag, false, QColor(0,255,0), false));
    predefined.append(new AnnotationSettings(DNAInfo::COMMENT.toLower(), false, QColor(255,85,127),false));
    return predefined;
}

QList<DNASequence> DocumentFormatUtils::toSequences(const GObject* obj) {
    QList<DNASequence> res;
    SAFE_POINT(obj != NULL, "Gobject is NULL", res);
    const U2SequenceObject* seqObj = qobject_cast<const U2SequenceObject*>(obj);
    U2OpStatus2Log os;
    if (seqObj != NULL) {
        res << seqObj->getWholeSequence(os);
        CHECK_OP_EXT(os, res.removeLast(), res);
        return res;
    }
    const MultipleSequenceAlignmentObject* maObj = qobject_cast<const MultipleSequenceAlignmentObject*>(obj);
    CHECK(maObj != NULL, res); //MultipleSequenceAlignmentObject is NULL
    const DNAAlphabet* al = maObj->getAlphabet();
    qint64 alLen = maObj->getMsa()->getLength();
    foreach (const MultipleSequenceAlignmentRow& row, maObj->getMsa()->getMsaRows()) {
        DNASequence seq;
        seq.seq = row->toByteArray(os, alLen);
        seq.setName(row->getName());
        seq.alphabet = al;
        res << seq;
    }
    return res;
}

// This property is replaced with (DocumentReadingMode_SequenceMergeGapSize "merge-gap") today
// we keep this property for compatibility with previous version of UGENE only
#define MERGE_MULTI_DOC_GAP_SIZE_SETTINGS_DEPRECATED   "merge_gap"

int DocumentFormatUtils::getMergeGap(const QVariantMap& hints) {
    int res = getIntSettings(hints, DocumentReadingMode_SequenceMergeGapSize, -1);
    if (res == -1)  {
        res = getIntSettings(hints, MERGE_MULTI_DOC_GAP_SIZE_SETTINGS_DEPRECATED, -1);
    }
    return res;
}


int DocumentFormatUtils::getMergedSize(const QVariantMap& hints, int defaultVal) {
    int res = getIntSettings(hints, DocumentReadingMode_SequenceMergingFinalSizeHint, defaultVal);
    return res;
}

void DocumentFormatUtils::updateFormatHints(QList<GObject*>& objects, QVariantMap& fs) {
    //1. remove all cached sequence sizes
    //2. add new sizes
    QList<GObject*> sequences;
    foreach(GObject* obj, objects) {
        if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
            sequences.append(obj);
        }
    }
    if (sequences.size() == 1) {
        U2SequenceObject* so = qobject_cast<U2SequenceObject*>(sequences.first());
        int len = so->getSequenceLength();
        fs[DocumentReadingMode_SequenceMergingFinalSizeHint] = len;
    }
}

QString DocumentFormatUtils::getFormatNameById(const DocumentFormatId &formatId) {
    DocumentFormatRegistry *registry = AppContext::getDocumentFormatRegistry();
    SAFE_POINT(NULL != registry, L10N::nullPointerError("document format registry"), "");
    DocumentFormat *format = registry->getFormatById(formatId);
    SAFE_POINT(NULL != format, QString("Document format '%1' is not registered").arg(formatId), "");
    return format->getFormatName();
}

} //namespace
