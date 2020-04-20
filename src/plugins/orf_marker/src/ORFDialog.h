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

#ifndef _U2_ORFDIALOG_H_
#define _U2_ORFDIALOG_H_

#include "ui_ORFDialogUI.h"

#include <U2Core/global.h>
#include <U2Core/U2Region.h>
#include <U2Gui/RegionSelector.h>

#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QTimer>

#include <U2Algorithm/ORFAlgorithmTask.h>

namespace U2 {

class ADVSequenceObjectContext;
class AutoAnnotationsUpdateTask;
class U2SequenceObject;
class CreateAnnotationWidgetController;
class ORFFindTask;
class Task;

class ORFDialog : public QDialog, public Ui_ORFDialogBase {
    Q_OBJECT

public:
    ORFDialog(ADVSequenceObjectContext* ctx);

public slots:
    virtual void reject();
    virtual void accept();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:

    //buttons:
    void sl_onClearList();
    void sl_onFindAll();

    void sl_onTaskFinished(Task*);
    void sl_onTimer();

    void sl_onResultActivated(QTreeWidgetItem* i, int col);
    void sl_translationChanged();


private:
    void connectGUI();
    void updateState();
    void updateStatus();
    void tunePercentBox();
    void initSettings();
    void getSettings(ORFAlgorithmSettings& s);

    void runTask();

    void importResults();

    U2Region getCompleteSearchRegion(bool *ok = NULL) const;
    ORFAlgorithmStrand getAlgStrand() const;

private:
    void createAnnotationWidget();
    void findStartedAAUpdateTask();
    ADVSequenceObjectContext* ctx;
    CreateAnnotationWidgetController* ac;

    U2Region panViewSelection;
    ORFFindTask* task;
    AutoAnnotationsUpdateTask *aaUpdateTask;
    QTimer* timer;
    RegionSelector* rs;
    bool isRegionOk;
};

}//namespace

#endif
