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

#ifndef _U2_AUTO_ANNOTATION_UTILS_H_
#define _U2_AUTO_ANNOTATION_UTILS_H_

#include <U2Core/GObjectReference.h>
#include "ADVSingleSequenceWidget.h"

namespace U2 {

class AnnotationGroup;
class AutoAnnotationObject;
class AutoAnnotationsUpdater;
class MWMDIWindow;

class AutoAnnotationsADVAction : public ADVSequenceWidgetAction {
    Q_OBJECT
public:
    AutoAnnotationsADVAction(ADVSequenceWidget* widget, AutoAnnotationObject* aaObj);
    ~AutoAnnotationsADVAction();
    QList<QAction*> getToggleActions();
    QAction* getSelectAllAction() { return selectAllAction; }
    QAction* getDeselectAllAction() { return deselectAllAction; }
    AutoAnnotationObject* getAAObj() {return aaObj; }
    QAction* findToggleAction(const QString& groupName);
    void addUpdaterToMenu(AutoAnnotationsUpdater* updater);
    static const QString ACTION_NAME;

private slots:
    void sl_toggle(bool toggled);
    void sl_autoAnnotationUpdateStarted();
    void sl_autoAnnotationUpdateFinished();
    void sl_onSelectAll();
    void sl_onDeselectAll();

private:
    void updateMenu();
    AutoAnnotationObject* aaObj;
    QMenu* menu;
    QAction *selectAllAction,*deselectAllAction;
    int updatesCount;
};

class ADVCreateAnnotationsTask;

class ExportAutoAnnotationsGroupTask : public Task {
    Q_OBJECT
public:
    ExportAutoAnnotationsGroupTask(AnnotationGroup *ag, GObjectReference &aRef, ADVSequenceObjectContext *seqCtx, const QString &annDescription = "");
    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);

private:
    AnnotationGroup *aGroup;
    GObjectReference aRef;
    ADVSequenceObjectContext* seqCtx;
    ADVCreateAnnotationsTask* createTask;
    const QString annDescription;
};

class U2VIEW_EXPORT AutoAnnotationUtils {
public:
    static AutoAnnotationsADVAction* findAutoAnnotationADVAction(ADVSequenceObjectContext* ctx);
    static QAction* findAutoAnnotationsToggleAction(ADVSequenceObjectContext* ctx, const QString& name);
    static QList<QAction*> getAutoAnnotationToggleActions(ADVSequenceObjectContext* ctx);
    static void triggerAutoAnnotationsUpdate(ADVSequenceObjectContext* ctx, const QString& aaGroupName);
};

}

#endif // _U2_AUTO_ANNOTATION_UTILS_H_
