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

#include <math.h>

#include <QDesktopWidget>
#include <QProxyStyle>
#include <QStyleFactory>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "DotPlotDialog.h"
#include "DotPlotTasks.h"

namespace U2 {

DotPlotDialog::DotPlotDialog(QWidget* parent, AnnotatedDNAView* currentADV, int minLen, int identity, ADVSequenceObjectContext* sequenceX, ADVSequenceObjectContext* sequenceY, bool dir, bool inv, const QColor& dColor, const QColor& iColor, bool hideLoadSequences)
    : QDialog(parent), xSeq(sequenceX), ySeq(sequenceY), adv(currentADV), directColor(dColor), invertedColor(iColor), openSequenceTask(nullptr) {
    setupUi(this);

    new HelpButton(this, buttonBox, "65929583");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    SAFE_POINT(adv != nullptr, "DotPlotDialog called without view context!", );

    directCheckBox->setChecked(dir);
    invertedCheckBox->setChecked(inv);

    auto buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
    buttonStyle->setParent(this);
    directColorButton->setStyle(buttonStyle);
    invertedColorButton->setStyle(buttonStyle);

    updateColors();

    // set algorithms
    algoCombo->addItem(tr("Auto"), RFAlgorithm_Auto);
    algoCombo->addItem(tr("Suffix index"), RFAlgorithm_Suffix);
    algoCombo->addItem(tr("Diagonals"), RFAlgorithm_Diagonal);

    minLenBox->setValue(minLen);
    identityBox->setValue(identity);

    connect(minLenHeuristicsButton, SIGNAL(clicked()), SLOT(sl_minLenHeuristics()));
    connect(hundredPercentButton, SIGNAL(clicked()), SLOT(sl_hundredPercent()));

    connect(directCheckBox, SIGNAL(clicked()), SLOT(sl_directInvertedCheckBox()));
    connect(invertedCheckBox, SIGNAL(clicked()), SLOT(sl_directInvertedCheckBox()));

    connect(directColorButton, SIGNAL(clicked()), SLOT(sl_directColorButton()));
    connect(invertedColorButton, SIGNAL(clicked()), SLOT(sl_invertedColorButton()));

    connect(directDefaultColorButton, SIGNAL(clicked()), SLOT(sl_directDefaultColorButton()));
    connect(invertedDefaultColorButton, SIGNAL(clicked()), SLOT(sl_invertedDefaultColorButton()));

    connect(loadSequenceButton, SIGNAL(clicked()), SLOT(sl_loadSequenceButton()));

    // listen to project modification to update list of available sequence objects.
    Project* project = AppContext::getProject();
    connect(project, SIGNAL(si_documentAdded(Document*)), SLOT(sl_documentAddedOrRemoved()));
    connect(project, SIGNAL(si_documentRemoved(Document*)), SLOT(sl_documentAddedOrRemoved()));
    reconnectAllProjectDocuments();
    updateSequenceSelectors();

    connect(xAxisCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_sequenceSelectorIndexChanged()));
    connect(yAxisCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_sequenceSelectorIndexChanged()));
    sl_sequenceSelectorIndexChanged();

    if (hideLoadSequences) {
        loadSequenceButton->hide();
    }
}

void DotPlotDialog::reconnectAllProjectDocuments() {
    Project* project = AppContext::getProject();
    foreach (Document* d, project->getDocuments()) {
        d->disconnect(this);
        connect(d, SIGNAL(si_objectAdded(GObject*)), SLOT(sl_objectAddedOrRemoved()));
        connect(d, SIGNAL(si_objectRemoved(GObject*)), SLOT(sl_objectAddedOrRemoved()));
        connect(d, SIGNAL(si_loadedStateChanged()), SLOT(sl_loadedStateChanged()));
    }
}

void DotPlotDialog::updateSequenceSelectors() {
    xAxisCombo->clear();
    yAxisCombo->clear();

    int xSeqIndex = -1, ySeqIndex = -1, curIndex = 0;

    // sequences in the project
    QList<GObject*> sequenceObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::SEQUENCE);
    foreach (GObject* obj, sequenceObjects) {
        auto seqObj = qobject_cast<U2SequenceObject*>(obj);
        QString name = seqObj->getGObjectName();

        xAxisCombo->addItem(name);
        yAxisCombo->addItem(name);

        if (xSeq && (xSeq->getSequenceGObject() == seqObj)) {
            xSeqIndex = curIndex;
        }
        if (ySeq && (ySeq->getSequenceGObject() == seqObj)) {
            ySeqIndex = curIndex;
        }
        curIndex++;
    }

    if (xSeqIndex >= 0) {
        xAxisCombo->setCurrentIndex(xSeqIndex);
    }
    if (ySeqIndex >= 0) {
        yAxisCombo->setCurrentIndex(ySeqIndex);
    } else if (sequenceObjects.size() > 1) {  // choose the second sequence for Y axis by default
        yAxisCombo->setCurrentIndex(1);
    }
}

void DotPlotDialog::sl_documentAddedOrRemoved() {
    reconnectAllProjectDocuments();
    updateSequenceSelectors();
}

void DotPlotDialog::sl_objectAddedOrRemoved() {
    updateSequenceSelectors();
}

void DotPlotDialog::sl_loadedStateChanged() {
    updateSequenceSelectors();
}

void DotPlotDialog::sl_sequenceSelectorIndexChanged() {
    int xIdx = xAxisCombo->currentIndex();
    int yIdx = yAxisCombo->currentIndex();

    QList<GObject*> sequenceObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::SEQUENCE);
    // 'sl_sequenceSelectorIndexChanged' is called twice: when the first and when the second index is set.
    // Process only valid callbacks, ignore non-valid ones.
    CHECK(xIdx >= 0 && xIdx < sequenceObjects.length(), );
    CHECK(yIdx >= 0 && yIdx < sequenceObjects.length(), );

    auto objX = qobject_cast<U2SequenceObject*>(sequenceObjects[xIdx]);
    auto objY = qobject_cast<U2SequenceObject*>(sequenceObjects[yIdx]);
    SAFE_POINT(objX != nullptr, "First object is not a sequence object", )
    SAFE_POINT(objY != nullptr, "Second object is not a sequence object", )

    bool isInvertModeAvailable = objX->getAlphabet()->isNucleic() && objY->getAlphabet()->isNucleic();
    invertedCheckBox->setEnabled(isInvertModeAvailable);
    invertedColorButton->setEnabled(isInvertModeAvailable);
    invertedDefaultColorButton->setEnabled(isInvertModeAvailable);

    int defaultWindow = qMin((int)qMin(objX->getSequenceLength(), objY->getSequenceLength()), 100);
    minLenBox->setValue(qMin(defaultWindow, minLenBox->value()));
}

void DotPlotDialog::accept() {
    int xIdx = xAxisCombo->currentIndex();
    int yIdx = yAxisCombo->currentIndex();

    QList<GObject*> sequenceObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::SEQUENCE);
    SAFE_POINT(xIdx >= 0 && xIdx < sequenceObjects.length(), QString("DotPlotDialog: index is out of range: %1").arg(xIdx), );
    SAFE_POINT(yIdx >= 0 && yIdx < sequenceObjects.length(), QString("DotPlotDialog: index is out of range: %1").arg(yIdx), );

    auto objX = qobject_cast<U2SequenceObject*>(sequenceObjects[xIdx]);
    auto objY = qobject_cast<U2SequenceObject*>(sequenceObjects[yIdx]);

    if (!isObjectInADV(objX)) {
        adv->addObject(objX);
    }

    if (!isObjectInADV(objY)) {
        adv->addObject(objY);
    }

    xSeq = adv->getSequenceContext(objX);
    ySeq = adv->getSequenceContext(objY);

    QDialog::accept();
}

void DotPlotDialog::sl_minLenHeuristics() {
    identityBox->setValue(100);

    // formula used here: nVariations / lenVariations = wantedResCount (==1000)
    // where nVariations == area size
    // lenVariations = 4^len where len is result
    // so we have len = ln(nVariations/wantedResCount)/ln(4)

    int xIdx = xAxisCombo->currentIndex();
    int yIdx = yAxisCombo->currentIndex();

    QList<GObject*> sequenceObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::SEQUENCE);
    SAFE_POINT(xIdx >= 0 && xIdx < sequenceObjects.length(), QString("DotPlotDialog: index is out of range: %1").arg(xIdx), );
    SAFE_POINT(yIdx >= 0 && yIdx < sequenceObjects.length(), QString("DotPlotDialog: index is out of range: %1").arg(yIdx), );

    auto objX = qobject_cast<U2SequenceObject*>(sequenceObjects[xIdx]);
    auto objY = qobject_cast<U2SequenceObject*>(sequenceObjects[yIdx]);

    qint64 xSeqLen = objX->getSequenceLength();
    qint64 ySeqLen = objY->getSequenceLength();

    double nVariations = xSeqLen * ySeqLen;
    double resCount = 1000;
    double len = log(nVariations / resCount) / log(double(4));

    minLenBox->setValue((int)len);
}

void DotPlotDialog::sl_hundredPercent() {
    identityBox->setValue(100);
}

int DotPlotDialog::getMismatches() const {
    return (100 - identityBox->value()) * minLenBox->value() / 100;
}

// which algorithm
RFAlgorithm DotPlotDialog::getAlgo() const {
    if (algoCheck->isChecked()) {
        int index = algoCombo->currentIndex();
        return RFAlgorithm(algoCombo->itemData(index).toInt());
    }
    return RFAlgorithm_Auto;
}

int DotPlotDialog::getMinLen() const {
    return minLenBox->value();
}

bool DotPlotDialog::isDirect() const {
    return directCheckBox->isChecked();
}

bool DotPlotDialog::isInverted() const {
    return invertedCheckBox->isChecked() && invertedCheckBox->isEnabled();
}

void DotPlotDialog::sl_directInvertedCheckBox() {
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isDirect() || isInverted());
}

void DotPlotDialog::sl_directColorButton() {
    QObjectScopedPointer<QColorDialog> d = new U2ColorDialog(directColor, this);
    d->exec();
    CHECK(!d.isNull(), );

    if (QDialog::Accepted == d->result()) {
        directColor = d->selectedColor();
        directCheckBox->setChecked(true);
    }

    updateColors();
}

void DotPlotDialog::sl_invertedColorButton() {
    QObjectScopedPointer<QColorDialog> d = new U2ColorDialog(invertedColor, this);
    d->exec();
    CHECK(!d.isNull(), );

    if (QDialog::Accepted == d->result()) {
        invertedColor = d->selectedColor();
        invertedCheckBox->setChecked(true);
    }

    updateColors();
}

void DotPlotDialog::sl_directDefaultColorButton() {
    directColor = QColor();
    directCheckBox->setChecked(true);
    updateColors();
}

void DotPlotDialog::sl_invertedDefaultColorButton() {
    invertedColor = QColor();
    invertedCheckBox->setChecked(true);
    updateColors();
}

void DotPlotDialog::sl_loadSequenceButton() {
    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::SEQUENCE});
    LastUsedDirHelper lod("DotPlot file");
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open file"), lod.dir, filter);
    if (!lod.url.isEmpty()) {
        auto tasks = new Task("Adding document to the project", TaskFlags_NR_FOSCOE | TaskFlag_SuppressErrorNotification);

        if (!AppContext::getProject()) {
            tasks->addSubTask(AppContext::getProjectLoader()->createNewProjectTask());
        }

        QVariantMap hints;
        hints[ProjectLoaderHint_LoadWithoutView] = false;
        hints[ProjectLoaderHint_LoadUnloadedDocument] = true;
        openSequenceTask = AppContext::getProjectLoader()->openWithProjectTask(lod.url, hints);
        if (openSequenceTask == nullptr) {
            return;
        }
        tasks->addSubTask(openSequenceTask);

        connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_loadTaskStateChanged(Task*)));

        AppContext::getTaskScheduler()->registerTopLevelTask(tasks);
    }
}

void DotPlotDialog::sl_loadTaskStateChanged(Task* t) {
    auto loadTask = qobject_cast<DotPlotLoadDocumentsTask*>(t);
    if (loadTask == nullptr) {
        return;
    }
    if (loadTask->getStateInfo().hasError()) {
        QMessageBox::critical(this, tr("Error"), tr("Error opening files"));
        return;
    }
}

void DotPlotDialog::updateColors() {
    QPalette palette = directColorButton->palette();
    palette.setColor(QPalette::Button, directColor);   
    directColorButton->setPalette(palette);

    palette = invertedColorButton->palette();
    palette.setColor(invertedColorButton->backgroundRole(), invertedColor);
    invertedColorButton->setPalette(palette);
}

bool DotPlotDialog::isObjectInADV(GObject* obj) {
    SAFE_POINT(obj != nullptr, "Object is NULL in DotPlotDialog::isObjectInADV(GObject* obj)", false);

    return adv->containsObject(obj);
}

GObject* DotPlotDialog::getGObjectByName(const QString& gObjectName) {
    QList<GObject*> allSequences = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::SEQUENCE);
    GObject* obj = nullptr;
    foreach (GObject* s, allSequences) {
        if (gObjectName == s->getGObjectName()) {
            obj = s;
        }
    }
    return obj;
}

}  // namespace U2
