include (remote_service.pri)

# Input
HEADERS += src/Base64File.h \
           src/BufferedDataReader.h \
           src/RemoteServiceCommon.h \
           src/RemoteServiceMachine.h \
           src/RemoteServicePingTask.h \
           src/RemoteServicePlugin.h \
           src/RemoteServiceSettingsUI.h \
           src/RemoteServiceUtilTasks.h \
           src/RemoteTasksDialog.h \
           src/UctpRequestBuilders.h \
           src/WebTransportProtocol.h \
           src/base64/cdecode.h \
           src/base64/cencode.h
FORMS += src/RemoteServiceSupportUI.ui src/TaskStatisticsDialog.ui
SOURCES += src/Base64File.cpp \
           src/BufferedDataReader.cpp \
           src/RemoteServiceMachine.cpp \
           src/RemoteServicePingTask.cpp \
           src/RemoteServicePlugin.cpp \
           src/RemoteServiceSettingsUI.cpp \
           src/RemoteServiceUtilTasks.cpp \
           src/RemoteTasksDialog.cpp \
           src/UctpRequestBuilders.cpp \
           src/WebTransportProtocol.cpp \
           src/base64/cdecode.c \
           src/base64/cencode.c
