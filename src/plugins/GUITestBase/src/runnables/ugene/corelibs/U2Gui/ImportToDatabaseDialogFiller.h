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

#ifndef _U2_IMPORT_TO_DATABASE_DIALOG_FILLER_H_
#define _U2_IMPORT_TO_DATABASE_DIALOG_FILLER_H_

#include <QPointer>

#include "utils/GTUtilsDialog.h"

class QTreeWidgetItem;

namespace U2 {
using namespace HI;

class ImportToDatabaseDialogFiller : public Filler {
public:
    class Action {
    public:
        enum Type {
            ADD_FILES,                  // add files with the "Add files" button
            ADD_DIRS,                   // add dirs with the "Add folder" button
            ADD_PROJECT_ITEMS,          // add project items with the "Add objects" button
            SELECT_ITEMS,               // select items in the tree view
            EDIT_DESTINATION_FOLDER,    // edit the item's destination folder by double click to the folder column
            EDIT_GENERAL_OPTIONS,       // edit general options with the "General options" button
            EDIT_PRIVATE_OPTIONS,       // edit the item's private options with a "Override options" context menu item
            RESET_PRIVATE_OPTIONS,      // reset the item's options to global with a "Reset to general options" context menu item
            REMOVE,                     // remove the item with the "Remove" button
            IMPORT,                     // close the dialog and start the import task with the "Import" button
            CANCEL                      // close the dialog the "Cancel" button
        };

        Action(Type type, const QVariantMap& data);
        Type type;
        QVariantMap data;

        static const QString ACTION_DATA__ITEM;
        static const QString ACTION_DATA__ITEMS_LIST;
        static const QString ACTION_DATA__DESTINATION_FOLDER;
        static const QString ACTION_DATA__PATHS_LIST;
        static const QString ACTION_DATA__PROJECT_ITEMS_LIST;
    };

    ImportToDatabaseDialogFiller(HI::GUITestOpStatus& os, const QList<Action>& actions);
    ImportToDatabaseDialogFiller(HI::GUITestOpStatus &os, CustomScenario* c): Filler(os, "ImportToDatabaseDialog", c){}

    void commonScenario();


private:
    void addFiles(const Action& action);
    void addDirs(const Action& action);
    void addProjectItems(const Action& action);
    void selectItems(const Action& action);
    void editDestinationFolder(const Action& action);
    void editGeneralOptions(const Action& action);
    void editPrivateOptions(const Action& action);
    void resetPrivateOptions(const Action& action);
    void remove(const Action& action);
    void import(const Action& action);
    void cancel(const Action& action);

    QPoint getItemCenter(const QString& text);
    QPoint getFolderColumnCenter(const QString& text);
    QTreeWidgetItem *findItem(const QString& text);

    QPointer<QWidget> dialog;
    QList<Action> actions;
};

}   // namespace U2

#endif // _U2_IMPORT_TO_DATABASE_DIALOG_FILLER_H_
