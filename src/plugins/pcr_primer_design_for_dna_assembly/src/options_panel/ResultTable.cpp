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

#include "ResultTable.h"
#include "src/tasks/PCRPrimerDesignForDNAAssemblyTask.h"

namespace U2 {

ResultTable::ResultTable(QWidget *parent)
    : QTableWidget(parent) {
    for (int i = 0; i < MAXIMUM_ROW_COUNT; i++) {
        currentProducts.append(U2Region());
    }
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList() << tr("Fragment") << tr("Region"));
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(sl_selectionChanged()));
    connect(this, SIGNAL(clicked(const QModelIndex &)), SLOT(sl_selectionChanged()));
}

void ResultTable::setCurrentProducts(const QList<U2Region> &_currentProducts, AnnotatedDNAView *_associatedView) {
    SAFE_POINT(_currentProducts.size() == MAXIMUM_ROW_COUNT, "Should be 8 results", );
    currentProducts = _currentProducts;
    int index = 0;
    int row = 0;
    setRowCount(MAXIMUM_ROW_COUNT);
    for (const U2Region &region : qAsConst(_currentProducts)) {
        if (!region.isEmpty()) {
            setItem(row, 0, new QTableWidgetItem(PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME.at(index)));
            setItem(row, 1, new QTableWidgetItem(QString("%1-%2")
                                                     .arg(QString::number(region.startPos + 1))
                                                     .arg(QString::number(region.endPos()))));
            row++;
        }
        index++;
    }
    setRowCount(row);
    associatedView = _associatedView;
}

void ResultTable::setAnnotationGroup(AnnotationGroup *_associatedGroup) {
    associatedGroup = _associatedGroup;
}

Annotation* ResultTable::getSelectedAnnotation() const {
    Annotation *selectedAnnotation = nullptr;
    QModelIndexList selectedIndexesList = selectedIndexes();
    if (selectedIndexesList.isEmpty()) {
        return selectedAnnotation;
    }
    //one row = 2 items
    CHECK(selectedIndexesList.size() == 2, selectedAnnotation);
    QTableWidgetItem *selectedItem = item(selectedIndexesList.first().row(), 0);
    QString selectedFragmentName = selectedItem->text();

    auto annotations = associatedGroup->getAnnotations();
    for (auto a : qAsConst(annotations)) {
        if (a->getName() == selectedFragmentName) {
            selectedAnnotation = a;
            break;
        }
    }
    return selectedAnnotation;
}

ResultTableData ResultTable::getPCRPrimerProductTableData() const {
    ResultTableData data;
    data.associatedGroup = associatedGroup;
    data.associatedView = associatedView;
    data.currentProducts = currentProducts;
    return data;
}

void ResultTable::sl_selectionChanged() {
    Annotation *selectedAnnotation = getSelectedAnnotation();
    if (selectedAnnotation != nullptr) {
        for (ADVSequenceObjectContext *context : qAsConst(associatedView->getSequenceContexts())) {
            if (selectedAnnotation->getGObject() != nullptr && context->getAnnotationObjects(true).contains(selectedAnnotation->getGObject())) {
                context->getAnnotationsSelection()->clear();
                context->getSequenceSelection()->clear();
                context->emitClearSelectedAnnotationRegions();
                context->emitAnnotationActivated(selectedAnnotation, 0);
            }
        }
    }
}

}
