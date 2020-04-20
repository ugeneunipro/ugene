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

#ifndef _U2_DNA_INFO_H_
#define _U2_DNA_INFO_H_

#include <U2Core/global.h>

#include <QString>
#include <QStringList>
#include <QPair>

namespace U2 {

class U2CORE_EXPORT DNAInfo {
public:

static const QString LOCUS; /*  - A short mnemonic name for the entry, chosen to suggest the
sequence's definition. Mandatory keyword/exactly one record. */

static const QString DEFINITION; /*- A concise description of the sequence. Mandatory
keyword/one or more records. */

static const QString ACCESSION; /* - The primary accession number is a unique, unchanging
identifier assigned to each GenBank sequence record. (Please use this
identifier when citing information from GenBank.) Mandatory keyword/one
or more records. */

static const QString VERSION; /*   - A compound identifier consisting of the primary
accession number and a numeric version number associated with the
current version of the sequence data in the record. This is followed
by an integer key (a "GI") assigned to the sequence by NCBI.
Mandatory keyword/exactly one record. */

static const QString PROJECT; /*   - The identifier of a project (such as a Genome
Sequencing Project) to which a GenBank sequence record belongs.
Optional keyword/one or more records. */

static const QString KEYWORDS; /*  - Short phrases describing gene products and other
information about an entry. Mandatory keyword in all annotated
entries/one or more records. */

static const QString SEGMENT; /* - Information on the order in which this entry appears in a
series of discontinuous sequences from the same molecule. Optional
keyword (only in segmented entries)/exactly one record. */

static const QString SOURCE; /*- Common name of the organism or the name most frequently used
in the literature. Mandatory keyword in all annotated entries/one or
more records/includes one subkeyword. */

static const QString REFERENCE; /* - Citations for all articles containing data reported
in this entry. Includes seven subkeywords and may repeat. Mandatory
keyword/one or more records. */

   static const QString AUTHORS; /*- Lists the authors of the citation. Optional
subkeyword/one or more records. */

   static const QString CONSRTM; /*- Lists the collective names of consortiums associated
with the citation (eg, International Human Genome Sequencing Consortium),
rather than individual author names. Optional subkeyword/one or more records. */

   static const QString TITLE; /*  - Full title of citation. Optional subkeyword (present
in all but unpublished citations)/one or more records. */

   static const QString JOURNAL; /*- Lists the journal name, volume, year, and page
numbers of the citation. Mandatory subkeyword/one or more records. */

   static const QString MEDLINE; /*- Provides the Medline unique identifier for a
citation. Optional subkeyword/one record.

   NOTE: The MEDLINE linetype is obsolete and was removed
   from the GenBank flatfile format in April 2005. */

    static const QString PUBMED; /*- Provides the PubMed unique identifier for a
citation. Optional subkeyword/one record. */

   static const QString REMARK; /* - Specifies the relevance of a citation to an
entry. Optional subkeyword/one or more records. */

static const QString COMMENT; /* - Cross-references to other sequence entries, comparisons to
other collections, notes of changes in LOCUS names, and other remarks.
Optional keyword/one or more records/may include blank records.*/

static const QString FEATURES; /*  - Table containing information on portions of the
sequence that code for proteins and RNA molecules and information on
experimentally determined sites of biological significance. Optional
keyword/one or more records. */

static const QString CHAIN_ID; /* - Molecular chain index from
corresponding macromolecular structure (PDB or MMDB) */

static const QString CONTIG; /*- This linetype provides information about how individual sequence
records can be combined to form larger-scale biological objects, such as
chromosomes or complete genomes. Rather than presenting actual sequence
data, a special join() statement on the CONTIG line provides the accession
numbers and basepair ranges of the underlying records which comprise the
object.

As of August 2005, the 2L chromosome arm of Drosophila melanogaster
(accession number AE014134) provided a good example of CONTIG use. */

static const QString ORIGIN; /*- Specification of how the first base of the reported sequence
is operationally located within the genome. Where possible, this
includes its location within a larger genetic map. Mandatory
keyword/exactly one record.

    - The ORIGIN line is followed by sequence data (multiple records). */

static const QString ID;
static const QString DATE;
static const QString EMBL_ID;
static const QString FASTA_HDR;
static const QString FASTQ_QUAL_CODES;
static const QString FASTQ_QUAL_TYPE;
static const QString GENBANK_HEADER;
static const QString FASTQ_COMMENT;

static QString getPrimaryAccession(const QVariantMap&);
static QString getContig(const QVariantMap&);
static QString getName(const QVariantMap&);
static QString getFastqComment(const QVariantMap&);
};

class U2CORE_EXPORT DNALocusInfo {
public:
    QString name;
    QString topology;
    QString molecule;
    QString division;
    QString date;
};

class U2CORE_EXPORT DNASourceInfo {
public:
    QString name;
    QString organism;
    QStringList taxonomy;
    QString organelle;
};

class U2CORE_EXPORT DNAReferenceInfo {
public:
//     unparsed references info
    QString referencesRecord;

//     TODO: parser reference info
    int id;
    QString range;
    QList< QPair<QString, QStringList> > data;
};

}//namespace

Q_DECLARE_METATYPE(U2::DNALocusInfo)
Q_DECLARE_METATYPE(U2::DNASourceInfo)
Q_DECLARE_METATYPE(U2::DNAReferenceInfo)

#endif
