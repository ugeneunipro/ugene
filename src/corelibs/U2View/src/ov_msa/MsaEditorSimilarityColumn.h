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

#include <QLabel>
#include <QPushButton>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/Msa.h>
#include <U2Core/Task.h>

#include "MaEditorNameList.h"
#include "MsaEditor.h"
#include "statistics/SimilarityStatisticsSettings.h"

namespace U2 {

class CreateDistanceMatrixTask;
class MsaDistanceMatrix;
class MaUtilsWidget;
class MsaEditorWgt;
class Task;

enum DataState {
    DataIsOutdated,
    DataIsValid,
    DataIsBeingUpdated
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

    void setMatrix(MsaDistanceMatrix* matrix);

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

    const Msa& msa;
    MsaDistanceMatrix* matrix = nullptr;
    SimilarityStatisticsSettings newSettings;
    SimilarityStatisticsSettings curSettings;

    BackgroundTaskRunner<MsaDistanceMatrix*> createDistanceMatrixTaskRunner;

    DataState state = DataIsOutdated;
};

class CreateDistanceMatrixTask : public BackgroundTask<MsaDistanceMatrix*> {
    Q_OBJECT
public:
    explicit CreateDistanceMatrixTask(const SimilarityStatisticsSettings& _s, const Msa& msa);

    void prepare() override;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    const Msa& msa;
    SimilarityStatisticsSettings s;
    QString resultText;
};

class MsaEditorAlignmentDependentWidget : public QWidget {
    Q_OBJECT
public:
    explicit MsaEditorAlignmentDependentWidget(MsaEditorWgt* msaEditorWgt, MsaEditorSimilarityColumn* _contentWidget);

    void cancelPendingTasks();
    const SimilarityStatisticsSettings* getSettings() const;

private:
    void setSettings(const SimilarityStatisticsSettings* _settings);
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
