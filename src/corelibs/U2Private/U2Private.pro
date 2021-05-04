include (U2Private.pri)

# Input
HEADERS += src/AppContextImpl.h \
           src/AppSettingsImpl.h \
           src/ConsoleLogDriver.h \
           src/CredentialsAskerCli.h \
           src/DocumentFormatRegistryImpl.h \
           src/IOAdapterRegistryImpl.h \
           src/LogSettings.h \
           src/PluginDescriptor.h \
           src/PluginSupportImpl.h \
           src/ServiceRegistryImpl.h \
           src/SettingsImpl.h \
           src/TaskSchedulerImpl.h \
           src/crash_handler/CrashHandler.h \
           src/crash_handler/CrashHandlerArgsHelper.h \
           src/crash_handler/CrashHandlerPrivate.h \
           src/crash_handler/CrashLogCache.h \
           src/crash_handler/StackWalker.h

SOURCES += src/AppContextImpl.cpp \
           src/AppSettingsImpl.cpp \
           src/ConsoleLogDriver.cpp \
           src/CredentialsAskerCli.cpp \
           src/DocumentFormatRegistryImpl.cpp \
           src/IOAdapterRegistryImpl.cpp \
           src/LogSettings.cpp \
           src/PluginDescriptor.cpp \
           src/PluginSupportImpl.cpp \
           src/ServiceRegistryImpl.cpp \
           src/SettingsImpl.cpp \
           src/TaskSchedulerImpl.cpp \
           src/crash_handler/CrashHandler.cpp \
           src/crash_handler/CrashHandlerArgsHelper.cpp \
           src/crash_handler/CrashHandlerPrivate.cpp \
           src/crash_handler/CrashLogCache.cpp \
           src/crash_handler/StackWalker.cpp

TRANSLATIONS += transl/russian.ts

win32 {
    HEADERS += src/crash_handler/CrashHandlerPrivateWin.h
    SOURCES += src/crash_handler/CrashHandlerPrivateWin.cpp
}

unix_not_mac() {
    HEADERS += src/crash_handler/CrashHandlerPrivateUnixNotMac.h
    SOURCES += src/crash_handler/CrashHandlerPrivateUnixNotMac.cpp
}

mac {
    HEADERS += src/crash_handler/CrashHandlerPrivateMac.h \
               src/SleepPreventerMac.h
    SOURCES += src/crash_handler/CrashHandlerPrivateMac.cpp \
               src/SleepPreventerMac.cpp
}
