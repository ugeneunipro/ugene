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

#include "MsaEditorSimilarityColumn.h"

#include <QVBoxLayout>

#include <U2Algorithm/MSADistanceAlgorithm.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MSAEditor.h"
#include "MSAEditorSequenceArea.h"
#include "MaEditorUtils.h"

namespace U2 {

MsaEditorSimilarityColumn::MsaEditorSimilarityColumn(MsaEditorWgt* ui, const SimilarityStatisticsSettings* _settings)
    : MaEditorNameList(ui, new QScrollBar(Qt::Horizontal)),
      newSettings(*_settings),
      curSettings(*_settings) {
    updateDistanceMatrix();
    setObjectName("msa_editor_similarity_column");
}

MsaEditorSimilarityColumn::~MsaEditorSimilarityColumn() {
    delete matrix;
}

const SimilarityStatisticsSettings& MsaEditorSimilarityColumn::getSettings() const {
    return curSettings;
}

QWidget* MsaEditorSimilarityColumn::getWidget() {
    return this;
}

void MsaEditorSimilarityColumn::updateWidget() {
    updateDistanceMatrix();
}

void MsaEditorSimilarityColumn::setMatrix(MSADistanceMatrix* _matrix) {
    matrix = _matrix;
}

QString MsaEditorSimilarityColumn::getTextForRow(int s) {
    if (matrix == nullptr || state == DataIsBeingUpdated) {
        return tr("-");
    }

    const MultipleAlignment ma = editor->getMaObject()->getMultipleAlignment();
    const qint64 referenceRowId = editor->getReferenceRowId();
    if (referenceRowId == U2MsaRow::INVALID_ROW_ID) {
        return tr("-");
    }

    U2OpStatusImpl os;
    int refSequenceIndex = ma->getRowIndexByRowId(referenceRowId, os);
    CHECK_OP(os, QString());

    int sim = matrix->getSimilarity(refSequenceIndex, s);
    CHECK(sim != -1, tr("-"));
    QString units = matrix->isPercentSimilarity() ? "%" : "";
    return QString("%1").arg(sim) + units;
}

void MsaEditorSimilarityColumn::updateScrollBar() {
    // Override default behaviour and do nothing (keep the scrollbar invisible).
}

void MsaEditorSimilarityColumn::setSettings(const SimilarityStatisticsSettings* settings) {
    SAFE_POINT(settings != nullptr, "Settings can't be nullptr!", );
    curSettings.autoUpdate = settings->autoUpdate;
    if (curSettings.algoId != settings->algoId) {
        state = DataIsOutdated;
    }
    if (curSettings.excludeGaps != settings->excludeGaps) {
        state = DataIsOutdated;
    }
    if (curSettings.usePercents != settings->usePercents) {
        if (matrix != nullptr) {
            matrix->setPercentSimilarity(settings->usePercents);
            sl_completeRedraw();
        }
        curSettings.usePercents = settings->usePercents;
    }
    newSettings = *settings;
    if (settings->autoUpdate && DataIsOutdated == state) {
        state = DataIsBeingUpdated;
        emit si_dataStateChanged(state);
        updateDistanceMatrix();
    }
    emit si_dataStateChanged(state);
}

void MsaEditorSimilarityColumn::cancelPendingTasks() {
    createDistanceMatrixTaskRunner.cancel();
}

QString MsaEditorSimilarityColumn::getHeaderText() const {
    return curSettings.usePercents ? "%" : tr("score");
}

void MsaEditorSimilarityColumn::updateDistanceMatrix() {
    createDistanceMatrixTaskRunner.cancel();

    auto createDistanceMatrixTask = new CreateDistanceMatrixTask(newSettings);
    connect(new TaskSignalMapper(createDistanceMatrixTask), &TaskSignalMapper::si_taskFinished, this, &MsaEditorSimilarityColumn::sl_createMatrixTaskFinished);

    state = DataIsBeingUpdated;
    createDistanceMatrixTaskRunner.run(createDistanceMatrixTask);
}

void MsaEditorSimilarityColumn::onAlignmentChanged() {
    if (curSettings.autoUpdate) {
        state = DataIsBeingUpdated;
        updateDistanceMatrix();
    } else {
        state = DataIsOutdated;
    }
    emit si_dataStateChanged(state);
}

void MsaEditorSimilarityColumn::sl_createMatrixTaskFinished(Task* t) {
    auto task = qobject_cast<CreateDistanceMatrixTask*>(t);
    SAFE_POINT(task != nullptr, "Not a CreateDistanceMatrixTask", );
    bool finishedSuccessfully = !task->hasError() && !task->isCanceled();
    if (finishedSuccessfully) {
        delete matrix;
        matrix = task->getResult();
        if (matrix != nullptr) {
            matrix->setPercentSimilarity(newSettings.usePercents);
        }
    }
    sl_completeRedraw();
    if (finishedSuccessfully) {
        state = DataIsValid;
        curSettings = newSettings;
    } else {
        state = DataIsOutdated;
    }
    emit si_dataStateChanged(state);
}

CreateDistanceMatrixTask::CreateDistanceMatrixTask(const SimilarityStatisticsSettings& _s)
    : BackgroundTask<MSADistanceMatrix*>(tr("Generate distance matrix"), TaskFlags_NR_FOSE_COSC),
      s(_s) {
    SAFE_POINT(s.ui != nullptr, "Incorrect MSAEditorUI in MsaEditorSimilarityColumnTask ctor!", );
    result = nullptr;
    setVerboseLogMode(true);
}

void CreateDistanceMatrixTask::prepare() {
    MSADistanceAlgorithmFactory* factory = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactory(s.algoId);
    CHECK(factory != nullptr, );
    if (s.excludeGaps) {
        factory->setFlag(DistanceAlgorithmFlag_ExcludeGaps);
    } else {
        factory->resetFlag(DistanceAlgorithmFlag_ExcludeGaps);
    }

    MSADistanceAlgorithm* algo = factory->createAlgorithm(s.ui->getEditor()->getMaObject()->getMultipleAlignment());
    CHECK(algo != nullptr, );
    addSubTask(algo);
}

QList<Task*> CreateDistanceMatrixTask::onSubTaskFinished(Task* subTask) {
    CHECK(!subTask->isCanceled() && !subTask->hasError(), {});
    auto algo = qobject_cast<MSADistanceAlgorithm*>(subTask);
    result = new MSADistanceMatrix(algo->getMatrix());
    return {};
}

MsaEditorAlignmentDependentWidget::MsaEditorAlignmentDependentWidget(MsaEditorSimilarityColumn* _contentWidget)
    : contentWidget(_contentWidget) {
    SAFE_POINT(_contentWidget != nullptr, "Argument is NULL in constructor MsaEditorAlignmentDependentWidget()", );

    dataIsOutdatedMessage = QString("<FONT COLOR=#FF0000>%1</FONT>").arg(tr("Data is outdated"));
    dataIsValidMessage = QString("<FONT COLOR=#00FF00>%1</FONT>").arg(tr("Data is valid"));
    dataIsBeingUpdatedMessage = QString("<FONT COLOR=#0000FF>%1</FONT>").arg(tr("Data is being updated"));

    settings = &contentWidget->getSettings();
    MSAEditor* editor = settings->ui->getEditor();
    connect(editor->getMaObject(), &MultipleSequenceAlignmentObject::si_alignmentChanged, this, [this] { contentWidget->onAlignmentChanged(); });
    connect(editor, &MaEditor::si_fontChanged, this, [this](const QFont& font) { nameWidget->setFont(font); });

    createWidgetUI();
    setSettings(settings);
}

const SimilarityStatisticsSettings* MsaEditorAlignmentDependentWidget::getSettings() const {
    return settings;
}

void MsaEditorAlignmentDependentWidget::createWidgetUI() {
    auto mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    nameWidget = new QLabel(contentWidget->getHeaderText());
    nameWidget->setObjectName("Distance column name");

    createHeaderWidget();

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(contentWidget->getWidget());

    this->setLayout(mainLayout);
}

void MsaEditorAlignmentDependentWidget::createHeaderWidget() {
    auto headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(0);

    nameWidget->setAlignment(Qt::AlignCenter);
    nameWidget->setFont(settings->ui->getEditor()->getFont());
    headerLayout->addWidget(nameWidget);

    state = DataIsValid;
    headerWidget = new MaUtilsWidget(settings->ui, settings->ui->getHeaderWidget());
    headerWidget->setLayout(headerLayout);
}

void MsaEditorAlignmentDependentWidget::setSettings(const SimilarityStatisticsSettings* _settings) {
    settings = _settings;
    contentWidget->setSettings(settings);
    nameWidget->setText(contentWidget->getHeaderText());
}

void MsaEditorAlignmentDependentWidget::cancelPendingTasks() {
    contentWidget->cancelPendingTasks();
}

}  // namespace U2
