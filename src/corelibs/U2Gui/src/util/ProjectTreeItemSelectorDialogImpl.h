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

#ifndef _U2_PROJECT_TREE_ITEM_SELECTOR_DIALOG_IMPL_H_
#define _U2_PROJECT_TREE_ITEM_SELECTOR_DIALOG_IMPL_H_

#include "ui_ProjectTreeItemSelectorDialogBase.h"

namespace U2 {

class ProjectTreeController;
class ProjectTreeControllerModeSettings;
class Document;
class GObject;

class ProjectTreeItemSelectorDialogImpl: public QDialog, public Ui_ProjectTreeItemSelectorDialogBase {
    Q_OBJECT
public:
    ~ProjectTreeItemSelectorDialogImpl();

    bool acceptByDoubleClick;

private slots:
    void sl_objectClicked(GObject* obj);

private:
    friend class ProjectTreeItemSelectorDialog;
    ProjectTreeItemSelectorDialogImpl(QWidget* p, const ProjectTreeControllerModeSettings& s);
    ProjectTreeController* controller;
};

} // namespace U2

#endif
