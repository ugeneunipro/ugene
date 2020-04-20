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
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/SaveDocumentController.h>

#include "ConstructMoleculeDialog.h"
#include "CreateFragmentDialog.h"
#include "EditFragmentDialog.h"

// This is maximum sequence size we allow to use with construct molecule task on 32-bit OSes: 300Mb.
// UGENE has 2Gb limitation on 32-bit systems and cloning algorithm is not optimized for memory limits.
#define MAX_MOLECULE_SEQUENCE_LENGTH_32_BIT_OS (300 * 1000 * 1000)

namespace U2 {

ConstructMoleculeDialog::ConstructMoleculeDialog(const QList<DNAFragment>& fragmentList, QWidget* p)
    : QDialog(p),
    fragments(fragmentList),
    saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748961");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    tabWidget->setCurrentIndex(0);
    QString coreLengthStr = ConstructMoleculeDialog::tr("core length");

    foreach(const DNAFragment& frag, fragments) {
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
    molConstructWidget->setColumnWidth(1, molConstructWidget->width()*0.5);

    connect(fragmentListWidget, SIGNAL( itemDoubleClicked ( QListWidgetItem* ) ),  SLOT(sl_onTakeButtonClicked()));
    connect(takeButton, SIGNAL(clicked()), SLOT(sl_onTakeButtonClicked()));
    connect(takeAllButton, SIGNAL(clicked()), SLOT(sl_onTakeAllButtonClicked()));
    connect(fromProjectButton, SIGNAL(clicked()), SLOT(sl_onAddFromProjectButtonClicked()));
    connect(clearButton, SIGNAL(clicked()), SLOT(sl_onClearButtonClicked()));
    connect(upButton, SIGNAL(clicked()), SLOT(sl_onUpButtonClicked()) );
    connect(downButton, SIGNAL(clicked()), SLOT(sl_onDownButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(sl_onRemoveButtonClicked()));
    connect(makeCircularBox, SIGNAL(clicked()), SLOT(sl_makeCircularBoxClicked()));
    connect(makeBluntBox, SIGNAL(clicked()), SLOT(sl_forceBluntBoxClicked()) );
    connect(editFragmentButton, SIGNAL(clicked()), SLOT(sl_onEditFragmentButtonClicked()));
    connect(molConstructWidget, SIGNAL(	itemClicked ( QTreeWidgetItem *, int)), SLOT(sl_onItemClicked(QTreeWidgetItem *, int)) );

    molConstructWidget->installEventFilter(this);
}

void ConstructMoleculeDialog::accept()
{
    if (selected.isEmpty()) {
        QMessageBox::information(this, windowTitle(), tr("No fragments are selected!\n Please construct molecule from available fragments."));
        return;
    }

    QList<DNAFragment> toLigate;
    qint64 resultSequenceSize = 0;
    foreach(int idx, selected) {
        const DNAFragment& fragment = fragments[idx];
        foreach (const U2Region& region, fragment.getFragmentRegions()) {
            resultSequenceSize += region.length;
        }
        toLigate.append(fragment);
    }
    if (AppResourcePool::is32BitBuild() && resultSequenceSize > MAX_MOLECULE_SEQUENCE_LENGTH_32_BIT_OS) {
        QMessageBox::warning(this->window(), L10N::warningTitle(),  tr("Selected region is too large to proceed!"));
        return;
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

void ConstructMoleculeDialog::sl_onTakeButtonClicked()
{
    QList<QListWidgetItem*> items = fragmentListWidget->selectedItems();

    foreach (QListWidgetItem* item, items) {
        int curRow = fragmentListWidget->row(item);
        if (!selected.contains(curRow)) {
            selected.append(curRow);
        }
    }

    update();

}

void ConstructMoleculeDialog::sl_onTakeAllButtonClicked()
{
    selected.clear();
    int count = fragmentListWidget->count();

    for (int i = 0; i < count; ++i) {
        selected.append(i);
    }
    update();
}

void ConstructMoleculeDialog::sl_onClearButtonClicked()
{
    selected.clear();
    update();
}

void ConstructMoleculeDialog::sl_onUpButtonClicked()
{
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == NULL || selected.size() == 1) {
        return;
    }

    int index = molConstructWidget->indexOfTopLevelItem(item);
    int newIndex = index - 1 == -1 ? selected.size() - 1 : index - 1;

    qSwap(selected[index], selected[newIndex]);

    update();

    molConstructWidget->setCurrentItem(molConstructWidget->topLevelItem(newIndex), true);

}

void ConstructMoleculeDialog::sl_onDownButtonClicked()
{
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == NULL || selected.size() == 1) {
        return;
    }

    int index = molConstructWidget->indexOfTopLevelItem(item);
    int newIndex = index + 1 == selected.count() ? 0 : index + 1;

    qSwap(selected[index], selected[newIndex]);

    update();

    molConstructWidget->setCurrentItem(molConstructWidget->topLevelItem(newIndex), true);

}

void ConstructMoleculeDialog::sl_onRemoveButtonClicked()
{
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == NULL) {
        return;
    }
    int index = molConstructWidget->indexOfTopLevelItem(item);
    selected.removeAt(index);

    update();
}

void ConstructMoleculeDialog::update()
{
    molConstructWidget->clear();

    foreach (int index, selected ) {
        QListWidgetItem* item = fragmentListWidget->item(index);
        assert(item != NULL);
        if (item != NULL) {
            QTreeWidgetItem* newItem = new QTreeWidgetItem(molConstructWidget);
            const DNAFragment& fragment = fragments.at(index);
            if (fragment.getLeftTerminus().type == OVERHANG_TYPE_STICKY ){
                newItem->setText(0, QString("%1 (%2)")
                    .arg(QString(fragment.getLeftTerminus().overhang))
                    .arg(fragment.getLeftTerminus().isDirect ? tr("Fwd") : tr("Rev")) );
            } else {
                newItem->setText(0, tr("Blunt"));
            }
            newItem->setToolTip(0, tr("Left end"));
            newItem->setText(1, item->text());
            if (fragment.getRightTerminus().type == OVERHANG_TYPE_STICKY ) {
                newItem->setText(2, QString("%1 (%2)")
                    .arg( QString(fragment.getRightTerminus().overhang) )
                    .arg( fragment.getRightTerminus().isDirect ? tr("Fwd") : tr("Rev") ) );
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
        QTreeWidgetItem* prevItem = NULL;
        int count = molConstructWidget->topLevelItemCount();
        for(int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = molConstructWidget->topLevelItem(i);
            if (prevItem != NULL) {

                QStringList prevItems = prevItem->text(2).split(" ");
                QString prevOverhang = prevItems.at(0);
                QString prevStrand = prevItems.count() > 1 ? prevItems.at(1) : QString();
                QStringList items = item->text(0).split(" ");
                QString overhang = items.at(0);
                QString strand =  items.count() > 1 ? items.at(1) : QString();

                QColor color = ( prevOverhang == overhang && strand != prevStrand ) ? Qt::green : Qt::red;

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

            QString  firstOverhang = firstItems.count() > 0 ? firstItems.at(0) : QString();
            QString lastOverhang = lastItems.count() > 0 ? lastItems.at(0) : QString();

            QString firstStrand = firstItems.count() > 1 ? firstItems.at(1) : QString();
            QString lastStrand = lastItems.count() > 1 ? lastItems.at(1) : QString();

            QColor color = ( firstOverhang == lastOverhang && (firstStrand != lastStrand || (firstStrand.isEmpty() && lastStrand.isEmpty())) ) ? Qt::green : Qt::red;

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

void ConstructMoleculeDialog::sl_makeCircularBoxClicked()
{
    update();
}

void ConstructMoleculeDialog::sl_forceBluntBoxClicked()
{
    update();
}

void ConstructMoleculeDialog::sl_onEditFragmentButtonClicked()
{
    QTreeWidgetItem* item = molConstructWidget->currentItem();
    if (item == NULL) {
        return;
    }

    int idx = molConstructWidget->indexOfTopLevelItem(item);
    DNAFragment& fragment = fragments[ selected[idx] ];

    QObjectScopedPointer<EditFragmentDialog> dlg = new EditFragmentDialog(fragment, this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Rejected) {
        return;
    }

    update();
}

bool ConstructMoleculeDialog::eventFilter( QObject* obj , QEvent* event )
{
    if (obj == molConstructWidget && event->type() == QEvent::FocusOut) {
        molConstructWidget->clearSelection();
    }

    return QDialog::eventFilter(obj, event);

}

void ConstructMoleculeDialog::sl_onItemClicked( QTreeWidgetItem * item, int column )
{
    if (column == 3) {
        int idx = molConstructWidget->indexOfTopLevelItem(item);
        DNAFragment& fragment = fragments[ selected[idx] ];
        if (item->checkState(column) == Qt::Checked) {
           fragment.setInverted(true);
        } else {
            fragment.setInverted(false);
        }
        update();
    }
}

void ConstructMoleculeDialog::sl_onAddFromProjectButtonClicked()
{
    ProjectTreeControllerModeSettings settings;
    settings.objectTypesToShow.insert(GObjectTypes::SEQUENCE);
    QScopedPointer<U2SequenceObjectConstraints> seqConstraints(new U2SequenceObjectConstraints());
    seqConstraints->alphabetType = DNAAlphabet_NUCL;
    settings.objectConstraints.insert(seqConstraints.data());

    QList<GObject*> objects = ProjectTreeItemSelectorDialog::selectObjects(settings,this);

    if (!objects.isEmpty()) {
        foreach(GObject* obj, objects) {
            if (obj->isUnloaded()) {
                continue;
            }
            U2SequenceObject* seqObj = qobject_cast<U2SequenceObject*>(obj);

            if (seqObj) {
                QObjectScopedPointer<CreateFragmentDialog> dlg = new CreateFragmentDialog(seqObj, U2Region(0, seqObj->getSequenceLength()), this);
                dlg->exec();
                CHECK(!dlg.isNull(), );

                if (dlg->result() == QDialog::Accepted) {
                    DNAFragment frag = dlg->getFragment();
                        QString fragItem = QString("%1 (%2) %3").arg(frag.getSequenceName())
                        .arg(frag.getSequenceDocName())
                        .arg(frag.getName());
                    fragments.append(frag);
                    fragmentListWidget->addItem(fragItem);
                    break;
                }
            }
        }
    }
}


} // U2
