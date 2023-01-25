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

#include "DigestSequenceDialog.h"

#include <QDialogButtonBox>
#include <QMessageBox>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/Theme.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/HelpButton.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "CloningUtilTasks.h"
#include "EnzymesIO.h"
#include "EnzymesQuery.h"
#include "FindEnzymesDialog.h"

namespace U2 {

const QString DigestSequenceDialog::WAIT_MESSAGE(QObject::tr("The restrictions sites are being updated. Please wait"));
const QString DigestSequenceDialog::HINT_MESSAGE(QObject::tr("Hint: there are no available enzymes. Use \"Analyze->Find Restrictions Sites\" feature to find them."));

DigestSequenceDialog::DigestSequenceDialog(ADVSequenceObjectContext* ctx, QWidget* p)
    : QDialog(p), seqCtx(ctx), timer(nullptr), animationCounter(0) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930758");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    tabWidget->setCurrentIndex(0);

    dnaObj = qobject_cast<U2SequenceObject*>(ctx->getSequenceGObject());
    sourceObj = nullptr;
    assert(dnaObj != nullptr);
    hintLabel->setText(QString());

    addAnnotationWidget();
    searchForAnnotatedEnzymes(ctx);

    availableEnzymeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    selectedEnzymeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(addButton, SIGNAL(clicked()), SLOT(sl_addPushButtonClicked()));
    connect(addAllButton, SIGNAL(clicked()), SLOT(sl_addAllPushButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(sl_removePushButtonClicked()));
    connect(clearButton, SIGNAL(clicked()), SLOT(sl_clearPushButtonClicked()));
    connect(addAnnBtn, SIGNAL(clicked()), SLOT(sl_addAnnBtnClicked()));
    connect(removeAnnBtn, SIGNAL(clicked()), SLOT(sl_removeAnnBtnClicked()));
    connect(removeAllAnnsBtn, SIGNAL(clicked()), SLOT(sl_removeAllAnnsBtnClicked()));

    updateAvailableEnzymeWidget();

    seqNameLabel->setText(dnaObj->getGObjectName());
    circularBox->setChecked(dnaObj->isCircular());

    QList<Task*> topLevelTasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    foreach (Task* t, topLevelTasks) {
        if (t->getTaskName() == AutoAnnotationsUpdateTask::NAME) {
            connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskStateChanged()));
            hintLabel->setStyleSheet("");
            hintLabel->setText(WAIT_MESSAGE);
            animationCounter = 0;
            setUiEnabled(false);
            timer = new QTimer();
            connect(timer, SIGNAL(timeout()), SLOT(sl_timerUpdate()));
            timer->start(400);
        }
    }
}

QList<SEnzymeData> DigestSequenceDialog::findEnzymeDataById(const QString& id) {
    QList<SEnzymeData> result;
    foreach (const SEnzymeData& enzyme, enzymesBase) {
        if (enzyme->id == id) {
            result.append(enzyme);
            break;
        }
    }
    return result;
}

void DigestSequenceDialog::accept() {
    if (selectedEnzymes.isEmpty()) {
        QMessageBox::information(this, windowTitle(), tr("No enzymes are selected! Please select enzymes."));
        return;
    }

    bool ok = loadEnzymesFile();
    if (!ok) {
        QMessageBox::critical(this, windowTitle(), tr("Cannot load enzymes library"));
        QDialog::reject();
    }

    QList<SEnzymeData> resultEnzymes;

    foreach (const QString& enzymeId, selectedEnzymes) {
        QList<SEnzymeData> foundEnzymes = findEnzymeDataById(enzymeId);
        resultEnzymes += foundEnzymes;
    }

    QString err = ac->validate();
    if (!err.isEmpty()) {
        QMessageBox::information(this, windowTitle(), err);
        return;
    }
    bool objectPrepared = ac->prepareAnnotationObject();
    if (!objectPrepared) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }
    const CreateAnnotationModel& m = ac->getModel();
    AnnotationTableObject* aObj = m.getAnnotationObject();
    SAFE_POINT(aObj != nullptr, "Invalid annotation table detected!", );

    DigestSequenceTaskConfig cfg;
    cfg.enzymeData = resultEnzymes;
    cfg.forceCircular = circularBox->isChecked();
    cfg.annDescription = m.description;

    int itemCount = conservedAnnsWidget->count();
    for (int row = 0; row < itemCount; ++row) {
        auto item = conservedAnnsWidget->item(row);
        QString aName = item->data((int)ItemDataRole::AnnotationNameRole).toString();
        QString aRegion = item->data((int)ItemDataRole::AnnotationLocationRole).toString();
        U2Location l;
        Genbank::LocationParser::parseLocation(qPrintable(aRegion), aRegion.size(), l);
        foreach (const U2Region& region, l->regions) {
            cfg.conservedRegions.insertMulti(aName, region);
        }
    }
    if (seqCtx != nullptr) {
        seqCtx->getAnnotatedDNAView()->tryAddObject(aObj);
    }

    DigestSequenceTask* task = new DigestSequenceTask(dnaObj, sourceObj, aObj, cfg);

    AppContext::getTaskScheduler()->registerTopLevelTask(task);

    QDialog::accept();
}

void DigestSequenceDialog::addAnnotationWidget() {
    CreateAnnotationModel acm;

    acm.sequenceObjectRef = GObjectReference(dnaObj);
    acm.hideAnnotationType = true;
    acm.hideAnnotationName = true;
    acm.hideLocation = true;
    acm.sequenceLen = dnaObj->getSequenceLength();
    acm.data->name = ANNOTATION_GROUP_FRAGMENTS;
    ac = new CreateAnnotationWidgetController(acm, this);
    QWidget* caw = ac->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->addWidget(caw);
    l->addStretch(1);
    annotationsArea->setLayout(l);
    annotationsArea->setMinimumSize(caw->layout()->minimumSize());
}

void DigestSequenceDialog::searchForAnnotatedEnzymes(ADVSequenceObjectContext* ctx) {
    QSet<AnnotationTableObject*> relatedAnnotationObjects = ctx->getAnnotationObjects(true);

    for (AnnotationTableObject* relatedAnnotationObject : qAsConst(relatedAnnotationObjects)) {
        AnnotationGroup* grp = relatedAnnotationObject->getRootGroup()->getSubgroup(ANNOTATION_GROUP_ENZYME, false);
        if (grp == nullptr) {
            continue;
        }
        sourceObj = relatedAnnotationObject;
        QList<Annotation*> subTreeAnnotations;
        grp->findAllAnnotationsInGroupSubTree(subTreeAnnotations);
        for (Annotation* subTreeAnnotation : qAsConst(subTreeAnnotations)) {
            QString enzymeId = subTreeAnnotation->getName();
            bool isDublicate = false;
            if (annotatedEnzymes.contains(enzymeId)) {
                QList<U2Region> regions = annotatedEnzymes.values(enzymeId);
                for (const U2Region& region : qAsConst(regions)) {
                    if (region == subTreeAnnotation->getRegions().first()) {
                        isDublicate = true;
                        break;
                    }
                }
            }
            if (!isDublicate) {
                annotatedEnzymes.insertMulti(enzymeId, subTreeAnnotation->getRegions().first());
                availableEnzymes.insert(enzymeId);
            }
        }
    }
}

void DigestSequenceDialog::updateAvailableEnzymeWidget() {
    availableEnzymeWidget->clear();

    QList<QString> enzymesList(availableEnzymes.values());
    std::sort(enzymesList.begin(), enzymesList.end());

    foreach (const QString& enzymeId, enzymesList) {
        QString cutInfo;
        if (annotatedEnzymes.contains(enzymeId)) {
            int numCuts = annotatedEnzymes.values(enzymeId).count();
            cutInfo = QString(tr(" : %1 cut(s)")).arg(numCuts);
        }
        availableEnzymeWidget->addItem(enzymeId + cutInfo);
    }

    bool empty = availableEnzymes.isEmpty();
    setUiEnabled(!empty);
    if (empty) {
        QString style = "QLabel { color: " + Theme::infoHintColor().name() + "; font: bold; }";
        hintLabel->setStyleSheet(style);
        hintLabel->setText(HINT_MESSAGE);
    }
}

void DigestSequenceDialog::updateSelectedEnzymeWidget() {
    selectedEnzymeWidget->clear();

    foreach (const QString& enzymeId, selectedEnzymes) {
        selectedEnzymeWidget->addItem(enzymeId);
    }
}

void DigestSequenceDialog::sl_addPushButtonClicked() {
    QList<QListWidgetItem*> items = availableEnzymeWidget->selectedItems();
    foreach (QListWidgetItem* item, items) {
        QString enzymeId = item->text().split(":").first().trimmed();
        selectedEnzymes.insert(enzymeId);
    }

    updateSelectedEnzymeWidget();
}

void DigestSequenceDialog::sl_addAllPushButtonClicked() {
    int itemCount = availableEnzymeWidget->count();
    for (int row = 0; row < itemCount; ++row) {
        QListWidgetItem* item = availableEnzymeWidget->item(row);
        QString enzymeId = item->text().split(":").first().trimmed();
        selectedEnzymes.insert(enzymeId);
    }

    updateSelectedEnzymeWidget();
}

void DigestSequenceDialog::sl_removePushButtonClicked() {
    QList<QListWidgetItem*> items = selectedEnzymeWidget->selectedItems();
    foreach (QListWidgetItem* item, items) {
        selectedEnzymes.remove(item->text());
    }
    updateSelectedEnzymeWidget();
}

void DigestSequenceDialog::sl_clearPushButtonClicked() {
    selectedEnzymes.clear();
    updateSelectedEnzymeWidget();
}

bool DigestSequenceDialog::loadEnzymesFile() {
    enzymesBase = EnzymesIO::getDefaultEnzymesList();
    return !enzymesBase.isEmpty();
}

void DigestSequenceDialog::sl_timerUpdate() {
    const int MAX_COUNT = 5;
    animationCounter++;
    if (animationCounter > MAX_COUNT) {
        animationCounter = 1;
    }

    QString dots;
    dots.fill('.', animationCounter);
    hintLabel->setStyleSheet("");
    hintLabel->setText(WAIT_MESSAGE + dots);
}

void DigestSequenceDialog::sl_taskStateChanged() {
    Task* task = qobject_cast<Task*>(sender());
    SAFE_POINT(task != nullptr, tr("Auto-annotations update task is NULL."), );

    if (task->getState() == Task::State_Finished) {
        timer->stop();
        hintLabel->setText(QString());
        searchForAnnotatedEnzymes(seqCtx);
        updateAvailableEnzymeWidget();
    }
}

void DigestSequenceDialog::setUiEnabled(bool enabled) {
    okButton->setEnabled(enabled);
    addButton->setEnabled(enabled);
    addAllButton->setEnabled(enabled);
    removeButton->setEnabled(enabled);
    clearButton->setEnabled(enabled);
}

void DigestSequenceDialog::sl_addAnnBtnClicked() {
    QObjectScopedPointer<QDialog> dlg = new QDialog(this);
    dlg->setObjectName("select_annotations_dialog");
    dlg->setWindowTitle(tr("Select annotations"));
    QVBoxLayout* layout = new QVBoxLayout(dlg.data());
    QListWidget* listWidget(new QListWidget(dlg.data()));
    QSet<AnnotationTableObject*> aObjs = seqCtx->getAnnotationObjects(false);
    foreach (AnnotationTableObject* aObj, aObjs) {
        QList<Annotation*> anns = aObj->getAnnotations();
        for (Annotation* a : qAsConst(anns)) {
            const SharedAnnotationData& d = a->getData();
            auto location = U1AnnotationUtils::buildLocationString(d);
            auto itemText = QString("%1 %2").arg(d->name).arg(location);
            auto item = new QListWidgetItem(itemText, listWidget);
            item->setData((int)ItemDataRole::AnnotationNameRole, d->name);
            item->setData((int)ItemDataRole::AnnotationLocationRole, location);
            listWidget->addItem(item);
        }
    }
    listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(listWidget);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dlg.data());
    buttonBox->setObjectName("buttonBox");
    connect(buttonBox, SIGNAL(accepted()), dlg.data(), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg.data(), SLOT(reject()));
    layout->addWidget(buttonBox);
    dlg->setLayout(layout);

    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        QList<QListWidgetItem*> items = listWidget->selectedItems();
        foreach (QListWidgetItem* item, items) {
            const QString& itemText = item->text();
            if (conservedAnnsWidget->findItems(itemText, Qt::MatchExactly).isEmpty()) {
                auto newItem = new QListWidgetItem(itemText, conservedAnnsWidget);
                auto aName = item->data((int)ItemDataRole::AnnotationNameRole).toString();
                auto aLocation = item->data((int)ItemDataRole::AnnotationLocationRole).toString();
                newItem->setData((int)ItemDataRole::AnnotationNameRole, aName);
                newItem->setData((int)ItemDataRole::AnnotationLocationRole, aLocation);
                conservedAnnsWidget->addItem(newItem);
            }
        }
    }
}

void DigestSequenceDialog::sl_removeAnnBtnClicked() {
    QList<QListWidgetItem*> items = conservedAnnsWidget->selectedItems();
    foreach (QListWidgetItem* item, items) {
        int row = conservedAnnsWidget->row(item);
        conservedAnnsWidget->takeItem(row);
        delete item;
    }
}

void DigestSequenceDialog::sl_removeAllAnnsBtnClicked() {
    conservedAnnsWidget->clear();
}

}  // namespace U2
