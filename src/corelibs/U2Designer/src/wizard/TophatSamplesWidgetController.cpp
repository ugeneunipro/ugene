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

#include <QFocusEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/WizardController.h>

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/WorkflowUtils.h>

#include "TophatSamplesWidgetController.h"

namespace U2 {

TophatSamplesWidgetController::TophatSamplesWidgetController(WizardController *wc, TophatSamplesWidget *tsw)
: WidgetController(wc), tsw(tsw)
{

}

QWidget * TophatSamplesWidgetController::createGUI(U2OpStatus & /*os*/) {
    initSamplesMap();
    commit();
    return new TophatSamples(samples, this);
}

void TophatSamplesWidgetController::renameSample(int pos, const QString &newName, U2OpStatus &os) {
    checkRange(pos, EXCLUSIVE, os);
    CHECK_OP(os, );

    // check new name
    if (newName.isEmpty()) {
        os.setError(tr("Sample name can not be empty"));
        return;
    }
    QRegExp regExp("\\w+");
    if (!regExp.exactMatch(newName)) {
        os.setError(tr("Sample name can consist only of Latin letters, numbers and the '_' symbol"));
        return;
    }

    // check duplicates
    for (int i=0; i<samples.size(); i++) {
        TophatSample &sample = samples[i];
        if ((pos != i) && (sample.name == newName)) {
            os.setError(tr("Duplicate sample name"));
            return;
        }
    }

    // update model
    TophatSample &sample = samples[pos];
    sample.name = newName;
    commit();
}

bool TophatSamplesWidgetController::removeSample(int pos, QStringList &insertToFirst, QList<TophatSample> &append) {
    U2OpStatusImpl os;
    checkRange(pos, EXCLUSIVE, os);
    CHECK_OP(os, false);

    TophatSample removed = samples.takeAt(pos);
    while (samples.size() < 2) {
        insertSample(samples.size(), os);
        CHECK_OP(os, true);
        append << samples.last();
    }
    insertToFirst << removed.datasets;

    // update model
    samples[0].datasets << removed.datasets;
    commit();
    return true;
}

TophatSample TophatSamplesWidgetController::insertSample(int pos, U2OpStatus &os) {
    TophatSample result("", QStringList());
    checkRange(pos, INCLUSIVE, os);
    CHECK_OP(os, result);

    QStringList names;
    foreach (const TophatSample &s, samples) {
        names << s.name;
    }

    QString name;
    int idx = 1;
    do {
        name = QString("Sample%1").arg(idx);
        idx++;
    } while (names.contains(name));

    QList<TophatSample>::Iterator posIter = samples.begin() + pos;
    result.name = name;

    // update model
    samples.insert(posIter, result);
    commit();
    return result;
}

void TophatSamplesWidgetController::replaceDataset(int oldSamplePos, int oldDatasetPos, int newSamplePos, int newDatasetPos, U2OpStatus &os) {
    checkDatasetRange(oldSamplePos, oldDatasetPos, EXCLUSIVE, os);
    CHECK_OP(os, );
    checkRange(newSamplePos, EXCLUSIVE, os);
    CHECK_OP(os, );

    TophatSample &oldSample = samples[oldSamplePos];
    TophatSample &newSample = samples[newSamplePos];
    RangeType type = (&oldSample == &newSample) ? EXCLUSIVE : INCLUSIVE;
    checkDatasetRange(newSamplePos, newDatasetPos, type, os);
    CHECK_OP(os, );

    // update model
    QString dataset = oldSample.datasets.takeAt(oldDatasetPos);
    newSample.datasets.insert(newDatasetPos, dataset);
    commit();
}

bool TophatSamplesWidgetController::canShowWarning() const {
    return !wc->isRejected();
}

void TophatSamplesWidgetController::initSamplesMap() {
    // prepare samples
    U2OpStatus2Log os;
    samples = WorkflowUtils::unpackSamples(wc->getAttributeValue(tsw->samplesAttr).toString(), os);
    removeMissedDatasets();
    while (samples.size() < 2) {
        U2OpStatusImpl os;
        insertSample(samples.size(), os);
        CHECK_OP(os, );
    }

    // sampled/unsampled
    QStringList sampledDatasets = getSampledDatasets();
    QStringList unsampledDatasets = getUnsampledDatasets(sampledDatasets);
    if (unsampledDatasets.isEmpty()) {
        return;
    }

    // 2 unsampled datasets can be divided into 2 samples
    if ((2 == unsampledDatasets.size()) && (sampledDatasets.isEmpty())) {
        samples[0].datasets << unsampledDatasets[0];
        samples[1].datasets << unsampledDatasets[1];
        return;
    }

    // otherwise, all unsampled datasets are moved into the first sample
    samples[0].datasets << unsampledDatasets;
}

void TophatSamplesWidgetController::removeMissedDatasets() {
    QStringList allDatasets = getAllDatasets();
    for (int i=0; i<samples.size(); i++) {
        QStringList &sampled = samples[i].datasets;
        foreach (const QString &d, sampled) {
            if (!allDatasets.contains(d)) {
                sampled.removeAll(d);
            }
        }
    }
}

void TophatSamplesWidgetController::checkRange(int pos, RangeType rangeType, U2OpStatus &os) const {
    int lastPos = (rangeType == INCLUSIVE) ? samples.size() : samples.size() - 1;
    SAFE_POINT_EXT(pos <= lastPos && pos > -1, os.setError("Out of range"), );
}

void TophatSamplesWidgetController::checkDatasetRange(int samplePos, int datasetPos, RangeType rangeType, U2OpStatus &os) const {
    checkRange(samplePos, EXCLUSIVE, os);
    CHECK_OP(os, );
    const TophatSample &s = samples[samplePos];

    int lastPos = (rangeType == INCLUSIVE) ? s.datasets.size() : s.datasets.size() - 1;
    SAFE_POINT_EXT(datasetPos <= lastPos && datasetPos > -1, os.setError("Out of range"), );
}

QStringList TophatSamplesWidgetController::getAllDatasets() const {
    AttributeInfo dsInfo(tsw->datasetsProvider, BaseAttributes::URL_IN_ATTRIBUTE().getId());
    QList<Dataset> datasets = wc->getAttributeValue(dsInfo).value< QList<Dataset> >();

    QStringList result;
    foreach (const Dataset &d, datasets) {
        result << d.getName();
    }
    return result;
}

QStringList TophatSamplesWidgetController::getSampledDatasets() const {
    QStringList result;
    foreach (const TophatSample &s, samples) {
        result << s.datasets;
    }
    return result;
}

QStringList TophatSamplesWidgetController::getUnsampledDatasets(const QStringList &sampledDatasets) const {
    QStringList result;
    QStringList allDatasets = getAllDatasets();
    foreach (const QString &d, allDatasets) {
        if (!sampledDatasets.contains(d)) {
            result << d;
        }
    }
    return result;
}

void TophatSamplesWidgetController::commit() {
    wc->setAttributeValue(tsw->samplesAttr, WorkflowUtils::packSamples(samples));
}

/************************************************************************/
/* SampleNameEdit */
/************************************************************************/
class SampleNameEdit : public QLineEdit {
public:
    SampleNameEdit(TophatSamples *samples, const QString &name, QWidget *parent)
        : QLineEdit(name, parent), samples(samples), initialName(name)
    {
        setValidator(new QRegExpValidator(QRegExp("\\w*"), this));
        setObjectName(name);
    }

protected:
    void focusOutEvent(QFocusEvent *e) {
        bool renamed = samples->rename(this);
        if (!renamed) {
            setText(initialName);
        }
        QLineEdit::focusOutEvent(e);
    }

private:
    TophatSamples *samples;
    QString initialName;
};

/************************************************************************/
/* TophatSamples */
/************************************************************************/
TophatSamples::TophatSamples(const QList<TophatSample> &samples, TophatSamplesWidgetController *ctrl, QWidget *parent)
: QWidget(parent), ctrl(ctrl), scrollArea(NULL), listLayout(NULL), upButton(NULL), downButton(NULL)
{
    init(samples);
}

void TophatSamples::init(const QList<TophatSample> &samples) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *hint = new QLabel(this);
    mainLayout->addWidget(hint);
    {
        hint->setWordWrap(true);
        hint->setText(tr("Divide the input datasets into samples for running Cuffdiff. There are must be at least 2 samples. "
            "It is not necessary to have the same number of datasets (replicates) for each sample. "
            "The samples names will be used by Cuffdiff as labels, which will be included in various output files produced by Cuffdiff."));
        QString style = "\
            background-color: rgb(255, 255, 191);\
            border-width: 1px;\
            border-style: solid;\
            border-radius: 4px;\
            ";
#ifdef Q_OS_WIN
        style += "padding: 5px;";
#else
        style += "padding: 0px 3px 0px 3px;";
#endif
        hint->setStyleSheet(style);
    }

    QHBoxLayout *hl = new QHBoxLayout();
    mainLayout->addLayout(hl);
    hl->setContentsMargins(0, 0, 0, 0);

    hl->addWidget(createScrollArea());
    hl->addLayout(createControlButtonsLayout());

    foreach (const TophatSample &sample, samples) {
        appendSample(sample);
    }

#ifndef Q_OS_WIN
    hint->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
#endif

    updateArrows();
}

void TophatSamples::appendSample(const TophatSample &sample) {
    QWidget *sw = initSample(sample.name, sample.datasets);
    order << sw;
    listLayout->addWidget(sw);
}

QWidget * TophatSamples::initSample(const QString &sampleName, const QStringList &datasets) {
    QWidget *result = new QGroupBox(this);
    QVBoxLayout *vl = new QVBoxLayout(result);
    vl->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *hl = new QHBoxLayout();
    { // header
        hl->setContentsMargins(0, 0, 0, 0);
        QToolButton *removeButton = createButton(this, ":U2Designer/images/exit.png");
        connect(removeButton, SIGNAL(clicked()), SLOT(sl_remove()));
        hl->addWidget(new SampleNameEdit(this, sampleName, this));
        hl->addWidget(removeButton);
    }
    vl->addLayout(hl);

    QListWidget *datasetsList = new QListWidget(this);
    datasetsList->setObjectName(sampleName + " datasetsList");
    { // datasets
        foreach (const QString &dataset, datasets) {
            datasetsList->addItem(dataset);
        }
        connect(datasetsList, SIGNAL(itemSelectionChanged()), SLOT(sl_selectionChanged()));
    }
    vl->addWidget(datasetsList);

    return result;
}

bool TophatSamples::rename(QLineEdit *nameEdit) {
    QWidget *sampleWidget = nameEdit->parentWidget();
    CHECK(order.contains(sampleWidget), true);

    U2OpStatusImpl os;
    ctrl->renameSample(order.indexOf(sampleWidget), nameEdit->text(), os);
    if (os.hasError()) {
        if (ctrl->canShowWarning()) {
            QMessageBox::critical(this, TophatSamples::tr("Error"), os.getError());
        }
        return false;
    }
    return true;
}

void TophatSamples::sl_remove() {
    QToolButton *toolButton = dynamic_cast<QToolButton*>(sender());
    SAFE_POINT(NULL != toolButton, "NULL button", );
    QWidget *sampleWidget = toolButton->parentWidget();
    CHECK(NULL != sampleWidget, );
    CHECK(order.contains(sampleWidget), );

    // remove
    QStringList insertToFirst;
    QList<TophatSample> append;
    bool removed = ctrl->removeSample(order.indexOf(sampleWidget), insertToFirst, append);
    CHECK(removed, );

    // update ui
    order.removeOne(sampleWidget);
    delete sampleWidget;
    foreach (const TophatSample &sample, append) {
        appendSample(sample);
    }

    QListWidget *list = getListWidget(0);
    CHECK(NULL != list, );
    list->addItems(insertToFirst);

    updateArrows();
}

void TophatSamples::sl_add() {
    U2OpStatusImpl os;
    TophatSample sample = ctrl->insertSample(order.size(), os);
    CHECK_OP(os, );

    appendSample(sample);
    updateArrows();
}

void TophatSamples::sl_selectionChanged() {
    QListWidget *selectedList = dynamic_cast<QListWidget*>(sender());
    CHECK(NULL != selectedList, );
    CHECK(selectedList->selectedItems().size() > 0, );

    QWidget *sampleWidget = selectedList->parentWidget();
    CHECK(NULL != sampleWidget, );
    CHECK(order.contains(sampleWidget), );

    selectSample(order.indexOf(sampleWidget));
    updateArrows();
}

void TophatSamples::selectSample(int selectedPos) {
    for (int pos=0; pos<order.size(); pos++) {
        QListWidget *list = getListWidget(pos);
        SAFE_POINT(NULL != list, "NULL list widget", );
        if (pos != selectedPos) {
            list->clearSelection();
            list->setCurrentRow(-1);
        }
    }
}

bool TophatSamples::isBorderCase(QListWidget *list, int datasetPos, Direction direction) const {
    switch (direction) {
    case UP:
        return (0 == datasetPos);
    case DOWN:
        return (list->count() - 1 == datasetPos);
    default:
        return false;
    }
}

void TophatSamples::getNewPositions(QListWidget *oldList, int oldSamplePos, int oldDatasetPos, Direction direction, int &newSamplePos, int &newDatasetPos, QListWidget * &newList) const {
    newSamplePos = oldSamplePos;
    newDatasetPos = (UP == direction) ? oldDatasetPos - 1 : oldDatasetPos + 1;
    newList = oldList;

    if (isBorderCase(oldList, oldDatasetPos, direction)) {
        newSamplePos = (UP == direction) ? oldSamplePos - 1 : oldSamplePos + 1;
        newList = getListWidget(newSamplePos);
        SAFE_POINT(NULL != newList, "NULL prev list widget", );
        newDatasetPos = (UP == direction) ? (newList)->count() : 0;
    }
}

void TophatSamples::findSelectedDataset(int &samplePos, int &datasetPos) const {
    for (int pos=0; pos<order.size(); pos++) {
        QListWidget *list = getListWidget(pos);
        SAFE_POINT(NULL != list, "NULL list widget", );

        int currentRow = list->currentRow();
        if (-1 != currentRow) {
            samplePos = pos;
            datasetPos = currentRow;
            return;
        }
    }
}

void TophatSamples::move(Direction direction) {
    // find selected dataset
    int oldSamplePos = -1;
    int oldDatasetPos = -1;
    findSelectedDataset(oldSamplePos, oldDatasetPos);
    CHECK(-1 != oldSamplePos, );
    QListWidget *oldList = getListWidget(oldSamplePos);
    SAFE_POINT(NULL != oldList, "NULL old list widget", );

    // find target position
    int newSamplePos = -1;
    int newDatasetPos = - 1;
    QListWidget *newList = NULL;
    getNewPositions(oldList, oldSamplePos, oldDatasetPos, direction, newSamplePos, newDatasetPos, newList);
    SAFE_POINT(NULL != newList, "NULL new list widget", );

    // move selected dataset
    U2OpStatusImpl os;
    ctrl->replaceDataset(oldSamplePos, oldDatasetPos, newSamplePos, newDatasetPos, os);
    CHECK_OP(os, );

    // update ui
    QListWidgetItem *item = oldList->takeItem(oldDatasetPos);
    newList->insertItem(newDatasetPos, item->text());
    delete item;

    // update selection
    selectSample(newSamplePos);
    newList->setCurrentRow(newDatasetPos);
    updateArrows();
    return;
}

void TophatSamples::sl_up() {
    move(UP);
}

void TophatSamples::sl_down() {
    move(DOWN);
}

QListWidget * TophatSamples::getListWidget(int pos) const {
    CHECK((pos > -1) && (pos < order.size()), NULL);
    QWidget *first = order[pos];
    return first->findChild<QListWidget*>();
}

QToolButton * TophatSamples::createButton(QWidget *parent, const QString &icon) const {
    QToolButton *result = new QToolButton(parent);
    result->setIcon(QIcon(icon));
    result->setAutoRaise(true);
    return result;
}

QScrollArea * TophatSamples::createScrollArea() {
    scrollArea = new QScrollArea(this);
    QWidget *scrollable = new QWidget(scrollArea);
    scrollArea->setWidget(scrollable);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    listLayout = new QVBoxLayout(scrollable);
    listLayout->setContentsMargins(0, 0, 0, 0);
    return scrollArea;
}

QVBoxLayout * TophatSamples::createControlButtonsLayout() {
    QVBoxLayout *result = new QVBoxLayout();
    result->setContentsMargins(0, 0, 0, 0);
    result->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Minimum));
    result->addLayout(createControlButtons());
    result->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Minimum));
    return result;
}

QVBoxLayout * TophatSamples::createControlButtons() {
    QVBoxLayout *result = new QVBoxLayout();
    result->setContentsMargins(0, 0, 0, 0);

    QToolButton *addButton = createButton(this, ":U2Designer/images/add.png");
    upButton = createButton(this, ":U2Designer/images/up.png");
    downButton = createButton(this, ":U2Designer/images/down.png");
    result->addWidget(addButton);
    result->addWidget(upButton);
    result->addWidget(downButton);

    connect(addButton, SIGNAL(clicked()), SLOT(sl_add()));
    connect(upButton, SIGNAL(clicked()), SLOT(sl_up()));
    connect(downButton, SIGNAL(clicked()), SLOT(sl_down()));

    return result;
}

void TophatSamples::updateArrows() {
    SAFE_POINT(order.size() > 1, "Unexpected count of samples", );
    { // check if the top dataset is selected
        QListWidget *topList = getListWidget(0);
        if (topList->selectedItems().size() > 0) {
            QListWidgetItem *topItem = topList->item(0);
            upButton->setEnabled(!topItem->isSelected());
            downButton->setEnabled(true);
            return;
        }
    }

    { // check if the bottom dataset is selected
        QListWidget *bottomList = getListWidget(order.size() - 1);
        if (bottomList->selectedItems().size() > 0) {
            QListWidgetItem *bottomItem = bottomList->item(bottomList->count() - 1);
            upButton->setEnabled(true);
            downButton->setEnabled(!bottomItem->isSelected());
            return;
        }
    }

    // check other items
    for (int pos=1; pos<order.size() - 1; pos++) {
        QListWidget *list = getListWidget(pos);
        if (list->selectedItems().size() > 0) {
            upButton->setEnabled(true);
            downButton->setEnabled(true);
            return;
        }
    }

    // no selected items
    upButton->setEnabled(false);
    downButton->setEnabled(false);
}

} // U2
