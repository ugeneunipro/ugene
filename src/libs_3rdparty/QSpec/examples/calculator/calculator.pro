QT += widgets

HEADERS       = button.h \
                calculator.h \
                calcguitests.h
SOURCES       = button.cpp \
                calculator.cpp \
                calcguitests.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/calculator
INSTALLS += target

INCLUDEPATH +=../../src/

CONFIG(debug, debug|release) {
    LIBS += -L../../_build/debug -lqspecd
}

CONFIG(release, debug|release) {
    LIBS += -L../../_build/release -lqspec
}

