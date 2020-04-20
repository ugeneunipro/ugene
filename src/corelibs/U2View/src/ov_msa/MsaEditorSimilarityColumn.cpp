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

#include "MsaEditorSimilarityColumn.h"
#include "MSAEditor.h"
#include "MSAEditorSequenceArea.h"
#include "view_rendering/MaEditorUtils.h"

#include <U2Core/AppContext.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/MSADistanceAlgorithm.h>

#include <QVBoxLayout>

namespace U2
{

MsaEditorSimilarityColumn::MsaEditorSimilarityColumn(MsaEditorWgt* ui, QScrollBar* nhBar, const SimilarityStatisticsSettings* _settings)
    : MaEditorNameList(ui, nhBar),
      matrix(NULL),
      newSettings(*_settings),
      curSettings(*_settings),
      autoUpdate(true)
{
    updateDistanceMatrix();
    setObjectName("msa_editor_similarity_column");
}

MsaEditorSimilarityColumn::~MsaEditorSimilarityColumn() {
    CHECK(NULL != matrix, );
    delete matrix;
}

QString MsaEditorSimilarityColumn::getTextForRow( int s ) {
    if (NULL == matrix || state == DataIsBeingUpdated) {
        return tr("-");
    }

    const MultipleAlignment ma = editor->getMaObject()->getMultipleAlignment();
    const qint64 referenceRowId = editor->getReferenceRowId();
    if(U2MsaRow::INVALID_ROW_ID == referenceRowId) {
        return tr("-");
    }

    U2OpStatusImpl os;
    const int refSequenceIndex = ma->getRowIndexByRowId(referenceRowId, os);
    CHECK_OP(os, QString());

    int sim = matrix->getSimilarity(refSequenceIndex, s);
    CHECK(-1 != sim, tr("-"));
    const QString units = matrix->isPercentSimilarity() ? "%" : "";
    return QString("%1").arg(sim) + units;
}

QString MsaEditorSimilarityColumn::getSeqName(int s) {
    const MultipleAlignment ma = editor->getMaObject()->getMultipleAlignment();
    return ma->getRowNames().at(s);
}

void MsaEditorSimilarityColumn::updateScrollBar() {
    // do nothing
}

void MsaEditorSimilarityColumn::setSettings(const UpdatedWidgetSettings* _settings) {
    const SimilarityStatisticsSettings* set= static_cast<const SimilarityStatisticsSettings*>(_settings);
    CHECK(NULL != set,);
    autoUpdate = set->autoUpdate;
    if (curSettings.algoId != set->algoId) {
        state = DataIsOutdated;
    }
    if(curSettings.excludeGaps != set->excludeGaps) {
        state = DataIsOutdated;
    }
    if(curSettings.usePercents != set->usePercents) {
        if(NULL != matrix) {
            matrix->setPercentSimilarity(set->usePercents);
            sl_completeRedraw();
        }
        curSettings.usePercents = set->usePercents;
    }
    newSettings = *set;
    if(autoUpdate && DataIsOutdated == state) {
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

    CreateDistanceMatrixTask* createDistanceMatrixTask = new CreateDistanceMatrixTask(newSettings);
    connect(new TaskSignalMapper(createDistanceMatrixTask), SIGNAL(si_taskFinished(Task*)), this, SLOT(sl_createMatrixTaskFinished(Task*)));

    state = DataIsBeingUpdated;
    createDistanceMatrixTaskRunner.run( createDistanceMatrixTask );
}

void MsaEditorSimilarityColumn::onAlignmentChanged(const MultipleSequenceAlignment&, const MaModificationInfo&) {
    if(autoUpdate) {
        state = DataIsBeingUpdated;
        updateDistanceMatrix();
    }
    else {
        state = DataIsOutdated;
    }
    emit si_dataStateChanged(state);
}

void MsaEditorSimilarityColumn::sl_createMatrixTaskFinished(Task* t) {
    CreateDistanceMatrixTask* task = qobject_cast<CreateDistanceMatrixTask*> (t);
    bool finishedSuccessfully = NULL != task && !task->hasError() && !task->isCanceled();
    if (finishedSuccessfully) {
        if(NULL != matrix) {
            delete matrix;
        }
        matrix = task->getResult();
        if(NULL != matrix) {
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
      s(_s),
      resMatrix(NULL) {
    SAFE_POINT(NULL != s.ma, QString("Incorrect MultipleSequenceAlignment in MsaEditorSimilarityColumnTask ctor!"), );
    SAFE_POINT(NULL != s.ui, QString("Incorrect MSAEditorUI in MsaEditorSimilarityColumnTask ctor!"), );
    setVerboseLogMode(true);
}

void CreateDistanceMatrixTask::prepare() {
    MSADistanceAlgorithmFactory* factory = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactory(s.algoId);
    CHECK(NULL != factory,);
    if(s.excludeGaps){
        factory->setFlag(DistanceAlgorithmFlag_ExcludeGaps);
    }else{
        factory->resetFlag(DistanceAlgorithmFlag_ExcludeGaps);
    }

    MSADistanceAlgorithm* algo = factory->createAlgorithm(s.ma->getMultipleAlignment());
    CHECK(NULL != algo,);
    addSubTask(algo);
}

QList<Task*> CreateDistanceMatrixTask::onSubTaskFinished(Task* subTask){
    QList<Task*> res;
    if (isCanceled()) {
        return res;
    }
    MSADistanceAlgorithm* algo = qobject_cast<MSADistanceAlgorithm*>(subTask);
    resMatrix = new MSADistanceMatrix(algo->getMatrix());
    return res;
}

MsaEditorAlignmentDependentWidget::MsaEditorAlignmentDependentWidget(UpdatedWidgetInterface* _contentWidget)
: contentWidget(_contentWidget), automaticUpdating(true){
    SAFE_POINT(NULL != _contentWidget, QString("Argument is NULL in constructor MsaEditorAlignmentDependentWidget()"),);

    DataIsOutdatedMessage = QString("<FONT COLOR=#FF0000>%1</FONT>").arg(tr("Data are outdated"));
    DataIsValidMessage = QString("<FONT COLOR=#00FF00>%1</FONT>").arg(tr("Data are valid"));
    DataIsBeingUpdatedMessage = QString("<FONT COLOR=#0000FF>%1</FONT>").arg(tr("Data are being updated"));


    settings = &contentWidget->getSettings();
    connect(settings->ma, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)),
        this, SLOT(sl_onAlignmentChanged(const MultipleAlignment&, const MaModificationInfo&)));
    connect(dynamic_cast<QObject*>(contentWidget), SIGNAL(si_dataStateChanged(DataState)),
        this, SLOT(sl_onDataStateChanged(DataState)));
    connect(settings->ui->getEditor(), SIGNAL(si_fontChanged(const QFont&)), SLOT(sl_onFontChanged(const QFont&)));

    createWidgetUI();

    setSettings(settings);
}
void MsaEditorAlignmentDependentWidget::createWidgetUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    createHeaderWidget();

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(contentWidget->getWidget());
    nameWidget.setText(contentWidget->getHeaderText());
    nameWidget.setObjectName("Distance column name");

    this->setLayout(mainLayout);
}
void MsaEditorAlignmentDependentWidget::createHeaderWidget() {
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(0);

    nameWidget.setAlignment(Qt::AlignCenter);
    nameWidget.setFont(settings->ui->getEditor()->getFont());
    headerLayout->addWidget(&nameWidget);

    state = DataIsValid;
    headerWidget = new MaUtilsWidget(settings->ui, settings->ui->getHeaderWidget());
    headerWidget->setLayout(headerLayout);
}

void MsaEditorAlignmentDependentWidget::setSettings(const UpdatedWidgetSettings* _settings) {
    settings = _settings;
    automaticUpdating = settings->autoUpdate;
    contentWidget->setSettings(settings);
    nameWidget.setText(contentWidget->getHeaderText());
}

void MsaEditorAlignmentDependentWidget::cancelPendingTasks() {\
    contentWidget->cancelPendingTasks();
}

void MsaEditorAlignmentDependentWidget::sl_onAlignmentChanged(const MultipleAlignment& maBefore, const MaModificationInfo& modInfo) {
    const MultipleSequenceAlignment msaBefore = maBefore.dynamicCast<MultipleSequenceAlignment>();
    contentWidget->onAlignmentChanged(msaBefore, modInfo);
}

void MsaEditorAlignmentDependentWidget::sl_onUpdateButonPressed() {
    contentWidget->updateWidget();
}

void MsaEditorAlignmentDependentWidget::sl_onDataStateChanged(DataState newState) {
    state = DataIsValid;
    switch(newState) {
        case DataIsValid:
            statusBar.setText(DataIsValidMessage);
            updateButton.setEnabled(false);
            break;
        case DataIsBeingUpdated:
            statusBar.setText(DataIsBeingUpdatedMessage);
            updateButton.setEnabled(false);
            break;
        case DataIsOutdated:
            statusBar.setText(DataIsOutdatedMessage);
            updateButton.setEnabled(true);
            break;
    }
}

void MsaEditorAlignmentDependentWidget::sl_onFontChanged(const QFont& font) {
    nameWidget.setFont(font);
}

} //namespace
