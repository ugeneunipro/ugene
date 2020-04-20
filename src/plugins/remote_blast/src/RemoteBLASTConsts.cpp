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

#include "RemoteBLASTConsts.h"

namespace U2 {

const QStringList ParametersLists::blastn_wordSize = QStringList() << "7" << "11" << "15";
const QStringList ParametersLists::blastp_wordSize = QStringList() << "2" << "3" << "6";
const QStringList ParametersLists::megablast_wordSize = QStringList() << "16" << "20" << "24"
                                                                << "28" << "32" << "48" << "64";
const QStringList ParametersLists::blastn_gapCost = QStringList() << "4 4" << "2 4" << "0 4"
                                                                << "3 3" << "5 2" << "4 2" << "2 2";
const QStringList ParametersLists::blastp_gapCost = QStringList() << "9 2" << "8 2" << "7 2" << "12 1"
                                                                << "11 1" << "10 1";

const QStringList ParametersLists::blastn_scores = QStringList() << "1 -2" << "1 -3" << "1 -4"
                                                                << "2 -3" << "4 -5" << "1 -1";

const QStringList ParametersLists::blastp_matrix = QStringList() << "PAM30" << "PAM70" << "BLOSUM80"
                                                                << "BLOSUM62" << "BLOSUM45";

const QStringList ParametersLists::blastn_dataBase = QStringList() << "Human genomic plus transcript - est_human"
                                                                << "Mouse genomic plus transcript - est_mouse"
                                                                << "Nucleotide collection - nr"
                                                                << "Reference nRNA sequences - refseq_rna"
                                                                << "Reference genomic seqence - refseq_genomic"
                                                                << "NCBI genomes - chromosome"
                                                                << "Express sequence tagsest - est"
                                                                << "Non-human, non-mouse ESTs - est_other"
                                                                << "Genomic survey sequences - gss"
                                                                << "High throughput genomic sequences - HTGS"
                                                                << "Patent sequences - pat"
                                                                << "Protein Data Bank - pdb"
                                                                << "Human ALU repeat elements - alu"
                                                                << "Sequence tagged sites - dbsts"
                                                                << "Whole-genome shotgun reads - wgs"
                                                                << "Environmental samples - env_nt";
const QStringList ParametersLists::blastp_dataBase = QStringList() << "Non-redundant protein sequences - nr"
                                                                << "Reference proteins - refseq_protein"
                                                                << "Swissprot protein sequences - swissprot"
                                                                << "Patented protein sequences - pat"
                                                                << "Protein Data Bank proteins - pdb"
                                                                << "Environmental samples - env_nr";
const QStringList ParametersLists::cdd_dataBase = QStringList() << "CDD" << "oasis_pfam" << "oasis_smart"
                                                                << "oasis_cog" << "oasis_kog"
                                                                << "oasis_prk" << "oasis_tigr";

const QString ReqParams::program = "PROGRAM";
const QString ReqParams::expect = "EXPECT";
const QString ReqParams::wordSize = "WORD_SIZE";
const QString ReqParams::hits = "HITLIST_SIZE";
const QString ReqParams::database = "DATABASE";
const QString ReqParams::gapCost = "GAPCOSTS";
const QString ReqParams::matchScore = "NUCL_REWARD";
const QString ReqParams::mismatchScore = "NUCL_PENALTY";
const QString ReqParams::matrix = "MATRIX";
const QString ReqParams::filter = "FILTER";
const QString ReqParams::megablast = "MEGABLAST";
const QString ReqParams::lowCaseMask = "LCASE_MASK";
const QString ReqParams::sequence = "QUERY";
const QString ReqParams::service = "SERVICE";
const QString ReqParams::phiPattern = "PHI_PATTERN";
const QString ReqParams::entrezQuery = "ENTREZ_QUERY";

const QString ReqParams::cdd_eValue = "evalue";
const QString ReqParams::cdd_hits = "maxhits";
const QString ReqParams::cdd_sequence = "seqinput";
const QString ReqParams::cdd_db = "db";

void addParametr(QString &requestParameters,const QString &paramName, const QString &param) {
    requestParameters.append("&");
    requestParameters.append(paramName);
    requestParameters.append("=");
    requestParameters.append(param);
}

void addParametr(QString &requestParameters,const QString &paramName, const double param) {
    requestParameters.append("&");
    requestParameters.append(paramName);
    requestParameters.append("=");
    requestParameters.append(QString::number(param));
}

void addParametr(QString &requestParameters,const QString &paramName, const int param) {
    requestParameters.append("&");
    requestParameters.append(paramName);
    requestParameters.append("=");
    requestParameters.append(QString::number(param));
}

}
