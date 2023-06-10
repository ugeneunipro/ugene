/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>

#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QHeaderView>
#include <QMainWindow>
#include <QTreeView>
#include <QUrl>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/ObjectViewModel.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsProject"

void GTUtilsProject::openFiles(const QList<QUrl>& urls, const OpenFileSettings& s, ProjectCheckType checkType) {
    switch (s.openMethod) {
        case OpenFileSettings::DragDrop:
            openFilesDrop(urls);
            break;
        case OpenFileSettings::Dialog:
            openFilesWithDialog(urls);
            break;
    }
    checkProject(checkType);
}

void GTUtilsProject::openFile(const GUrl& path, const OpenFileSettings& s, ProjectCheckType checkType) {
    openFiles(QList<QUrl>() << path.getURLString(), s, checkType);
    GTUtilsTaskTreeView::waitTaskFinished();
}

void GTUtilsProject::openFileExpectNoProject(const GUrl& path, const OpenFileSettings& s) {
    GTUtilsProject::openFile(path, s, NotExists);
}

#define GT_METHOD_NAME "checkProject"
void GTUtilsProject::checkProject(ProjectCheckType checkType) {
    GTGlobals::sleep(500);

    Project* project = AppContext::getProject();
    if (checkType == NotExists) {
        GT_CHECK(project == nullptr, "checkProject: There is a project");
        return;
    }

    GT_CHECK(project != nullptr, "There is no project");
    if (checkType == Empty) {
        GT_CHECK(project->getDocuments().isEmpty(), "Project is not empty");
    }
}
#undef GT_METHOD_NAME

void GTUtilsProject::openFilesDrop(const QList<QUrl>& urls) {
    QWidget* widget = AppContext::getMainWindow()->getQMainWindow();
    QPoint widgetPos(widget->width() / 2, widget->height() / 2);

    QMimeData* mimeData = new QMimeData();
    mimeData->setUrls(urls);

    Qt::DropActions dropActions = Qt::DropActions(Qt::CopyAction | Qt::MoveAction | Qt::LinkAction);
    Qt::MouseButtons mouseButtons = Qt::LeftButton;

    if (urls.size() > 1) {
        GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    }

    QDragEnterEvent* dragEnterEvent = new QDragEnterEvent(widgetPos, dropActions, mimeData, mouseButtons, 0);
    GTGlobals::sendEvent(widget, dragEnterEvent);

    QDropEvent* dropEvent = new QDropEvent(widgetPos, dropActions, mimeData, mouseButtons, 0);
    GTGlobals::sendEvent(widget, dropEvent);
}

void GTUtilsProject::openFilesWithDialog(const QList<QUrl>& urls) {
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(QUrl::toStringList(urls)));
    GTMenu::clickMainMenuItem({"File", "Open..."});
    GTGlobals::sleep(2000);
    GTUtilsTaskTreeView::waitTaskFinished();
}

#define GT_METHOD_NAME "openFileExpectSequence"
ADVSingleSequenceWidget* GTUtilsProject::openFileExpectSequence(
    const QString& dirPath,
    const QString& fileName,
    const QString& seqName) {
    return openFileExpectSequence(dirPath + "/" + fileName, seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectSequence"
ADVSingleSequenceWidget* GTUtilsProject::openFileExpectSequence(
    const QString& filePath,
    const QString& seqName) {
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();
    GT_CHECK_OP_RESULT("Error opening file!", nullptr);

    GTGlobals::sleep(200);

    int seqWidgetNum = GTUtilsSequenceView::getSeqWidgetsNumber();
    GT_CHECK_OP_RESULT("Error getting the number of sequence widgets!", nullptr);
    GT_CHECK_RESULT(1 == seqWidgetNum, QString("Number of sequences is %1").arg(seqWidgetNum), nullptr);

    ADVSingleSequenceWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber();
    GT_CHECK_OP_RESULT("Error grtting sequence widget!", nullptr);

    QString actualName = GTUtilsSequenceView::getSeqName(seqWidget);
    GT_CHECK_OP_RESULT("Error getting sequence widget name!", nullptr);
    GT_CHECK_RESULT(actualName == seqName, QString("Expected sequence name: %1, actual: %2!").arg(seqName).arg(actualName), nullptr);

    return seqWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectRawSequence"
ADVSingleSequenceWidget* GTUtilsProject::openFileExpectRawSequence(
    const QString& dirPath,
    const QString& fileName,
    const QString& seqName) {
    return openFileExpectRawSequence(dirPath + "/" + fileName, seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectRawSequence"
ADVSingleSequenceWidget* GTUtilsProject::openFileExpectRawSequence(
    const QString& filePath,
    const QString& seqName) {
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    return openFileExpectSequence(filePath, seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectSequences"
QList<ADVSingleSequenceWidget*> GTUtilsProject::openFileExpectSequences(
    const QString& path,
    const QString& fileName,
    const QList<QString>& seqNames) {
    QList<ADVSingleSequenceWidget*> result;
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(path + fileName);
    GTUtilsTaskTreeView::waitTaskFinished();
    GT_CHECK_OP_RESULT("Error opening file!", QList<ADVSingleSequenceWidget*>());

    GTGlobals::sleep(200);

    int seqWidgetNum = GTUtilsSequenceView::getSeqWidgetsNumber();
    GT_CHECK_OP_RESULT("Error getting the number of sequence widgets!", QList<ADVSingleSequenceWidget*>());
    GT_CHECK_RESULT(seqWidgetNum == seqNames.size(), QString("Expected number of sequences: %1, actual: %2!").arg(seqNames.size()).arg(seqWidgetNum), QList<ADVSingleSequenceWidget*>());

    for (int i = 0; i < seqNames.size(); ++i) {
        ADVSingleSequenceWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(i);
        GT_CHECK_OP_RESULT(QString("Error grtting sequence widget: %1!").arg(i), QList<ADVSingleSequenceWidget*>());

        QString actualName = GTUtilsSequenceView::getSeqName(seqWidget);
        GT_CHECK_RESULT(seqNames.at(i) == actualName,
                        QString("Unexpected sequence widget at position %1. Expected sequence name: %2, actual: %3!").arg(i).arg(seqNames.at(i)).arg(actualName),
                        QList<ADVSingleSequenceWidget*>());
        result << seqWidget;
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsSequences"
void GTUtilsProject::openMultiSequenceFileAsSequences(const QString& path, const QString& fileName) {
    openMultiSequenceFileAsSequences(path + "/" + fileName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsSequences"
void GTUtilsProject::openMultiSequenceFileAsSequences(const QString& filePath) {
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsMergedSequence"
void GTUtilsProject::openMultiSequenceFileAsMergedSequence(const QString& filePath) {
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge));
    GTUtilsProject::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsMalignment"
void GTUtilsProject::openMultiSequenceFileAsMalignment(const QString& dirPath, const QString& fileName) {
    openMultiSequenceFileAsMalignment(dirPath + "/" + fileName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsMalignment"
void GTUtilsProject::openMultiSequenceFileAsMalignment(const QString& filePath) {
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "saveProjectAs"
void GTUtilsProject::saveProjectAs(const QString& path) {
    const QFileInfo info(path);
    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("New Project", info.absoluteFilePath()));
    GTMenu::clickMainMenuItem({"File", "Save project as..."});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeProject"
void GTUtilsProject::closeProject(bool isExpectSaveProjectDialog, bool isExpectAppMessageBox) {
    if (isExpectAppMessageBox) {
        GTUtilsDialog::waitForDialog(new AppCloseMessageBoxDialogFiller());
    }
    if (isExpectSaveProjectDialog) {
        GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    }
    GTMenu::clickMainMenuItem({"File", "Close project"});
}
#undef GT_METHOD_NAME

GTUtilsProject::OpenFileSettings::OpenFileSettings()
    : openMethod(Dialog) {
}

#undef GT_CLASS_NAME

}  // namespace U2
