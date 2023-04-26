/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QLabel>
#include <QPushButton>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>

#include "MSAEditor.h"
#include "MaEditorNameList.h"

namespace U2 {

class CreateDistanceMatrixTask;
class MSADistanceMatrix;
class MaUtilsWidget;
class MsaEditorWgt;
class Task;

enum DataState {
    DataIsOutdated,
    DataIsValid,
    DataIsBeingUpdated
};

class SimilarityStatisticsSettings {
public:
    QPointer<MSAEditor> editor;
    bool autoUpdate = true;
    /** Selected algorithm. */
    QString algoId;
    bool usePercents = false;
    bool excludeGaps = false;
};

class U2VIEW_EXPORT MsaEditorSimilarityColumn : public MaEditorNameList {
    friend class GTUtilsMSAEditorSequenceArea;
    Q_OBJECT
public:
    MsaEditorSimilarityColumn(MsaEditorWgt* ui, const SimilarityStatisticsSettings* settings);
    ~MsaEditorSimilarityColumn() override;

    void setSettings(const SimilarityStatisticsSettings* settings);

    void cancelPendingTasks();

    const SimilarityStatisticsSettings& getSettings() const;

    QWidget* getWidget();

    void updateWidget();

    QString getHeaderText() const;

    void setMatrix(MSADistanceMatrix* matrix);

    QString getTextForRow(int maRowIndex) override;

protected:
    void updateScrollBar() override;

signals:
    void si_dataStateChanged(const DataState& newState);

public:
    void onAlignmentChanged();

private slots:
    void sl_createMatrixTaskFinished(Task*);

private:
    void updateDistanceMatrix();

    MSADistanceMatrix* matrix = nullptr;
    SimilarityStatisticsSettings newSettings;
    SimilarityStatisticsSettings curSettings;

    BackgroundTaskRunner<MSADistanceMatrix*> createDistanceMatrixTaskRunner;

    DataState state = DataIsOutdated;
};

class CreateDistanceMatrixTask : public BackgroundTask<MSADistanceMatrix*> {
    Q_OBJECT
public:
    explicit CreateDistanceMatrixTask(const SimilarityStatisticsSettings& _s);

    void prepare() override;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    SimilarityStatisticsSettings s;
    QString resultText;
};

class MsaEditorAlignmentDependentWidget : public QWidget {
    Q_OBJECT
public:
    explicit MsaEditorAlignmentDependentWidget(MsaEditorWgt* msaEditorWgt, MsaEditorSimilarityColumn* _contentWidget);

    void setSettings(const SimilarityStatisticsSettings* _settings);
    void cancelPendingTasks();
    const SimilarityStatisticsSettings* getSettings() const;

private:
    void createWidgetUI();
    void createHeaderWidget();

    MsaEditorWgt* msaEditorWgt = nullptr;
    MaUtilsWidget* headerWidget = nullptr;
    QLabel* nameWidget = nullptr;
    MsaEditorSimilarityColumn* contentWidget;
    const SimilarityStatisticsSettings* settings;
    DataState state = DataIsOutdated;
    QString dataIsOutdatedMessage;
    QString dataIsValidMessage;
    QString dataIsBeingUpdatedMessage;
};
}  // namespace U2
