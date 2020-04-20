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

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2Gui/HelpButton.h>

#include <U2View/ADVAnnotationCreation.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "SaveGraphCutoffsDialogController.h"

namespace U2{

SaveGraphCutoffsDialogController::SaveGraphCutoffsDialogController(GSequenceGraphDrawer *_d,
                                                                   QSharedPointer<GSequenceGraphData>& _gd,
                                                                   QWidget *parent,
                                                                   SequenceObjectContext* ctx)
    : QDialog(parent),
      ctx(ctx),
      d(_d),
      gd(_gd)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748819");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Save"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    CreateAnnotationModel m;
    m.hideLocation = true;
    m.data->name = QString("graph_cutoffs");
    m.sequenceObjectRef = ctx->getSequenceObject();
    m.useUnloadedObjects = false;
    m.useAminoAnnotationTypes = ctx->getAlphabet()->isAmino();
    m.sequenceLen = ctx->getSequenceObject()->getSequenceLength();
    ac = new CreateAnnotationWidgetController(m, this);

    QWidget* caw = ac->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setSizeConstraint(QLayout::SetMinAndMaxSize);
    l->setMargin(0);
    l->addWidget(caw);
    annotationsWidget->setLayout(l);

    betweenRadioButton->setChecked(true);

    float min = d->getGlobalMin(), max = d->getGlobalMax();

    if (max < 1){
        maxCutoffBox->setDecimals(4);
        minCutoffBox->setDecimals(4);
    }else if (max < 10){
        maxCutoffBox->setDecimals(3);
        minCutoffBox->setDecimals(3);
    }else if (max < 100){
        maxCutoffBox->setDecimals(2);
        minCutoffBox->setDecimals(2);
    }else if (max < 1000){
        maxCutoffBox->setDecimals(1);
        minCutoffBox->setDecimals(1);
    }else{
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

void SaveGraphCutoffsDialogController::accept(){
    if(!validate()){
        return;
    }
    bool objectPrepared = ac->prepareAnnotationObject();
    if (!objectPrepared){
        QMessageBox::critical(this, tr("Error!"), "Cannot create an annotation object. Please check settings");
        return;
    }
    const CreateAnnotationModel &mm = ac->getModel();
    int startPos = gd->cachedFrom, step = gd->cachedS, window = gd->cachedW;
    PairVector& points = gd->cachedData;

    int curPos = (startPos < window) ? (window/2 - 1) : startPos, startOffset = window/2, prevAccepetedPos = 0;
    curPos++;
    for (int i=0, n = points.cutoffPoints.size(); i < n; i++) {
        if (isAcceptableValue(points.cutoffPoints[i])){
            if (resultRegions.isEmpty()){
                resultRegions.append(U2Region(curPos - startOffset, window));
            }else{
                QList<U2Region>::iterator it = resultRegions.end();
                it--;
                if((prevAccepetedPos + step) == curPos){            //expand if accepted values in a row
                    it->length += step;
                }else{                                          //remove previous empty region, and add new region to list
                    resultRegions.append(U2Region(curPos - startOffset, window));
                }
            }
            prevAccepetedPos = curPos;
        }
        curPos += step;
    }

    QList<SharedAnnotationData> data;
    foreach (const U2Region &r, resultRegions) {
        SharedAnnotationData d(new AnnotationData);
        d->location->regions.append(r);
        d->type = mm.data->type;
        d->name = mm.data->name;
        U1AnnotationUtils::addDescriptionQualifier(d, mm.description);
        data.append(d);
    }
    AnnotationTableObject *aobj = mm.getAnnotationObject();
    tryAddObject(aobj);
    Task *t  = new CreateAnnotationsTask(aobj, data, mm.groupName);
    AppContext::getTaskScheduler()->registerTopLevelTask(t) ;
    QDialog::accept();
}

bool SaveGraphCutoffsDialogController::isAcceptableValue(float val){
    return
        (val > minCutoffBox->value() &&
        val < maxCutoffBox->value() &&
        betweenRadioButton->isChecked())
        ||
        (val < minCutoffBox->value() &&
        val > maxCutoffBox->value() &&
        outsideRadioButton->isChecked());

}

bool SaveGraphCutoffsDialogController::validate(){
    if(minCutoffBox->value() >= maxCutoffBox->value()){
        QMessageBox::critical(this, tr("Error!"), "Minimum cutoff value greater or equal maximum!");
        return false;
    }
    return true;
}

void SaveGraphCutoffsDialogController::tryAddObject(AnnotationTableObject *annotationTableObject) {
    ADVSequenceObjectContext *advContext = qobject_cast<ADVSequenceObjectContext *>(ctx);
    CHECK(NULL != advContext, );
    advContext->getAnnotatedDNAView()->tryAddObject(annotationTableObject);
}

}
