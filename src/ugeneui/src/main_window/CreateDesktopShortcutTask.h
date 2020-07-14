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

#ifndef _U2_CREATE_DESKTOP_SHORTCUT_TASKS_H_
#define _U2_CREATE_DESKTOP_SHORTCUT_TASKS_H_

#include <QMessageBox>

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/Version.h>

class QPushButton;

namespace U2 {

class CreateDesktopShortcutTask : public Task {
    Q_OBJECT

public:
    enum Answer { Create,
                  DoNothing };

    CreateDesktopShortcutTask(bool startUp = false);
    void run();
    bool createDesktopShortcut();
    ReportResult report();
    static Answer getAnswer();

private:
    bool runOnStartup;
    bool startError;
};

}    // namespace U2

#endif
