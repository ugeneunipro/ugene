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

#ifndef _U2_C_INTERFACE_SAS_TESTS_H_
#define _U2_C_INTERFACE_SAS_TESTS_H_

#include <U2Core/U2OpStatus.h>
#include <unittest.h>

namespace U2 {

DECLARE_TEST( CInterfaceSasTests, align_with_clustalO_sas );
DECLARE_TEST( CInterfaceSasTests, align_with_clustalW_sas );
DECLARE_TEST( CInterfaceSasTests, align_with_kalign_sas );
DECLARE_TEST( CInterfaceSasTests, align_with_mafft_sas );
DECLARE_TEST( CInterfaceSasTests, align_with_muscle_sas );
DECLARE_TEST( CInterfaceSasTests, align_with_tcoffee_sas );
DECLARE_TEST( CInterfaceSasTests, annotate_with_uql_sas );
DECLARE_TEST( CInterfaceSasTests, basic_align_sas );
DECLARE_TEST( CInterfaceSasTests, build_weight_matrix_sas );
DECLARE_TEST( CInterfaceSasTests, cd_search_sas );
DECLARE_TEST( CInterfaceSasTests, dna_statistics_sas );
DECLARE_TEST( CInterfaceSasTests, faqual2fastq_sas );
DECLARE_TEST( CInterfaceSasTests, filter_annotations_by_name_sas );
DECLARE_TEST( CInterfaceSasTests, find_repeats_sas );
DECLARE_TEST( CInterfaceSasTests, hmm2_build_sas );
DECLARE_TEST( CInterfaceSasTests, import_phred_qualities_sas );
DECLARE_TEST( CInterfaceSasTests, join_sequences_into_alignment_sas );
DECLARE_TEST( CInterfaceSasTests, local_blast_plus_search_sas );
DECLARE_TEST( CInterfaceSasTests, merge_annotations_sas );
DECLARE_TEST( CInterfaceSasTests, merge_assemblies_with_cuffmerge_sas );
DECLARE_TEST( CInterfaceSasTests, orf_marker_sas );
DECLARE_TEST( CInterfaceSasTests, remote_blast_sas );
DECLARE_TEST( CInterfaceSasTests, reverse_complement_sas );
DECLARE_TEST( CInterfaceSasTests, split_alignment_into_sequences_sas );
DECLARE_TEST( CInterfaceSasTests, split_assembly_into_sequences_sas );
DECLARE_TEST( CInterfaceSasTests, text2sequence_sas );
DECLARE_TEST( CInterfaceSasTests, extract_consensus_sas );

} // namespace U2

DECLARE_METATYPE( CInterfaceSasTests, align_with_clustalO_sas )
DECLARE_METATYPE( CInterfaceSasTests, align_with_clustalW_sas )
DECLARE_METATYPE( CInterfaceSasTests, align_with_kalign_sas )
DECLARE_METATYPE( CInterfaceSasTests, align_with_mafft_sas )
DECLARE_METATYPE( CInterfaceSasTests, align_with_muscle_sas )
DECLARE_METATYPE( CInterfaceSasTests, align_with_tcoffee_sas )
DECLARE_METATYPE( CInterfaceSasTests, annotate_with_uql_sas )
DECLARE_METATYPE( CInterfaceSasTests, basic_align_sas )
DECLARE_METATYPE( CInterfaceSasTests, build_weight_matrix_sas )
DECLARE_METATYPE( CInterfaceSasTests, cd_search_sas )
DECLARE_METATYPE( CInterfaceSasTests, dna_statistics_sas )
DECLARE_METATYPE( CInterfaceSasTests, faqual2fastq_sas )
DECLARE_METATYPE( CInterfaceSasTests, filter_annotations_by_name_sas )
DECLARE_METATYPE( CInterfaceSasTests, find_repeats_sas )
DECLARE_METATYPE( CInterfaceSasTests, hmm2_build_sas )
DECLARE_METATYPE( CInterfaceSasTests, import_phred_qualities_sas )
DECLARE_METATYPE( CInterfaceSasTests, join_sequences_into_alignment_sas )
DECLARE_METATYPE( CInterfaceSasTests, local_blast_plus_search_sas )
DECLARE_METATYPE( CInterfaceSasTests, merge_annotations_sas )
DECLARE_METATYPE( CInterfaceSasTests, merge_assemblies_with_cuffmerge_sas )
DECLARE_METATYPE( CInterfaceSasTests, orf_marker_sas )
DECLARE_METATYPE( CInterfaceSasTests, remote_blast_sas )
DECLARE_METATYPE( CInterfaceSasTests, reverse_complement_sas )
DECLARE_METATYPE( CInterfaceSasTests, split_alignment_into_sequences_sas )
DECLARE_METATYPE( CInterfaceSasTests, split_assembly_into_sequences_sas )
DECLARE_METATYPE( CInterfaceSasTests, text2sequence_sas )
DECLARE_METATYPE( CInterfaceSasTests, extract_consensus_sas )

#endif // _U2_C_INTERFACE_SAS_TESTS_H_
