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

#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include "CoreTests.h"

//built-in test impls
#include "AnnotationTableObjectTest.h"
#include "AsnParserTests.h"
#include "BinaryFindOpenCLTests.h"
#include "BioStruct3DObjectTests.h"
#include "CMDLineTests.h"
#include "DNASequenceObjectTests.h"
#include "DNATranslationImplTests.h"
#include "DnaAssemblyTests.h"
#include "DnaStatisticsTests.h"
#include "DocumentModelTests.h"
#include "EditAlignmentTests.h"
#include "EditSequenceTests.h"
#include "FindAlgorithmTests.h"
#include "FindPatternMsaTaskTest.h"
#include "FormatDetectionTests.h"
#include "GUrlTests.h"
#include "LoadRemoteDocumentTests.h"
#include "McaObjectTests.h"
#include "MsaObjectTests.h"
#include "PWMatrixTests.h"
#include "PhyTreeObjectTests.h"
#include "RealignSequencesInAlignmentTaskTest.h"
#include "SMatrixTests.h"
#include "SecStructPredictTests.h"
#include "SequenceWalkerTests.h"
#include "AnnotationUtilsTests.h"
#include "TaskTests.h"
#include "TextObjectTests.h"
#include "UtilTestActions.h"

namespace U2 {

extern "C" Q_DECL_EXPORT U2::Plugin* U2_PLUGIN_INIT_FUNC()
{
    return new CoreTests();
}

CoreTests::CoreTests() : Plugin("Core tests", "Core lib tests") {
    registerFactories();
}
CoreTests::~CoreTests() {
}

template <class Factory>
bool CoreTests::registerFactory(XMLTestFormat *xmlTestFormat) {
    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = Factory::createTestFactories();

    bool res = true;
    foreach(XMLTestFactory* f, l->qlist) {
        bool ok = xmlTestFormat->registerTestFactory(f);
        res = res && ok;
    }

    assert(res);
    return res;
}

void CoreTests::registerFactories() {

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    registerFactory<SMatrixTests>(xmlTestFormat);

    //Document model tests
    registerFactory<DocumentModelTests>(xmlTestFormat);

    // MultipleChromatogramAlignmentObject Tests
    registerFactory<McaObjectTests>(xmlTestFormat);

    // MultipleSequenceAlignmentObject Tests
    registerFactory<MsaObjectTests>(xmlTestFormat);

    //U2SequenceObject Tests
    registerFactory<DNASequenceObjectTests>(xmlTestFormat);

    //DNATranslationImplTests Tests
    registerFactory<DNATranslationImplTests>(xmlTestFormat);

    //Sequence Walker Test
    registerFactory<SequenceWalkerTests>(xmlTestFormat);

    //BioStruct3D Object Tests
    registerFactory<BioStruct3DObjectTests>(xmlTestFormat);

    //PhyTree Object Tests
    registerFactory<PhyTreeObjectTests>(xmlTestFormat);

    //Task Tests
    registerFactory<TaskTests>(xmlTestFormat);

    // Edit sequence tests
    registerFactory<EditSequenceTests>(xmlTestFormat);

    // Annotation tests
    registerFactory<AnnotationTableObjectTest>(xmlTestFormat);

    // AsnParser tests
    registerFactory<AsnParserTests>(xmlTestFormat);

    // SecStructPredict tests
    registerFactory<SecStructPredictTests>(xmlTestFormat);

    // DnaAssembly tests
    registerFactory<DnaAssemblyTests>(xmlTestFormat);

    registerFactory<CreateSubalignimentTests>(xmlTestFormat);

    registerFactory<LoadRemoteDocumentTests>(xmlTestFormat);

    // PWMatrix tests
    registerFactory<PWMatrixTests>(xmlTestFormat);

    // CMDLine tests
    registerFactory<CMDLineTests>(xmlTestFormat);

    // GUrl tests
    registerFactory<GUrlTests>(xmlTestFormat);

    // BinaryFindOpencl tests
#ifdef OPENCL_SUPPORT
    registerFactory<BinaryFindOpenCLTests>(xmlTestFormat);
#endif

    // FindAlforithm tests
    registerFactory<FindAlgorithmTests>(xmlTestFormat);

    // Automatic format detection tests
    registerFactory<FormatDetectionTests>(xmlTestFormat);

    registerFactory<TextObjectTests>(xmlTestFormat);

    // Some utility actions to use them in tests
    registerFactory<UtilTestActions>(xmlTestFormat);

    registerFactory<RealignTests>(xmlTestFormat);

    registerFactory<FindPatternMsaTests>(xmlTestFormat);

    registerFactory<AnnotationUtilsTests>(xmlTestFormat);

    registerFactory<DnaStatisticsTests>(xmlTestFormat);
}

}//namespace
