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

#include "MsaEditorSortSequencesWidget.h"

#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <U2View/MSAEditor.h>

namespace U2 {

MsaEditorSortSequencesWidget::MsaEditorSortSequencesWidget(QWidget* parent, MSAEditor* msaEditor)
    : QWidget(parent), msaEditor(msaEditor) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto sortByLabel = new QLabel();
    sortByLabel->setText(tr("Sort by"));
    layout->addWidget(sortByLabel);

    sortByCombo = new QComboBox();
    sortByCombo->setObjectName("sortByComboBox");
    sortByCombo->addItem(tr("Name"), MultipleAlignment::SortByName);
    sortByCombo->addItem(tr("Length"), MultipleAlignment::SortByLength);
    sortByCombo->addItem(tr("Leading gap"), MultipleAlignment::SortByLeadingGap);
    layout->addWidget(sortByCombo);

    auto sortOrderLabel = new QLabel();
    sortOrderLabel->setText(tr("Sort order"));
    layout->addWidget(sortOrderLabel);

    sortOrderCombo = new QComboBox();
    sortOrderCombo->setObjectName("sortOrderComboBox");
    sortOrderCombo->addItem(tr("Ascending"), MultipleAlignment::Ascending);
    sortOrderCombo->addItem(tr("Descending"), MultipleAlignment::Descending);
    layout->addWidget(sortOrderCombo);

    auto lastRowLayout = new QHBoxLayout();
    layout->addLayout(lastRowLayout);
    lastRowLayout->addStretch(1);

    sortButton = new QPushButton(tr("Sort"));
    sortButton->setObjectName("sortButton");
    sortButton->setToolTip(tr("Click to sort selected sequences range or the whole alignment"));
    lastRowLayout->addWidget(sortButton);
    connect(sortButton, SIGNAL(clicked()), SLOT(sl_sortClicked()));

    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    sortButton->setEnabled(!msaObject->isStateLocked());
    connect(msaObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_msaObjectStateChanged()));
}

void MsaEditorSortSequencesWidget::sl_sortClicked() {
    auto sortType = (MultipleAlignment::SortType)sortByCombo->currentData().toInt();
    auto sortOrder = (MultipleAlignment::Order)sortOrderCombo->currentData().toInt();
    msaEditor->sortSequences(sortType, sortOrder);
}

void MsaEditorSortSequencesWidget::sl_msaObjectStateChanged() {
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    sortButton->setEnabled(!msaObject->isStateLocked());
}

}  // namespace U2
