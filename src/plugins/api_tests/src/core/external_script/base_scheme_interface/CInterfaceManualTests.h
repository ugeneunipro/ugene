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

#ifndef _U2_C_INTERFACE_MANUAL_TESTS_H_
#define _U2_C_INTERFACE_MANUAL_TESTS_H_

#include <U2Core/U2OpStatus.h>
#include <unittest.h>

namespace U2 {

DECLARE_TEST( CInterfaceManualTests, basic_align );
DECLARE_TEST( CInterfaceManualTests, faqual2fastq );
DECLARE_TEST( CInterfaceManualTests, msa2clustal );
DECLARE_TEST( CInterfaceManualTests, query2alignment );
DECLARE_TEST( CInterfaceManualTests, seq2gen );
DECLARE_TEST( CInterfaceManualTests, tfbs );
DECLARE_TEST( CInterfaceManualTests, build_test_HMM );
DECLARE_TEST( CInterfaceManualTests, search_HMM );
DECLARE_TEST( CInterfaceManualTests, search_TFBS );
DECLARE_TEST( CInterfaceManualTests, call_variants );
DECLARE_TEST( CInterfaceManualTests, chip_seq );
DECLARE_TEST( CInterfaceManualTests, chip_seq_with_control );
DECLARE_TEST( CInterfaceManualTests, tuxedo_main );
DECLARE_TEST( CInterfaceManualTests, tuxedo_main_paired );
DECLARE_TEST( CInterfaceManualTests, tuxedo_multiple_dataset );
DECLARE_TEST( CInterfaceManualTests, tuxedo_multiple_dataset_paired );
DECLARE_TEST( CInterfaceManualTests, tuxedo_single_dataset );
DECLARE_TEST( CInterfaceManualTests, tuxedo_single_dataset_paired );
DECLARE_TEST( CInterfaceManualTests, find_sequences );
DECLARE_TEST( CInterfaceManualTests, gene_by_gene_report );
DECLARE_TEST( CInterfaceManualTests, merge_sequence_annotation );
DECLARE_TEST( CInterfaceManualTests, consensus );

} // namespace U2

DECLARE_METATYPE( CInterfaceManualTests, basic_align );
DECLARE_METATYPE( CInterfaceManualTests, faqual2fastq );
DECLARE_METATYPE( CInterfaceManualTests, msa2clustal );
DECLARE_METATYPE( CInterfaceManualTests, query2alignment );
DECLARE_METATYPE( CInterfaceManualTests, seq2gen );
DECLARE_METATYPE( CInterfaceManualTests, tfbs );
DECLARE_METATYPE( CInterfaceManualTests, build_test_HMM );
DECLARE_METATYPE( CInterfaceManualTests, search_HMM );
DECLARE_METATYPE( CInterfaceManualTests, search_TFBS );
DECLARE_METATYPE( CInterfaceManualTests, call_variants );
DECLARE_METATYPE( CInterfaceManualTests, chip_seq );
DECLARE_METATYPE( CInterfaceManualTests, chip_seq_with_control );
DECLARE_METATYPE( CInterfaceManualTests, tuxedo_main );
DECLARE_METATYPE( CInterfaceManualTests, tuxedo_main_paired );
DECLARE_METATYPE( CInterfaceManualTests, tuxedo_multiple_dataset );
DECLARE_METATYPE( CInterfaceManualTests, tuxedo_multiple_dataset_paired );
DECLARE_METATYPE( CInterfaceManualTests, tuxedo_single_dataset );
DECLARE_METATYPE( CInterfaceManualTests, tuxedo_single_dataset_paired );
DECLARE_METATYPE( CInterfaceManualTests, find_sequences );
DECLARE_METATYPE( CInterfaceManualTests, gene_by_gene_report );
DECLARE_METATYPE( CInterfaceManualTests, merge_sequence_annotation );
DECLARE_METATYPE( CInterfaceManualTests, consensus );

#endif // _U2_C_INTERFACE_MANUAL_TESTS_H_
