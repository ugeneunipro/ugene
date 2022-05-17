/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "SaveGraphCutoffsDialogController.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2Gui/HelpButton.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

namespace U2 {

SaveGraphCutoffsDialogController::SaveGraphCutoffsDialogController(QSharedPointer<GSequenceGraphData>& _graph,
                                                                   const GSequenceGraphMinMaxCutOffState& cutOffState,
                                                                   QWidget* parent,
                                                                   SequenceObjectContext* ctx)
    : QDialog(parent), ctx(ctx), graph(_graph) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929579");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Save"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    CreateAnnotationModel m;
    m.hideLocation = true;
    m.data->name = QString("graph_cutoffs");
    m.sequenceObjectRef = ctx->getSequenceObject()->getReference();
    m.useUnloadedObjects = false;
    m.useAminoAnnotationTypes = ctx->getAlphabet()->isAmino();
    m.sequenceLen = ctx->getSequenceObject()->getSequenceLength();
    createAnnotationController = new CreateAnnotationWidgetController(m, this);

    QWidget* caw = createAnnotationController->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setSizeConstraint(QLayout::SetMinAndMaxSize);
    l->setMargin(0);
    l->addWidget(caw);
    annotationsWidget->setLayout(l);

    betweenRadioButton->setChecked(true);

    float min = cutOffState.min;
    float max = cutOffState.max;
    if (!cutOffState.isEnabled) {
        // Automatically set some meaningful value between min and max visible values.
        float delta = graph->visibleMax - graph->visibleMin;
        min = graph->visibleMin + delta / 3;
        max = graph->visibleMax - delta / 3;
    }

    if (max < 1) {
        maxCutoffBox->setDecimals(4);
        minCutoffBox->setDecimals(4);
    } else if (max < 10) {
        maxCutoffBox->setDecimals(3);
        minCutoffBox->setDecimals(3);
    } else if (max < 100) {
        maxCutoffBox->setDecimals(2);
        minCutoffBox->setDecimals(2);
    } else if (max < 1000) {
        maxCutoffBox->setDecimals(1);
        minCutoffBox->setDecimals(1);
    } else {
        maxCutoffBox->setDecimals(0);
        minCutoffBox->setDecimals(0);
    }

    maxCutoffBox->setMaximum(max);
    maxCutoffBox->setMinimum(min);
    maxCutoffBox->setValue(max);

    minCutoffBox->setMaximum(max);
    minCutoffBox->setMinimum(min);
    minCutoffBox->setValue(min);
}

void SaveGraphCutoffsDialogController::accept() {
    if (!validate()) {
        return;
    }
    bool objectPrepared = createAnnotationController->prepareAnnotationObject();
    if (!objectPrepared) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Cannot create an annotation object. Please check settings"));
        return;
    }
    QList<U2Region> resultRegions;
    U2Region currentRegion;
    for (int i = 0, n = graph->dataPoints.size(); i < n; i++) {
        float value = graph->dataPoints[i];
        if (isAcceptableValue(value)) {
            U2Region dataPointRegion(i * graph->step, graph->window);
            currentRegion = currentRegion.isEmpty() ? dataPointRegion : U2Region::containingRegion(currentRegion, dataPointRegion);
        } else if (!currentRegion.isEmpty()) {
            resultRegions.append(currentRegion);
            currentRegion = {};
        }
    }
    if (!currentRegion.isEmpty()) {
        resultRegions.append(currentRegion);
    }

    if (resultRegions.isEmpty()) {
        QMessageBox::warning(this, L10N::warningTitle(), tr("No regions to cutoff. Try change the cutoff range."));
        return;
    }

    const CreateAnnotationModel& annotationModel = createAnnotationController->getModel();
    QList<SharedAnnotationData> data;
    for (const U2Region& r : qAsConst(resultRegions)) {
        SharedAnnotationData annotationData(new AnnotationData());
        annotationData->location->regions.append(r);
        annotationData->type = annotationModel.data->type;
        annotationData->name = annotationModel.data->name;
        U1AnnotationUtils::addDescriptionQualifier(annotationData, annotationModel.description);
        data.append(annotationData);
    }
    auto annotationObject = annotationModel.getAnnotationObject();
    tryAddObject(annotationObject);

    auto task = new CreateAnnotationsTask(annotationObject, {{annotationModel.groupName, data}});
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    QDialog::accept();
}

bool SaveGraphCutoffsDialogController::isAcceptableValue(float val) const {
    bool isOutside = val <= minCutoffBox->value() || val <= maxCutoffBox->value();
    return outsideRadioButton->isChecked() ? isOutside : !isOutside;
}

bool SaveGraphCutoffsDialogController::validate() {
    if (minCutoffBox->value() >= maxCutoffBox->value()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Invalid cutoff range."));
        return false;
    }
    return true;
}

void SaveGraphCutoffsDialogController::tryAddObject(AnnotationTableObject* annotationTableObject) {
    ADVSequenceObjectContext* advContext = qobject_cast<ADVSequenceObjectContext*>(ctx);
    CHECK(advContext != nullptr, );
    advContext->getAnnotatedDNAView()->tryAddObject(annotationTableObject);
}

}  // namespace U2
