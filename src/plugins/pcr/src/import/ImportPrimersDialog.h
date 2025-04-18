/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/Folder.h>
#include <U2Core/ProjectTreeControllerModeSettings.h>

#include "ui_ImportPrimersDialog.h"

namespace U2 {

class ImportPrimersMultiTask;

class ImportPrimersDialog : public QDialog, private Ui_ImportPrimersDialog {
    Q_OBJECT
public:
    ImportPrimersDialog(QWidget* parent = nullptr);

private slots:
    void sl_updateState();
    void sl_addFileClicked();
    void sl_removeFileClicked();
    void sl_addObjectClicked();
    void sl_removeObjectClicked();
    void sl_selectionChanged();
    void sl_contentChanged();
    void accept();

private:
    void connectSignals();
    ProjectTreeControllerModeSettings prepareProjectItemsSelectionSettings() const;

    QMap<QListWidgetItem*, QString> item2file;
    QMap<QListWidgetItem*, Folder> item2folder;
    QMap<QListWidgetItem*, GObject*> item2object;
};

}  // namespace U2
