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

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/StrPackUtils.h>

#include "BaseDocumentFormats.h"

namespace U2 {

const DocumentFormatId BaseDocumentFormats::ABIF("abi");
const DocumentFormatId BaseDocumentFormats::ACE("ace");
const DocumentFormatId BaseDocumentFormats::BAM("bam");
const DocumentFormatId BaseDocumentFormats::BED("bed");
const DocumentFormatId BaseDocumentFormats::CLUSTAL_ALN("clustal");
const DocumentFormatId BaseDocumentFormats::DATABASE_CONNECTION("database_connection");
const DocumentFormatId BaseDocumentFormats::DIFF("diff");
const DocumentFormatId BaseDocumentFormats::FASTA("fasta");
const DocumentFormatId BaseDocumentFormats::FASTQ("fastq");
const DocumentFormatId BaseDocumentFormats::FPKM_TRACKING_FORMAT("fpkm-tracking");
const DocumentFormatId BaseDocumentFormats::GFF("gff");
const DocumentFormatId BaseDocumentFormats::GTF("gtf");
const DocumentFormatId BaseDocumentFormats::INDEX("index");
const DocumentFormatId BaseDocumentFormats::MEGA("mega");
const DocumentFormatId BaseDocumentFormats::MSF("msf");
const DocumentFormatId BaseDocumentFormats::NEWICK("newick");
const DocumentFormatId BaseDocumentFormats::NEXUS("nexus");
const DocumentFormatId BaseDocumentFormats::PDW("pdw");
const DocumentFormatId BaseDocumentFormats::PHYLIP_INTERLEAVED("phylip-interleaved");
const DocumentFormatId BaseDocumentFormats::PHYLIP_SEQUENTIAL("phylip-sequential");
const DocumentFormatId BaseDocumentFormats::PLAIN_ASN("mmdb");
const DocumentFormatId BaseDocumentFormats::PLAIN_EMBL("embl");
const DocumentFormatId BaseDocumentFormats::PLAIN_GENBANK("genbank");
const DocumentFormatId BaseDocumentFormats::PLAIN_PDB("pdb");
const DocumentFormatId BaseDocumentFormats::PLAIN_SWISS_PROT("swiss-prot");
const DocumentFormatId BaseDocumentFormats::PLAIN_TEXT("text");
const DocumentFormatId BaseDocumentFormats::RAW_DNA_SEQUENCE("raw");
const DocumentFormatId BaseDocumentFormats::SAM("sam");
const DocumentFormatId BaseDocumentFormats::SCF("scf");
const DocumentFormatId BaseDocumentFormats::SNP("snp");
const DocumentFormatId BaseDocumentFormats::SRF("srfasta");
const DocumentFormatId BaseDocumentFormats::STOCKHOLM("stockholm");
const DocumentFormatId BaseDocumentFormats::UGENEDB("usqlite");
const DocumentFormatId BaseDocumentFormats::VCF4("vcf");
const DocumentFormatId BaseDocumentFormats::VECTOR_NTI_ALIGNX("Vector_nti_alignx");
const DocumentFormatId BaseDocumentFormats::VECTOR_NTI_SEQUENCE("vector_nti_sequence");

DocumentFormat* BaseDocumentFormats::get(const DocumentFormatId& formatId) {
    return AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
}

namespace {

StrStrMap initInvalidFormatIdsMap() {
    StrStrMap invalidIds2trueIds;

    // IDs from 1.26.0
    invalidIds2trueIds.insert("ABI", BaseDocumentFormats::ABIF);
    invalidIds2trueIds.insert("ACE", BaseDocumentFormats::ACE);
    invalidIds2trueIds.insert("BAM", BaseDocumentFormats::BAM);
    invalidIds2trueIds.insert("BED", BaseDocumentFormats::BED);
    invalidIds2trueIds.insert("CLUSTAL", BaseDocumentFormats::CLUSTAL_ALN);
    invalidIds2trueIds.insert("database_connection", BaseDocumentFormats::DATABASE_CONNECTION);
    invalidIds2trueIds.insert("Diff", BaseDocumentFormats::DIFF);
    invalidIds2trueIds.insert("FASTA", BaseDocumentFormats::FASTA);
    invalidIds2trueIds.insert("FASTQ", BaseDocumentFormats::FASTQ);
    invalidIds2trueIds.insert("FPKM-Tracking", BaseDocumentFormats::FPKM_TRACKING_FORMAT);
    invalidIds2trueIds.insert("GFF", BaseDocumentFormats::GFF);
    invalidIds2trueIds.insert("GTF", BaseDocumentFormats::GTF);
    invalidIds2trueIds.insert("Index", BaseDocumentFormats::INDEX);
    invalidIds2trueIds.insert("MEGA", BaseDocumentFormats::MEGA);
    invalidIds2trueIds.insert("MSF", BaseDocumentFormats::MSF);
    invalidIds2trueIds.insert("Newick", BaseDocumentFormats::NEWICK);
    invalidIds2trueIds.insert("Nexus", BaseDocumentFormats::NEXUS);
    invalidIds2trueIds.insert("PDW", BaseDocumentFormats::PDW);
    invalidIds2trueIds.insert("PHYLIP-Interleaved", BaseDocumentFormats::PHYLIP_INTERLEAVED);
    invalidIds2trueIds.insert("PHYLIP-Sequential", BaseDocumentFormats::PHYLIP_SEQUENTIAL);
    invalidIds2trueIds.insert("MMDB", BaseDocumentFormats::PLAIN_ASN);
    invalidIds2trueIds.insert("EMBL", BaseDocumentFormats::PLAIN_EMBL);
    invalidIds2trueIds.insert("Genbank", BaseDocumentFormats::PLAIN_GENBANK);
    invalidIds2trueIds.insert("GenBank", BaseDocumentFormats::PLAIN_GENBANK);
    invalidIds2trueIds.insert("PDB", BaseDocumentFormats::PLAIN_PDB);
    invalidIds2trueIds.insert("Swiss-Prot", BaseDocumentFormats::PLAIN_SWISS_PROT);
    invalidIds2trueIds.insert("Text", BaseDocumentFormats::PLAIN_TEXT);
    invalidIds2trueIds.insert("Raw", BaseDocumentFormats::RAW_DNA_SEQUENCE);
    invalidIds2trueIds.insert("SAM", BaseDocumentFormats::SAM);
    invalidIds2trueIds.insert("SCF", BaseDocumentFormats::SCF);
    invalidIds2trueIds.insert("SNP", BaseDocumentFormats::SNP);
    invalidIds2trueIds.insert("SRFASTA", BaseDocumentFormats::SRF);
    invalidIds2trueIds.insert("Stockholm", BaseDocumentFormats::STOCKHOLM);
    invalidIds2trueIds.insert("Usqlite", BaseDocumentFormats::UGENEDB);
    invalidIds2trueIds.insert("VCF", BaseDocumentFormats::VCF4);
    invalidIds2trueIds.insert("Vector NTI/AlignX", BaseDocumentFormats::VECTOR_NTI_ALIGNX);
    invalidIds2trueIds.insert("Vector NTI Sequence", BaseDocumentFormats::VECTOR_NTI_SEQUENCE);

    return invalidIds2trueIds;
}

StrStrMap initFormatIdsMap() {
    StrStrMap anyIds2trueIds;

    // IDs from 1.25.0 and lower
    anyIds2trueIds.insert(BaseDocumentFormats::ABIF, BaseDocumentFormats::ABIF);
    anyIds2trueIds.insert(BaseDocumentFormats::ACE, BaseDocumentFormats::ACE);
    anyIds2trueIds.insert(BaseDocumentFormats::BAM, BaseDocumentFormats::BAM);
    anyIds2trueIds.insert(BaseDocumentFormats::BED, BaseDocumentFormats::BED);
    anyIds2trueIds.insert(BaseDocumentFormats::CLUSTAL_ALN, BaseDocumentFormats::CLUSTAL_ALN);
    anyIds2trueIds.insert(BaseDocumentFormats::DATABASE_CONNECTION, BaseDocumentFormats::DATABASE_CONNECTION);
    anyIds2trueIds.insert(BaseDocumentFormats::DIFF, BaseDocumentFormats::DIFF);
    anyIds2trueIds.insert(BaseDocumentFormats::FASTA, BaseDocumentFormats::FASTA);
    anyIds2trueIds.insert(BaseDocumentFormats::FASTQ, BaseDocumentFormats::FASTQ);
    anyIds2trueIds.insert(BaseDocumentFormats::FPKM_TRACKING_FORMAT, BaseDocumentFormats::FPKM_TRACKING_FORMAT);
    anyIds2trueIds.insert(BaseDocumentFormats::GFF, BaseDocumentFormats::GFF);
    anyIds2trueIds.insert(BaseDocumentFormats::GTF, BaseDocumentFormats::GTF);
    anyIds2trueIds.insert(BaseDocumentFormats::INDEX, BaseDocumentFormats::INDEX);
    anyIds2trueIds.insert(BaseDocumentFormats::MEGA, BaseDocumentFormats::MEGA);
    anyIds2trueIds.insert(BaseDocumentFormats::MSF, BaseDocumentFormats::MSF);
    anyIds2trueIds.insert(BaseDocumentFormats::NEWICK, BaseDocumentFormats::NEWICK);
    anyIds2trueIds.insert(BaseDocumentFormats::NEXUS, BaseDocumentFormats::NEXUS);
    anyIds2trueIds.insert(BaseDocumentFormats::PDW, BaseDocumentFormats::PDW);
    anyIds2trueIds.insert(BaseDocumentFormats::PHYLIP_INTERLEAVED, BaseDocumentFormats::PHYLIP_INTERLEAVED);
    anyIds2trueIds.insert(BaseDocumentFormats::PHYLIP_SEQUENTIAL, BaseDocumentFormats::PHYLIP_SEQUENTIAL);
    anyIds2trueIds.insert(BaseDocumentFormats::PLAIN_ASN, BaseDocumentFormats::PLAIN_ASN);
    anyIds2trueIds.insert(BaseDocumentFormats::PLAIN_EMBL, BaseDocumentFormats::PLAIN_EMBL);
    anyIds2trueIds.insert(BaseDocumentFormats::PLAIN_GENBANK, BaseDocumentFormats::PLAIN_GENBANK);
    anyIds2trueIds.insert(BaseDocumentFormats::PLAIN_PDB, BaseDocumentFormats::PLAIN_PDB);
    anyIds2trueIds.insert(BaseDocumentFormats::PLAIN_SWISS_PROT, BaseDocumentFormats::PLAIN_SWISS_PROT);
    anyIds2trueIds.insert(BaseDocumentFormats::PLAIN_TEXT, BaseDocumentFormats::PLAIN_TEXT);
    anyIds2trueIds.insert(BaseDocumentFormats::RAW_DNA_SEQUENCE, BaseDocumentFormats::RAW_DNA_SEQUENCE);
    anyIds2trueIds.insert(BaseDocumentFormats::SAM, BaseDocumentFormats::SAM);
    anyIds2trueIds.insert(BaseDocumentFormats::SCF, BaseDocumentFormats::SCF);
    anyIds2trueIds.insert(BaseDocumentFormats::SNP, BaseDocumentFormats::SNP);
    anyIds2trueIds.insert(BaseDocumentFormats::SRF, BaseDocumentFormats::SRF);
    anyIds2trueIds.insert(BaseDocumentFormats::STOCKHOLM, BaseDocumentFormats::STOCKHOLM);
    anyIds2trueIds.insert(BaseDocumentFormats::UGENEDB, BaseDocumentFormats::UGENEDB);
    anyIds2trueIds.insert(BaseDocumentFormats::VCF4, BaseDocumentFormats::VCF4);
    anyIds2trueIds.insert(BaseDocumentFormats::VECTOR_NTI_ALIGNX, BaseDocumentFormats::VECTOR_NTI_ALIGNX);
    anyIds2trueIds.insert(BaseDocumentFormats::VECTOR_NTI_SEQUENCE, BaseDocumentFormats::VECTOR_NTI_SEQUENCE);

    // IDs from 1.26.0
    anyIds2trueIds.unite(initInvalidFormatIdsMap());

    return anyIds2trueIds;
}

}

bool BaseDocumentFormats::equal(const DocumentFormatId &first, const DocumentFormatId &second) {
    // After UGENE-5719 fix format IDs were occasionally changed
    // Case insensitive comparison grants a correct comparison result
    static const StrStrMap formatIds = initFormatIdsMap();
    return formatIds.value(first, first) == formatIds.value(second, second);
}

bool BaseDocumentFormats::isInvalidId(const DocumentFormatId &formatId) {
    static const QStringList invalidIdsList = initInvalidFormatIdsMap().keys();
    return invalidIdsList.contains(formatId);
}

DocumentFormatId BaseDocumentFormats::toValidId(const DocumentFormatId &invalidFormatId) {
    static const StrStrMap invalidIds = initInvalidFormatIdsMap();
    return invalidIds.value(invalidFormatId, invalidFormatId);
}

}   // namespace U2
