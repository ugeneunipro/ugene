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

#include <QHeaderView>
#include <QMessageBox>
#include <QMutableListIterator>
#include <QPushButton>

#include <U2Algorithm/SecStructPredictAlgRegistry.h>
#include <U2Algorithm/SecStructPredictTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>
#include <U2Core/PluginModel.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Gui/CreateAnnotationDialog.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/AnnotatedDNAView.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/LicenseDialog.h>

#include "SecStructDialog.h"

namespace U2 {

SecStructDialog::SecStructDialog( ADVSequenceObjectContext* _ctx, QWidget *p ) : QDialog(p), ctx(_ctx), task(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748971");

    sspr = AppContext::getSecStructPredictAlgRegistry();
    algorithmComboBox->addItems(sspr->getAlgNameList());

    startButton = buttonBox->button(QDialogButtonBox::Ok);
    saveAnnotationButton = buttonBox->button(QDialogButtonBox::Save);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Predict"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    buttonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));

    saveAnnotationButton->setDisabled(true);

    regionSelector = new RegionSelector(this, ctx->getSequenceLength(), false, ctx->getSequenceSelection());
    rangeSelectorLayout->addWidget(regionSelector);

    resultsTable->setColumnCount(2);
    QStringList headerNames;
    headerNames.append(tr("Region"));
    headerNames.append(tr("Structure Type"));
    resultsTable->setHorizontalHeaderLabels(headerNames);
    resultsTable->horizontalHeader()->setStretchLastSection(true);

    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_onTaskFinished(Task*)));
    connectGUI();

}

void SecStructDialog::connectGUI() {
    connect(startButton, SIGNAL(clicked()), this, SLOT(sl_onStartPredictionClicked()));
    connect(saveAnnotationButton, SIGNAL(clicked()), this, SLOT(sl_onSaveAnnotations()));
}

void SecStructDialog::updateState() {
    bool haveActiveTask = task!=NULL;
    bool haveResults = !results.isEmpty();

    algorithmComboBox->setEnabled(!haveActiveTask);
    startButton->setEnabled(!haveActiveTask);
    cancelButton->setEnabled(!haveActiveTask);
    saveAnnotationButton->setEnabled(haveResults);
    totalPredictedStatus->setText( QString("%1").arg(results.size()));
    showResults();

}

void SecStructDialog::sl_onStartPredictionClicked() {
    SAFE_POINT(task == NULL, "Found pending prediction task!", );

    SecStructPredictTaskFactory* factory = sspr->getAlgorithm(algorithmComboBox->currentText());
    SAFE_POINT(NULL != factory, "Unregistered factory name", );

    //Check license
    QString algorithm=algorithmComboBox->currentText();
    QList<Plugin*> plugins=AppContext::getPluginSupport()->getPlugins();
    foreach (Plugin* plugin, plugins){
        if(plugin->getName() == algorithm){
            if(!plugin->isFree() && !plugin->isLicenseAccepted()){
                QObjectScopedPointer<LicenseDialog> licenseDialog = new LicenseDialog(plugin);
                const int ret = licenseDialog->exec();
                CHECK(!licenseDialog.isNull(), );
                if(ret != QDialog::Accepted){
                    return;
                }
            }
            break;
        }
    }

    //prepare target sequence
    region = regionSelector->getRegion();
    SAFE_POINT(region.length > 0 && region.startPos >= 0 && region.endPos() <= ctx->getSequenceLength(), "Illegal region!", );

    U2OpStatusImpl os;
    QByteArray seqPart = ctx->getSequenceData(region, os);
    CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );

    task = factory->createTaskInstance(seqPart);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    results.clear();

    updateState();
}

void SecStructDialog::sl_onTaskFinished(Task* t) {
    if (t != task || t->getState()!= Task::State_Finished) {
        return;
    }
    results = task->getResults();

    //shifting results according to startPos
    for (QMutableListIterator<SharedAnnotationData> it_ad(results); it_ad.hasNext(); ) {
        SharedAnnotationData &ad = it_ad.next();
        U2Region::shift(region.startPos, ad->location->regions);
    }
    task = NULL;
    updateState();
}

void SecStructDialog::showResults() {
    int rowIndex = 0;
    resultsTable->setRowCount(results.size());
    foreach(const SharedAnnotationData &data, results) {
        U2Region annRegion = data->getRegions().first();
        QTableWidgetItem *locItem = new QTableWidgetItem(QString("[%1..%2]").arg(annRegion.startPos).arg(annRegion.endPos()));
        resultsTable->setItem(rowIndex, 0, locItem);
        SAFE_POINT( data->qualifiers.size() == 1, "Only one qualifier expected!", );
        QTableWidgetItem *nameItem = new QTableWidgetItem(QString(data->qualifiers.first().value));
        resultsTable->setItem(rowIndex, 1, nameItem);
        ++rowIndex;
    }
}

#define SEC_STRUCT_ANNOTATION_GROUP_NAME "predicted"

void SecStructDialog::sl_onSaveAnnotations() {
    CreateAnnotationModel m;
    m.sequenceObjectRef = ctx->getSequenceObject();
    m.hideLocation = true;
    m.hideAnnotationType = true;
    m.hideAnnotationName = true;
    m.data->name = SEC_STRUCT_ANNOTATION_GROUP_NAME;
    m.sequenceLen = ctx->getSequenceObject()->getSequenceLength();

    QObjectScopedPointer<CreateAnnotationDialog> d = new CreateAnnotationDialog(this, m);
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    ctx->getAnnotatedDNAView()->tryAddObject(m.getAnnotationObject());

    U1AnnotationUtils::addDescriptionQualifier(results, m.description);

    CreateAnnotationsTask* t = new CreateAnnotationsTask(m.getAnnotationObject(), results, m.groupName);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    QDialog::accept();
}

} // namespace U2
