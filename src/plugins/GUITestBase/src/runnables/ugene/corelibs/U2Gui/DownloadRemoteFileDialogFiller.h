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

#ifndef _U2_GT_RUNNABLES_DOWNLOAD_REMOTE_FILE_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_DOWNLOAD_REMOTE_FILE_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class DownloadRemoteFileDialogFiller : public Filler {
public:
    enum ActionType {                           // An appropriate action data:
        SetResourceIds,                         // QStringList with IDs
        SetDatabase,                            // QString with exact database name
        EnterSaveToDirectoryPath,               // QString with path
        SelectSaveToDirectoryPath,              // QString with path
        SetOutputFormat,                        // QString with format
        SetForceSequenceDownload,               // bool value
        CheckDatabase,                          // QString with expected database name
        CheckDatabasesCount,                    // int with expected databases count
        CheckOutputFormatVisibility,            // bool expected state (is visible)
        CheckOutputFormat,                      // QString with expected format name
        CheckForceSequenceDownloadVisibility,   // bool expected state (is visible)
        CheckForceSequenceDownload,             // bool expected state (is checked)
        ClickOk,                                // ignored
        ClickCancel                             // ignored
    };

    typedef QPair<ActionType, QVariant> Action;

    DownloadRemoteFileDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions);
    DownloadRemoteFileDialogFiller(HI::GUITestOpStatus &os, CustomScenario* c);

    void commonScenario();

private:
    void setResourceIds(const QVariant &actionData);
    void setDatabase(const QVariant &actionData);
    void enterSaveToDirectoryPath(const QVariant &actionData);
    void selectSaveToDirectoryPath(const QVariant &actionData);
    void setOutputFormat(const QVariant &actionData);
    void setForceSequenceDownload(const QVariant &actionData);
    void checkDatabase(const QVariant &actionData);
    void checkDatabasesCount(const QVariant &actionData);
    void checkOutputFormatVisibility(const QVariant &actionData);
    void checkOutputFormat(const QVariant &actionData);
    void checkForceSequenceDownloadVisibility(const QVariant &actionData);
    void checkForceSequenceDownload(const QVariant &actionData);
    void clickOk();
    void clickCancel();

    const QList<Action> actions;
    QWidget *dialog;
};

// Use DownloadRemoteFileDialogFiller instead
class RemoteDBDialogFillerDeprecated : public Filler {
public:
    RemoteDBDialogFillerDeprecated(HI::GUITestOpStatus &_os, const QString _resID, int _DBItemNum, bool addToProject = true, bool forceGetSequence = true, bool _pressCancel = false, const QString _saveDirPath = QString(),
                         GTGlobals::UseMethod _useMethod = GTGlobals::UseMouse, int _outFormatVal = -1)
        :Filler(_os, "DownloadRemoteFileDialog"),
          resID(_resID),
          DBItemNum(_DBItemNum),
          addToProject(addToProject),
          forceGetSequence(forceGetSequence),
          pressCancel(_pressCancel),
          saveDirPath(_saveDirPath),
          useMethod(_useMethod),
          outFormatVal(_outFormatVal){}
    virtual void commonScenario();
private:
    QString resID;
    int DBItemNum;
    bool addToProject;
    bool forceGetSequence;
    bool pressCancel;
    QString saveDirPath;
    GTGlobals::UseMethod useMethod;
    int outFormatVal;
};

}   // namespace U2

Q_DECLARE_METATYPE(QList<U2::DownloadRemoteFileDialogFiller::Action>)

#endif // _U2_GT_RUNNABLES_DOWNLOAD_REMOTE_FILE_DIALOG_FILLER_H_
