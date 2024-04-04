/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "ConstructMoleculeDialog.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/SaveDocumentController.h>

#include "CreateFragmentDialog.h"
#include "EditFragmentDialog.h"

// This is maximum sequence size we allow to use with construct molecule task on 32-bit OSes: 300Mb.
// UGENE has 2Gb limitation on 32-bit systems and cloning algorithm is not optimized for memory limits.
#define MAX_MOLECULE_SEQUENCE_LENGTH_32_BIT_OS (300 * 1000 * 1000)

namespace U2 {

ConstructMoleculeDialog::ConstructMoleculeDialog(const QList<DNAFragment>& fragmentList, QWidget* p)
    : QDialog(p),
      fragments(fragmentList),
      saveController(nullptr) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930763");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    tabWidget->setCurrentIndex(0);
    QString coreLengthStr = ConstructMoleculeDialog::tr("core length");

    foreach (const DNAFragment& frag, fragments) {
        QString fragItem = QString("%1 (%2) %3 [%4 - %5 bp]")
                               .arg(frag.getSequenceName())
                               .arg(frag.getSequenceDocName())
                               .arg(frag.getName())
                               .arg(coreLengthStr)
                               .arg(frag.getLength());
        fragmentListWidget->addItem(fragItem);
    }

    initSaveController();

    fragmentListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    molConstructWidget->setColumnWidth(1, molConstructWidget->width() * 0.5);

    connect(fragmentListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(sl_onTakeButtonClicked()));
    connect(takeButton, SIGNAL(clicked()), SLOT(sl_onTakeButtonClicked()));
    connect(takeAllButton, SIGNAL(clicked()), SLOT(sl_onTakeAllButtonClicked()));
    connect(fromProjectButton, SIGNAL(clicked()), SLOT(sl_onAddFromProjectButtonClicked()));
    connect(clearButton, SIGNAL(clicked()), SLOT(sl_onClearButtonClicked()));
    connect(upButton, SIGNAL(clicked()), SLOT(sl_onUpButtonClicked()));
    connect(downButton, SIGNAL(clicked()), SLOT(sl_onDownButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(sl_onRemoveButtonClicked()));
    connect(makeCircularBox, SIGNAL(clicked()), SLOT(sl_makeCircularBoxClicked()));
    connect(makeBluntBox, SIGNAL(clicked()), SLOT(sl_forceBluntBoxClicked()));
    connect(editFragmentButton, SIGNAL(clicked()), SLOT(sl_onEditFragmentButtonClicked()));
    connect(molConstructWidget, &QTreeWidget::itemDoubleClicked, this, &ConstructMoleculeDialog::sl_onEditFragmentButtonClicked);
    connect(molConstructWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(sl_onItemClicked(QTreeWidgetItem*, int)));
    connect(tbAdjustLeft, &QToolButton::clicked, this, &ConstructMoleculeDialog::sl_adjustLeftEnd);
    connect(tbAdjustRight, &QToolButton::clicked, this, &ConstructMoleculeDialog::sl_adjustRightEnd);

    molConstructWidget->installEventFilter(this);
}

void ConstructMoleculeDialog::accept() {
    if (selected.isEmpty()) {
        QMessageBox::information(this, windowTitle(), tr("No fragments are selected!\n Please construct molecule from available fragments."));
        return;
    }

    QList<DNAFragment> toLigate;
    foreach (int idx, selected) {
        const DNAFragment& fragment = fragments[idx];
        toLigate.append(fragment);
    }
    LigateFragmentsTaskConfig cfg;
    cfg.checkOverhangs = !makeBluntBox->isChecked();
    cfg.makeCircular = makeCircularBox->isChecked();
    cfg.docUrl = saveController->getSaveFileName();
    cfg.openView = openViewBox->isChecked();
    cfg.saveDoc = saveImmediatlyBox->isChecked();
    cfg.annotateFragments = annotateFragmentsBox->isChecked();

    Task* task = new LigateFragmentsTask(toLigate, cfg);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);

    QDialog::accept();
}

void ConstructMoleculeDialog::sl_onTakeButtonClicked() {
    QList<QListWidgetItem*> items = fragmentListWidget->selectedItems();

    foreach (QListWidgetItem* item, items) {
        int curRow = fragmentListWidget->row(item);
        if (!selected.contains(curRow)) {
            selected.append(curRow);
        }
    }

    update();
}

void ConstructMoleculeDialog::sl_onTakeAllButtonClicked() {
    selected.clear();
    int count = fragmentListWidget->count();

    for (int i = 0; i < count; ++i) {
        selected.append(i);
    }
    update();
}

void ConstructMoleculeDialog::sl_onClearButtonClicked() {
    selected.clear();
    update();
}

void ConstructMoleculeDialog::sl_onUpButtonClicked() {
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == nullptr || selected.size() == 1) {
        return;
    }

    int index = molConstructWidget->indexOfTopLevelItem(item);
    int newIndex = index - 1 == -1 ? selected.size() - 1 : index - 1;

    qSwap(selected[index], selected[newIndex]);

    update();

    molConstructWidget->setCurrentItem(molConstructWidget->topLevelItem(newIndex), true);
}

void ConstructMoleculeDialog::sl_onDownButtonClicked() {
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == nullptr || selected.size() == 1) {
        return;
    }

    int index = molConstructWidget->indexOfTopLevelItem(item);
    int newIndex = index + 1 == selected.count() ? 0 : index + 1;

    qSwap(selected[index], selected[newIndex]);

    update();

    molConstructWidget->setCurrentItem(molConstructWidget->topLevelItem(newIndex), true);
}

void ConstructMoleculeDialog::sl_onRemoveButtonClicked() {
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == nullptr) {
        return;
    }
    int index = molConstructWidget->indexOfTopLevelItem(item);
    selected.removeAt(index);

    update();
}

void ConstructMoleculeDialog::update() {
    molConstructWidget->clear();

    foreach (int index, selected) {
        QListWidgetItem* item = fragmentListWidget->item(index);
        assert(item != nullptr);
        if (item != nullptr) {
            auto newItem = new QTreeWidgetItem(molConstructWidget);
            const DNAFragment& fragment = fragments.at(index);
            if (fragment.getLeftTerminus().type == OVERHANG_TYPE_STICKY) {
                newItem->setText(0, QString("%1 (%2)").arg(QString(fragment.getLeftTerminus().overhang)).arg(fragment.getLeftTerminus().isDirect ? tr("Fwd") : tr("Rev")));
            } else {
                newItem->setText(0, tr("Blunt"));
            }
            newItem->setToolTip(0, tr("Left end"));
            newItem->setText(1, item->text());
            if (fragment.getRightTerminus().type == OVERHANG_TYPE_STICKY) {
                newItem->setText(2, QString("%1 (%2)").arg(QString(fragment.getRightTerminus().overhang)).arg(fragment.getRightTerminus().isDirect ? tr("Fwd") : tr("Rev")));
            } else {
                newItem->setText(2, tr("Blunt"));
            }
            newItem->setToolTip(2, tr("Right end"));
            newItem->setCheckState(3, fragment.isInverted() ? Qt::Checked : Qt::Unchecked);
            newItem->setText(3, fragment.isInverted() ? tr("yes") : tr("no"));
            newItem->setToolTip(3, tr("Make fragment reverse complement"));

            molConstructWidget->addTopLevelItem(newItem);
        }
    }

    bool checkTermsConsistency = !makeBluntBox->isChecked();

    if (checkTermsConsistency) {
        QTreeWidgetItem* prevItem = nullptr;
        int count = molConstructWidget->topLevelItemCount();
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = molConstructWidget->topLevelItem(i);
            if (prevItem != nullptr) {
                QStringList prevItems = prevItem->text(2).split(" ");
                QString prevOverhang = prevItems.at(0);
                QString prevStrand = prevItems.count() > 1 ? prevItems.at(1) : QString();
                QStringList items = item->text(0).split(" ");
                QString overhang = items.at(0);
                QString strand = items.count() > 1 ? items.at(1) : QString();

                bool equaledOverhangs = prevOverhang == overhang;
                bool differentStrands = strand != prevStrand;
                bool noStrands = strand.isEmpty() && prevStrand.isEmpty();
                QColor color = (equaledOverhangs && (differentStrands || noStrands)) ? Qt::green : Qt::red;

                prevItem->setTextColor(2, color);
                item->setTextColor(0, color);
            }
            prevItem = item;
        }
        if (makeCircularBox->isChecked() && count > 0) {
            QTreeWidgetItem* first = molConstructWidget->topLevelItem(0);
            QTreeWidgetItem* last = molConstructWidget->topLevelItem(count - 1);

            QStringList firstItems = first->text(0).split(" ");
            QStringList lastItems = last->text(2).split(" ");

            QString firstOverhang = firstItems.count() > 0 ? firstItems.at(0) : QString();
            QString lastOverhang = lastItems.count() > 0 ? lastItems.at(0) : QString();

            QString firstStrand = firstItems.count() > 1 ? firstItems.at(1) : QString();
            QString lastStrand = lastItems.count() > 1 ? lastItems.at(1) : QString();

            QColor color = (firstOverhang == lastOverhang && (firstStrand != lastStrand || (firstStrand.isEmpty() && lastStrand.isEmpty()))) ? Qt::green : Qt::red;

            first->setTextColor(0, color);
            last->setTextColor(2, color);
        }
    }
}

void ConstructMoleculeDialog::initSaveController() {
    LastUsedDirHelper lod;

    SaveDocumentControllerConfig config;
    config.defaultFileName = lod.dir + "/new_mol.gb";
    config.defaultFormatId = BaseDocumentFormats::PLAIN_GENBANK;
    config.fileDialogButton = browseButton;
    config.fileNameEdit = filePathEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Set new molecule file name");
    config.rollOutProjectUrls = true;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::PLAIN_GENBANK;

    saveController = new SaveDocumentController(config, formats, this);
}

const QString ConstructMoleculeDialog::createEndSign(const DNAFragmentTerm& term) {
    QString result;
    if (term.type == OVERHANG_TYPE_STICKY) {
        result = QString("%1 (%2)").arg(QString(term.overhang)).arg(term.isDirect ? tr("Fwd") : tr("Rev"));
    } else {
        result = tr("Blunt");
    }

    return result;
}

void ConstructMoleculeDialog::sl_makeCircularBoxClicked() {
    update();
}

void ConstructMoleculeDialog::sl_forceBluntBoxClicked() {
    update();
}

void ConstructMoleculeDialog::sl_onEditFragmentButtonClicked() {
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == nullptr) {
        return;
    }

    int idx = molConstructWidget->indexOfTopLevelItem(item);
    DNAFragment& fragment = fragments[selected[idx]];

    QObjectScopedPointer<EditFragmentDialog> dlg = new EditFragmentDialog(fragment, this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Rejected) {
        return;
    }

    update();
}

bool ConstructMoleculeDialog::eventFilter(QObject* obj, QEvent* event) {
    if (obj == molConstructWidget) {
        switch (event->type()) {
        case QEvent::FocusOut:
            molConstructWidget->clearSelection();
            break;
        case QEvent::KeyPress:
            QKeyEvent* ke = (QKeyEvent*)event;
            if (ke->key() == Qt::Key_Delete) {
                sl_onRemoveButtonClicked();
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}

void ConstructMoleculeDialog::sl_onItemClicked(QTreeWidgetItem* item, int column) {
    static constexpr int INVERTED_COLUMN = 3;

    bool adjustLeftIsActive = true;
    bool adjustRightIsActive = true;
    if (column == INVERTED_COLUMN) {
        int idx = molConstructWidget->indexOfTopLevelItem(item);
        DNAFragment& fragment = fragments[selected[idx]];
        if (item->checkState(column) == Qt::Checked) {
            fragment.setInverted(true);
        } else {
            fragment.setInverted(false);
        }
        update();
    }
    if (molConstructWidget->itemAbove(item) == nullptr) {
        adjustLeftIsActive = makeCircularBox->isChecked();
    }
    if (molConstructWidget->itemBelow(item) == nullptr) {
        adjustRightIsActive = makeCircularBox->isChecked();
    }

    tbAdjustLeft->setEnabled(adjustLeftIsActive);
    tbAdjustRight->setEnabled(adjustRightIsActive);
}

static constexpr int LEFT_END_COLUMN = 0;
static constexpr int RIGHT_END_COLUMN = 2;

void ConstructMoleculeDialog::sl_adjustLeftEnd() {
    auto selectedItem = molConstructWidget->currentItem();
    SAFE_POINT_NN(selectedItem, );

    int idx = molConstructWidget->indexOfTopLevelItem(selectedItem);
    DNAFragment& fragment = fragments[selected[idx]];

    auto itemAbove = molConstructWidget->itemAbove(selectedItem);
    if (itemAbove == nullptr) {
        SAFE_POINT(makeCircularBox->isChecked(), "Should be circular", );

        auto downItem = selectedItem;
        while (molConstructWidget->itemBelow(downItem) != nullptr) {
            downItem = molConstructWidget->itemBelow(downItem);
        }
        itemAbove = downItem;
    }
    SAFE_POINT_NN(itemAbove, );

    int idx2 = molConstructWidget->indexOfTopLevelItem(itemAbove);
    const DNAFragment& fragmentAbove = fragments[selected[idx2]];
    const auto& rightTerm = fragmentAbove.getRightTerminus();
    QByteArray overhang;
    if (rightTerm.type == OVERHANG_TYPE_STICKY) {
        overhang = rightTerm.overhang;
    }
    fragment.setLeftOverhang(overhang);
    fragment.setLeftTermType(overhang.isEmpty() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY);
    fragment.setLeftOverhangStrand(!rightTerm.isDirect);

    selectedItem->setText(LEFT_END_COLUMN, createEndSign(fragment.getLeftTerminus()));
    selectedItem->setTextColor(LEFT_END_COLUMN, Qt::green);
    itemAbove->setTextColor(RIGHT_END_COLUMN, Qt::green);
}

void ConstructMoleculeDialog::sl_adjustRightEnd() {
    auto selectedItem = molConstructWidget->currentItem();
    SAFE_POINT_NN(selectedItem, );

    int idx = molConstructWidget->indexOfTopLevelItem(selectedItem);
    DNAFragment& fragment = fragments[selected[idx]];

    auto itemBelow = molConstructWidget->itemBelow(selectedItem);
    if (itemBelow == nullptr) {
        SAFE_POINT(makeCircularBox->isChecked(), "Should be circular", );

        auto topItem = selectedItem;
        while (molConstructWidget->itemAbove(topItem) != nullptr) {
            topItem = molConstructWidget->itemAbove(topItem);
        }
        itemBelow = topItem;
    }
    SAFE_POINT_NN(itemBelow, );

    int idx2 = molConstructWidget->indexOfTopLevelItem(itemBelow);
    const DNAFragment& fragmentBelow = fragments[selected[idx2]];
    const auto& leftTerm = fragmentBelow.getLeftTerminus();
    QByteArray overhang;
    if (leftTerm.type == OVERHANG_TYPE_STICKY) {
        overhang = leftTerm.overhang;
        if (leftTerm.isDirect) {
            overhang = DNASequenceUtils::reverseComplement(overhang, U2AlphabetUtils::findBestAlphabet(overhang));
        }
    }
    fragment.setRightOverhang(overhang);
    fragment.setRightTermType(overhang.isEmpty() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY);
    fragment.setRightOverhangStrand(!leftTerm.isDirect);

    selectedItem->setText(RIGHT_END_COLUMN, createEndSign(fragment.getRightTerminus()));
    selectedItem->setTextColor(RIGHT_END_COLUMN, Qt::green);
    itemBelow->setTextColor(LEFT_END_COLUMN, Qt::green);
}

void ConstructMoleculeDialog::sl_onAddFromProjectButtonClicked() {
    ProjectTreeControllerModeSettings settings;
    settings.allowMultipleSelection = false;
    settings.objectTypesToShow.insert(GObjectTypes::SEQUENCE);
    QScopedPointer<U2SequenceObjectConstraints> seqConstraints(new U2SequenceObjectConstraints());
    seqConstraints->alphabetType = DNAAlphabet_NUCL;
    settings.objectConstraints.insert(seqConstraints.data());
    QList<GObject*> objects = ProjectTreeItemSelectorDialog::selectObjects(settings, this);
    CHECK(!objects.isEmpty(), );
    SAFE_POINT(objects.size() == 1, QString("Incorrect selected objects size, expected: 1, current: %1").arg(objects.size()), );

    auto obj = objects.first();
    CHECK(!obj->isUnloaded(), );

    auto seqObj = qobject_cast<U2SequenceObject*>(obj);
    CHECK(seqObj != nullptr, );

    QObjectScopedPointer<CreateFragmentDialog> dlg = new CreateFragmentDialog(seqObj, U2Region(0, seqObj->getSequenceLength()), this);
    dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(dlg->result() == QDialog::Accepted, );

    DNAFragment frag = dlg->getFragment();
    QString fragItem = QString("%1 (%2) %3").arg(frag.getSequenceName()).arg(frag.getSequenceDocName()).arg(frag.getName());
    fragments.append(frag);
    fragmentListWidget->addItem(fragItem);
}

}  // namespace U2
