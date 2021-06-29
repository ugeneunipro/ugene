/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "PCRPrimerProductTable.h"
#include "src/tasks/PCRPrimerDesignForDNAAssemblyTask.h"

namespace U2 {

 PCRPrimerProductTable::PCRPrimerProductTable(QWidget *parent)
    : QTableWidget(parent) {
     setColumnCount(2);
     setHorizontalHeaderLabels(QStringList() << tr("Fragment") << tr("Region"));
     setSelectionBehavior(QAbstractItemView::SelectRows);
     setSelectionMode(QAbstractItemView::SingleSelection);
     connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(sl_selectionChanged()));
     connect(this, SIGNAL(clicked(const QModelIndex &)), SLOT(sl_selectionChanged()));
 }

void PCRPrimerProductTable::setCurrentProducts(const QList<U2Region> &currentProducts, AnnotatedDNAView *_associatedView) {
    SAFE_POINT(currentProducts.size() == MAXIMUM_ROW_COUNT, "Should be 8 results", );
    int index = 0;
    int row = 0;
    setRowCount(MAXIMUM_ROW_COUNT);
    for (const U2Region &region : currentProducts) {
        if (region != U2Region()) {
            setItem(row, 0, new QTableWidgetItem(PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME.at(index)));
            setItem(row, 1, new QTableWidgetItem(tr("%1-%2").arg(QString::number(region.startPos)).arg(QString::number(region.endPos()))));
            row++;
        }
        index++;
    }
    setRowCount(row);
    associatedView = _associatedView;
}

void PCRPrimerProductTable::setAnnotationTableObject(AnnotationTableObject *ato) {
    associatedTableObject = ato;
}

void PCRPrimerProductTable::sl_selectionChanged() {
    QModelIndexList selectedIndexesList = selectedIndexes();
    if (selectedIndexesList.isEmpty()) {
        return;
    }
    //one row = 2 items
    CHECK(selectedIndexesList.size() == 2, );
    QTableWidgetItem *selectedItem = item(selectedIndexesList.first().row(), 0);
    QString selectedFragmentName = selectedItem->text();
    
    Annotation *selectedAnnotation = nullptr;
    auto annotations = associatedTableObject->getAnnotations();
    for (auto a : qAsConst(annotations)) {
        if (a->getName() == selectedFragmentName) {
            selectedAnnotation = a;
            break;
        }
    }
    CHECK(selectedAnnotation != nullptr, );
    for (ADVSequenceObjectContext *context : associatedView->getSequenceContexts()) {
        context->getAnnotationsSelection()->clear();
        context->getSequenceSelection()->clear();
        context->emitClearSelectedAnnotationRegions();
        context->emitAnnotationActivated(selectedAnnotation, 0);
    }
}

}