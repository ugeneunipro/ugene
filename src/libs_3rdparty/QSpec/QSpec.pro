include (QSpec.pri)

# Check the Qt version. If QT_VERSION is not set, it is probably Qt 3.
isEmpty(QT_VERSION) {
    error("QT_VERSION not defined. QScore does not work with Qt 3.")
}

!minQtVersion(5, 0, 0) {
    message("Cannot build QScore with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.0.0.")
}

# Input
HEADERS += \
            src/GTGlobals.h \
           src/base_dialogs/ColorDialogFiller.h \
           src/base_dialogs/DefaultDialogFiller.h \
           src/base_dialogs/FontDialogFiller.h \
           src/base_dialogs/GTFileDialog.h \
           src/base_dialogs/MessageBoxFiller.h \
           src/drivers/GTKeyboardDriver.h \
           src/drivers/GTMouseDriver.h \
           src/primitives/GTAction.h \
           src/primitives/GTCheckBox.h \
           src/primitives/GTComboBox.h \
           src/primitives/GTDoubleSpinBox.h \
           src/primitives/GTGroupBox.h \
           src/primitives/GTLineEdit.h \
           src/primitives/GTListWidget.h \
           src/primitives/GTMainWindow.h \
           src/primitives/GTMenu.h \
           src/primitives/GTMenuBar.h \
           src/primitives/GTPlainTextEdit.h \
           src/primitives/GTRadioButton.h \
           src/primitives/GTScrollBar.h \
           src/primitives/GTSlider.h \
           src/primitives/GTSpinBox.h \
           src/primitives/GTSplitter.h \
           src/primitives/GTTabBar.h \
           src/primitives/GTTableView.h \
           src/primitives/GTTabWidget.h \
           src/primitives/GTTextEdit.h \
           src/primitives/GTToolbar.h \
           src/primitives/GTTreeView.h \
           src/primitives/GTTreeWidget.h \
           src/primitives/GTWebView.h \
           src/primitives/GTWidget.h \
           src/primitives/PopupChooser.h \
           src/primitives/private/GTMenuPrivate.h \
           src/system/GTClipboard.h \
           src/system/GTFile.h \
           src/utils/GTKeyboardUtils.h \
           src/utils/GTThread.h \
           src/utils/GTUtilsApp.h \
           src/utils/GTUtilsDialog.h \
           src/utils/GTUtilsToolTip.h \
           src/utils/GTMouseUtils.h \
           src/core/GUITestOpStatus.h \
           src/core/global.h \
           src/core/CustomScenario.h \
           src/core/GUITest.h \
           src/core/MainThreadRunnable.h \
           src/core/MainThreadTimer.h \
           src/core/GUITestBase.h \
           src/core/TestThread.h \
           src/core/GUITestsLauncher.h

SOURCES += \
           src/GTGlobals.cpp \
           src/base_dialogs/ColorDialogFiller.cpp \
           src/base_dialogs/DefaultDialogFiller.cpp \
           src/base_dialogs/FontDialogFiller.cpp \
           src/base_dialogs/GTFileDialog.cpp \
           src/base_dialogs/MessageBoxFiller.cpp \
           src/drivers/GTKeyboardDriver.cpp \
           src/drivers/GTKeyboardDriverLinux.cpp \
           src/drivers/GTKeyboardDriverMac.cpp \
           src/drivers/GTKeyboardDriverWindows.cpp \
           src/drivers/GTMouseDriver.cpp \
           src/drivers/GTMouseDriverLinux.cpp \
           src/drivers/GTMouseDriverMac.cpp \
           src/drivers/GTMouseDriverWindows.cpp \
           src/primitives/GTAction.cpp \
           src/primitives/GTCheckBox.cpp \
           src/primitives/GTComboBox.cpp \
           src/primitives/GTDoubleSpinBox.cpp \
           src/primitives/GTGroupBox.cpp \
           src/primitives/GTLineEdit.cpp \
           src/primitives/GTListWidget.cpp \
           src/primitives/GTMainWindow.cpp \
           src/primitives/GTMenu.cpp \
           src/primitives/GTMenuBar.cpp \
           src/primitives/GTPlainTextEdit.cpp \
           src/primitives/GTRadioButton.cpp \
           src/primitives/GTScrollBar.cpp \
           src/primitives/GTSlider.cpp \
           src/primitives/GTSpinBox.cpp \
           src/primitives/GTSplitter.cpp \
           src/primitives/GTTabBar.cpp \
           src/primitives/GTTableView.cpp \
           src/primitives/GTTabWidget.cpp \
           src/primitives/GTTextEdit.cpp \
           src/primitives/GTToolbar.cpp \
           src/primitives/GTTreeView.cpp \
           src/primitives/GTTreeWidget.cpp \
           src/primitives/GTWebView.cpp \
           src/primitives/GTWidget.cpp \
           src/primitives/PopupChooser.cpp \
           src/primitives/private/GTMenuPrivate.cpp \
           src/system/GTClipboard.cpp \
           src/system/GTFile.cpp \
           src/utils/GTKeyboardUtils.cpp \
           src/utils/GTThread.cpp \
           src/utils/GTUtilsApp.cpp \
           src/utils/GTUtilsDialog.cpp \
           src/utils/GTUtilsToolTip.cpp \
           src/utils/GTMouseUtils.cpp \
           src/core/CustomScenario.cpp \
           src/core/GUITest.cpp \
           src/core/MainThreadRunnable.cpp \
           src/core/MainThreadTimer.cpp \
           src/core/GUITestBase.cpp \
           src/core/TestThread.cpp \
           src/core/GUITestsLauncher.cpp

useWebKit() {
    HEADERS += src/primitives/GTWebkitView.h
    SOURCES += src/primitives/GTWebkitView.cpp
} else {
    HEADERS += src/primitives/GTWebEngineView.h
    SOURCES += src/primitives/GTWebEngineView.cpp
}

macx {
OBJECTIVE_HEADERS += src/primitives/private/GTMenuPrivateMac.h
OBJECTIVE_SOURCES += src/primitives/private/GTMenuPrivateMac.mm
}
