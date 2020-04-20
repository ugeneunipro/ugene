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

#include <assert.h>

#include <QFileInfo>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationDialog.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "SiteconAlgorithm.h"
#include "SiteconIO.h"
#include "SiteconSearchDialogController.h"
#include "SiteconSearchTask.h"

namespace U2 {

class SiteconResultItem : public QTreeWidgetItem {
public:
    SiteconResultItem(const SiteconSearchResult& r);
    SiteconSearchResult res;
    virtual bool operator< (const QTreeWidgetItem & other) const {
        const SiteconResultItem* o = (const SiteconResultItem*)&other;
        int n = treeWidget()->sortColumn();
        switch (n) {
            case 0 :
                return res.region.startPos < o->res.region.startPos;
            case 1:
                return res.strand != o->res.strand ? res.strand.isCompementary():  (res.region.startPos < o->res.region.startPos);
            case 2:
                return res.psum < o->res.psum;
            case 3:
                return res.err1 < o->res.err1;
            case 4:
                return res.err2 < o->res.err2;
        }
        return false;
    }
};


/* TRANSLATOR U2::SiteconSearchDialogController */

SiteconSearchDialogController::SiteconSearchDialogController(ADVSequenceObjectContext* _ctx, QWidget *p):QDialog(p) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748973");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    pbSearch = buttonBox->button(QDialogButtonBox::Ok);
    pbClose = buttonBox->button(QDialogButtonBox::Cancel);

    model = NULL;

    //props = _props;

    ctx = _ctx;
    task = NULL;

    initialSelection = ctx->getSequenceSelection()->isEmpty() ? U2Region() : ctx->getSequenceSelection()->getSelectedRegions().first();
    int seqLen = ctx->getSequenceLength();
    rs=new RegionSelector(this, seqLen, true, ctx->getSequenceSelection());
    rs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rs->setMinimumHeight(96);
    rangeSelectorLayout->addWidget(rs);

    connectGUI();
    updateState();

    pbSelectModelFile->setFocus();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(sl_onTimer()));

}

SiteconSearchDialogController::~SiteconSearchDialogController() {
    if (model!=NULL) {
        delete model;
        model = NULL;
    }
}

void SiteconSearchDialogController::connectGUI() {
    //buttons
    connect(pbSelectModelFile, SIGNAL(clicked()), SLOT(sl_selectModelFile()));
    connect(pbSaveAnnotations, SIGNAL(clicked()), SLOT(sl_onSaveAnnotations()));
    connect(pbClear, SIGNAL(clicked()), SLOT(sl_onClearList()));
    connect(pbSearch, SIGNAL(clicked()), SLOT(sl_onSearch()));
    connect(pbClose, SIGNAL(clicked()), SLOT(sl_onClose()));

    //results list
    connect(resultsTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(sl_onResultActivated(QTreeWidgetItem*, int)));

    resultsTree->installEventFilter(this);
}


void SiteconSearchDialogController::updateState() {

    bool hasActiveTask = task!=NULL;
    bool hasCompl = ctx->getComplementTT()!=NULL;

    bool hasResults = resultsTree->topLevelItemCount() > 0;
    //bool hasModel = false;//TODO

    pbSearch->setEnabled(!hasActiveTask);

    pbSaveAnnotations->setEnabled(!hasActiveTask && hasResults);
    pbClear->setEnabled(!hasActiveTask && hasResults);
    pbClose->setText(hasActiveTask ? tr("Cancel") : tr("Close"));

    rbBoth->setEnabled(!hasActiveTask && hasCompl);
    rbDirect->setEnabled(!hasActiveTask);
    rbComplement->setEnabled(!hasActiveTask && hasCompl);

    updateStatus();
}

void SiteconSearchDialogController::updateStatus() {
    QString message;
    if (task != NULL) {
        message = tr("Progress: %1% ").arg(qMax(0, task->getProgress()));
    }
    message += tr("%1 results found").arg(resultsTree->topLevelItemCount());
    statusLabel->setText(message);
}

bool SiteconSearchDialogController::eventFilter(QObject *obj, QEvent *ev) {
    if (obj == resultsTree && ev->type() == QEvent::KeyPress) {
        QKeyEvent* ke = (QKeyEvent*)ev;
        if (ke->key() == Qt::Key_Space) {
            SiteconResultItem* item = static_cast<SiteconResultItem*>(resultsTree->currentItem());
            if (item != NULL) {
                sl_onResultActivated(item, 0);
            }
        }
    }
    return false;
}

void SiteconSearchDialogController::sl_selectModelFile() {
    LastUsedDirHelper lod(SiteconIO::SITECON_ID);
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select file with SITECON model"), lod, SiteconIO::getFileFilter());
    if (lod.url.isEmpty()) {
        return;
    }

    TaskStateInfo si;
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(lod.url));
    SiteconModel m = SiteconIO::readModel(iof, lod.url, si);
    if (si.hasError()) {
        QMessageBox::critical(this, tr("Error"), si.getError());
        return;
    }
    assert(!m.modelName.isEmpty());
    assert(m.checkState());
    updateModel(m);
    QFileInfo fi(lod.url);
    modelFileEdit->setText(fi.absoluteFilePath());
}

void SiteconSearchDialogController::updateModel(const SiteconModel& m) {
    if (model!=NULL) {
        delete model;
        model = NULL;
    }
    model = new SiteconModel();
    *model =  m;

    errLevelBox->clear();
    descTextEdit->setText(model->description);
    int pStart = -1;
    while (++pStart!=99) {
        if (model->err1[pStart]!=0 && model->err2[pStart]!=1) {
            break;
        }
    }
    int pEnd = 100;

    while (--pEnd!=pStart) {
        if (model->err1[pEnd]!=1 && model->err2[pEnd]!=0) {
            break;
        }
    }
    assert(pStart<=pEnd);
    int activeIdx = -1;
    for (int i=qMax(pStart-1, 0); i <=qMin(pEnd+1, 99); i++) {
        QString text = tr("%1%,  first type error %2,  second type error %3").arg(i).arg(model->err1[i]).arg(model->err2[i]);
        errLevelBox->addItem(text, QVariant(i));
        if (activeIdx == -1 && model->err1[i] >= 0.5F) {
            activeIdx = errLevelBox->count()-1;
        }
    }
    if (activeIdx >= 0) {
        errLevelBox->setCurrentIndex(activeIdx);
    } else if (errLevelBox->count() > 0) {
        errLevelBox->setCurrentIndex(errLevelBox->count() - 1);
    }
}

void SiteconSearchDialogController::sl_onSaveAnnotations() {
    if (resultsTree->topLevelItemCount() == 0) {
        return;
    }

    CreateAnnotationModel m;
    m.sequenceObjectRef = ctx->getSequenceObject();
    m.hideLocation = true;
    m.useAminoAnnotationTypes = ctx->getAlphabet()->isAmino();
    m.sequenceLen = ctx->getSequenceObject()->getSequenceLength();

    QObjectScopedPointer<CreateAnnotationDialog> d = new CreateAnnotationDialog(this, m);
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    ctx->getAnnotatedDNAView()->tryAddObject(m.getAnnotationObject());
    const QString& name = m.data->name;
    QList<SharedAnnotationData> list;
    for (int i=0, n = resultsTree->topLevelItemCount(); i<n; ++i) {
        SiteconResultItem* item = static_cast<SiteconResultItem* >(resultsTree->topLevelItem(i));
        SharedAnnotationData data = item->res.toAnnotation(name);
        U1AnnotationUtils::addDescriptionQualifier(data, m.description);
        list.append(data);
    }

    CreateAnnotationsTask* t = new CreateAnnotationsTask(m.getAnnotationObject(), list, m.groupName);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void SiteconSearchDialogController::sl_onClearList() {
    resultsTree->clear();
    updateState();
}

void SiteconSearchDialogController::sl_onSearch() {
    runTask();
}

void SiteconSearchDialogController::reject() {
    if (task != NULL) {
        task->cancel();
    }
    QDialog::reject();
}


void SiteconSearchDialogController::sl_onClose() {
    reject();
}

void SiteconSearchDialogController::runTask() {
    assert(task == NULL);
    if (model == NULL) {
        QMessageBox::critical(this, tr("Error"), tr("No model selected"));
        return;
    }
    bool isRegionOk=false;
    U2Region reg=rs->getRegion(&isRegionOk);
    if(!isRegionOk){
        rs->showErrorMessage();
        return;
    }
    if (reg.length <= model->settings.windowSize) {
        QMessageBox::critical(this, tr("Error"), tr("Range is too small"));
        return;
    }
    U2OpStatusImpl os;
    QByteArray seq = ctx->getSequenceData(reg, os);
    CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );

    SiteconSearchCfg cfg;
    cfg.complTT = rbBoth->isChecked() || rbComplement->isChecked() ? ctx->getComplementTT() : NULL;
    cfg.complOnly = rbComplement->isChecked();
    cfg.minPSUM = errLevelBox->itemData(errLevelBox->currentIndex()).toInt();
    if (cfg.minPSUM == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Error parsing minimum score"));
        return;
    }

    //TODO: ask if to clear
    sl_onClearList();

    task = new SiteconSearchTask(*model, seq, cfg, reg.startPos);
    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_onTaskFinished()));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    updateState();
    timer->start(400);
}

void SiteconSearchDialogController::sl_onTaskFinished() {
    task = qobject_cast<SiteconSearchTask*>(sender());
    if (!task->isFinished()) {
        return;
    }
    timer->stop();
    importResults();
    task = NULL;
    updateState();
}

void SiteconSearchDialogController::sl_onTimer() {
    importResults();
}

void SiteconSearchDialogController::importResults() {
    resultsTree->setSortingEnabled(false);

    QList<SiteconSearchResult> newResults = task->takeResults();
    foreach(const SiteconSearchResult& r, newResults) {
        SiteconResultItem* item  = new SiteconResultItem(r);
        resultsTree->addTopLevelItem(item);
    }
    updateStatus();

    resultsTree->setSortingEnabled(true);
}

void SiteconSearchDialogController::sl_onResultActivated(QTreeWidgetItem* i, int col) {
    Q_UNUSED(col);
    assert(i);
    SiteconResultItem* item = static_cast<SiteconResultItem*>(i);

    ctx->getSequenceSelection()->setRegion(item->res.region);
}


//////////////////////////////////////////////////////////////////////////
/// tree

SiteconResultItem::SiteconResultItem(const SiteconSearchResult& r) : res(r)
{
    QString range = QString("%1..%2").arg(r.region.startPos + 1).arg(r.region.endPos());
    setTextAlignment(0, Qt::AlignRight);
    setTextAlignment(1, Qt::AlignRight);
    setTextAlignment(2, Qt::AlignRight);
    setTextAlignment(3, Qt::AlignRight);
    setTextAlignment(4, Qt::AlignRight);

    setText(0, range);
    QString strand = res.strand.isCompementary()? SiteconSearchDialogController::tr("Complement strand") : SiteconSearchDialogController::tr("Direct strand") ;
    setText(1, strand);
    setText(2, QString::number(res.psum, 'f', 2)+"%");
    setText(3, QString::number(res.err1, 'g', 4));
    setText(4, QString::number(res.err2, 'g', 4));
}


}//namespace
