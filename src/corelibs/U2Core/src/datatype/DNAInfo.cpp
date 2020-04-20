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

#include <QVariantMap>

#include "DNAInfo.h"

namespace U2 {

const QString DNAInfo::LOCUS = "LOCUS";

const QString DNAInfo::DEFINITION = "DEFINITION";

const QString DNAInfo::ACCESSION = "ACCESSION";

const QString DNAInfo::VERSION = "VERSION";

const QString DNAInfo::PROJECT = "PROJECT";

const QString DNAInfo::SEGMENT = "SEGMENT";

const QString DNAInfo::SOURCE = "SOURCE";

const QString DNAInfo::KEYWORDS = "KEYWORDS";

const QString DNAInfo::REFERENCE = "REFERENCE";

const QString DNAInfo::AUTHORS = "AUTHORS";

const QString DNAInfo::CONSRTM = "CONSRTM";

const QString DNAInfo::TITLE = "TITLE";

const QString DNAInfo::JOURNAL = "JOURNAL";

const QString DNAInfo::MEDLINE = "MEDLINE";

const QString DNAInfo::PUBMED = "PUBMED";

const QString DNAInfo::REMARK = "REMARK";

const QString DNAInfo::COMMENT = "COMMENT";

const QString DNAInfo::FEATURES = "FEATURES";

const QString DNAInfo::CONTIG = "CONTIG";

const QString DNAInfo::ORIGIN = "ORIGIN";

const QString DNAInfo::CHAIN_ID = "CHAIN_ID";

const QString DNAInfo::DATE = "DATE";
const QString DNAInfo::FASTA_HDR = "FASTA_HDR";
const QString DNAInfo::FASTQ_QUAL_CODES = "FASTQ_QUAL_CODES";
const QString DNAInfo::FASTQ_QUAL_TYPE = "FASTQ_QUAL_TYPE";
const QString DNAInfo::EMBL_ID = "EMBL_ID";
const QString DNAInfo::ID = "ID";
const QString DNAInfo::GENBANK_HEADER = "GENBANK_HEADER";
const QString DNAInfo::FASTQ_COMMENT = "FASTQ_COMMENT";


QString DNAInfo::getPrimaryAccession( const QVariantMap& vm)
{
    if (vm.contains(ACCESSION)) {
        QVariant v = vm.value(ACCESSION);
        assert(v.type() == QVariant::StringList);
        QStringList l = v.toStringList();
        assert(!l.isEmpty());
        return l.isEmpty() ? v.toString() : l.first();
    }
    return QString();
}

QString DNAInfo::getContig( const QVariantMap& vm)
{
    if (vm.contains(CONTIG)) {
        QVariant v = vm.value(CONTIG);
        assert(v.type() == QVariant::StringList);
        QStringList l = v.toStringList();
        assert(!l.isEmpty());
        return l.isEmpty() ? v.toString() : l.join(QString());
    }
    return QString();
}

QString DNAInfo::getName( const QVariantMap& vm) {
    QString name;
    if (vm.contains(LOCUS)) {
        DNALocusInfo loi = vm.value(LOCUS).value<DNALocusInfo>();
        name = loi.name;
    }
    if (name.isEmpty() && vm.contains(ID)) {
        name = vm.value(ID).toString();
    }
    if (name.isEmpty() && vm.contains(ACCESSION)) {
        name = getPrimaryAccession(vm);
    }
    return name;
}

QString DNAInfo::getFastqComment(const QVariantMap & vm){
    QString comment;
    if (vm.contains(FASTQ_COMMENT)) {
        comment = vm.value(FASTQ_COMMENT).toString();
    }
    return comment;
}
}//namespace
