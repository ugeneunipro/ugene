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

#ifndef _U2_MSA_EDITOR_DATA_LIST_H_
#define _U2_MSA_EDITOR_DATA_LIST_H_

#include <QLabel>
#include <QPushButton>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>

#include "MaEditorNameList.h"
#include "MsaUpdatedWidgetInterface.h"

namespace U2 {

class CreateDistanceMatrixTask;
class MSADistanceMatrix;
class MaUtilsWidget;
class Task;

class SimilarityStatisticsSettings : public UpdatedWidgetSettings {
public:
    SimilarityStatisticsSettings() : usePercents(false), excludeGaps(false){}
    QString                         algoId;// selected algorithm
    bool                            usePercents;
    bool                            excludeGaps;
};

class MsaEditorSimilarityColumn : public MaEditorNameList, public UpdatedWidgetInterface {
    friend class GTUtilsMSAEditorSequenceArea;
    Q_OBJECT
public:
    MsaEditorSimilarityColumn(MsaEditorWgt* ui, QScrollBar* nhBar, const SimilarityStatisticsSettings* _settings);
    virtual ~MsaEditorSimilarityColumn();

    void setSettings(const UpdatedWidgetSettings* _settings);

    void cancelPendingTasks();

    const UpdatedWidgetSettings& getSettings() const {return curSettings;}
    QWidget* getWidget(){return this;}
    void updateWidget(){updateDistanceMatrix();}
    QString getHeaderText() const;

    DataState getState() {return state;}

    void setMatrix(MSADistanceMatrix* _matrix) {matrix = _matrix;}

protected:
    QString getTextForRow(int s);
    QString getSeqName(int s);
    void updateScrollBar();

signals:
    void si_dataStateChanged(DataState newState);
private slots:

    void onAlignmentChanged(const MultipleSequenceAlignment& maBefore, const MaModificationInfo& modInfo);
    void sl_createMatrixTaskFinished(Task*);
private:
    void updateDistanceMatrix();

    MSADistanceMatrix* matrix;
    SimilarityStatisticsSettings newSettings;
    SimilarityStatisticsSettings curSettings;

    BackgroundTaskRunner<MSADistanceMatrix*> createDistanceMatrixTaskRunner;

    DataState state;
    bool      autoUpdate;
};


class CreateDistanceMatrixTask : public BackgroundTask<MSADistanceMatrix*> {
    Q_OBJECT
public:
    CreateDistanceMatrixTask(const SimilarityStatisticsSettings& _s);

    virtual void prepare();

    MSADistanceMatrix* getResult() const {return resMatrix;}

    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    SimilarityStatisticsSettings   s;
    QString                             resultText;
    MSADistanceMatrix*                  resMatrix;
};


class MsaEditorAlignmentDependentWidget : public QWidget {
    Q_OBJECT
public:
    MsaEditorAlignmentDependentWidget(UpdatedWidgetInterface* _contentWidget);

    void setSettings(const UpdatedWidgetSettings* _settings);
    void cancelPendingTasks();
    const DataState& getDataState() const{return state;}
    const UpdatedWidgetSettings* getSettings() const {return settings;}

private slots:
    void sl_onAlignmentChanged(const MultipleAlignment &maBefore, const MaModificationInfo& modInfo);
    void sl_onUpdateButonPressed();
    void sl_onDataStateChanged(DataState newState);
    void sl_onFontChanged(const QFont&);
private:
    void createWidgetUI();
    void createHeaderWidget();

    MaUtilsWidget*               headerWidget;
    QLabel                       statusBar;
    QLabel                       nameWidget;
    QPushButton                  updateButton;
    UpdatedWidgetInterface*      contentWidget;
    const UpdatedWidgetSettings* settings;
    DataState                    state;
    QWidget*                     updateBar;

    bool                         automaticUpdating;

    QString DataIsOutdatedMessage;
    QString DataIsValidMessage;
    QString DataIsBeingUpdatedMessage;
};
} //namespace


#endif
