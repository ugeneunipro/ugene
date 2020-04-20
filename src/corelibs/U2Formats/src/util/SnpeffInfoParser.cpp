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

#include <U2Core/SnpeffDictionary.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "SnpeffInfoParser.h"

namespace U2 {

const QString SnpeffInfoParser::PAIRS_SEPARATOR = ";";
const QString SnpeffInfoParser::KEY_VALUE_SEPARATOR = "=";

SnpeffInfoParser::SnpeffInfoParser()
{
    initPartParsers();
}

SnpeffInfoParser::~SnpeffInfoParser() {
    qDeleteAll(partParsers.values());
}

QList<QList<U2Qualifier> > SnpeffInfoParser::parse(U2OpStatus &os, const QString &snpeffInfo) const {
    QList<QList<U2Qualifier> > qualifiers;
    const QStringList keyValuePairs = snpeffInfo.split(PAIRS_SEPARATOR, QString::SkipEmptyParts);
    foreach (const QString &keyValuePair, keyValuePairs) {
        const QStringList splittedKeyValuePair = keyValuePair.split(KEY_VALUE_SEPARATOR);
        if (splittedKeyValuePair.size() > 2) {
            os.addWarning(tr("Can't parse the next INFO part: '%1'").arg(keyValuePair));
            continue;
        }

        if (splittedKeyValuePair.size() == 1) {
            continue;
        }

        InfoPartParser *partParser = partParsers.value(splittedKeyValuePair.first(), NULL);
        if (NULL == partParser) {
            // This INFO part is not added by SnpEff
            continue;
        }
        qualifiers << partParser->parse(os, splittedKeyValuePair.last());
        CHECK_OP(os, qualifiers);
    }

    return qualifiers;
}

void SnpeffInfoParser::initPartParsers() {
    partParsers.insert(AnnParser::KEY_WORD, new AnnParser);
    partParsers.insert(EffParser::KEY_WORD, new EffParser);
    partParsers.insert(LofParser::KEY_WORD, new LofParser);
    partParsers.insert(NmdParser::KEY_WORD, new NmdParser);
}

const QString InfoPartParser::ERROR = "error";
const QString InfoPartParser::WARNING = "warning";
const QString InfoPartParser::INFO = "info";
const QString InfoPartParser::MESSAGE = "message";
const QString InfoPartParser::MESSAGE_DESCRIPTION = "message_desc";
const QString InfoPartParser::ANNOTATION_SEPARATOR = ",";
const QString InfoPartParser::SNPEFF_TAG = "SnpEff_tag";

InfoPartParser::InfoPartParser(const QString &keyWord, bool canStoreMessages)
    : keyWord(keyWord),
      canStoreMessages(canStoreMessages)
{

}

const QString & InfoPartParser::getKeyWord() const {
    return keyWord;
}

QList<QList<U2Qualifier> > InfoPartParser::parse(U2OpStatus &os, const QString &infoPart) const {
    QList<QList<U2Qualifier> > qualifiers;
    const QStringList entries = infoPart.split(ANNOTATION_SEPARATOR);
    foreach (const QString &entry, entries) {
        qualifiers << parseEntry(os, entry);
        CHECK_OP(os, qualifiers);
    }
    return qualifiers;
}

QList<U2Qualifier> InfoPartParser::processValue(const QString &qualifierName, const QString &value) const {
    QList<U2Qualifier> qualifiers;
    qualifiers << U2Qualifier(qualifierName, value);
    return qualifiers;
}

QList<U2Qualifier> InfoPartParser::parseEntry(U2OpStatus &os, const QString &entry) const {
    QList<U2Qualifier> qualifiers;
    const QStringList qualifierNames = getQualifierNames();
    const QStringList values = getValues(entry);
    CHECK_EXT(values.size() >= qualifierNames.size(), os.addWarning(tr("Too few values in the entry: '%1'. Expected at least %2 values.").arg(entry).arg(qualifierNames.size())), qualifiers);

    qualifiers << U2Qualifier(SNPEFF_TAG, keyWord);

    int i = 0;
    for (i = 0; i < qualifierNames.size(); i++) {
        if (!values[i].isEmpty()) {
            qualifiers << processValue(qualifierNames[i], values[i]);
        }
    }
    if (canStoreMessages) {
        for (; i < values.size(); i++) {
            if (!values[i].isEmpty()) {
                qualifiers << U2Qualifier(MESSAGE, values[i]);
                if (SnpeffDictionary::messageDescriptions.contains(values[i])) {
                    qualifiers << U2Qualifier(MESSAGE_DESCRIPTION, SnpeffDictionary::messageDescriptions[values[i]]);
                }
            }
        }
    } else if (i < values.size()) {
        os.addWarning(tr("Too many values in the entry '%1', extra entries are ignored").arg(entry));
    }
    return qualifiers;
}

const QString AnnParser::KEY_WORD = "ANN";
const QString AnnParser::VALUES_SEPARATOR = "|";
const QString AnnParser::EFFECTS_SEPARATOR = "&";
const QString AnnParser::EFFECT = "Effect";
const QString AnnParser::EFFECT_DESCRIPTION = "Effect_desc";
const QString AnnParser::PUTATIVE_IMPACT = "Putative_impact";
const QString AnnParser::PUTATIVE_IMPACT_DESCRIPTION = "Putative_imp_desc";

AnnParser::AnnParser()
    : InfoPartParser(KEY_WORD, true)
{

}

QStringList AnnParser::getQualifierNames() const {
    return QStringList() << "Allele"
                         << EFFECT
                         << PUTATIVE_IMPACT
                         << "Gene_name"
                         << "Gene_ID"
                         << "Feature_type"
                         << "Feature_ID"
                         << "Transcript_biotype"
                         << "Rank_total"
                         << "HGVS_c"
                         << "HGVS_p"
                         << "cDNA_pos_len"
                         << "CDS_pos_len"
                         << "Protein_pos_len"
                         << "Distance_to_feature";
}

QStringList AnnParser::getValues(const QString &entry) const {
    return entry.split(VALUES_SEPARATOR);
}

QList<U2Qualifier> AnnParser::processValue(const QString &qualifierName, const QString &value) const {
    QList<U2Qualifier> qualifiers = InfoPartParser::processValue(qualifierName, value);
    if (qualifierName == PUTATIVE_IMPACT && SnpeffDictionary::impactDescriptions.contains(value)) {
        qualifiers << U2Qualifier(PUTATIVE_IMPACT_DESCRIPTION, SnpeffDictionary::impactDescriptions[value]);
    } else if (qualifierName == EFFECT) {
        const QStringList effects = value.split(EFFECTS_SEPARATOR, QString::SkipEmptyParts);
        foreach (const QString &effect, effects) {
            if (SnpeffDictionary::effectDescriptions.contains(effect)) {
                qualifiers << U2Qualifier(EFFECT_DESCRIPTION, effect + ": " + SnpeffDictionary::effectDescriptions[value]);
            }
        }
    }
    return qualifiers;
}

const QString EffParser::KEY_WORD = "EFF";
const QString EffParser::EFFECT_DATA_SEPARATOR = "|";
const QString EffParser::EFFECT = "Effect";
const QString EffParser::EFFECT_DESCRIPTION = "Effect_desc";
const QString EffParser::EFFECT_IMPACT = "Effect_impact";
const QString EffParser::EFFECT_IMPACT_DESCRIPTION = "Effect_impact_desc";

EffParser::EffParser()
    : InfoPartParser(KEY_WORD, true)
{

}

QStringList EffParser::getQualifierNames() const {
    return QStringList() << EFFECT
                         << EFFECT_IMPACT
                         << "Functional_class"
                         << "Codon_change_dist"
                         << "Amino_acid_change"
                         << "Amino_acid_length"
                         << "Gene_name"
                         << "Transcript_biotype"
                         << "Gene_coding"
                         << "Transcript_ID"
                         << "Exon_intron_rank"
                         << "Genotype_number";
}

QStringList EffParser::getValues(const QString &entry) const {
    QRegExp regexp("^(\\w+)\\((.*)\\)$");
    QStringList values;
    regexp.indexIn(entry);
    values << regexp.cap(1);
    values << regexp.cap(2).split(EFFECT_DATA_SEPARATOR);
    return values;
}

QList<U2Qualifier> EffParser::processValue(const QString &qualifierName, const QString &value) const {
    QList<U2Qualifier> qualifiers = InfoPartParser::processValue(qualifierName, value);
    if (qualifierName == EFFECT && SnpeffDictionary::effectDescriptions.contains(value)) {
        qualifiers << U2Qualifier(EFFECT_DESCRIPTION, SnpeffDictionary::effectDescriptions[value]);
    } else if (qualifierName == EFFECT_IMPACT && SnpeffDictionary::impactDescriptions.contains(value)) {
        qualifiers << U2Qualifier(EFFECT_IMPACT_DESCRIPTION, SnpeffDictionary::impactDescriptions[value]);
    }
    return qualifiers;
}

const QString LofParser::KEY_WORD = "LOF";
const QString LofParser::VALUES_SEPARATOR = "|";

LofParser::LofParser()
    : InfoPartParser(KEY_WORD)
{

}

QStringList LofParser::getQualifierNames() const {
    return QStringList() << "Gene"
                         << "ID"
                         << "Num_transcripts"
                         << "percent_affected";
}

QStringList LofParser::getValues(const QString &entry) const {
    return entry.mid(1, entry.length() - 2).split(VALUES_SEPARATOR);
}

const QString NmdParser::KEY_WORD = "NMD";
const QString NmdParser::VALUES_SEPARATOR = "|";

NmdParser::NmdParser()
    : InfoPartParser(KEY_WORD)
{

}

QStringList NmdParser::getQualifierNames() const {
    return QStringList() << "Gene"
                         << "ID"
                         << "Num_transcripts"
                         << "percent_affected";
}

QStringList NmdParser::getValues(const QString &entry) const {
    return entry.mid(1, entry.length() - 2).split(VALUES_SEPARATOR);
}

}   // namespace U2
