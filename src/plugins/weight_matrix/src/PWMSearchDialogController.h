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

#ifndef _U2_WEIGHT_MATRIX_SEARCH_DIALOG_H_
#define _U2_WEIGHT_MATRIX_SEARCH_DIALOG_H_

#include <U2Core/global.h>
#include <U2Core/U2Region.h>

#include <ui_PWMSearchDialog.h>

#include <QList>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QTimer>

#include <U2Core/PFMatrix.h>
#include <U2Core/PWMatrix.h>
#include <U2Gui/RegionSelector.h>

#include "WeightMatrixSearchTask.h"

namespace U2 {

class ADVSequenceObjectContext;
class U2SequenceObject;
class WeightMatrixSearchTask;
class Task;
class WeightMatrixResultItem;
class WeightMatrixQueueItem;

class PWMSearchDialogController : public QDialog, public Ui_PWMSearchDialog {
    Q_OBJECT
public:
    PWMSearchDialogController(ADVSequenceObjectContext* ctx, QWidget *p = NULL);

public slots:
    virtual void reject();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:

    //buttons:
    void sl_selectModelFile();
    void sl_onSaveAnnotations();
    void sl_onClearList();
    void sl_onSearch();
    void sl_onClose();
    void sl_onBuildMatrix();
    void sl_onSearchJaspar();

    void sl_onSliderMoved(int value);
    void sl_onAlgoChanged(QString newAlgo);
    void sl_onViewMatrix();
    void sl_onAddToQueue();
    void sl_onLoadList();
    void sl_onSaveList();
    void sl_onLoadFolder();
    void sl_onClearQueue();

    // groups
    void sl_onTaskFinished();
    void sl_onTimer();

    void sl_onResultActivated(QTreeWidgetItem* i, int col);

private:
    void connectGUI();
    void updateState();
    void updateStatus();
    void updateModel(const PWMatrix& m);
    void loadFile(QString filename);
    void addToQueue();

    bool checkPrevSettings();
    void savePrevSettings();

    void runTask();
    void importResults();

private:
    ADVSequenceObjectContext*                           ctx;
    U2Region                                            initialSelection;

    PFMatrix                                            intermediate;
    PWMatrix                                            model;

    QList< QPair<PWMatrix, WeightMatrixSearchCfg> >     queue;

    WeightMatrixSearchTask*                             task;
    QTimer*                                             timer;
    RegionSelector*                                     rs;

    QPushButton*                                        pbSearch;
    QPushButton*                                        pbClose;
    QPushButton*                                        queueButton;

};

}//namespace

#endif
