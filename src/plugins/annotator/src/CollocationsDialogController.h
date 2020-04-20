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

#ifndef _U2_COLLOCATION_DIALOG_CONTROLLER_H_
#define _U2_COLLOCATION_DIALOG_CONTROLLER_H_

#include "CollocationsSearchAlgorithm.h"

#include <ui_FindAnnotationCollocationsDialog.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>
#include <U2Core/AnnotationData.h>

#include <QTimer>
#include <QMutex>
#include <QDialog>
#include <QToolButton>

namespace U2 {

class AnnotationTableObject;
class ADVSequenceObjectContext;
class CollocationSearchTask;

//TODO: listen for allocation add/remove to the view
class CollocationsDialogController : public QDialog, Ui_FindAnnotationCollocationsDialog {
    Q_OBJECT
public:
    CollocationsDialogController(QStringList names, ADVSequenceObjectContext* ctx);

public slots:
    void reject();

private slots:
    void sl_searchClicked();
    void sl_cancelClicked();
    void sl_plusClicked();
    void sl_minusClicked();
    void sl_addName();
    void sl_onTaskFinished(Task*);
    void sl_onTimer();
    void sl_onResultActivated(QListWidgetItem * item );
    void sl_clearClicked();
    void sl_saveClicked();

private:
    void updateState();
    void updateStatus();
    void importResults();

    QStringList                 allNames;
    QSet<QString>               usedNames;
    ADVSequenceObjectContext*   ctx;
    QToolButton*                plusButton;
    CollocationSearchTask*      task;
    QTimer*                     timer;
    QPushButton*                searchButton;
    QPushButton*                cancelButton;

};

class CDCResultItem : public QListWidgetItem {
public:
    CDCResultItem(const U2Region& _r);
    U2Region r;
};

//////////////////////////////////////////////////////////////////////////
// task

class CollocationSearchTask : public Task , public CollocationsAlgorithmListener{
    Q_OBJECT
public:
    CollocationSearchTask(const QList<AnnotationTableObject *> &table, const QSet<QString>& names, const CollocationsAlgorithmSettings& cfg);
    CollocationSearchTask(const QList<SharedAnnotationData> &table, const QSet<QString>& names, const CollocationsAlgorithmSettings& cfg,
        bool keepSourceAnns = false);
    void run();

    QVector<U2Region> popResults();
    QList<SharedAnnotationData> popResultAnnotations();

    virtual void onResult(const U2Region& r);

private:
    CollocationsAlgorithmItem& getItem(const QString& name);
    bool isSuitableRegion(const U2Region &r, const QVector<U2Region> &resultRegions) const;
    U2Region cutResult(const U2Region &res) const;

    QMap<QString, CollocationsAlgorithmItem> items;
    CollocationsAlgorithmSettings cfg;
    QVector<U2Region>  results;
    QMutex          lock;

    const bool keepSourceAnns;
    QList<SharedAnnotationData> sourceAnns;
};

}//namespace

#endif
