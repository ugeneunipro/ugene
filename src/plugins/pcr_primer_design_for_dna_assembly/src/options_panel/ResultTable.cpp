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
#include <U2View/ADVSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>

#include "ResultTable.h"
#include "src/tasks/PCRPrimerDesignForDNAAssemblyTask.h"

namespace U2 {

ResultTable::ResultTable(QWidget *parent)
    : QTableWidget(parent) {
    for (int i = 0; i < MAXIMUM_ROW_COUNT * 2; i++) {
        currentProducts.append(U2Region());
    }
    const QStringList headerLabels = {tr("Name"), tr("Region"), tr("Length"), tr("Forward"), tr("Reverse")};
    setColumnCount(headerLabels.size());
    setHorizontalHeaderLabels(headerLabels);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(sl_selectionChanged()));
    connect(this, SIGNAL(clicked(const QModelIndex &)), SLOT(sl_selectionChanged()));
}

void ResultTable::setCurrentProducts(const QList<U2Region>& _currentProducts, const QPair<int, int>& _primersLengths, AnnotatedDNAView* _associatedView) {
    SAFE_POINT(_currentProducts.size() == MAXIMUM_ROW_COUNT * 2, "Should be 8 results", );
    currentProducts = _currentProducts;
    primerLengths = _primersLengths;
    int row = 0;
    setRowCount(MAXIMUM_ROW_COUNT);
    for (int i = 0; i < MAXIMUM_ROW_COUNT; i++) {
        if (currentProducts[i * 2].isEmpty() && currentProducts[i * 2 + 1].isEmpty()) {
            continue;
        }
        U2Region forwardPrimerRegion = currentProducts[i * 2];
        U2Region reversePrimerRegion = currentProducts[i * 2 + 1];
        setItem(row, 0, new QTableWidgetItem(PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME.at(i)));
        setItem(row, 1, new QTableWidgetItem(QString("%1-%2").arg(forwardPrimerRegion.startPos + 1).arg(reversePrimerRegion.endPos())));
        setItem(row, 2, new QTableWidgetItem(QString("%1").arg(reversePrimerRegion.endPos() - forwardPrimerRegion.startPos)));
        setItem(row, 3, new QTableWidgetItem(QString("%1-%2").arg(forwardPrimerRegion.startPos + 1).arg(forwardPrimerRegion.endPos())));
        setItem(row, 4, new QTableWidgetItem(QString("%1-%2").arg(reversePrimerRegion.startPos + 1).arg(reversePrimerRegion.endPos())));
        row++;
    }
    setRowCount(row);
    associatedView = _associatedView;
}

void ResultTable::setAnnotationGroup(AnnotationGroup *_associatedGroup) {
    associatedGroup = _associatedGroup;
}

QList<QPair<QString, U2Region> > ResultTable::getSelectedFragment(FragmentLocation location) const {
    QList<QPair<QString, U2Region> > fragmentList;
    QModelIndexList selectedIndexesList = selectedIndexes();
    if (selectedIndexesList.isEmpty()) {
        return fragmentList;
    }
    //one row = 5 items
    CHECK(selectedIndexesList.size() == 5, fragmentList);
    QTableWidgetItem *selectedItem = item(selectedIndexesList.first().row(), 0);
    QString selectedFragmentName = selectedItem->text();
    auto annotations = associatedGroup->getAnnotations();
    if (location == Whole) {
        for (auto a : qAsConst(annotations)) {
            if (a->getName() == QString(selectedFragmentName + " Forward")) {
                QPair<QString, U2Region> fragmentProduct;
                fragmentProduct.first = selectedFragmentName;
                fragmentProduct.second.startPos = a->getRegions().first().startPos;
                fragmentProduct.second.length = item(selectedIndexesList.first().row(), 2)->text().toInt();
                QPair<QString, U2Region> fragmentForward;
                fragmentForward.first = selectedFragmentName + " Forward";
                fragmentForward.second = a->getRegions().first();
                fragmentList << fragmentProduct << fragmentForward;
            } else if (a->getName() == QString(selectedFragmentName + " Reverse")) {
                QPair<QString, U2Region> fragmentReverse;
                fragmentReverse.first = selectedFragmentName + " Forward";
                fragmentReverse.second = a->getRegions().first();
                fragmentList << fragmentReverse;
            }
        }
    } else {
        selectedFragmentName += location == Forward ? " Forward" : " Reverse";
        QPair<QString, U2Region>  fragment;
        fragment.first = selectedFragmentName;
        for (auto a : qAsConst(annotations)) {
            if (a->getName() == selectedFragmentName) {
                fragment.second = a->getRegions().first();
                break;
            }
        }
        fragmentList << fragment;
    }
    return fragmentList;
}

ResultTableData ResultTable::getPCRPrimerProductTableData() const {
    ResultTableData data;
    data.associatedGroup = associatedGroup;
    data.associatedView = associatedView;
    data.currentProducts = currentProducts;
    data.primerLengths = primerLengths;
    return data;
}

void ResultTable::sl_selectionChanged() {
    QList<QPair<QString, U2Region>> selectedResultList = getSelectedFragment(ResultTable::Whole);
    if (selectedResultList.isEmpty()) {
        return;
    }
    QPair<QString, U2Region> location = selectedResultList.first();
    if (location != QPair<QString, U2Region>()) {
        for (ADVSequenceObjectContext *context : qAsConst(associatedView->getSequenceContexts())) {
            if (context->getAnnotationObjects(true).contains(associatedGroup->getAnnotations().first()->getGObject())) {
                context->getSequenceSelection()->setRegion(location.second);
                for (ADVSequenceWidget* view : qAsConst(context->getSequenceWidgets())) {
                    if (!view->getVisibleRange().intersects(location.second)) {
                        view->setVisibleRange(location.second);
                    }
                }
            }
        }
    }
}

}
