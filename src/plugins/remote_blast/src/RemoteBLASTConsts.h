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

#ifndef _REMOTE_QUERY_CONSTS_
#define _REMOTE_QUERY_CONSTS_

#include <QStringList>

namespace U2 {

#define MAX_BLAST_SEQ_LEN 3000

#define CDD_DESCRIPTION "CDD (Conserved Domain Database) is a protein annotation resource that consists of a collection of well-annotated multiple sequence alignment models for ancient domains and full-length proteins."
#define BLAST_DESCRIPTION "Basic Local Alignment Search Tool, or BLAST, is an algorithm for comparing primary biological sequence information, such as the amino-acid sequences of different proteins or the nucleotides of DNA sequences. A BLAST search enables a researcher to compare a query sequence with a library or database of sequences, and identify library sequences that resemble the query sequence above a certain threshold."

#define EXPECTED_ATTR "expected_results"
#define SIMPLE_ATTR "simple"
#define MIN_LEN_ATTR "min_length"
#define MAX_LEN_ATTR "max_length"
#define TRANSL_ATTR "transl"
#define DATABASE_ATTR "db"
#define EVALUE_ATTR "e-value"
#define INDEX_ATTR "index"
#define SEQUENCE_ATTR "seq"
#define SHORTSEQ_ATTR "short_seq"
#define HITS_ATTR "hits"
#define ALG_ATTR "alg"
#define FILTERS_ATTR "filters"
#define GAP_ATTR "gap_costs"
#define MATCHSCORE_ATTR "match_scores"
#define MATRIX_ATTR "matrix"
#define MEGABLAST_ATTR "megablast"
#define WORD_SIZE_ATTR "word_size"
#define ALPH_ATTR "alph"
#define SERVICE_ATTR "service"
#define PATTERN_ATTR "phi-pattern"
#define ENTREZ_QUERY_ATTR "entrez_query"

class ParametersLists {
public:
    static const QStringList blastn_wordSize;
    static const QStringList blastp_wordSize;
    static const QStringList megablast_wordSize;

    static const QStringList blastn_gapCost;
    static const QStringList blastp_gapCost;

    static const QStringList blastn_scores;

    static const QStringList blastn_dataBase;
    static const QStringList blastp_dataBase;
    static const QStringList cdd_dataBase;

    static const QStringList blastp_matrix;
};

class ReqParams {
public:
    static const QString program;
    static const QString expect;
    static const QString wordSize;
    static const QString hits;
    static const QString database;
    static const QString gapCost;
    static const QString matchScore;
    static const QString mismatchScore;
    static const QString matrix;
    static const QString filter;
    static const QString megablast;
    static const QString lowCaseMask;
    static const QString sequence;
    static const QString service;
    static const QString phiPattern;
    static const QString entrezQuery;

    static const QString cdd_eValue;
    static const QString cdd_hits;
    static const QString cdd_sequence;
    static const QString cdd_db;
};

void addParametr(QString &str,const QString &paramName, const QString &param);
void addParametr(QString &str,const QString &paramName, const double param);
void addParametr(QString &str,const QString &paramName, const int param);

}


#endif
