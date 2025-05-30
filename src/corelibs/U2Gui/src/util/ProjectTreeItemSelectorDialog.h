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

class QWidget;

namespace U2 {

class Document;
class GObject;
class ProjectTreeControllerModeSettings;
class StateLockableTreeItem;

class U2GUI_EXPORT ProjectTreeItemSelectorDialog {
public:
    static QList<Document*> selectDocuments(const ProjectTreeControllerModeSettings& s, QWidget* p);
    static QList<GObject*> selectObjects(const ProjectTreeControllerModeSettings& s, QWidget* p);
    static void selectObjectsAndDocuments(const ProjectTreeControllerModeSettings& s, QWidget* p, QList<Document*>& docList, QList<GObject*>& objList);
    static void selectObjectsAndFolders(const ProjectTreeControllerModeSettings& s, QWidget* p, QList<Folder>& folderList, QList<GObject*>& objList);
    static Folder selectFolder(QWidget* parent);
};

}  // namespace U2
