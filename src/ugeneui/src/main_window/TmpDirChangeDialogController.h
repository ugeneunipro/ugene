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

#ifndef _U2_TMP_DIR_CHANGE_DIALOG_CONTROLLER_
#define _U2_TMP_DIR_CHANGE_DIALOG_CONTROLLER_

#include <QDialog>
#include <QLabel>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/UserApplicationsSettings.h>

#include <ui_TmpDirChangeDialog.h>

namespace U2 {

class TmpDirChangeDialogController : public QDialog, public Ui_TmpDirChangeDialog {
    Q_OBJECT
public:
    TmpDirChangeDialogController(const QString& path, QWidget* p);

    const QString& getTmpDirPath() const;

public slots:
    void sl_changeDirButtonClicked();
    void sl_exitAppButtonClicked();
    void sl_okButtonClicked();

private:
    QString tmpDirPath;
    TmpDirChecker* tmpDirChecker;
};

}  // namespace U2

#endif  // _U2_TMP_DIR_CHANGE_DIALOG_CONTROLLER_
