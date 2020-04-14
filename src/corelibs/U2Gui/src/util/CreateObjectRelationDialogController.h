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

#ifndef _U2_CREATE_OBJECT_RELATION_DIALOG_CONTROLLER_H_
#define _U2_CREATE_OBJECT_RELATION_DIALOG_CONTROLLER_H_

#include <U2Core/global.h>
#include <U2Core/GObjectRelationRoles.h>

#include <QDialog>

class Ui_CreateObjectRelationDialog;

namespace U2 {

class GObject;

class U2GUI_EXPORT CreateObjectRelationDialogController : public QDialog {
    Q_OBJECT
public:
    CreateObjectRelationDialogController(GObject* obj, const QList<GObject*>& objects,
        GObjectRelationRole role, bool removeDuplicates, const QString& relationHint, QWidget* p = NULL);
    ~CreateObjectRelationDialogController();

    GObject*            selectedObject;
    GObject*            assObj;
    QList<GObject*>     objects;
    GObjectRelationRole role;
    bool                removeDuplicates;
    bool relationIsSet;

public slots:
    virtual void accept();

private:
    Ui_CreateObjectRelationDialog* ui;
};

} //namespace

#endif
