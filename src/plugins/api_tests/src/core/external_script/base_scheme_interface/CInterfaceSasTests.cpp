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
#include <U2Core/GAutoDeleteList.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/ActorModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Core/U2OpStatusUtils.h>

#include "SchemeSimilarityUtils.h"
#include "CInterfaceSasTests.h"

static const QString WORKING_DIR = U2::AppContext::getWorkingDirectoryPath( );
#ifndef Q_OS_MAC
static const QString PROPER_WD_SCHEMES_PATH = WORKING_DIR + "/../../test/_common_data/cmdline/wd-sas-schemes/";
#else
static const QString PROPER_WD_SCHEMES_PATH = WORKING_DIR + "/../../../../../../test/_common_data/cmdline/wd-sas-schemes/";
#endif

static U2ErrorType getActorDisplayName( const QString &actorId, QString &actorName ) {
    U2::Workflow::ActorPrototypeRegistry *prototypeRegistry
        = U2::Workflow::WorkflowEnv::getProtoRegistry( );
    CHECK( NULL != prototypeRegistry, U2_INVALID_CALL );
    U2::Workflow::ActorPrototype *prototype = prototypeRegistry->getProto( actorId );
    CHECK( NULL != prototype, U2_UNKNOWN_ELEMENT );
    actorName = prototype->getDisplayName( );
    return U2_OK;
}

static wchar_t * toDisposableWString( const QString &source ) {
    CHECK( !source.isEmpty( ), NULL );
    wchar_t *result = new wchar_t[source.size( ) + 1 ];
    source.toWCharArray( result );
    result[source.size( )] = '\0';
    return result;
}

namespace U2 {

/////////////   TESTS IMPLEMENTATION   ///////////////////////////////////////////////////////

IMPLEMENT_TEST( CInterfaceSasTests, align_with_clustalO_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"ClustalO", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "align_with_clustalO.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, align_with_clustalW_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"clustalw", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "align_with_clustalW.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, align_with_kalign_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"kalign", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "align_with_kalign.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, align_with_mafft_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"mafft", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "align_with_mafft.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, align_with_muscle_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"muscle", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "align_with_muscle.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, align_with_tcoffee_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"tcoffee", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "align_with_tcoffee.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, annotate_with_uql_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"query", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "annotate_with_uql.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceSasTests, basic_align_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"muscle", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString writerName;
    error = getActorDisplayName( "write-msa", writerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wWriterName( toDisposableWString( writerName ) );
    error = setSchemeElementAttribute( scheme, wWriterName.get( ), L"document-format", L"stockholm" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "basic_align.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceSasTests, build_weight_matrix_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"wmatrix-build", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "build_weight_matrix.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, cd_search_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"cd-search", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "cd_search.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, dna_statistics_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"dna-stats", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "dna_statistics.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceSasTests, faqual2fastq_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"import-phred-qualities", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "write-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "faqual2fastq.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceSasTests, filter_annotations_by_name_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"filter-annotations", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-annotations", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "filter_annotations_by_name.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, find_repeats_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"repeats-search", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    QString algoName;
    error = getActorDisplayName( "repeats-search", algoName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wAlgoName( toDisposableWString( algoName ) );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"algorithm", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"exclude-tandems", L"false" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"filter-algorithm", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"identity", L"100" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"inverted", L"false" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"max-distance", L"5000" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"min-distance", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"min-length", L"5" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wAlgoName.get( ), L"threads", L"0" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "find_repeats.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, hmm2_build_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"hmm2-build", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "hmm2_build.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, import_phred_qualities_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"import-phred-qualities", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    CHECK_U2_ERROR( error );
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "import_phred_qualities.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceSasTests, join_sequences_into_alignment_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"sequences-to-msa", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "join_sequences_into_alignment.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceSasTests, local_blast_plus_search_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"blast-plus", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "local_blast_plus_search.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, merge_annotations_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"import-annotations", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-annotations", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "merge_annotations.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, merge_assemblies_with_cuffmerge_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"cuffmerge", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-annotations", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "merge_assemblies_with_cuffmerge.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, orf_marker_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"orf-search", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "orf_marker.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, remote_blast_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"blast-ncbi", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "remote_blast.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, reverse_complement_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"reverse-complement", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-sequence", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "reverse_complement.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, split_alignment_into_sequences_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"convert-alignment-to-sequence", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-msa", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "split_alignment_into_sequences.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, split_assembly_into_sequences_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"assembly-to-sequences", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-assembly", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "split_assembly_into_sequences.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, text2sequence_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"convert-text-to-sequence", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-text", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "text2sequence.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}
IMPLEMENT_TEST( CInterfaceSasTests, extract_consensus_sas ) {
    SchemeHandle scheme = NULL;
    U2ErrorType error = createSas( L"extract-consensus", NULL, NULL, &scheme );
    CHECK_U2_ERROR( error );
    QString readerName;
    error = getActorDisplayName( "read-assembly", readerName );
    CHECK_U2_ERROR( error );
    gauto_array<wchar_t> wReaderName( toDisposableWString( readerName ) );
    error = setSchemeElementAttribute( scheme, wReaderName.get( ), L"url-in.dataset",
        L"Dataset 1" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "consensus.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

} // namespace U2
