include (ugeneui.pri)

# Input
HEADERS += src/app_settings/AppSettingsDialogController.h \
           src/app_settings/AppSettingsDialogTree.h \
           src/app_settings/AppSettingsGUIImpl.h \
           src/app_settings/directories_settings/DirectoriesSettingsGUIController.h \
           src/app_settings/format_settings/FormatSettingsGUIController.h \
           src/app_settings/logview_settings/LogSettingsGUIController.h \
           src/app_settings/network_settings/NetworkSettingsGUIController.h \
           src/app_settings/resource_settings/ResourceSettingsGUIController.h \
           src/app_settings/user_apps_settings/UserApplicationsSettingsGUIController.h \
           src/main_window/AboutDialogController.h \
           src/main_window/DockWidgetPainter.h \
           src/main_window/SplashScreen.h \
           src/main_window/CheckUpdatesTask.h \
           src/main_window/DockManagerImpl.h \
           src/main_window/MainWindowImpl.h \
           src/main_window/MDIManagerImpl.h \
           src/main_window/MenuManager.h \
           src/main_window/ShutdownTask.h \
           src/main_window/TmpDirChangeDialogController.h \
           src/main_window/ToolBarManager.h \
           src/plugin_viewer/PluginViewerController.h \
           src/plugin_viewer/PluginViewerImpl.h \
           src/project_support/DocumentFormatSelectorController.h \
           src/project_support/DocumentProviderSelectorController.h \
           src/project_support/DocumentReadingModeSelectorController.h \
           src/project_support/MultipleDocumentsReadingModeSelectorController.h \
           src/project_support/ExportProjectDialogController.h \
           src/project_support/ProjectImpl.h \
           src/project_support/ProjectLoaderImpl.h \
           src/project_support/ProjectServiceImpl.h \
           src/project_support/ProjectTasksGui.h \
           src/project_view/ProjectViewImpl.h \
           src/shtirlitz/Shtirlitz.h \
           src/shtirlitz/StatisticalReportController.h \
           src/task_view/TaskStatusBar.h \
           src/task_view/TaskViewController.h \
           src/update/UgeneUpdater.h \
           src/welcome_page/WelcomePageMdi.h \
           src/welcome_page/WelcomePageMdiController.h \
           src/welcome_page/WelcomePageWidget.h

FORMS += src/app_settings/directories_settings/DirectoriesSettingsWidget.ui \
         src/app_settings/format_settings/FormatSettingsWidget.ui \
         src/app_settings/logview_settings/LogSettingsWidget.ui \
         src/app_settings/network_settings/NetworkSettingsWidget.ui \
         src/app_settings/resource_settings/ResourceSettingsWidget.ui \
         src/app_settings/AppSettingsDialog.ui \
         src/app_settings/user_apps_settings/UserApplicationsSettingsWidget.ui \
         src/main_window/AboutDialog.ui \
         src/main_window/TmpDirChangeDialog.ui \
         src/plugin_viewer/PluginViewerWidget.ui \
         src/project_support/CreateNewProjectWidget.ui \
         src/project_support/DocumentFormatSelectorDialog.ui \
         src/project_support/DocumentProviderSelectorDialog.ui \
         src/project_support/ExportProjectDialog.ui \
         src/project_support/MultipleSequenceFilesReadingMode.ui \
         src/project_support/SequenceReadingModeSelectorDialog.ui \
         src/project_support/SaveProjectDialog.ui \
         src/project_view/ProjectViewWidget.ui \
         src/shtirlitz/StatisticalReport.ui

SOURCES += src/Main.cpp \
           src/app_settings/AppSettingsDialogController.cpp \
           src/app_settings/AppSettingsGUIImpl.cpp \
           src/app_settings/directories_settings/DirectoriesSettingsGUIController.cpp \
           src/app_settings/format_settings/FormatSettingsGUIController.cpp \
           src/app_settings/logview_settings/LogSettingsGUIController.cpp \
           src/app_settings/network_settings/NetworkSettingsGUIController.cpp \
           src/app_settings/resource_settings/ResourceSettingsGUIController.cpp \
           src/app_settings/user_apps_settings/UserApplicationsSettingsGUIController.cpp \
           src/main_window/AboutDialogController.cpp \
           src/main_window/DockWidgetPainter.cpp \
           src/main_window/SplashScreen.cpp \
           src/main_window/CheckUpdatesTask.cpp \
           src/main_window/DockManagerImpl.cpp \
           src/main_window/MainWindowImpl.cpp \
           src/main_window/MDIManagerImpl.cpp \
           src/main_window/MenuManager.cpp \
           src/main_window/ShutdownTask.cpp \
           src/main_window/TmpDirChangeDialogController.cpp \
           src/main_window/ToolBarManager.cpp \
           src/plugin_viewer/PluginViewerController.cpp \
           src/plugin_viewer/PluginViewerImpl.cpp \
           src/project_support/DocumentFormatSelectorController.cpp \
           src/project_support/DocumentProviderSelectorController.cpp \
           src/project_support/DocumentReadingModeSelectorController.cpp \
           src/project_support/MultipleDocumentsReadingModeSelectorController.cpp \
           src/project_support/ExportProjectDialogController.cpp \
           src/project_support/ProjectImpl.cpp \
           src/project_support/ProjectLoaderImpl.cpp \
           src/project_support/ProjectServiceImpl.cpp \
           src/project_support/ProjectTasksGui.cpp \
           src/project_view/BuiltInObjectViews.cpp \
           src/project_view/ProjectViewImpl.cpp \
           src/shtirlitz/Shtirlitz.cpp \
           src/shtirlitz/StatisticalReportController.cpp \
           src/task_view/TaskStatusBar.cpp \
           src/task_view/TaskViewController.cpp \
           src/update/UgeneUpdater.cpp \
           src/welcome_page/WelcomePageMdi.cpp \
           src/welcome_page/WelcomePageMdiController.cpp \
           src/welcome_page/WelcomePageWidget.cpp

macx {
OBJECTIVE_HEADERS += src/app_settings/ResetSettingsMac.h
OBJECTIVE_SOURCES += src/app_settings/ResetSettingsMac.mm
}

RESOURCES += ugeneui.qrc
TRANSLATIONS += transl/russian.ts
