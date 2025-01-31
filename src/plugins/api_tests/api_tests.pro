include (api_tests.pri)

# Input
HEADERS += \
    src/ApiTestsPlugin.h \
    src/unittest.h \
    src/UnitTestSuite.h \
    src/core/datatype/annotations/AnnotationGroupUnitTests.h \
    src/core/datatype/annotations/AnnotationUnitTests.h \
    src/core/datatype/msa/MsaConsensusAlgorithmUnitTests.h \
    src/core/datatype/msa/MsaRowUnitTests.h \
    src/core/datatype/msa/MsaRowUtilsUnitTests.h \
    src/core/datatype/msa/MsaUnitTests.h \
    src/core/datatype/udr/RawDataUdrSchemaUnitTests.h \
    src/core/datatype/udr/UdrSchemaUnitTests.h \
    src/core/dbi/DbiTest.h \
    src/core/dbi/assembly/AssemblyDbiTestUtil.h \
    src/core/dbi/assembly/AssemblyDbiUnitTests.h \
    src/core/dbi/attribute/AttributeDbiUnitTests.h \
    src/core/dbi/features/FeatureDbiUnitTests.h \
    src/core/dbi/msa/MsaDbiUnitTests.h \
    src/core/dbi/sequence/SequenceDbiUnitTests.h \
    src/core/dbi/udr/UdrDbiUnitTests.h \
    src/core/external_script/base_scheme_interface/CInterfaceManualTests.h \
    src/core/external_script/base_scheme_interface/CInterfaceSasTests.h \
    src/core/external_script/base_scheme_interface/SchemeSimilarityUtils.h \
    src/core/format/fastq/FastqUnitTests.h \
    src/core/format/genbank/LocationParserUnitTests.h \
    src/core/format/sqlite_mod_dbi/ModDbiSQLiteSpecificUnitTests.h \
    src/core/format/sqlite_msa_dbi/MsaDbiSQLiteSpecificUnitTests.h \
    src/core/format/sqlite_object_dbi/SQLiteObjectDbiUnitTests.h \
    src/core/format/sqlite_sequence_dbi/SequenceDbiSQLiteSpecificUnitTests.h \
    src/core/gobjects/BioStruct3DObjectUnitTests.h \
    src/core/gobjects/ChromatogramObjectUnitTests.h \
    src/core/gobjects/FeaturesTableObjectUnitTest.h \
    src/core/gobjects/MsaObjectUnitTests.h \
    src/core/gobjects/PhyTreeObjectUnitTests.h \
    src/core/gobjects/TextObjectUnitTests.h \
    src/core/util/BAMUtilsUnitTests.h \
    src/core/util/DatatypeSerializeUtilsUnitTest.h \
    src/core/util/DynTableTests.h \
    src/core/util/MsaDbiUtilsUnitTests.h \
    src/core/util/MsaImporterExporterUnitTests.h \
    src/core/util/MsaUtilsUnitTests.h

SOURCES += \
    src/ApiTestsPlugin.cpp \
    src/UnitTestSuite.cpp \
    src/core/datatype/annotations/AnnotationGroupUnitTests.cpp \
    src/core/datatype/annotations/AnnotationUnitTests.cpp \
    src/core/datatype/msa/MsaConsensusAlgorithmUnitTests.cpp \
    src/core/datatype/msa/MsaRowUnitTests.cpp \
    src/core/datatype/msa/MsaRowUtilsUnitTests.cpp \
    src/core/datatype/msa/MsaUnitTests.cpp \
    src/core/datatype/udr/RawDataUdrSchemaUnitTests.cpp \
    src/core/datatype/udr/UdrSchemaUnitTests.cpp \
    src/core/dbi/DbiTest.cpp \
    src/core/dbi/assembly/AssemblyDbiTestUtil.cpp \
    src/core/dbi/assembly/AssemblyDbiUnitTests.cpp \
    src/core/dbi/attribute/AttributeDbiUnitTests.cpp \
    src/core/dbi/features/FeatureDbiUnitTests.cpp \
    src/core/dbi/msa/MsaDbiUnitTests.cpp \
    src/core/dbi/sequence/SequenceDbiUnitTests.cpp \
    src/core/dbi/udr/UdrDbiUnitTests.cpp \
    src/core/external_script/base_scheme_interface/CInterfaceManualTests.cpp \
    src/core/external_script/base_scheme_interface/CInterfaceSasTests.cpp \
    src/core/external_script/base_scheme_interface/SchemeSimilarityUtils.cpp \
    src/core/format/fastq/FastqUnitTests.cpp \
    src/core/format/genbank/LocationParserUnitTests.cpp \
    src/core/format/sqlite_mod_dbi/ModDbiSQLiteSpecificUnitTests.cpp \
    src/core/format/sqlite_msa_dbi/MsaDbiSQLiteSpecificUnitTests.cpp \
    src/core/format/sqlite_object_dbi/SQLiteObjectDbiUnitTests.cpp \
    src/core/format/sqlite_sequence_dbi/SequenceDbiSQLiteSpecificUnitTests.cpp \
    src/core/gobjects/BioStruct3DObjectUnitTests.cpp \
    src/core/gobjects/ChromatogramObjectUnitTests.cpp \
    src/core/gobjects/FeaturesTableObjectUnitTest.cpp \
    src/core/gobjects/MsaObjectUnitTests.cpp \
    src/core/gobjects/PhyTreeObjectUnitTests.cpp \
    src/core/gobjects/TextObjectUnitTests.cpp \
    src/core/util/BAMUtilsUnitTests.cpp \
    src/core/util/DatatypeSerializeUtilsUnitTest.cpp \
    src/core/util/DynTableTests.cpp \
    src/core/util/MsaDbiUtilsUnitTests.cpp \
    src/core/util/MsaImporterExporterUnitTests.cpp \
    src/core/util/MsaUtilsUnitTests.cpp
