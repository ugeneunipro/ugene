/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSequenceWidget.h>

#include "InSilicoPcrTask.h"
#include "Primer.h"
#include "PrimerStatistics.h"

#include "InSilicoPcrProductsTable.h"

namespace U2 {

InSilicoPcrProductsTable::InSilicoPcrProductsTable(QWidget *parent)
: QTableWidget(parent), sequenceContext(NULL)
{
    connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(sl_selectionChanged()));
    connect(this, SIGNAL(clicked(const QModelIndex &)), SLOT(sl_selectionChanged()));
}

void InSilicoPcrProductsTable::replaceContext(ADVSequenceObjectContext *newSequenceContext) {
    if (NULL != sequenceContext) {
        sequenceContext->disconnect(this);
    }
    sequenceContext = newSequenceContext;
}

void InSilicoPcrProductsTable::showProducts(const QList<InSilicoPcrProduct> &products, ADVSequenceObjectContext *newSequenceContext) {
    replaceContext(newSequenceContext);
    setCurrentProducts(products);
}

ADVSequenceObjectContext * InSilicoPcrProductsTable::getCurrentSequenceContext() const {
    return sequenceContext;
}

void InSilicoPcrProductsTable::setCurrentProducts(const QList<InSilicoPcrProduct> &products) {
    currentProducts = products;
    setRowCount(currentProducts.size());

    const qint64 seqLength = sequenceContext->getSequenceLength();
    SAFE_POINT(seqLength > 0, "Invalid sequence length", );
    int row = 0;
    foreach(const InSilicoPcrProduct &product, currentProducts) {
        qint64 startPos = product.region.startPos + 1;
        qint64 endPos = product.region.endPos();
        const int ledge = product.forwardPrimerLedge.size() + product.reversePrimerLedge.size();
        if (endPos > seqLength && endPos != seqLength + ledge) {
            endPos = endPos % seqLength;
        }

        QTableWidgetItem *regionItem = new QTableWidgetItem(QString("%1 - %2").arg(startPos).arg(endPos));
        setItem(row, 0, regionItem);
        setItem(row, 1, new QTableWidgetItem(QString::number(product.region.length)));
        setItem(row, 2, new QTableWidgetItem(product.ta != Primer::INVALID_TM
                ? PrimerStatistics::getDoubleStringValue(product.ta)
                : tr("N/A")));
        row++;
    }
    if (row > 0) {
        setRangeSelected(QTableWidgetSelectionRange(0, 0, 0, columnCount() - 1), true);
    }
}

ADVSequenceObjectContext * InSilicoPcrProductsTable::productsContext() const {
    return sequenceContext;
}

QList<InSilicoPcrProduct> InSilicoPcrProductsTable::getSelectedProducts() const {
    QList<InSilicoPcrProduct> result;
    foreach (const QModelIndex &index, selectedIndexes()) {
        if (index.column() != 0) {
            continue;
        }
        SAFE_POINT(currentProducts.size() > index.row(), "Out of range", result);

        result << currentProducts[index.row()];
    }
    return result;
}

const QList<InSilicoPcrProduct> & InSilicoPcrProductsTable::getAllProducts() const {
    return currentProducts;
}

QVector<U2Region> InSilicoPcrProductsTable::getSelection() const {
    QVector<U2Region> result;
    CHECK(NULL != sequenceContext, result);

    QList<InSilicoPcrProduct> products = getSelectedProducts();
    CHECK(1 == products.size(), result);

    const InSilicoPcrProduct& product = products.first();
    U2Region region = product.region;
    region.length -= (product.forwardPrimerLedge.size() + product.reversePrimerLedge.size());

    qint64 maxLength = sequenceContext->getSequenceLength();
    if (region.endPos() <= maxLength) {
        result << region;
    } else {
        result << U2Region(region.startPos, maxLength - region.startPos);
        result << U2Region(0, region.endPos() - maxLength);
    }
    return result;
}

void InSilicoPcrProductsTable::sl_selectionChanged() {
    CHECK(NULL != sequenceContext, );
    QVector<U2Region> selection = getSelection();
    CHECK(!selection.isEmpty(), );

    sequenceContext->getSequenceSelection()->setSelectedRegions(getSelection());
    foreach (ADVSequenceWidget *sequenceWidget, sequenceContext->getSequenceWidgets()) {
        sequenceWidget->centerPosition(selection.first().startPos);
    }
}

bool InSilicoPcrProductsTable::onSequenceChanged(ADVSequenceObjectContext *changedContext) {
    if (sequenceContext == changedContext) {
        replaceContext(NULL);
        setRowCount(0);
        return true;
    }
    return false;
}

} // U2
