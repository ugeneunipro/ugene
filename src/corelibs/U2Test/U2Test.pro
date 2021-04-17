include (U2Test.pri)

# Input
HEADERS += src/GTest.h \
           src/GTestFrameworkComponents.h \
           src/TestRunnerSettings.h \
           src/TestRunnerTask.h \
           src/xmltest/XMLTestFormat.h \
           src/xmltest/XMLTestUtils.h

SOURCES += src/GTest.cpp \
           src/GTestFrameworkComponents.cpp \
           src/TestRunnerTask.cpp \
           src/xmltest/XMLTestFormat.cpp \
           src/xmltest/XMLTestUtils.cpp

TRANSLATIONS += transl/russian.ts
