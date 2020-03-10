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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "SchemeSimilarityUtils.h"

#include "CInterfaceManualTests.h"

static const int MAX_ELEMENT_NAME_LENGTH = 100;
static const QString PROPER_WD_SCHEMES_PATH = PATH_PREFIX_DATA + QString( ":workflow_samples" );

namespace U2 {

/////////////   TESTS IMPLEMENTATION   ///////////////////////////////////////////////////////

IMPLEMENT_TEST( CInterfaceManualTests, basic_align ) {
    wchar_t readMsa[MAX_ELEMENT_NAME_LENGTH], muscle[MAX_ELEMENT_NAME_LENGTH],
        writeMsa[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-msa", MAX_ELEMENT_NAME_LENGTH, readMsa );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-msa", MAX_ELEMENT_NAME_LENGTH, writeMsa );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"muscle", MAX_ELEMENT_NAME_LENGTH, muscle );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"document-format", L"clustal" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"url-out", L"muscle_alignment.aln" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, readMsa, L"out-msa", muscle, L"in-msa" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, muscle, L"out-msa", writeMsa, L"in-msa" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readMsa, L"msa", muscle, L"in-msa.msa" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, muscle, L"msa", writeMsa, L"in-msa.msa" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Alignment/basic_align.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, faqual2fastq ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], importPhredQualities[MAX_ELEMENT_NAME_LENGTH],
        writeSequence[MAX_ELEMENT_NAME_LENGTH];

    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"import-phred-qualities", MAX_ELEMENT_NAME_LENGTH,
        importPhredQualities );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readSequence, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"qualified_sequence.fastq" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, readSequence, L"out-sequence", importPhredQualities,
        L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, importPhredQualities, L"out-sequence", writeSequence,
        L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", importPhredQualities,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, importPhredQualities, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Conversions/faqual2fastq.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, msa2clustal ) {
    wchar_t readMsa[MAX_ELEMENT_NAME_LENGTH], writeMsa[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-msa", MAX_ELEMENT_NAME_LENGTH, readMsa );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-msa", MAX_ELEMENT_NAME_LENGTH, writeMsa );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"document-format", L"clustal" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"write-mode", L"2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"url-out", L"output.aln" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readMsa, L"msa", writeMsa, L"in-msa.msa" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Conversions/msa2clustal.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, query2alignment ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], query[MAX_ELEMENT_NAME_LENGTH],
        extractAnnotatedSequence[MAX_ELEMENT_NAME_LENGTH], sequencesToMsa[MAX_ELEMENT_NAME_LENGTH],
        writeMsa[MAX_ELEMENT_NAME_LENGTH], filterAnnotations[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"query", MAX_ELEMENT_NAME_LENGTH, query );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"extract-annotated-sequence", MAX_ELEMENT_NAME_LENGTH,
        extractAnnotatedSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sequences-to-msa", MAX_ELEMENT_NAME_LENGTH,
        sequencesToMsa );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-msa", MAX_ELEMENT_NAME_LENGTH, writeMsa );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"filter-annotations", MAX_ELEMENT_NAME_LENGTH,
        filterAnnotations );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"merge-gap", L"10" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readSequence, L"mode", L"0" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, query, L"merge", L"false" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, query, L"offset", L"0" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, extractAnnotatedSequence, L"complement", L"true" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, extractAnnotatedSequence, L"extend-left", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, extractAnnotatedSequence, L"extend-right", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, extractAnnotatedSequence, L"merge-gap-length", L"1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, extractAnnotatedSequence, L"translate", L"false" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeMsa, L"document-format", L"clustal" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"write-mode", L"2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeMsa, L"url-out", L"output.aln" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filterAnnotations, L"accept-or-filter", L"true" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", query, L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filterAnnotations, L"annotations",
        extractAnnotatedSequence, L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", extractAnnotatedSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, extractAnnotatedSequence, L"sequence", sequencesToMsa,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, sequencesToMsa, L"msa", writeMsa, L"in-msa.msa" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, query, L"annotations", filterAnnotations,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Conversions/query2alignment.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, seq2gen ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"merge-gap", L"10" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readSequence, L"mode", L"0" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"accumulate", L"true" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"write-mode", L"2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"output.gb" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Conversions/seq2gen.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tfbs ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH],
        multiplexer[MAX_ELEMENT_NAME_LENGTH], siteconSearch[MAX_ELEMENT_NAME_LENGTH],
        wmatrixSearch[MAX_ELEMENT_NAME_LENGTH], siteconRead[MAX_ELEMENT_NAME_LENGTH],
        wmatrixRead[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"multiplexer", MAX_ELEMENT_NAME_LENGTH, multiplexer );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-search", MAX_ELEMENT_NAME_LENGTH, siteconSearch );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"wmatrix-search", MAX_ELEMENT_NAME_LENGTH, wmatrixSearch );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-read", MAX_ELEMENT_NAME_LENGTH, siteconRead );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"wmatrix-read", MAX_ELEMENT_NAME_LENGTH, wmatrixRead );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"tfbs.gb" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, siteconSearch, L"result-name", L"sitecon" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, wmatrixSearch, L"result-name", L"weight_matrix" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, multiplexer, L"output-data", writeSequence, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconSearch, L"out-annotations", multiplexer, L"input-data-2" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, wmatrixSearch, L"out-annotations", multiplexer, L"input-data-1" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconRead, L"out-sitecon", siteconSearch, L"in-sitecon" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, wmatrixRead, L"out-wmatrix", wmatrixSearch, L"in-wmatrix" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-sequence", wmatrixSearch, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-sequence", siteconSearch, L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconSearch, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, wmatrixSearch, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", siteconSearch,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconRead, L"sitecon-model", siteconSearch,
        L"in-sitecon.sitecon-model" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", wmatrixSearch,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, wmatrixRead, L"wmatrix", wmatrixSearch,
        L"in-wmatrix.wmatrix" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Data merging/tfbs.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, build_test_HMM ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], readMsa[MAX_ELEMENT_NAME_LENGTH],
        hmm2Build[MAX_ELEMENT_NAME_LENGTH], hmm2WriteProfile[MAX_ELEMENT_NAME_LENGTH],
        hmm2Search[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-msa", MAX_ELEMENT_NAME_LENGTH, readMsa );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"hmm2-build", MAX_ELEMENT_NAME_LENGTH, hmm2Build );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"hmm2-write-profile", MAX_ELEMENT_NAME_LENGTH,
        hmm2WriteProfile );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"hmm2-search", MAX_ELEMENT_NAME_LENGTH, hmm2Search );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"merge-gap", L"10" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readSequence, L"mode", L"0" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, hmm2Build, L"calibrate", L"true" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"calibration-threads", L"1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"deviation", L"200" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"fix-samples-length", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"mean-samples-length", L"325" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"samples-num", L"5000" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"seed", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Build, L"strategy", L"1" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, hmm2WriteProfile, L"write-mode", L"2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2WriteProfile, L"url-out", L"profile.hmm" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, hmm2Search, L"e-val", L"-1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Search, L"result-name", L"hmm_signal" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Search, L"score", L"-1000000000" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Search, L"seqs-num", L"1" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"accumulate", L"true" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"write-mode", L"2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"output.gb" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readMsa, L"msa", hmm2Build, L"in-msa.msa" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, hmm2Build, L"hmm2-profile", hmm2WriteProfile,
        L"in-hmm2.hmm2-profile" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, hmm2Build, L"hmm2-profile", hmm2Search,
        L"in-hmm2.hmm2-profile" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", hmm2Search,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, hmm2Search, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/HMMER/build-test-HMM.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, search_HMM ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], hmm2ReadProfile[MAX_ELEMENT_NAME_LENGTH],
        hmm2Search[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"hmm2-read-profile", MAX_ELEMENT_NAME_LENGTH,
        hmm2ReadProfile );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"hmm2-search", MAX_ELEMENT_NAME_LENGTH, hmm2Search );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"merge-gap", L"10" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readSequence, L"mode", L"0" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, hmm2Search, L"e-val", L"-1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Search, L"result-name", L"hmm_signal" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Search, L"score", L"-1000000000" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, hmm2Search, L"seqs-num", L"1" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"accumulate", L"true" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"write-mode", L"2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"output.gb" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, hmm2ReadProfile, L"hmm2-profile", hmm2Search,
        L"in-hmm2.hmm2-profile" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", hmm2Search,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, hmm2Search, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/HMMER/searchHMM.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, search_TFBS ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH],
        siteconSearch[MAX_ELEMENT_NAME_LENGTH], siteconSearch1[MAX_ELEMENT_NAME_LENGTH],
        siteconSearch2[MAX_ELEMENT_NAME_LENGTH], siteconRead[MAX_ELEMENT_NAME_LENGTH],
        siteconRead1[MAX_ELEMENT_NAME_LENGTH], siteconRead2[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-search", MAX_ELEMENT_NAME_LENGTH, siteconSearch );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-search", MAX_ELEMENT_NAME_LENGTH, siteconSearch1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-search", MAX_ELEMENT_NAME_LENGTH, siteconSearch2 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-read", MAX_ELEMENT_NAME_LENGTH, siteconRead );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-read", MAX_ELEMENT_NAME_LENGTH, siteconRead1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"sitecon-read", MAX_ELEMENT_NAME_LENGTH, siteconRead2 );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"output.gb" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, siteconSearch, L"strand", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, siteconSearch1, L"strand", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, siteconSearch2, L"strand", L"0" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, siteconRead1, L"out-sitecon", siteconSearch1, L"in-sitecon" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconSearch1, L"out-annotations", siteconSearch2,
        L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconRead, L"out-sitecon", siteconSearch, L"in-sitecon" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconSearch2, L"out-annotations", writeSequence,
        L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconSearch, L"out-annotations", siteconSearch1,
        L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, siteconRead2, L"out-sitecon", siteconSearch2, L"in-sitecon" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-sequence", siteconSearch, L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", siteconSearch,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconRead, L"sitecon-model", siteconSearch,
        L"in-sitecon.sitecon-model" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", siteconSearch1,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconRead1, L"sitecon-model", siteconSearch1,
        L"in-sitecon.sitecon-model" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", siteconSearch2,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconRead2, L"sitecon-model", siteconSearch2,
        L"in-sitecon.sitecon-model" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconSearch2, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconSearch, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, siteconSearch1, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Transcriptomics/SearchTFBS.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, call_variants ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], readAssembly[MAX_ELEMENT_NAME_LENGTH],
        callVariants[MAX_ELEMENT_NAME_LENGTH],
        convertSequence[MAX_ELEMENT_NAME_LENGTH], convertBam[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, readAssembly );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"call_variants", MAX_ELEMENT_NAME_LENGTH, callVariants );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH,
        convertSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, convertBam );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readAssembly, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, convertSequence, L"document-format", L"fasta" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, convertBam, L"document-format", L"bam" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, callVariants, L"variants-url", L"variations.vcf" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, callVariants, L"max-dep", L"100" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, readAssembly, L"out-url", convertBam, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, convertBam, L"out-file", callVariants, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-url", convertSequence, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, convertSequence, L"out-file", callVariants, L"in-sequence" );
    CHECK_U2_ERROR( error );

    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readAssembly, L"dataset", callVariants,
        L"in-assembly.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readAssembly, L"url", convertBam, L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, convertBam, L"url", callVariants,
        L"in-assembly.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"url", convertSequence,
        L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, convertSequence, L"url", callVariants,
        L"in-sequence.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/ngs_variant_calling.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, chip_seq ) {
    wchar_t readAnnotations[MAX_ELEMENT_NAME_LENGTH], macsId[MAX_ELEMENT_NAME_LENGTH],
        ceasReport[MAX_ELEMENT_NAME_LENGTH], conservationPlotId[MAX_ELEMENT_NAME_LENGTH],
        seqPosId[MAX_ELEMENT_NAME_LENGTH], peak2GeneId[MAX_ELEMENT_NAME_LENGTH],
        conductGoId[MAX_ELEMENT_NAME_LENGTH], writeAnnotations[MAX_ELEMENT_NAME_LENGTH],
        writeAnnotations1[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH,
        readAnnotations );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"macs-id", MAX_ELEMENT_NAME_LENGTH, macsId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"ceas-report", MAX_ELEMENT_NAME_LENGTH, ceasReport );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"conservation_plot-id", MAX_ELEMENT_NAME_LENGTH,
        conservationPlotId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"seqpos-id", MAX_ELEMENT_NAME_LENGTH, seqPosId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"peak2gene-id", MAX_ELEMENT_NAME_LENGTH, peak2GeneId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"conduct-go-id", MAX_ELEMENT_NAME_LENGTH, conductGoId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-annotations", MAX_ELEMENT_NAME_LENGTH,
        writeAnnotations );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-annotations", MAX_ELEMENT_NAME_LENGTH,
        writeAnnotations1 );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readAnnotations, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, macsId, L"output-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, ceasReport, L"anns-file", L"ceas_annotations.xls" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, ceasReport, L"image-file", L"ceas_report.pdf" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, conservationPlotId, L"output-file",
        L"conservation.bmp" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, seqPosId, L"output-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, conductGoId, L"output-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeAnnotations, L"url-out", L"genes.gb" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeAnnotations1, L"url-out", L"peaks.gb" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, readAnnotations, L"out-url", macsId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", ceasReport, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", conservationPlotId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", seqPosId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", peak2GeneId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, peak2GeneId, L"out-data", conductGoId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, peak2GeneId, L"out-data", writeAnnotations,
        L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, peak2GeneId, L"out-data", writeAnnotations1,
        L"in-annotations" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readAnnotations, L"url", macsId,
        L"in-data._treatment-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"wiggle-treat", ceasReport,
        L"in-data.enrichment-signal" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-regions", ceasReport,
        L"in-data.peak-regions" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-summits", conservationPlotId,
        L"in-data.cp_treat-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-summits", seqPosId,
        L"in-data.cp_treat-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-summits", peak2GeneId,
        L"in-data._treat-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, peak2GeneId, L"gene-annotation-url", conductGoId,
        L"in-data.in-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, peak2GeneId, L"gene-annotation", writeAnnotations,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, peak2GeneId, L"peak-annotation", writeAnnotations1,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/cistrome/chip_seq.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, chip_seq_with_control ) {
    wchar_t readAnnotations1[MAX_ELEMENT_NAME_LENGTH], readAnnotations2[MAX_ELEMENT_NAME_LENGTH],
        macsId[MAX_ELEMENT_NAME_LENGTH], ceasReport[MAX_ELEMENT_NAME_LENGTH],
        conservationPlotId[MAX_ELEMENT_NAME_LENGTH], seqPosId[MAX_ELEMENT_NAME_LENGTH],
        peak2GeneId[MAX_ELEMENT_NAME_LENGTH], conductGoId[MAX_ELEMENT_NAME_LENGTH],
        multiplexer[MAX_ELEMENT_NAME_LENGTH], writeAnnotations[MAX_ELEMENT_NAME_LENGTH],
        writeAnnotations1[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH,
        readAnnotations1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH,
        readAnnotations2 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"macs-id", MAX_ELEMENT_NAME_LENGTH, macsId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"ceas-report", MAX_ELEMENT_NAME_LENGTH, ceasReport );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"conservation_plot-id", MAX_ELEMENT_NAME_LENGTH,
        conservationPlotId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"seqpos-id", MAX_ELEMENT_NAME_LENGTH, seqPosId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"peak2gene-id", MAX_ELEMENT_NAME_LENGTH, peak2GeneId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"conduct-go-id", MAX_ELEMENT_NAME_LENGTH, conductGoId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"multiplexer", MAX_ELEMENT_NAME_LENGTH, multiplexer );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-annotations", MAX_ELEMENT_NAME_LENGTH,
        writeAnnotations );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-annotations", MAX_ELEMENT_NAME_LENGTH,
        writeAnnotations1 );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readAnnotations1, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readAnnotations2, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, macsId, L"output-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, ceasReport, L"anns-file", L"ceas_annotations.xls" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, ceasReport, L"image-file", L"ceas_report.pdf" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, conservationPlotId, L"output-file",
        L"conservation.bmp" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, seqPosId, L"output-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, conductGoId, L"output-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeAnnotations, L"url-out", L"genes.gb" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, writeAnnotations1, L"url-out", L"peaks.gb" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, readAnnotations1, L"out-url", multiplexer, L"input-data-1" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readAnnotations2, L"out-url", multiplexer, L"input-data-2" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, multiplexer, L"output-data", macsId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", ceasReport, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", conservationPlotId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", seqPosId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, macsId, L"out-data", peak2GeneId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, peak2GeneId, L"out-data", conductGoId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, peak2GeneId, L"out-data", writeAnnotations,
        L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, peak2GeneId, L"out-data", writeAnnotations1,
        L"in-annotations" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readAnnotations1, L"url", macsId,
        L"in-data._treatment-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readAnnotations2, L"url", macsId,
        L"in-data.control-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"wiggle-treat", ceasReport,
        L"in-data.enrichment-signal" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-regions", ceasReport,
        L"in-data.peak-regions" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-summits", conservationPlotId,
        L"in-data.cp_treat-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-summits", seqPosId,
        L"in-data.cp_treat-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, macsId, L"peak-summits", peak2GeneId,
        L"in-data._treat-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, peak2GeneId, L"gene-annotation-url", conductGoId,
        L"in-data.in-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, peak2GeneId, L"gene-annotation", writeAnnotations,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, peak2GeneId, L"peak-annotation", writeAnnotations1,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/cistrome/chip_seq_with_control.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tuxedo_main ) {
    wchar_t getFileList[MAX_ELEMENT_NAME_LENGTH], topHat[MAX_ELEMENT_NAME_LENGTH],
        cufflinks[MAX_ELEMENT_NAME_LENGTH], cuffmerge[MAX_ELEMENT_NAME_LENGTH],
        cuffdiff[MAX_ELEMENT_NAME_LENGTH], filesConversion[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cuffdiff", MAX_ELEMENT_NAME_LENGTH, cuffdiff );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cuffmerge", MAX_ELEMENT_NAME_LENGTH, cuffmerge );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cufflinks", MAX_ELEMENT_NAME_LENGTH, cufflinks );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"tophat", MAX_ELEMENT_NAME_LENGTH, topHat );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH,
        filesConversion );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, topHat, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cufflinks, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cuffmerge, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cuffdiff, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, getFileList, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList, L"url-in.dataset", L"Dataset 2" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, getFileList, L"out-url", filesConversion, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion, L"out-file", topHat, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, cufflinks, L"out-annotations", cuffmerge, L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, cuffmerge, L"out-annotations", cuffdiff, L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, topHat, L"out-assembly", cufflinks, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, topHat, L"out-assembly", cuffdiff, L"in-assembly" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, getFileList, L"dataset", topHat,
        L"in-sequence.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList, L"url", filesConversion, L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion, L"url", topHat, L"in-sequence.in-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cufflinks, L"in-assembly.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, cufflinks, L"isolevel.slot", cuffmerge,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, cuffmerge, L"annotations", cuffdiff,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cuffdiff, L"in-assembly.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/tuxedo/tuxedo_main.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tuxedo_main_paired ) {
    wchar_t getFileList1[MAX_ELEMENT_NAME_LENGTH], getFileList2[MAX_ELEMENT_NAME_LENGTH],
        topHat[MAX_ELEMENT_NAME_LENGTH], cufflinks[MAX_ELEMENT_NAME_LENGTH],
        cuffmerge[MAX_ELEMENT_NAME_LENGTH], cuffdiff[MAX_ELEMENT_NAME_LENGTH],
        multiplexer[MAX_ELEMENT_NAME_LENGTH], filesConversion1[MAX_ELEMENT_NAME_LENGTH],
        filesConversion2[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList2 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cuffdiff", MAX_ELEMENT_NAME_LENGTH, cuffdiff );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cuffmerge", MAX_ELEMENT_NAME_LENGTH, cuffmerge );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cufflinks", MAX_ELEMENT_NAME_LENGTH, cufflinks );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"tophat", MAX_ELEMENT_NAME_LENGTH, topHat );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"multiplexer", MAX_ELEMENT_NAME_LENGTH, multiplexer );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion2 );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, topHat, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cufflinks, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cuffmerge, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cuffdiff, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, getFileList1, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList1, L"url-in.dataset", L"Dataset 2" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, getFileList2, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList2, L"url-in.dataset", L"Dataset 2" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion1, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion1, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion2, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion2, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, topHat, L"out-assembly", cufflinks, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, topHat, L"out-assembly", cuffdiff, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion2, L"out-file", multiplexer, L"input-data-2" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion1, L"out-file", multiplexer, L"input-data-1" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, multiplexer, L"output-data", topHat, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, cufflinks, L"out-annotations", cuffmerge, L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, cuffmerge, L"out-annotations", cuffdiff, L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList1, L"out-url", filesConversion1, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList2, L"out-url", filesConversion2, L"in-file" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, getFileList1, L"url", filesConversion1,
        L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList2, L"url", filesConversion2,
        L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList1, L"dataset", topHat,
        L"in-sequence.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion1, L"url", topHat,
        L"in-sequence.in-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion1, L"url", topHat,
        L"in-sequence.paired-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cufflinks, L"in-assembly.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, cufflinks, L"isolevel.slot", cuffmerge,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, cuffmerge, L"annotations", cuffdiff,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cuffdiff, L"in-assembly.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/tuxedo/tuxedo_main_paired.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tuxedo_multiple_dataset ) {
    wchar_t getFileList[MAX_ELEMENT_NAME_LENGTH], readAnnotations[MAX_ELEMENT_NAME_LENGTH],
        topHat[MAX_ELEMENT_NAME_LENGTH], cuffdiff[MAX_ELEMENT_NAME_LENGTH],
        filesConversion[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-annotations", MAX_ELEMENT_NAME_LENGTH, readAnnotations );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cuffdiff", MAX_ELEMENT_NAME_LENGTH, cuffdiff );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"tophat", MAX_ELEMENT_NAME_LENGTH, topHat );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, topHat, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cuffdiff, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readAnnotations, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList, L"url-in.dataset", L"Dataset 2" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, topHat, L"out-assembly", cuffdiff, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion, L"out-file", topHat, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readAnnotations, L"out-annotations", cuffdiff,
        L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList, L"out-url", filesConversion, L"in-file" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, getFileList, L"dataset", topHat,
        L"in-sequence.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion, L"url", topHat,
        L"in-sequence.in-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readAnnotations, L"annotations", cuffdiff,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cuffdiff, L"in-assembly.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList, L"url", filesConversion, L"in-file.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/tuxedo/tuxedo_no_novel_transcr.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tuxedo_multiple_dataset_paired ) {
    wchar_t getFileList1[MAX_ELEMENT_NAME_LENGTH], getFileList2[MAX_ELEMENT_NAME_LENGTH],
        readAnnotations[MAX_ELEMENT_NAME_LENGTH], topHat[MAX_ELEMENT_NAME_LENGTH],
        cuffdiff[MAX_ELEMENT_NAME_LENGTH], multiplexer[MAX_ELEMENT_NAME_LENGTH],
        filesConversion1[MAX_ELEMENT_NAME_LENGTH], filesConversion2[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList2 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-annotations", MAX_ELEMENT_NAME_LENGTH,
        readAnnotations );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cuffdiff", MAX_ELEMENT_NAME_LENGTH, cuffdiff );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"tophat", MAX_ELEMENT_NAME_LENGTH, topHat );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"multiplexer", MAX_ELEMENT_NAME_LENGTH, multiplexer );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion2 );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, topHat, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cuffdiff, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readAnnotations, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList1, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList1, L"url-in.dataset", L"Dataset 2" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList2, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList2, L"url-in.dataset", L"Dataset 2" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion1, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion1, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion2, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion2, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, topHat, L"out-assembly", cuffdiff, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion2, L"out-file", multiplexer, L"input-data-1" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion1, L"out-file", multiplexer, L"input-data-2" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, multiplexer, L"output-data", topHat, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readAnnotations, L"out-annotations", cuffdiff,
        L"in-annotations" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList1, L"out-url", filesConversion2, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList2, L"out-url", filesConversion1, L"in-file" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, getFileList1, L"dataset", topHat,
        L"in-sequence.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion2, L"url", topHat, L"in-sequence.in-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion1, L"url", topHat,
        L"in-sequence.paired-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readAnnotations, L"annotations", cuffdiff,
        L"in-annotations.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cuffdiff, L"in-assembly.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList2, L"url", filesConversion1,
        L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList1, L"url", filesConversion2,
        L"in-file.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/tuxedo/tuxedo_no_novel_transcr_paired.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tuxedo_single_dataset ) {
    wchar_t getFileList[MAX_ELEMENT_NAME_LENGTH], topHat[MAX_ELEMENT_NAME_LENGTH],
        cufflinks[MAX_ELEMENT_NAME_LENGTH], filesConversion[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cufflinks", MAX_ELEMENT_NAME_LENGTH, cufflinks );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"tophat", MAX_ELEMENT_NAME_LENGTH, topHat );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH,
        filesConversion );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, topHat, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cufflinks, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, getFileList, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, topHat, L"out-assembly", cufflinks, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion, L"out-file", topHat, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList, L"out-url", filesConversion, L"in-file" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, getFileList, L"dataset", topHat,
        L"in-sequence.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion, L"url", topHat,
        L"in-sequence.in-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList, L"url", filesConversion, L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cufflinks, L"in-assembly.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/tuxedo/tuxedo_single_dataset.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, tuxedo_single_dataset_paired ) {
    wchar_t getFileList1[MAX_ELEMENT_NAME_LENGTH], getFileList2[MAX_ELEMENT_NAME_LENGTH],
        topHat[MAX_ELEMENT_NAME_LENGTH], cufflinks[MAX_ELEMENT_NAME_LENGTH],
        multiplexer[MAX_ELEMENT_NAME_LENGTH], filesConversion1[MAX_ELEMENT_NAME_LENGTH],
        filesConversion2[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"get-file-list", MAX_ELEMENT_NAME_LENGTH, getFileList2 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"cufflinks", MAX_ELEMENT_NAME_LENGTH, cufflinks );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"tophat", MAX_ELEMENT_NAME_LENGTH, topHat );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"multiplexer", MAX_ELEMENT_NAME_LENGTH, multiplexer );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion1 );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"files-conversion", MAX_ELEMENT_NAME_LENGTH, filesConversion2 );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, topHat, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, cufflinks, L"out-dir", L"tools_output" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, getFileList1, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, getFileList2, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion1, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion1, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, filesConversion2, L"document-format", L"fastq" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, filesConversion2, L"excluded-formats", L"fasta" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, topHat, L"out-assembly", cufflinks, L"in-assembly" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion1, L"out-file", multiplexer, L"input-data-1" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList1, L"out-url", filesConversion1, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, filesConversion2, L"out-file", multiplexer, L"input-data-2" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, getFileList2, L"out-url", filesConversion2, L"in-file" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, multiplexer, L"output-data", topHat, L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, getFileList1, L"dataset", topHat,
        L"in-sequence.dataset" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion1, L"url", topHat,
        L"in-sequence.in-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList1, L"url", filesConversion1,
        L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, filesConversion2, L"url", topHat,
        L"in-sequence.paired-url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, getFileList2, L"url", filesConversion2,
        L"in-file.url" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, topHat, L"hits-url", cufflinks, L"in-assembly.url" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/tuxedo/tuxedo_single_dataset_paired.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, find_sequences ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], search[MAX_ELEMENT_NAME_LENGTH],
        writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"search", MAX_ELEMENT_NAME_LENGTH, search );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, search, L"result-name", L"misc_feature" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, search, L"use-names", L"true" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"write-mode", L"0" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"annotations.gb" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, search, L"out-annotations", writeSequence, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-sequence", search, L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", search,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, search, L"annotations", writeSequence, L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Scenarios/find_sequences.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, gene_by_gene_report ) {
    wchar_t readSequence[MAX_ELEMENT_NAME_LENGTH], geneByGeneReportId[MAX_ELEMENT_NAME_LENGTH],
        blast[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"genebygene-report-id", MAX_ELEMENT_NAME_LENGTH,
        geneByGeneReportId );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"blast-plus", MAX_ELEMENT_NAME_LENGTH, blast );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"annotated_sequence.gb" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, geneByGeneReportId, L"output-file", L"report.txt" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, blast, L"out-annotations", geneByGeneReportId, L"in-data" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-sequence", blast, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, blast, L"out-annotations", writeSequence, L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", geneByGeneReportId,
        L"in-data.gene-seq" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", blast,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, blast, L"annotations", geneByGeneReportId,
        L"in-data.gene-ann" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, blast, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Scenarios/gene_by_gene_report.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, merge_sequence_annotation ) {
    wchar_t readAnnotations[MAX_ELEMENT_NAME_LENGTH], readSequence[MAX_ELEMENT_NAME_LENGTH],
        multiplexer[MAX_ELEMENT_NAME_LENGTH], writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-annotations", MAX_ELEMENT_NAME_LENGTH,
        readAnnotations );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"read-sequence", MAX_ELEMENT_NAME_LENGTH, readSequence );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"multiplexer", MAX_ELEMENT_NAME_LENGTH, multiplexer );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readSequence, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, readAnnotations, L"url-in.dataset", L"Dataset" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"document-format", L"genbank" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"merged.gb" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, readAnnotations, L"out-annotations", multiplexer, L"input-data-1" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readSequence, L"out-sequence", multiplexer, L"input-data-2" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, multiplexer, L"output-data", writeSequence, L"in-sequence" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, readAnnotations, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"annotations", writeSequence,
        L"in-sequence.annotations" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readSequence, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/Scenarios/merge_sequence_annotation.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

IMPLEMENT_TEST( CInterfaceManualTests, consensus ) {
    wchar_t readAssembly[MAX_ELEMENT_NAME_LENGTH];
    wchar_t extractConsensus[MAX_ELEMENT_NAME_LENGTH];
    wchar_t writeSequence[MAX_ELEMENT_NAME_LENGTH];
    SchemeHandle scheme = NULL;
    U2ErrorType error = createScheme( NULL, &scheme );
    CHECK_U2_ERROR( error );

    error = addElementToScheme( scheme, L"read-assembly", MAX_ELEMENT_NAME_LENGTH, readAssembly );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"extract-consensus", MAX_ELEMENT_NAME_LENGTH, extractConsensus );
    CHECK_U2_ERROR( error );
    error = addElementToScheme( scheme, L"write-sequence", MAX_ELEMENT_NAME_LENGTH, writeSequence );
    CHECK_U2_ERROR( error );

    error = setSchemeElementAttribute( scheme, readAssembly, L"url-in.dataset", L"Dataset 1" );
    CHECK_U2_ERROR( error );
    error = setSchemeElementAttribute( scheme, writeSequence, L"url-out", L"consensus.fa" );
    CHECK_U2_ERROR( error );

    error = addFlowToScheme( scheme, extractConsensus, L"out-sequence", writeSequence, L"in-sequence" );
    CHECK_U2_ERROR( error );
    error = addFlowToScheme( scheme, readAssembly, L"out-assembly", extractConsensus, L"in-assembly" );
    CHECK_U2_ERROR( error );

    error = addSchemeActorsBinding( scheme, extractConsensus, L"sequence", writeSequence,
        L"in-sequence.sequence" );
    CHECK_U2_ERROR( error );
    error = addSchemeActorsBinding( scheme, readAssembly, L"assembly", extractConsensus,
        L"in-assembly.assembly" );
    CHECK_U2_ERROR( error );

    U2OpStatusImpl stateInfo;
    SchemeSimilarityUtils::checkSchemesSimilarity( scheme,
        PROPER_WD_SCHEMES_PATH + "/NGS/consensus.uwl", stateInfo );
    CHECK_NO_ERROR( stateInfo );

    releaseScheme( scheme );
}

} // namespace U2
