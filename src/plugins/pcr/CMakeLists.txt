set(UGENE_PLUGIN_ID pcr)
set(UGENE_PLUGIN_NAME "In silico PCR")

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)
target_link_libraries(pcr U2Test)

# Opt-in standalone runner; the same regression tests also run via pcr_suite.xml.
option(UGENE_BUILD_PCR_TESTS "Build the PCR regression and benchmark runner" OFF)
if (UGENE_BUILD_PCR_TESTS)
    add_executable(pcr_tests
            ${CMAKE_SOURCE_DIR}/tests/pcr/PcrTestRunner.cpp
            src/InSilicoPcrTask.cpp src/InSilicoPcrTask.h
            src/InSilicoPcrTests.cpp src/InSilicoPcrTests.h)
    target_include_directories(pcr_tests PRIVATE ${CMAKE_SOURCE_DIR})
    target_compile_definitions(pcr_tests PRIVATE PCR_TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/data")
    target_link_libraries(pcr_tests PRIVATE U2Core U2Algorithm U2Private U2Test Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml)
endif ()
