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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>

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

void GTUtilsProject::openFiles(HI::GUITestOpStatus &os, const QList<QUrl> &urls, const OpenFileSettings& s) {

    switch (s.openMethod) {
    case OpenFileSettings::DragDrop:
        openFilesDrop(os, urls);
        break;
    case OpenFileSettings::Dialog:
        openFilesWithDialog(os, urls);
        break;
    }

    checkProject(os);
}

void GTUtilsProject::openFiles(HI::GUITestOpStatus &os, const GUrl &path, const OpenFileSettings& s) {
    openFiles(os, QList<QUrl>() << path.getURLString(), s);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

#define GT_METHOD_NAME "checkProject"
void GTUtilsProject::checkProject(HI::GUITestOpStatus &os, CheckType checkType) {

    GTGlobals::sleep(500);

    if (checkType == NotExists) {
        GT_CHECK(AppContext::getProject() == NULL, "There is a project");
        return;
    }

    GT_CHECK(AppContext::getProject() != NULL, "There is no project");
    if (checkType == Empty) {
        GT_CHECK(AppContext::getProject()->getDocuments().isEmpty() == true, "Project is not empty");
    }
}
#undef GT_METHOD_NAME

void GTUtilsProject::openFilesDrop(HI::GUITestOpStatus &os, const QList<QUrl>& urls) {

    QWidget* widget = AppContext::getMainWindow()->getQMainWindow();
    QPoint widgetPos(widget->width()/2, widget->height()/2);

    QMimeData *mimeData = new QMimeData();
    mimeData->setUrls(urls);

    Qt::DropActions dropActions = Qt::DropActions(Qt::CopyAction | Qt::MoveAction | Qt::LinkAction);
    Qt::MouseButtons mouseButtons = Qt::LeftButton;

    if (urls.size() > 1) {
        GTUtilsDialog::waitForDialog(os, new GTSequenceReadingModeDialogUtils(os));
    }

    QDragEnterEvent* dragEnterEvent = new QDragEnterEvent(widgetPos, dropActions, mimeData, mouseButtons, 0);
    GTGlobals::sendEvent(widget, dragEnterEvent);

    QDropEvent* dropEvent = new QDropEvent(widgetPos, dropActions, mimeData, mouseButtons, 0);
    GTGlobals::sendEvent(widget, dropEvent);
}

void GTUtilsProject::openFilesWithDialog(HI::GUITestOpStatus &os, const QList<QUrl> &urls) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new GTSequenceReadingModeDialogUtils(os));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, QUrl::toStringList(urls)));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Open...");
    GTGlobals::sleep(2000);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

#define GT_METHOD_NAME "openFileExpectSequence"
ADVSingleSequenceWidget *GTUtilsProject::openFileExpectSequence(HI::GUITestOpStatus &os,
                                                                const QString &dirPath,
                                                                const QString &fileName,
                                                                const QString &seqName) {
    return openFileExpectSequence(os, dirPath + "/" + fileName, seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectSequence"
ADVSingleSequenceWidget *GTUtilsProject::openFileExpectSequence(GUITestOpStatus &os,
                                                                const QString &filePath,
                                                                const QString &seqName) {
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GT_CHECK_OP_RESULT(os, "Error opening file!", NULL);

    GTGlobals::sleep(200);

    int seqWidgetNum = GTUtilsSequenceView::getSeqWidgetsNumber(os);
    GT_CHECK_OP_RESULT(os, "Error getting the number of sequence widgets!", NULL);
    GT_CHECK_RESULT(1 == seqWidgetNum, QString("Number of sequences is %1").arg(seqWidgetNum), NULL);


    ADVSingleSequenceWidget *seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    GT_CHECK_OP_RESULT(os, "Error grtting sequence widget!", NULL);

    QString actualName = GTUtilsSequenceView::getSeqName(os, seqWidget);
    GT_CHECK_OP_RESULT(os, "Error getting sequence widget name!", NULL);
    GT_CHECK_RESULT(actualName == seqName, QString("Expected sequence name: %1, actual: %2!").
                    arg(seqName).arg(actualName), NULL);

    return seqWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectRawSequence"
ADVSingleSequenceWidget *GTUtilsProject::openFileExpectRawSequence(HI::GUITestOpStatus &os,
                                                                 const QString &dirPath,
                                                                 const QString &fileName,
                                                                 const QString &seqName) {
    return openFileExpectRawSequence(os, dirPath + "/" + fileName, seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectRawSequence"
ADVSingleSequenceWidget *GTUtilsProject::openFileExpectRawSequence(GUITestOpStatus &os,
                                                                   const QString &filePath,
                                                                   const QString &seqName) {
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    return openFileExpectSequence(os, filePath, seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFileExpectSequences"
QList<ADVSingleSequenceWidget*> GTUtilsProject::openFileExpectSequences(HI::GUITestOpStatus &os,
                                                                        const QString &path,
                                                                        const QString &fileName,
                                                                        const QList<QString> &seqNames)
{
    QList<ADVSingleSequenceWidget*> result;
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFile(os, path, fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GT_CHECK_OP_RESULT(os, "Error opening file!", QList<ADVSingleSequenceWidget*>());

    GTGlobals::sleep(200);

    int seqWidgetNum = GTUtilsSequenceView::getSeqWidgetsNumber(os);
    GT_CHECK_OP_RESULT(os, "Error getting the number of sequence widgets!", QList<ADVSingleSequenceWidget*>());
    GT_CHECK_RESULT(seqWidgetNum == seqNames.size(), QString("Expected number of sequences: %1, actual: %2!").
                    arg(seqNames.size()).arg(seqWidgetNum),
                    QList<ADVSingleSequenceWidget*>());

    for (int i = 0; i < seqNames.size(); ++i) {
        ADVSingleSequenceWidget *seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os, i);
        GT_CHECK_OP_RESULT(os, QString("Error grtting sequence widget: %1!").arg(i), QList<ADVSingleSequenceWidget*>());

        QString actualName = GTUtilsSequenceView::getSeqName(os, seqWidget);
        GT_CHECK_RESULT(seqNames.at(i) == actualName,
                        QString("Unexpected sequence widget at position %1. Expected sequence name: %2, actual: %3!").
                        arg(i).arg(seqNames.at(i)).arg(actualName),
                        QList<ADVSingleSequenceWidget*>());
        result << seqWidget;
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsSequences"
void GTUtilsProject::openMultiSequenceFileAsSequences(HI::GUITestOpStatus &os, const QString &path, const QString &fileName) {
    openMultiSequenceFileAsSequences(os, path + "/" + fileName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsSequences"
void GTUtilsProject::openMultiSequenceFileAsSequences(HI::GUITestOpStatus &os, const QString &filePath) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsMergedSequence"
void GTUtilsProject::openMultiSequenceFileAsMergedSequence(HI::GUITestOpStatus &os, const QString &filePath) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge));
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsMalignment"
void GTUtilsProject::openMultiSequenceFileAsMalignment(HI::GUITestOpStatus &os, const QString &dirPath, const QString &fileName) {
    openMultiSequenceFileAsMalignment(os, dirPath + "/" + fileName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openMultiSequenceFileAsMalignment"
void GTUtilsProject::openMultiSequenceFileAsMalignment(HI::GUITestOpStatus &os, const QString &filePath) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "saveProjectAs"
void GTUtilsProject::saveProjectAs(HI::GUITestOpStatus &os, const QString &path) {
    const QFileInfo info(path);
    GTUtilsDialog::waitForDialog(os, new SaveProjectAsDialogFiller(os, "New Project", info.dir().path(), info.fileName()));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Save project as...");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeProject"
void GTUtilsProject::closeProject(HI::GUITestOpStatus &os){
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new AppCloseMessageBoxDialogFiller(os));
    GTGlobals::sleep(500);
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Close project");
}

GTUtilsProject::OpenFileSettings::OpenFileSettings() :
    openMethod(Dialog)
{

}

#undef GT_METHOD_NAME


#undef GT_CLASS_NAME

} // U2
