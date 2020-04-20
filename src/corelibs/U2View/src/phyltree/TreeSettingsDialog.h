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

#ifndef _TREE_SETTIHS_DIALOG_
#define _TREE_SETTIHS_DIALOG_

#include "BaseSettingsDialog.h"
#include <ui_TreeSettingsDialog.h>
#include "ov_phyltree/TreeSettings.h"

namespace U2 {

class TreeSettingsDialog : public BaseSettingsDialog, public Ui_TreeSettingsDialog{
    Q_OBJECT
public:
    TreeSettingsDialog(QWidget *parent, const OptionsMap &settings, bool isRectLayout);

    virtual void accept();

    static QString treeDefaultText();
    static QString treePhylogramText();
    static QString treeCladogramText();
private slots:
    void sl_treeTypeChanged(int);
};

} //namespace
#endif
