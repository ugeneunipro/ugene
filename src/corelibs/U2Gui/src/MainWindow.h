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

#ifndef _U2_MAIN_WINDOW_H_
#define _U2_MAIN_WINDOW_H_

#include <U2Core/global.h>
#include <U2Core/PluginModel.h>
#include <U2Core/ServiceTypes.h>

#include "NotificationsTypes.h"

#include <QEvent>
#include <QWidget>

class QAction;
class QMdiSubWindow;
class QDockWidget;
class QMenu;
class QToolBar;
class QMainWindow;

// menu
#define MWMENU                              "mw_menu_bar"
#define MWMENU_FILE                         "mwmenu_file"
#define MWMENU_ACTIONS                      "mwmenu_actions"
#define MWMENU_SETTINGS                     "mwmenu_settings"
#define MWMENU_TOOLS                        "mwmenu_tools"
#define MWMENU_WINDOW                       "mwmenu_window"
#define MWMENU_HELP                         "mwmenu_help"



// toolbar
#define MWTOOLBAR_MAIN                      "mwtoolbar_main"
#define MWTOOLBAR_ACTIVEMDI                 "mwtoolbar_activemdi"



//actions
#define ACTION__EXIT            "action__exit"
#define ACTION__ABOUT           "action__about"
#define ACTION__LOGVIEW         "action__logview"
#define ACTION__PLUGINS_VIEW    "action__plugins"

#define ACTION_PROJECTSUPPORT__NEW_PROJECT              "action_projectsupport__new_project"
#define ACTION_PROJECTSUPPORT__OPEN_PROJECT             "action_projectsupport__open_project"
#define ACTION_PROJECTSUPPORT__PASTE                    "action_projectsupport__paste"
#define ACTION_PROJECTSUPPORT__OPEN_AS                  "action_projectsupport__open_as"
#define ACTION_PROJECTSUPPORT__SAVE_PROJECT             "action_projectsupport__save_project"
#define ACTION_PROJECTSUPPORT__SAVE_AS_PROJECT          "action_projectsupport__save_as_project"
#define ACTION_PROJECTSUPPORT__CLOSE_PROJECT            "action_projectsupport__close_project"
#define ACTION_PROJECTSUPPORT__RECENT_PROJECTS_MENU     "action_projectsupport__recent_projects_menu"
#define ACTION_PROJECTSUPPORT__EXPORT_PROJECT           "action_projectsupport__export_project"
#define ACTION_PROJECTSUPPORT__ACCESS_REMOTE_DB         "action_projectsupport__access_remote_db"
#define ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB         "action_projectsupport__access_shared_db"
#define ACTION_PROJECTSUPPORT__SEARCH_GENBANK           "action_projectsupport__search_genbank"
#define ACTION_PROJECTSUPPORT__NEW_SECTION_SEPARATOR    "action_projectsupport__new_section_separator"

#define ACTION_PROJECT__ADD_MENU                    "action_project__add_menu"
#define ACTION_PROJECT__ADD_OBJECT                  "action_project__add_object"
#define ACTION_PROJECT__REMOVE_MENU                 "action_project__remove_menu"
#define ACTION_PROJECT__REMOVE_SELECTED             "action_project__remove_selected_action"
#define ACTION_PROJECT__ADD_NEW_DOCUMENT            "action_project__add_new_document"
#define ACTION_PROJECT__ADD_EXISTING_DOCUMENT       "action_project__add_existing_document"
#define ACTION_PROJECT__UNLOAD_SELECTED             "action_project__unload_selected_action"
#define ACTION_PROJECT__IMPORT_TO_DATABASE          "action_project__import_to_database"
#define ACTION_PROJECT__CREATE_FOLDER               "action_project__create_folder"
#define ACTION_PROJECT__SAVE_DOCUMENT               "action_prpject__save_document"

#define PROJECT_MENU_SEPARATOR_1                    "project_menu_separator_1"
#define ACTION_DOCUMENT__LOCK                       "action_document_lock"
#define ACTION_DOCUMENT__UNLOCK                     "action_document_unlock"

#define ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION   "action_project__export_import_menu_action"
#define ACTION_PROJECT__EXPORT_AS_SEQUENCES_ACTION  "action_project__export_as_sequence_action"
#define ACTION_PROJECT__EXPORT_TO_AMINO_ACTION      "action_project__export_to_amino_action"

#define ACTION_BLAST_SUBMENU                        "BLAST"
#define ACTION_BLAST_FORMAT_DB                      "FormatDB"

#define ACTION_EXPORT_SEQUENCE                      "export sequences"
#define ACTION_EXPORT_CORRESPONDING_SEQ             "export corresponding sequence"
#define ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT         "export sequences as alignment"
#define ACTION_EXPORT_ANNOTATIONS                   "action_export_annotations"
#define ACTION_EXPORT_CHROMATOGRAM                  "action_export_chromatogram"

#define ACTION_EXPORT_OBJECT                        "export object"

#define ACTION_EDIT_SEQUENCE                        "edit sequences"
#define ACTION_EDIT_SEQUENCE_SETTINGS               "action_edit_sequences_settings"
#define ACTION_EDIT_INSERT_SUBSEQUENCE              "action_edit_insert_sub_sequences"
#define ACTION_EDIT_REMOVE_SUBSEQUENCE              "action_edit_remove_sub_sequences"
#define ACTION_EDIT_REPLACE_SUBSEQUENCE             "action_edit_replace_sub_sequences"
#define ACTION_EDIT_RESERVE_COMPLEMENT_SEQUENCE     "action_edit_reserve_complement_sequences"
#define ACTION_EDIT_RESERVE_SEQUENCE                "action_edit_reserve_sequences"
#define ACTION_EDIT_COMPLEMENT_SEQUENCE             "action_edit_complement_sequences"
#define ACTION_EDIT_SELECT_SEQUENCE_FROM_VIEW       "action_edit_select_sequences_from_view"

#define ACTION_BOOKMARK_TREE_VIEW                   "action_bookmark_tree_view"
#define ACTION_ACTIVATE_VIEW                        "action_activate_view"
#define ACTION_ADD_BOOKMARK                         "action_add_bookmark"
#define ACTION_RENAME_BOOKMARK                      "action_rename_bookmark"
#define ACTION_REMOVE_BOOKMARK                      "action_remove_bookmark"

#define DOCK_PROJECT_VIEW    "dock_project_view"
#define DOCK_TASK_VIEW       "dock_task_view"
#define DOCK_LOG_VIEW        "dock_log_view"
#define DOCK_TASK_TREE_VIEW  "taskViewTree"

namespace U2 {

class MWMDIManager;
class MWDockManager;
class NotificationStack;

class U2GUI_EXPORT MainWindow : public QObject {
    Q_OBJECT
public:
    virtual QMenu*       getTopLevelMenu(const QString& sysName) const = 0;
    virtual QToolBar*    getToolbar(const QString& sysName) const  = 0;
    virtual QMainWindow* getQMainWindow() const = 0;

    virtual MWMDIManager*           getMDIManager() const = 0;
    virtual MWDockManager*          getDockManager() const = 0;
    virtual NotificationStack*      getNotificationStack() const = 0;


    virtual void setWindowTitle(const QString& title) = 0;

    virtual void addNotification(const QString& message, NotificationType type) = 0;

    virtual void registerAction(QAction *action) = 0;
};

//////////////////////////////////////////////////////////////////////////
/// MDI

class U2GUI_EXPORT MWMDIWindow : public QWidget {
    Q_OBJECT

    friend class MWMDIManager;
public:
    MWMDIWindow(const QString& _windowName);
    ~MWMDIWindow();

    virtual void setupMDIToolbar(QToolBar*) {}
    virtual void setupViewMenu(QMenu*) {}

    int getId() const {return windowId;}

protected:
    /* returns true if a window may be closed, false otherwise */
    virtual bool onCloseEvent() {return true;}

protected:
    int     windowId;
};

class U2GUI_EXPORT MWMDIManager : public QObject {
    Q_OBJECT
public:
    MWMDIManager(QObject* p) : QObject(p){}

    /** Adds the specified window to MainWindow layout and activates it. */
    virtual void addMDIWindow(MWMDIWindow* w) = 0;

    virtual bool closeMDIWindow(MWMDIWindow* w) = 0;

    virtual QList<MWMDIWindow*> getWindows() const = 0;

    virtual MWMDIWindow* getWindowById(int id) const = 0;

    virtual void activateWindow(MWMDIWindow* w) = 0;

    virtual MWMDIWindow* getActiveWindow() const = 0;

signals:
    void si_windowAdded(MWMDIWindow* w);
    void si_windowClosing(MWMDIWindow* w);
    void si_windowActivated(MWMDIWindow* w);
    void si_windowDeactivated(MWMDIWindow* w);

protected:
    /* returns true if the window agreed to close, false otherwise */
    bool onCloseEvent(MWMDIWindow* w) {return w->onCloseEvent();}
};

//////////////////////////////////////////////////////////////////////////
/// DOCK

enum MWDockArea {
    MWDockArea_Left,
    MWDockArea_Right,
    MWDockArea_Bottom,
    MWDockArea_MaxDocks
};

class U2GUI_EXPORT MWDockManager: public QObject {
    Q_OBJECT
public:
    MWDockManager(QObject* p) : QObject(p){}

    virtual QAction* registerDock(MWDockArea area, QWidget* w, const QKeySequence& ks = QKeySequence()) = 0;

    virtual QWidget* findWidget(const QString& widgetObjName) = 0;

    virtual QWidget* getActiveWidget(MWDockArea a) = 0;

    virtual QWidget* activateDock(const QString& widgetObjName) = 0;

    virtual QWidget* toggleDock(const QString& widgetObjName) = 0;

    virtual void dontActivateNextTime(MWDockArea a) = 0;
};

}//namespace
Q_DECLARE_METATYPE(U2::MainWindow*)
Q_DECLARE_METATYPE(U2::MWMDIManager*)
Q_DECLARE_METATYPE(U2::MWDockManager*)
#endif
