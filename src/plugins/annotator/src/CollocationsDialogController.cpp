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

#include <U2Core/AnnotationSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationDialog.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVAnnotationCreation.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "CollocationsDialogController.h"

namespace U2 {
class U2SequenceObject;

//TODO: support results separation on complement and direct strands

CollocationsDialogController::CollocationsDialogController(QStringList _names, ADVSequenceObjectContext* _ctx)
: allNames(_names), ctx(_ctx)
{
    task = NULL;
    qSort(allNames);
    setupUi(this);
    new HelpButton(this, buttonBox, "24748938");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QStringList list;
    list.append(tr("<<click '+' button to add new annotation>>"));
    QTreeWidgetItem* item = new QTreeWidgetItem(annotationsTree, list);
    plusButton = new QToolButton(annotationsTree);
    plusButton->setText("+");
    annotationsTree->addTopLevelItem(item);
    annotationsTree->setItemWidget(item, 1, plusButton);

    int w = annotationsTree->minimumWidth();
    annotationsTree->setColumnWidth(1, 20);
    annotationsTree->setColumnWidth(0, w - 30);
    annotationsTree->setUniformRowHeights(true);

    searchButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(plusButton,   SIGNAL(clicked()), SLOT(sl_plusClicked()));
    connect(searchButton, SIGNAL(clicked()), SLOT(sl_searchClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_cancelClicked()));
    connect(clearResultsButton, SIGNAL(clicked()), SLOT(sl_clearClicked()));
    connect(saveResultsButton, SIGNAL(clicked()), SLOT(sl_saveClicked()));
    connect(resultsList, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(sl_onResultActivated(QListWidgetItem*)));

    timer = new QTimer(this);
    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_onTaskFinished(Task*)));
    connect(timer, SIGNAL(timeout()), SLOT(sl_onTimer()));

    updateState();
    setWindowIcon(QIcon(":/ugene/images/ugene_16.png"));

    rbBoth->setChecked(true);
}

void CollocationsDialogController::updateState() {
    bool hasActiveTask = task!=NULL;
    searchButton->setEnabled(!hasActiveTask);
    bool readyToSearch = usedNames.size() >= 2;
    searchButton->setEnabled(!hasActiveTask && readyToSearch);
    saveResultsButton->setEnabled(!hasActiveTask && resultsList->count() > 0);
    clearResultsButton->setEnabled(!hasActiveTask && resultsList->count() > 0);
    cancelButton->setText(hasActiveTask ? tr("Stop") : tr("Cancel"));
    updateStatus();
}

void CollocationsDialogController::updateStatus() {
    if (task!=NULL) {
        statusBar->setText(tr("Searching... found %1 regions. Progress: %2%").arg(resultsList->count()).arg(task->getProgress()));
    } else if (resultsList->count() > 0) {
        statusBar->setText(tr("Found %1 regions").arg(resultsList->count()));
    } else {
        statusBar->setText(searchButton->isEnabled() ? tr("Ready") : tr("Select annotation names to search"));
    }
}

void CollocationsDialogController::sl_plusClicked() {
    if (task != NULL) {
        return;
    }
    QMenu m;
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    foreach(const QString& name, allNames) {
        if (usedNames.contains(name)) {
            continue;
        }
        QColor c = asr->getAnnotationSettings(name)->color;
        QAction* a = m.addAction(GUIUtils::createSquareIcon(c, 10), name, this, SLOT(sl_addName()));
        assert(a->parent() == &m); Q_UNUSED(a);
    }
    if (m.isEmpty()) {
        m.addAction(tr("No annotations left"));
    }
    m.exec(QCursor::pos());
}


void CollocationsDialogController::sl_addName() {
    QString name = ((QAction*)sender())->text();
    assert(allNames.contains(name));
    assert(!usedNames.contains(name));

    bool remove = false;
    //UGENE-2318 inserting and removed unused item because of QT bug
    if(annotationsTree->topLevelItemCount() == 1) remove = true;

    usedNames.insert(name);
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    QColor c = asr->getAnnotationSettings(name)->color;

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setIcon(0, GUIUtils::createSquareIcon(c, 10));
    QToolButton* minusButton = new QToolButton(annotationsTree);
    minusButton->setMinimumSize(plusButton->size());
    minusButton->setText("-");
    minusButton->setObjectName(name);
    annotationsTree->insertTopLevelItem(annotationsTree->topLevelItemCount()-1, item);
    annotationsTree->setItemWidget(item, 1, minusButton);

    //UGENE-2318
    if(remove){
        QTreeWidgetItem* ii = new QTreeWidgetItem();
        int index = annotationsTree->topLevelItemCount()-1;
        annotationsTree->insertTopLevelItem(index, ii);
        annotationsTree->takeTopLevelItem(index);
        delete ii;
    }

    connect(minusButton, SIGNAL(clicked()), SLOT(sl_minusClicked()));
    updateState();
}


void CollocationsDialogController::sl_minusClicked() {
    if (task != NULL) {
        return;
    }

    QObject* o = sender();
    QString name = o->objectName();

    assert(usedNames.contains(name));
    usedNames.remove(name);
    for (int i=0, n = annotationsTree->topLevelItemCount(); i<n; i++) {
        QTreeWidgetItem* item = annotationsTree->topLevelItem(i);
        if (item->text(0) == name) {
            annotationsTree->takeTopLevelItem(i);
            delete item;
            break;
        }
    }
    updateState();
}

void CollocationsDialogController::sl_searchClicked() {
    resultsList->clear();
    assert(usedNames.size() >= 2);
    CollocationsAlgorithmSettings cfg;
    cfg.distance = regionSpin->value();
    assert(task == NULL);
    const QList<AnnotationTableObject*>& aObjects = ctx->getAnnotationObjects().toList();
    cfg.searchRegion = U2Region(0, ctx->getSequenceLength());
    if (!wholeAnnotationsBox->isChecked()) {
        cfg.st = CollocationsAlgorithm::PartialSearch;
    }
    if(rbDirect->isChecked()){
        cfg.strand = StrandOption_DirectOnly;
    }else if(rbComplement->isChecked()){
        cfg.strand = StrandOption_ComplementOnly;
    }else if (rbBoth->isChecked()){
        cfg.strand = StrandOption_Both;
    }
    task = new CollocationSearchTask(aObjects, usedNames, cfg);

    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    timer->start(400);
    updateState();
}

void CollocationsDialogController::sl_cancelClicked() {
    reject();
}

void CollocationsDialogController::sl_clearClicked() {
    resultsList->clear();
    updateState();
}

void CollocationsDialogController::sl_saveClicked() {
    assert(resultsList->count() > 0);

    CreateAnnotationModel m;
    m.sequenceObjectRef = ctx->getSequenceGObject();
    m.hideLocation = true;
    m.useAminoAnnotationTypes = ctx->getAlphabet()->isAmino();
    m.sequenceLen = ctx->getSequenceObject()->getSequenceLength();

    QObjectScopedPointer<CreateAnnotationDialog> d = new CreateAnnotationDialog(this, m);
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    QList<SharedAnnotationData> list;
    for (int i=0, n = resultsList->count(); i<n; ++i) {
        CDCResultItem* item = static_cast<CDCResultItem*>(resultsList->item(i));
        SharedAnnotationData data = m.data;
        data->location->regions.append(item->r);
        data->setStrand(U2Strand::Direct);
        U1AnnotationUtils::addDescriptionQualifier(data, m.description);
        list.append(data);
    }

    ADVCreateAnnotationsTask* t = new ADVCreateAnnotationsTask(ctx->getAnnotatedDNAView(), m.getAnnotationObject(), m.groupName, list);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void CollocationsDialogController::reject() {
    if (task!=NULL) {
        task->cancel();
    }
    QDialog::reject();
}

void CollocationsDialogController::sl_onTimer() {
    importResults();
    updateState();
}

void CollocationsDialogController::sl_onTaskFinished(Task* t) {
    if (t != task || t->getState()!= Task::State_Finished) {
        return;
    }
    importResults();
    task = NULL;
    updateState();
    timer->stop();
}

void CollocationsDialogController::importResults() {
    if (task == NULL) {
        return;
    }

    QVector<U2Region> newResults = task->popResults();

    foreach(const U2Region& r, newResults) {
        CDCResultItem* item = new CDCResultItem(r);
        bool inserted = false;
        for(int i=0, n = resultsList->count(); i<n; i++) {
            CDCResultItem* tmp = static_cast<CDCResultItem*>(resultsList->item(i));
            assert(!tmp->r.contains(r) && !r.contains(tmp->r));
            if (tmp->r.startPos > r.startPos) {
                resultsList->insertItem(i, item);
                inserted = true;
            }
        }
        if (!inserted) {
            resultsList->addItem(item);
        }
    }
}


void CollocationsDialogController::sl_onResultActivated(QListWidgetItem * item) {
    assert(item!=NULL);
    CDCResultItem* ri = static_cast<CDCResultItem*>(item);
    Q_UNUSED(ri);
    //todo: add to selection?
    //ctx->getPanView()->setVisibleRange(ri->r);
    //ctx->getDetView()->setCenterPos(ri->r.startPos);
}


CDCResultItem::CDCResultItem(const U2Region& _r) : r(_r) {
    setText(QString("[%1, %2]").arg(QString::number(r.startPos+1)).arg(r.endPos()));
}


//////////////////////////////////////////////////////////////////////////
// task
CollocationSearchTask::CollocationSearchTask(const QList<AnnotationTableObject*> &table, const QSet<QString>& names,
    const CollocationsAlgorithmSettings& cfg)
    : Task(tr("Search for annotated regions"), TaskFlag_None), cfg(cfg), lock(QMutex::Recursive), keepSourceAnns(false)
{
    GCOUNTER(cvar, tvar, "CollocationSearchTask");
    assert(cfg.distance >= 0);
    assert(!names.isEmpty());
    foreach(const QString& name, names) {
        getItem(name);
    }
    foreach(AnnotationTableObject* ao, table) {
        foreach(Annotation *a, ao->getAnnotations()) {
            const QString& name = a->getName();
            if((a->getStrand().isDirect() && cfg.strand == StrandOption_ComplementOnly) ||
                (a->getStrand().isCompementary() && cfg.strand == StrandOption_DirectOnly)){
                    items.remove(name);
                    continue;
            }
            if (names.contains(name)) {
                CollocationsAlgorithmItem& item = getItem(name);
                foreach(const U2Region& r, a->getRegions()) {
                    if (cfg.searchRegion.intersects(r)) {
                        item.regions.append(r);
                    }
                }
            }
        }
    }
}

CollocationSearchTask::CollocationSearchTask(const QList<SharedAnnotationData> &table, const QSet<QString>& names, const CollocationsAlgorithmSettings& cfg,
    bool _keepSourceAnns)
    : Task(tr("Search for annotated regions"), TaskFlag_None), cfg(cfg), lock(QMutex::Recursive), keepSourceAnns(_keepSourceAnns)
{
    assert(cfg.distance >= 0);
    assert(!names.isEmpty());
    foreach(const QString& name, names) {
        getItem(name);
    }
    foreach (const SharedAnnotationData &a, table) {
        const QString& name = a->name;
        if ((a->getStrand().isDirect() && cfg.strand == StrandOption_ComplementOnly) ||
           (a->getStrand().isCompementary() && cfg.strand == StrandOption_DirectOnly))
        {
               items.remove(name);
               continue;
        }
        if (names.contains(name)) {
            CollocationsAlgorithmItem& item = getItem(name);
            bool hasRegions = false;
            foreach (const U2Region &r, a->location->regions) {
                if (cfg.searchRegion.intersects(r)) {
                    hasRegions = true;
                    item.regions.append(r);
                }
            }
            if (keepSourceAnns && hasRegions) {
                sourceAnns << a;
            }
        }
    }
}

CollocationsAlgorithmItem& CollocationSearchTask::getItem(const QString& name) {
    if (!items.contains(name)) {
        items[name] = CollocationsAlgorithmItem(name);
    }
    return items[name];
}

void CollocationSearchTask::run() {
    CollocationsAlgorithm::find(items.values(), stateInfo, this, cfg);
}

void CollocationSearchTask::onResult(const U2Region& r) {
    QMutexLocker locker(&lock);
    results.append(r);
}

QVector<U2Region> CollocationSearchTask::popResults() {
    QMutexLocker locker(&lock);
    QVector<U2Region> tmp = results;
    results.clear();
    return tmp;
}

QList<SharedAnnotationData> CollocationSearchTask::popResultAnnotations() {
    QMutexLocker locker(&lock);
    QVector<U2Region> res = this->popResults();

    QList<SharedAnnotationData> result;
    if (keepSourceAnns) {
        for (int i = 0; i < sourceAnns.size(); ++i) {
            SharedAnnotationData &a = sourceAnns[i];
            QVector<U2Region> resRegs;
            foreach (const U2Region &r, a->location->regions) {
                if (isSuitableRegion(r, res)) {
                    resRegs << r;
                }
            }
            if (!resRegs.isEmpty()) {
                a->location->regions = resRegs;
                result << a;
            }
        }
    } else {
        foreach(const U2Region &r, res) {
            SharedAnnotationData data(new AnnotationData);
            if (cfg.includeBoundaries) {
                data->location->regions.append(r);
            } else {
                data->location->regions.append(cutResult(r));
            }
            data->setStrand(U2Strand::Direct);
            data->name = cfg.resultAnnotationsName;
            result.append(data);
        }
    }

    return result;
}

U2Region CollocationSearchTask::cutResult(const U2Region &res) const {
    qint64 left = res.endPos();
    qint64 right = res.startPos;

    foreach (const CollocationsAlgorithmItem &item, items) {
        foreach(const U2Region &r, item.regions) {
            if (r.startPos == res.startPos) {
                if (r.endPos() < left) {
                    left = r.endPos();
                }
            }
            if (r.endPos() == res.endPos()) {
                if (right < r.startPos) {
                    right = r.startPos;
                }
            }
        }
    }
    if (left < right) {
        return U2Region(left, right - left);
    }
    return res;
}

bool CollocationSearchTask::isSuitableRegion(const U2Region &r, const QVector<U2Region> &resultRegions) const {
    foreach(const U2Region &res, resultRegions) {
        if (CollocationsAlgorithm::NormalSearch == cfg.st) {
            if (res.contains(r)) {
                return true;
            }
        } else {
            if (res.intersects(r)) {
                return true;
            }
        }
    }

    return false;
}

}//namespace
