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
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "GenbankLocationParser.h"
#include "VectorNtiSequenceFormat.h"

namespace U2 {

VectorNtiSequenceFormat::VectorNtiSequenceFormat(QObject* parent)
    : GenbankPlainTextFormat(parent)
{
    id = BaseDocumentFormats::VECTOR_NTI_SEQUENCE;
    formatName = tr("Vector NTI sequence");
    formatDescription = tr("Vector NTI sequence format is a rich format based on NCBI GenBank format for storing sequences and associated annotations");
    fileExtensions << "gb" << "gp";
}

FormatCheckResult VectorNtiSequenceFormat::checkRawTextData(const QByteArray &rawData, const GUrl &) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    bool textOnly = !TextUtils::contains(TextUtils::BINARY, data, size);
    if (!textOnly || size < 100) {
        return FormatDetection_NotMatched;
    }

    bool hasLocus = rawData.contains("\nLOCUS ") || rawData.startsWith("LOCUS ");
    bool hasVectorNtiMark = rawData.contains("COMMENT     This file is created by Vector NTI") ||
            rawData.contains("COMMENT     This Vector NTI compatible file is created by UGENE");
    if (!hasLocus || !hasVectorNtiMark) {
        return FormatDetection_NotMatched;
    }

    FormatCheckResult res(FormatDetection_Matched);

    QByteArray seqStartPattern1 = "\n        1";
    QByteArray seqStartPattern2 = "\nORIGIN";

    res.properties[RawDataCheckResult_Sequence] = rawData.contains(seqStartPattern1) || rawData.contains(seqStartPattern2);

    bool multi = (rawData.indexOf(seqStartPattern1) != rawData.lastIndexOf(seqStartPattern1))
        || (rawData.indexOf(seqStartPattern2) != rawData.lastIndexOf(seqStartPattern2));
    res.properties[RawDataCheckResult_MultipleSequences] = multi;
    return res;
}

void VectorNtiSequenceFormat::storeEntry(IOAdapter *io, const QMap<GObjectType, QList<GObject *> > &objectsMap, U2OpStatus &os) {
    U2SequenceObject *seq = NULL;
    QList<GObject*> anns;
    if (objectsMap.contains(GObjectTypes::SEQUENCE)) {
        const QList<GObject*> &seqs = objectsMap[GObjectTypes::SEQUENCE];
        SAFE_POINT(1 >= seqs.size(), "Vector NTI entry storing: sequence objects count error", );
        if (1 == seqs.size()) {
            seq = dynamic_cast<U2SequenceObject *>(seqs.first());
        }
    }
    SAFE_POINT(NULL != seq, "Vector NTI entry storing: NULL sequence object", );

    if (objectsMap.contains(GObjectTypes::ANNOTATION_TABLE)) {
        anns = objectsMap[GObjectTypes::ANNOTATION_TABLE];
    }

    //reading header attribute
    QString locusFromAttributes;
    QString gbHeader;

    DbiConnection con(seq->getSequenceRef().dbiRef, os);
    CHECK_OP(os, );
    U2AttributeDbi *attributeDbi = con.dbi->getAttributeDbi();
    U2StringAttribute attr = U2AttributeUtils::findStringAttribute(attributeDbi, seq->getSequenceRef().entityId, DNAInfo::GENBANK_HEADER, os);
    if (attr.hasValidId()) {
        gbHeader = attr.value;
    }

    if (gbHeader.startsWith("LOCUS")) { //trim the first line
        int locusStringEndIndex = gbHeader.indexOf("\n");
        assert(locusStringEndIndex != -1);
        locusFromAttributes = gbHeader.left(locusStringEndIndex);
        gbHeader = gbHeader.mid(locusStringEndIndex + 1);
    }

    // write mandatory locus string
    QString locusString = genLocusString(anns, seq, locusFromAttributes);
    if (!writeKeyword(io, os, DNAInfo::LOCUS, locusString, false)) {
        return;
    }
    // write other keywords

    //header
    io->writeBlock(gbHeader.toLocal8Bit());

    //write tool mark
    QList<GObject*> annsAndSeqObjs;
    annsAndSeqObjs << anns;
    annsAndSeqObjs << seq;

    if (!annsAndSeqObjs.isEmpty()) {
        QString unimark = annsAndSeqObjs[0]->getGObjectName();
        if (!writeKeyword(io, os, UGENE_MARK, unimark, false)) {
            return;
        }

        for (int x = 1; x < annsAndSeqObjs.size(); x++) {
            if (!writeKeyword(io, os, QString(), annsAndSeqObjs[x]->getGObjectName(), false)) {
                return;
            }
        }
    }

    // write annotations
    if (!anns.isEmpty()) {
        SAFE_POINT_EXT(NULL != seq->getAlphabet(), os.setError(L10N::nullPointerError("sequence alphabet")), );
        const bool isAmino = seq->getAlphabet()->isAmino();
        writeAnnotations(io, anns, isAmino, os);
        CHECK_OP(os, );
    }

    QList<U2Region> lowerCaseRegs = U1AnnotationUtils::getRelatedLowerCaseRegions(seq, anns);
    writeSequence(io, seq, lowerCaseRegs, os);
    CHECK_OP(os, );

    // write last line marker
    QByteArray lastLine("//\n");
    qint64 len = io->writeBlock(lastLine);
    if (len != lastLine.size()) {
        os.setError(L10N::errorWritingFile(io->getURL()));
        return;
    }
}

QList<GenbankPlainTextFormat::StrPair> VectorNtiSequenceFormat::processCommentKeys(QMultiMap<QString, QVariant> &tags) {
    QList<StrPair> res;
    QStringList comments;

    while (tags.contains(DNAInfo::COMMENT)) {
        const QVariant v = tags.take(DNAInfo::COMMENT);
        CHECK_EXT(v.canConvert<QStringList>(), coreLog.info("Unexpected COMMENT section"), res);
        comments << v.value<QStringList>();
    }

    foreach (QString comment, comments) {
        CHECK_BREAK(!comment.contains("Vector_NTI_Display_Data"));
        res << qMakePair(DNAInfo::COMMENT, comment.replace("\n", "\n" + QString(VAL_OFF, ' ')));
    }

    return res;
}

void VectorNtiSequenceFormat::createCommentAnnotation(const QStringList &comments, int sequenceLength, AnnotationTableObject *annTable) const {
    const StrStrMap parsedComments = parseComments(comments);
    CHECK(!parsedComments.isEmpty(), );

    SharedAnnotationData f(new AnnotationData);
    f->type = U2FeatureTypes::Comment;
    f->name = "comment";
    f->location->regions.append(U2Region(0, sequenceLength));
    foreach (const QString &qualName, parsedComments.keys()) {
        f->qualifiers.append(U2Qualifier(qualName, parsedComments[qualName]));
    }

    annTable->addAnnotations(QList<SharedAnnotationData>() << f, "comment");
}

U2Qualifier VectorNtiSequenceFormat::createQualifier(const QString &qualifierName, const QString &qualifierValue, bool containsDoubleQuotes) const {
    QString parsedQualifierValue = qualifierValue;
    if ("label" == qualifierName) {
        parsedQualifierValue.replace(QString("\\"), " ");
    }
    return EMBLGenbankAbstractDocument::createQualifier(qualifierName, parsedQualifierValue, containsDoubleQuotes);
}

U2FeatureType VectorNtiSequenceFormat::getFeatureType(const QString &typeString) const {
    if (proteinFeatureType2StringMap.values().contains(typeString)) {
        return proteinFeatureTypesMap.key(proteinFeatureType2StringMap.key(typeString, ProteinMiscFeature), U2FeatureTypes::MiscFeature);
    } else {
        return dnaFeatureTypesMap.key(dnaFeatureType2StringMap.key(typeString, DnaMiscFeature), U2FeatureTypes::MiscFeature);
    }
}

QString VectorNtiSequenceFormat::getFeatureTypeString(U2FeatureType featureType, bool isAmino) const {
    if (isAmino) {
        return proteinFeatureType2StringMap.value(proteinFeatureTypesMap.value(featureType, ProteinMiscFeature), DEFAULT_FEATURE_TYPE_NAME);
    } else {
        return dnaFeatureType2StringMap.value(dnaFeatureTypesMap.value(featureType, DnaMiscFeature), DEFAULT_FEATURE_TYPE_NAME);
    }
}

StrStrMap VectorNtiSequenceFormat::parseComments(const QStringList &comments) const {
    // TODO: not all comment keys are precessed
    StrStrMap result;
    int commentsCounter = 1;

    foreach (const QString &comment, comments) {
        if (Q_UNLIKELY(comment.contains("Vector_NTI_Display_Data"))) {
            // Vector NTI display data are not saved:
            // UGENE can't keep it valid if objects are modified
            // Vector NTI should recalculate data
            break;
        }

        const int sepIndex = comment.indexOf('|');
        const QString vntiKey = comment.left(sepIndex);

        if (-1 < sepIndex && vntiMetaKeys.contains(vntiKey)) {
            QString value = comment.mid(sepIndex + 1, comment.length() - vntiKey.length() - 1);
            value = !value.isEmpty() && value[value.length() - 1] == '|' ? value.mid(0, value.length() - 1) : value;
            if (Q_UNLIKELY(vntiCreationDateKey == vntiKey || vntiModificationDateKey == vntiKey)) {
                value = parseDate(value.toInt());
            }
            result[vntiMetaKeys[vntiKey]] = value;
        } else {
            result[QString("%1").arg(commentsCounter++, 2, 10, QChar('0'))] = comment;
        }
    }

    return result;
}

QString VectorNtiSequenceFormat::parseDate(int date) {
    static const QDateTime startTime(QDate(1994, 11, 30));
    static const int secsInYear = 35942400;
    static const int secsInMonth = 2764800;

    CHECK(date >= 0, "");

    const int years = date / secsInYear;
    date = date % secsInYear;
    const int months = date / secsInMonth;
    const int secs = date % secsInMonth;

    QDateTime time = startTime.addYears(years);
    time = time.addMonths(months);
    time = time.addSecs(secs);

    return time.toString();
}

QList<SharedAnnotationData> VectorNtiSequenceFormat::prepareAnnotations(const QList<GObject *> &tablesList, bool isAmino, U2OpStatus &os) const {
    QMap<AnnotationGroup *, QList<SharedAnnotationData> > annotationsByGroups;
    foreach (GObject *object, tablesList) {
        AnnotationTableObject *atObject = qobject_cast<AnnotationTableObject *>(object);
        CHECK_EXT(NULL != atObject, os.setError("Invalid annotation table"), QList<SharedAnnotationData>());
        foreach (Annotation *annotation, atObject->getAnnotations()) {
            annotationsByGroups[annotation->getGroup()] << annotation->getData();
        }
    }
    CHECK(!annotationsByGroups.isEmpty(), QList<SharedAnnotationData>());

    prepareQualifiersToWrite(annotationsByGroups, isAmino);

    QList<SharedAnnotationData> sortedAnnotations;
    foreach (AnnotationGroup *group, annotationsByGroups.keys()) {
        sortedAnnotations += annotationsByGroups[group];
    }
    qStableSort(sortedAnnotations.begin(), sortedAnnotations.end());

    return sortedAnnotations;
}

void VectorNtiSequenceFormat::writeAnnotations(IOAdapter *io, const QList<GObject *> &aos, bool isAmino, U2OpStatus &os) {
    CHECK(!aos.isEmpty(), );
    QByteArray header("FEATURES             Location/Qualifiers\n");

    //write "FEATURES"
    qint64 len = io->writeBlock(header);
    CHECK_EXT(len == header.size(), os.setError(tr("Error writing document")), );

    //write every feature
    const char *spaceLine = TextUtils::SPACE_LINE.data();
    QList<SharedAnnotationData> sortedAnnotations = prepareAnnotations(aos, isAmino, os);
    CHECK_OP(os, );

    for (int i = 0; i < sortedAnnotations.size(); ++i) {
        const SharedAnnotationData &a = sortedAnnotations.at(i);

        if (a->name == U1AnnotationUtils::lowerCaseAnnotationName || a->name == U1AnnotationUtils::upperCaseAnnotationName || a->name == "comment") {
            continue;
        }

        // write name of the feature
        len = io->writeBlock(spaceLine, 5);
        CHECK_EXT(len == 5, os.setError(tr("Error writing document")), );

        const QString keyStr = getFeatureTypeString(a->type, isAmino);
        len = io->writeBlock(keyStr.toLocal8Bit());
        CHECK_EXT(len == keyStr.length(), os.setError(tr("Error writing document")), );

        int nspaces = 22 - keyStr.length() - 6;
        assert(nspaces > 0);
        len = io->writeBlock(spaceLine, nspaces);
        CHECK_EXT(len == nspaces, os.setError(tr("Error writing document")), );

        //write location
        QString multiLineLocation = U1AnnotationUtils::buildLocationString(a);
        prepareMultiline(multiLineLocation, 21);
        len = io->writeBlock(multiLineLocation.toLocal8Bit());
        CHECK_EXT(len == multiLineLocation.size(), os.setError(tr("Error writing document")), );

        //write qualifiers
        foreach(const U2Qualifier &q, a->qualifiers) {
            writeQualifier(q.name, q.value, io, os, spaceLine);
            CHECK_OP(os, );
        }
    }
}

void VectorNtiSequenceFormat::prepareQualifiersToWrite(QMap<AnnotationGroup *, QList<SharedAnnotationData> > &annotationsByGroups, bool isAmino) const {
    foreach (AnnotationGroup *group, annotationsByGroups.keys()) {
        QList<SharedAnnotationData> &annotations = annotationsByGroups[group];
        for (int i = 0; i < annotations.size(); i++) {
            SharedAnnotationData &annotation = annotations[i];

            bool labelExists = false;
            QVector<U2Qualifier> qualifiers;

            foreach(const U2Qualifier &qualifier, annotation->qualifiers) {
                if (VNTIFKEY_QUALIFIER_NAME == qualifier.name || GBFeatureUtils::QUALIFIER_NAME == qualifier.name
                    || GBFeatureUtils::QUALIFIER_GROUP == qualifier.name)
                {
                    continue;
                }

                if (QUALIFIER_LABEL == qualifier.name) {
                    if (!labelExists) {
                        labelExists = true;
                        U2Qualifier labelQualifier(qualifier);
                        labelQualifier.value.replace(" ", QString("\\"));
                        qualifiers << labelQualifier;
                    }
                    continue;
                }

                qualifiers << qualifier;
            }

            if (!labelExists) {
                qualifiers << U2Qualifier(QUALIFIER_LABEL, annotation->name);
            }

            if (isAmino) {
                qualifiers << U2Qualifier(VNTIFKEY_QUALIFIER_NAME, QString::number(proteinFeatureTypesMap.value(annotation->type)));
            } else {
                qualifiers << U2Qualifier(VNTIFKEY_QUALIFIER_NAME, QString::number(dnaFeatureTypesMap.value(annotation->type)));
            }

            if (annotation->name != getFeatureTypeString(annotation->type, isAmino)) {
                qualifiers << U2Qualifier(GBFeatureUtils::QUALIFIER_NAME, annotation->name);
            }

            if (annotation->name != group->getName()) {
                qualifiers << U2Qualifier(GBFeatureUtils::QUALIFIER_GROUP, group->getGroupPath());
            }

            annotation->qualifiers = qualifiers;
        }
    }
}

const QString VectorNtiSequenceFormat::vntiCreationDateKey = "VNTDATE";
const QString VectorNtiSequenceFormat::vntiModificationDateKey = "VNTDBDATE";
const StrStrMap VectorNtiSequenceFormat::vntiMetaKeys = VectorNtiSequenceFormat::initVntiMetaKeys();
const QMap<U2FeatureType, VectorNtiSequenceFormat::VntiDnaFeatureTypes> VectorNtiSequenceFormat::dnaFeatureTypesMap
    = VectorNtiSequenceFormat::initDnaFeatureTypesMap();
const QMap<U2FeatureType, VectorNtiSequenceFormat::VntiProteinFeatureTypes> VectorNtiSequenceFormat::proteinFeatureTypesMap
    = VectorNtiSequenceFormat::initProteinFeatureTypesMap();
const QMap<VectorNtiSequenceFormat::VntiDnaFeatureTypes, QString> VectorNtiSequenceFormat::dnaFeatureType2StringMap
    = VectorNtiSequenceFormat::initDnaFeatureType2StringMap();
const QMap<VectorNtiSequenceFormat::VntiProteinFeatureTypes, QString> VectorNtiSequenceFormat::proteinFeatureType2StringMap
    = VectorNtiSequenceFormat::initProteinFeatureType2StringMap();
const QString VectorNtiSequenceFormat::DEFAULT_FEATURE_TYPE_NAME
    = VectorNtiSequenceFormat::dnaFeatureType2StringMap[VectorNtiSequenceFormat::DnaMiscFeature];
const QString VectorNtiSequenceFormat::QUALIFIER_LABEL = "label";
const QString VectorNtiSequenceFormat::VNTIFKEY_QUALIFIER_NAME = "vntifkey";

StrStrMap VectorNtiSequenceFormat::initVntiMetaKeys() {
    StrStrMap vntiMetaKeys;
    vntiMetaKeys["LSOWNER"]                 = "Owner";
    vntiMetaKeys["VNTNAME"]                 = "Object name";
    vntiMetaKeys["VNTAUTHORNAME"]           = "Author name";
    vntiMetaKeys["VNTAUTHORTEL"]            = "Author telephone";
    vntiMetaKeys["VNTAUTHORFAX"]            = "Author fax";
    vntiMetaKeys["VNTAUTHOREML"]            = "Author e-mail";
    vntiMetaKeys["VNTAUTHORWWW"]            = "Author www";
    vntiMetaKeys[vntiCreationDateKey]       = "Creation date";
    vntiMetaKeys[vntiModificationDateKey]   = "Last modification date";
    vntiMetaKeys["VNTAUTHORAD1"]            = "Author: additional info";
    vntiMetaKeys["VNTAUTHORAD2"]            = "Author: additional info";
    vntiMetaKeys["VNTAUTHORAD3"]            = "Author: additional info";
    vntiMetaKeys["VNTAUTHORAD4"]            = "Author: additional info";
    vntiMetaKeys["ORIGDB"]                  = "Original database";

    // Unknown keys:
//    "VNTREPLTYPE" - possible values: "Plasmid", "Virus"
//    "VNTEXTCHREPL" - possible values: "Bacteria", "Yeast", "Animal/Other Eukaryotic", ect.
//    "VNTKW" - possible values: "pMB1", "ampR", "URA3", "ARS1", "CEN4", "tetO-CYC1", "Updated 1/29/98 MB3", "ATCC 87661", etc.

    return vntiMetaKeys;
}

QMap<U2FeatureType, VectorNtiSequenceFormat::VntiDnaFeatureTypes> VectorNtiSequenceFormat::initDnaFeatureTypesMap() {
    QMap<U2FeatureType, VntiDnaFeatureTypes> dnaFeatureTypesMap;

    dnaFeatureTypesMap.insert(U2FeatureTypes::Allele,               DnaAllele);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Attenuator,           DnaAttenuator);
    dnaFeatureTypesMap.insert(U2FeatureTypes::BHlhDomain,           DnaBHlhDomain);
    dnaFeatureTypesMap.insert(U2FeatureTypes::CRegion,              DnaCRegion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::CaatSignal,           DnaCaatSignal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Cds,                  DnaCds);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Cellular,             DnaCellular);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Centromere,           DnaCentromere);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Conflict,             DnaConflict);
    dnaFeatureTypesMap.insert(U2FeatureTypes::DLoop,                DnaDLoop);
    dnaFeatureTypesMap.insert(U2FeatureTypes::DSegment,             DnaDSegment);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Enhancer,             DnaEnhancer);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Exon,                 DnaExon);
    dnaFeatureTypesMap.insert(U2FeatureTypes::FivePrimeClip,        DnaFivePrimeClip);
    dnaFeatureTypesMap.insert(U2FeatureTypes::FivePrimeUtr,         DnaFivePrimeUtr);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Gap,                  DnaGap);
    dnaFeatureTypesMap.insert(U2FeatureTypes::GcSignal,             DnaGcSignal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Gene,                 DnaGene);
    dnaFeatureTypesMap.insert(U2FeatureTypes::GlycosylationSite,    DnaGlycosylationSite);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Homeodomain,          DnaHomeodomain);
    dnaFeatureTypesMap.insert(U2FeatureTypes::IDna,                 DnaIDna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Insertion,            DnaInsertion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Intron,               DnaIntron);
    dnaFeatureTypesMap.insert(U2FeatureTypes::JSegment,             DnaJSegment);
    dnaFeatureTypesMap.insert(U2FeatureTypes::LeucineZipperDomain,  DnaLeucineZipperDomain);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Loci,                 DnaLoci);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Ltr,                  DnaLtr);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MRna,                 DnaMRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MaturePeptide,        DnaMaturePeptide);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscBindingSite,      DnaMiscBindingSite);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscDifference,       DnaMiscDifference);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscFeature,          DnaMiscFeature);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscMarker,           DnaMiscMarker);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscRecombination,    DnaMiscRecombination);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscRna,              DnaRnaMisc);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscSignal,           DnaMiscSignal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::MiscStructure,        DnaMiscStructure);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ModifiedBase,         DnaModifiedBase);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Mutation,             DnaMutation);
    dnaFeatureTypesMap.insert(U2FeatureTypes::NRegion,              DnaNRegion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::OldSequence,          DnaOldSequence);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Operon,               DnaOperon);
    dnaFeatureTypesMap.insert(U2FeatureTypes::OriT,                 DnaOriT);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Overhang,             DnaOverhang);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PolyASignal,          DnaPolyASignal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PolyASite,            DnaPolyASite);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PrecursorRna,         DnaPrecursorRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Primer,               DnaPrimer);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PrimerBindingSite,    DnaPrimerBindingSite);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PrimaryTranscript,    DnaPrimerTranscript);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PromoterEukaryotic,   DnaPromoterEukaryotic);
    dnaFeatureTypesMap.insert(U2FeatureTypes::PromoterProkaryotic,  DnaPromoterProkaryotic);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ProteinBindingSite,   DnaProteinBindingSite);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Provirus,             DnaProvirus);
    dnaFeatureTypesMap.insert(U2FeatureTypes::RRna,                 DnaRRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Rbs,                  DnaRbs);
    dnaFeatureTypesMap.insert(U2FeatureTypes::RepeatRegion,         DnaRepeatRegion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::RepeatUnit,           DnaRepeatUnit);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ReplicationOrigin,    DnaReplicationOrigin);
    dnaFeatureTypesMap.insert(U2FeatureTypes::SRegion,              DnaSRegion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Satellite,            DnaSatellite);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ScRna,                DnaScRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Minus10Signal,        DnaMinus10Signal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Minus35Signal,        DnaMinus35Signal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::SignalPeptide,        DnaSignalPeptide);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Silencer,             DnaSilencer);
    dnaFeatureTypesMap.insert(U2FeatureTypes::SnRna,                DnaSnRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::SnoRna,               DnaSnoRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Source,               DnaSource);
    dnaFeatureTypesMap.insert(U2FeatureTypes::SplicingSignal,       DnaSplicingSignal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::StemLoop,             DnaStemLoop);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Sts,                  DnaSts);
    dnaFeatureTypesMap.insert(U2FeatureTypes::TRna,                 DnaTRna);
    dnaFeatureTypesMap.insert(U2FeatureTypes::TataSignal,           DnaTataSignal);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Telomere,             DnaTelomere);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Terminator,           DnaTerminator);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ThreePrimeClip,       DnaThreePrimeClip);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ThreePrimeUtr,        DnaThreePrimeUtr);
    dnaFeatureTypesMap.insert(U2FeatureTypes::TransitPeptide,       DnaTransitPeptide);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Transposon,           DnaTransposon);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Unsure,               DnaUnsure);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Variation,            DnaVariation);
    dnaFeatureTypesMap.insert(U2FeatureTypes::VRegion,              DnaVRegion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::VSegment,             DnaVSegment);
    dnaFeatureTypesMap.insert(U2FeatureTypes::Virion,               DnaVirion);
    dnaFeatureTypesMap.insert(U2FeatureTypes::ZincFingerDomain,     DnaZincFingerDomain);

    return dnaFeatureTypesMap;
}

QMap<U2FeatureType, VectorNtiSequenceFormat::VntiProteinFeatureTypes> VectorNtiSequenceFormat::initProteinFeatureTypesMap() {
    QMap<U2FeatureType, VntiProteinFeatureTypes> proteinFeatureTypesMap;

    proteinFeatureTypesMap.insert(U2FeatureTypes::AaRich,                       ProteinAaRich);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Acetylation,                  ProteinAcetylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::ActiveSite,                   ProteinActiveSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Adenylation,                  ProteinAdenylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::AlphaHelix,                   ProteinAlphaHelix);
    proteinFeatureTypesMap.insert(U2FeatureTypes::AlteredSite,                  ProteinAlteredSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Amidation,                    ProteinAmidation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Basic,                        ProteinBasic);
    proteinFeatureTypesMap.insert(U2FeatureTypes::BetaSheet,                    ProteinBetaSheet);
    proteinFeatureTypesMap.insert(U2FeatureTypes::BetaStrandRegion,             ProteinBetaStrandRegion);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscBindingSite,              ProteinBindingSiteMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::BiotinBindingSite,            ProteinBiotinBindingSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Blocked,                      ProteinBlocked);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscBond,                     ProteinBondMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::C2,                           ProteinC2);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Calcium,                      ProteinCalcium);
    proteinFeatureTypesMap.insert(U2FeatureTypes::CatalyticRegion,              ProteinCatalyticRegion);
    proteinFeatureTypesMap.insert(U2FeatureTypes::CellAttachment,               ProteinCellAttachment);
    proteinFeatureTypesMap.insert(U2FeatureTypes::CholesterolBindingSite,       ProteinCholesterolBindingSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::CleavageSite,                 ProteinCleavageSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::CoiledCoil,                   ProteinCoiledCoil);
    proteinFeatureTypesMap.insert(U2FeatureTypes::CollagenType,                 ProteinCollagenType);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Conflict,                     ProteinConflict);
    proteinFeatureTypesMap.insert(U2FeatureTypes::ConnectingPeptide,            ProteinConnectingPeptide);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Cub,                          ProteinCub);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Cytoplasmic,                  ProteinCytoplasmic);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Disulfide,                    ProteinDisulfide);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscDnaRnaBindingRegion,      ProteinDnaRnaBindingRegionMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscDomain,                   ProteinDomainMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Egf,                          ProteinEgf);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Exoplasmic,                   ProteinExoplasmic);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Extracellular,                ProteinExtracellular);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Farnesyl,                     ProteinFarnesyl);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Fibronectin,                  ProteinFibronectin);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Formylation,                  ProteinFormylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::GammaCarboxyglumaticAcid,     ProteinGammaCarboxyglumaticAcid);
    proteinFeatureTypesMap.insert(U2FeatureTypes::GeranylGeranyl,               ProteinGeranylGeranyl);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Glycosylation,                ProteinGlycosylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::GpiAnchor,                    ProteinGpiAnchor);
    proteinFeatureTypesMap.insert(U2FeatureTypes::HelicalRegion,                ProteinHelicalRegion);
    proteinFeatureTypesMap.insert(U2FeatureTypes::HemeBindingSite,              ProteinHemeBindingSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::HmgBox,                       ProteinHmgBox);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Homeodomain,                  ProteinHomeodomain);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Hth,                          ProteinHth);
    proteinFeatureTypesMap.insert(U2FeatureTypes::HydrogenBondedTurn,           ProteinHydrogenBondedTurn);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Hydroxylation,                ProteinHydroxylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Immunoglobulin,               ProteinImmunoglobulin);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Intracellular,                ProteinIntracellular);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Kh,                           ProteinKh);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Kinase,                       ProteinKinase);
    proteinFeatureTypesMap.insert(U2FeatureTypes::LeucineZipper,                ProteinLeucineZipper);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscLipid,                    ProteinLipidMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MatureChain,                  ProteinMatureChain);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscMetal,                    ProteinMetalMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Methylation,                  ProteinMethylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscFeature,                  ProteinMiscFeature);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Myristate,                    ProteinMyristate);
    proteinFeatureTypesMap.insert(U2FeatureTypes::NAcylDiglyceride,             ProteinNAcylDiglyceride);
    proteinFeatureTypesMap.insert(U2FeatureTypes::NonConsecutiveResidues,       ProteinNonConsecutiveResidues);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscNpBindingRegion,          ProteinNpBindingRegionMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Nuclease,                     ProteinNuclease);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Palmitate,                    ProteinPalmitate);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Periplasmic,                  ProteinPeriplasmic);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Ph,                           ProteinPh);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Phosphorylation,              ProteinPhosphorylation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::PolyAa,                       ProteinPolyAa);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Precursor,                    ProteinPrecursor);
    proteinFeatureTypesMap.insert(U2FeatureTypes::ProcessedActivePeptide,       ProteinProcessedActivePeptide);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Propeptide,                   ProteinPropeptide);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Proprotein,                   ProteinProprotein);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Protease,                     ProteinProtease);
    proteinFeatureTypesMap.insert(U2FeatureTypes::PyridoxalPhBindingSite,       ProteinPyridoxalPhBindingSite);
    proteinFeatureTypesMap.insert(U2FeatureTypes::PyrrolidoneCarboxylicAcid,    ProteinPyrrolidoneCarboxylicAcid);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscRegion,                   ProteinRegionMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::RepetitiveRegion,             ProteinRepetitiveRegion);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscResidueModification,      ProteinResidueModificationMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::SeconadaryStructure,          ProteinSeconadaryStructure);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Sh2,                          ProteinSh2);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Sh3,                          ProteinSh3);
    proteinFeatureTypesMap.insert(U2FeatureTypes::SignalSequence,               ProteinSignalSequence);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Similarity,                   ProteinSimilarity);
    proteinFeatureTypesMap.insert(U2FeatureTypes::MiscSite,                     ProteinSiteMisc);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Source,                       ProteinSource);
    proteinFeatureTypesMap.insert(U2FeatureTypes::SplicingVariant,              ProteinSplicingVariant);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Sulfatation,                  ProteinSulfatation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Thioether,                    ProteinThioether);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Thiolester,                   ProteinThiolester);
    proteinFeatureTypesMap.insert(U2FeatureTypes::TransitPeptide,               ProteinTransitPeptide);
    proteinFeatureTypesMap.insert(U2FeatureTypes::TransmembraneRegion,          ProteinTransmembraneRegion);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Uncertainty,                  ProteinUncertainty);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Variant,                      ProteinVariant);
    proteinFeatureTypesMap.insert(U2FeatureTypes::Variation,                    ProteinVariation);
    proteinFeatureTypesMap.insert(U2FeatureTypes::ZincFinger,                   ProteinZincFinger);

    return proteinFeatureTypesMap;
}

QMap<VectorNtiSequenceFormat::VntiDnaFeatureTypes, QString> VectorNtiSequenceFormat::initDnaFeatureType2StringMap() {
    QMap<VntiDnaFeatureTypes, QString> dnaFeatureType2StringMap;
    dnaFeatureType2StringMap.insert(DnaAttenuator,          "attenuator");
    dnaFeatureType2StringMap.insert(DnaBHlhDomain,          "bHLH_domain");
    dnaFeatureType2StringMap.insert(DnaCRegion,             "C_region");
    dnaFeatureType2StringMap.insert(DnaCaatSignal,          "CAAT_signal");
    dnaFeatureType2StringMap.insert(DnaCds,                 "CDS");
    dnaFeatureType2StringMap.insert(DnaCentromere,          "centromere");
    dnaFeatureType2StringMap.insert(DnaDLoop,               "D-loop");
    dnaFeatureType2StringMap.insert(DnaDSegment,            "D_segment");
    dnaFeatureType2StringMap.insert(DnaEnhancer,            "enhancer");
    dnaFeatureType2StringMap.insert(DnaGcSignal,            "GC_signal");
    dnaFeatureType2StringMap.insert(DnaGlycosylationSite,   "glycosil_site");
    dnaFeatureType2StringMap.insert(DnaGene,                "gene");
    dnaFeatureType2StringMap.insert(DnaHomeodomain,         "homeodomain");
    dnaFeatureType2StringMap.insert(DnaIDna,                "iDNA");
    dnaFeatureType2StringMap.insert(DnaInsertion,           "insertion_seq");
    dnaFeatureType2StringMap.insert(DnaIntron,              "intron");
    dnaFeatureType2StringMap.insert(DnaJSegment,            "J_segment");
    dnaFeatureType2StringMap.insert(DnaLeucineZipperDomain, "LZ_domain");
    dnaFeatureType2StringMap.insert(DnaLoci,                "loci");
    dnaFeatureType2StringMap.insert(DnaLtr,                 "LTR");
    dnaFeatureType2StringMap.insert(DnaMiscBindingSite,     "misc_binding");
    dnaFeatureType2StringMap.insert(DnaMiscFeature,         "misc_feature");
    dnaFeatureType2StringMap.insert(DnaMiscMarker,          "misc_marker");
    dnaFeatureType2StringMap.insert(DnaModifiedBase,        "modified_base");
    dnaFeatureType2StringMap.insert(DnaNRegion,             "N_region");
    dnaFeatureType2StringMap.insert(DnaPolyASignal,         "polyA_signal");
    dnaFeatureType2StringMap.insert(DnaPolyASite,           "polyA_site");
    dnaFeatureType2StringMap.insert(DnaPrimer,              "primer");
    dnaFeatureType2StringMap.insert(DnaPrimerBindingSite,   "primer_bind");
    dnaFeatureType2StringMap.insert(DnaPromoterEukaryotic,  "promoter");
    dnaFeatureType2StringMap.insert(DnaPromoterProkaryotic, "promoter");
    dnaFeatureType2StringMap.insert(DnaProteinBindingSite,  "protein_bind");
    dnaFeatureType2StringMap.insert(DnaRbs,                 "RBS");
    dnaFeatureType2StringMap.insert(DnaReplicationOrigin,   "rep_origin");
    dnaFeatureType2StringMap.insert(DnaRepeatRegion,        "repeat_region");
    dnaFeatureType2StringMap.insert(DnaRepeatUnit,          "repeat_unit");
    dnaFeatureType2StringMap.insert(DnaSRegion,             "S_region");
    dnaFeatureType2StringMap.insert(DnaSilencer,            "silencer");
    dnaFeatureType2StringMap.insert(DnaSource,              "source");
    dnaFeatureType2StringMap.insert(DnaSplicingSignal,      "splicing_signal");
    dnaFeatureType2StringMap.insert(DnaStemLoop,            "stem_loop");
    dnaFeatureType2StringMap.insert(DnaSts,                 "STS");
    dnaFeatureType2StringMap.insert(DnaTataSignal,          "TATA_signal");
    dnaFeatureType2StringMap.insert(DnaTelomere,            "telomere");
    dnaFeatureType2StringMap.insert(DnaTerminator,          "terminator");
    dnaFeatureType2StringMap.insert(DnaTransposon,          "transposon");
    dnaFeatureType2StringMap.insert(DnaVRegion,             "V_region");
    dnaFeatureType2StringMap.insert(DnaZincFingerDomain,    "ZF_domain");
    dnaFeatureType2StringMap.insert(DnaMinus10Signal,       "-10_signal");
    dnaFeatureType2StringMap.insert(DnaMinus35Signal,       "-35_signal");
    dnaFeatureType2StringMap.insert(DnaThreePrimeClip,      "3'clip");
    dnaFeatureType2StringMap.insert(DnaThreePrimeUtr,       "3'UTR");
    dnaFeatureType2StringMap.insert(DnaFivePrimeClip,       "5'clip");
    dnaFeatureType2StringMap.insert(DnaFivePrimeUtr,        "5'UTR");
    dnaFeatureType2StringMap.insert(DnaRnaMisc,             "misc_RNA");
    dnaFeatureType2StringMap.insert(DnaMRna,                "mRNA");
    dnaFeatureType2StringMap.insert(DnaPrecursorRna,        "precursor_RNA");
    dnaFeatureType2StringMap.insert(DnaScRna,               "scRNA");
    dnaFeatureType2StringMap.insert(DnaSnRna,               "snRNA");
    dnaFeatureType2StringMap.insert(DnaRRna,                "rRNA");
    dnaFeatureType2StringMap.insert(DnaTRna,                "tRNA");
    dnaFeatureType2StringMap.insert(DnaExon,                "exon");
    dnaFeatureType2StringMap.insert(DnaMutation,            "mutation");
    dnaFeatureType2StringMap.insert(DnaAllele,              "allele");
    dnaFeatureType2StringMap.insert(DnaCellular,            "cellular");
    dnaFeatureType2StringMap.insert(DnaConflict,            "conflict");
    dnaFeatureType2StringMap.insert(DnaMaturePeptide,       "mat_peptide");
    dnaFeatureType2StringMap.insert(DnaMiscDifference,      "misc_difference");
    dnaFeatureType2StringMap.insert(DnaMiscRecombination,   "misc_recomb");
    dnaFeatureType2StringMap.insert(DnaMiscSignal,          "misc_signal");
    dnaFeatureType2StringMap.insert(DnaMiscStructure,       "misc_structure");
    dnaFeatureType2StringMap.insert(DnaOldSequence,         "old_sequence");
    dnaFeatureType2StringMap.insert(DnaPrimerTranscript,    "prim_transcript");
    dnaFeatureType2StringMap.insert(DnaProvirus,            "provirus");
    dnaFeatureType2StringMap.insert(DnaSatellite,           "satellite");
    dnaFeatureType2StringMap.insert(DnaSignalPeptide,       "sig_peptide");
    dnaFeatureType2StringMap.insert(DnaTransitPeptide,      "transit_peptide");
    dnaFeatureType2StringMap.insert(DnaVariation,           "variation");
    dnaFeatureType2StringMap.insert(DnaVirion,              "virion");
    dnaFeatureType2StringMap.insert(DnaUnsure,              "unsure");
    dnaFeatureType2StringMap.insert(DnaVSegment,            "V_segment");
    dnaFeatureType2StringMap.insert(DnaGap,                 "gap");
    dnaFeatureType2StringMap.insert(DnaOperon,              "operon");
    dnaFeatureType2StringMap.insert(DnaOriT,                "oriT");
    dnaFeatureType2StringMap.insert(DnaSnoRna,              "snoRNA");
    dnaFeatureType2StringMap.insert(DnaOverhang,            "overhang");

    return dnaFeatureType2StringMap;
}

QMap<VectorNtiSequenceFormat::VntiProteinFeatureTypes, QString> VectorNtiSequenceFormat::initProteinFeatureType2StringMap() {
    QMap<VntiProteinFeatureTypes, QString> proteinFeatureType2StringMap;

    proteinFeatureType2StringMap.insert(ProteinAaRich,                      "Region");
    proteinFeatureType2StringMap.insert(ProteinAcetylation,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinActiveSite,                  "Site");
    proteinFeatureType2StringMap.insert(ProteinAdenylation,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinAlphaHelix,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinAlteredSite,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinAmidation,                   "Site");
    proteinFeatureType2StringMap.insert(ProteinBasic,                       "Site");
    proteinFeatureType2StringMap.insert(ProteinBetaSheet,                   "Region");
    proteinFeatureType2StringMap.insert(ProteinBetaStrandRegion,            "Region");
    proteinFeatureType2StringMap.insert(ProteinBindingSiteMisc,             "Site");
    proteinFeatureType2StringMap.insert(ProteinBiotinBindingSite,           "Site");
    proteinFeatureType2StringMap.insert(ProteinBlocked,                     "Site");
    proteinFeatureType2StringMap.insert(ProteinBondMisc,                    "Bond");
    proteinFeatureType2StringMap.insert(ProteinC2,                          "Region");
    proteinFeatureType2StringMap.insert(ProteinCalcium,                     "Region");
    proteinFeatureType2StringMap.insert(ProteinCatalyticRegion,             "Region");
    proteinFeatureType2StringMap.insert(ProteinCellAttachment,              "Site");
    proteinFeatureType2StringMap.insert(ProteinCholesterolBindingSite,      "Site");
    proteinFeatureType2StringMap.insert(ProteinCleavageSite,                "Site");
    proteinFeatureType2StringMap.insert(ProteinCoiledCoil,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinCollagenType,                "Region");
    proteinFeatureType2StringMap.insert(ProteinConflict,                    "Region");
    proteinFeatureType2StringMap.insert(ProteinConnectingPeptide,           "Region");
    proteinFeatureType2StringMap.insert(ProteinCub,                         "Region");
    proteinFeatureType2StringMap.insert(ProteinCytoplasmic,                 "Region");
    proteinFeatureType2StringMap.insert(ProteinDisulfide,                   "Bond");
    proteinFeatureType2StringMap.insert(ProteinDnaRnaBindingRegionMisc,     "Site");
    proteinFeatureType2StringMap.insert(ProteinDomainMisc,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinEgf,                         "Region");
    proteinFeatureType2StringMap.insert(ProteinExoplasmic,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinExtracellular,               "Region");
    proteinFeatureType2StringMap.insert(ProteinFarnesyl,                    "Site");
    proteinFeatureType2StringMap.insert(ProteinFibronectin,                 "Region");
    proteinFeatureType2StringMap.insert(ProteinFormylation,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinGammaCarboxyglumaticAcid,    "Site");
    proteinFeatureType2StringMap.insert(ProteinGeranylGeranyl,              "Site");
    proteinFeatureType2StringMap.insert(ProteinGlycosylation,               "Site");
    proteinFeatureType2StringMap.insert(ProteinGpiAnchor,                   "Site");
    proteinFeatureType2StringMap.insert(ProteinHelicalRegion,               "Region");
    proteinFeatureType2StringMap.insert(ProteinHemeBindingSite,             "Site");
    proteinFeatureType2StringMap.insert(ProteinHmgBox,                      "Site");
    proteinFeatureType2StringMap.insert(ProteinHomeodomain,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinHth,                         "Site");
    proteinFeatureType2StringMap.insert(ProteinHydrogenBondedTurn,          "Region");
    proteinFeatureType2StringMap.insert(ProteinHydroxylation,               "Site");
    proteinFeatureType2StringMap.insert(ProteinImmunoglobulin,              "Region");
    proteinFeatureType2StringMap.insert(ProteinIntracellular,               "Region");
    proteinFeatureType2StringMap.insert(ProteinKh,                          "Region");
    proteinFeatureType2StringMap.insert(ProteinKinase,                      "Region");
    proteinFeatureType2StringMap.insert(ProteinLeucineZipper,               "Region");
    proteinFeatureType2StringMap.insert(ProteinLipidMisc,                   "Site");
    proteinFeatureType2StringMap.insert(ProteinMatureChain,                 "Region");
    proteinFeatureType2StringMap.insert(ProteinMetalMisc,                   "Site");
    proteinFeatureType2StringMap.insert(ProteinMethylation,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinMiscFeature,                 "misc_feat");
    proteinFeatureType2StringMap.insert(ProteinMyristate,                   "Site");
    proteinFeatureType2StringMap.insert(ProteinNAcylDiglyceride,            "Site");
    proteinFeatureType2StringMap.insert(ProteinNonConsecutiveResidues,      "Site");
    proteinFeatureType2StringMap.insert(ProteinNpBindingRegionMisc,         "Site");
    proteinFeatureType2StringMap.insert(ProteinNuclease,                    "Region");
    proteinFeatureType2StringMap.insert(ProteinPalmitate,                   "Site");
    proteinFeatureType2StringMap.insert(ProteinPeriplasmic,                 "Region");
    proteinFeatureType2StringMap.insert(ProteinPh,                          "Region");
    proteinFeatureType2StringMap.insert(ProteinPhosphorylation,             "Site");
    proteinFeatureType2StringMap.insert(ProteinPolyAa,                      "Region");
    proteinFeatureType2StringMap.insert(ProteinPrecursor,                   "Precursor");
    proteinFeatureType2StringMap.insert(ProteinProcessedActivePeptide,      "Region");
    proteinFeatureType2StringMap.insert(ProteinPropeptide,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinProprotein,                  "proprotein");
    proteinFeatureType2StringMap.insert(ProteinProtease,                    "Region");
    proteinFeatureType2StringMap.insert(ProteinPyridoxalPhBindingSite,      "Site");
    proteinFeatureType2StringMap.insert(ProteinPyrrolidoneCarboxylicAcid,   "Site");
    proteinFeatureType2StringMap.insert(ProteinRegionMisc,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinSource,                      "source");
    proteinFeatureType2StringMap.insert(ProteinRepetitiveRegion,            "Region");
    proteinFeatureType2StringMap.insert(ProteinResidueModificationMisc,     "Site");
    proteinFeatureType2StringMap.insert(ProteinSeconadaryStructure,         "SecStr");
    proteinFeatureType2StringMap.insert(ProteinSh2,                         "Region");
    proteinFeatureType2StringMap.insert(ProteinSh3,                         "Region");
    proteinFeatureType2StringMap.insert(ProteinSimilarity,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinSiteMisc,                    "Site");
    proteinFeatureType2StringMap.insert(ProteinSplicingVariant,             "Region");
    proteinFeatureType2StringMap.insert(ProteinSulfatation,                 "Site");
    proteinFeatureType2StringMap.insert(ProteinThioether,                   "Bond");
    proteinFeatureType2StringMap.insert(ProteinThiolester,                  "Bond");
    proteinFeatureType2StringMap.insert(ProteinTransitPeptide,              "Region");
    proteinFeatureType2StringMap.insert(ProteinTransmembraneRegion,         "Region");
    proteinFeatureType2StringMap.insert(ProteinUncertainty,                 "Region");
    proteinFeatureType2StringMap.insert(ProteinVariant,                     "Region");
    proteinFeatureType2StringMap.insert(ProteinVariation,                   "variation");
    proteinFeatureType2StringMap.insert(ProteinZincFinger,                  "Region");
    proteinFeatureType2StringMap.insert(ProteinSignalSequence,              "Region");

    return proteinFeatureType2StringMap;
}

}   // namespace U2
