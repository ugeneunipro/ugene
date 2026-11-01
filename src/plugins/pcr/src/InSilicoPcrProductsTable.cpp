/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "InSilicoPcrProductsTable.h"

#include <QAbstractTableModel>
#include <QCoreApplication>
#include <QHeaderView>

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>
#include <U2Core/Primer.h>
#include <U2Core/PrimerStatistics.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSequenceWidget.h>

#include "InSilicoPcrTask.h"

namespace U2 {

class InSilicoPcrProductsModel : public QAbstractTableModel {
public:
    explicit InSilicoPcrProductsModel(QObject* parent)
        : QAbstractTableModel(parent) {
    }

    int rowCount(const QModelIndex& parent = {}) const override {
        return parent.isValid() ? 0 : products.size();
    }

    int columnCount(const QModelIndex& parent = {}) const override {
        return parent.isValid() ? 0 : 3;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole || !index.isValid() || index.row() >= products.size()) {
            return {};
        }
        const auto& product = products.at(index.row());
        switch (index.column()) {
            case 0: {
                qint64 end = product.region.endPos();
                const int ledge = product.forwardPrimerLedge.size() + product.reversePrimerLedge.size();
                if (end > sequenceLength && end != sequenceLength + ledge) {
                    end %= sequenceLength;
                }
                return QString("%1 - %2").arg(product.region.startPos + 1).arg(end);
            }
            case 1:
                return QString::number(product.region.length);
            case 2:
                return product.ta != TmCalculator::INVALID_TM ? PrimerStatistics::getDoubleStringValue(product.ta) : InSilicoPcrProductsTable::tr("N/A");
            default:
                return {};
        }
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole || section < 0 || section >= 3) {
            return {};
        }
        const char* headers[] = {QT_TRANSLATE_NOOP("InSilicoPcrOptionPanelWidget", "Region"),
                                 QT_TRANSLATE_NOOP("InSilicoPcrOptionPanelWidget", "Length"),
                                 QT_TRANSLATE_NOOP("InSilicoPcrOptionPanelWidget", "Ta")};
        return QCoreApplication::translate("InSilicoPcrOptionPanelWidget", headers[section]);
    }

    void setProducts(const QList<InSilicoPcrProduct>& newProducts, qint64 newSequenceLength) {
        beginResetModel();
        products = newProducts;
        sequenceLength = newSequenceLength;
        endResetModel();
    }

    const QList<InSilicoPcrProduct>& getProducts() const {
        return products;
    }

private:
    QList<InSilicoPcrProduct> products;
    qint64 sequenceLength = 0;
};

InSilicoPcrProductsTable::InSilicoPcrProductsTable(QWidget* parent)
    : QTableView(parent), sequenceContext(nullptr), productsModel(new InSilicoPcrProductsModel(this)) {
    setModel(productsModel);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(sl_selectionChanged()));
    connect(this, SIGNAL(clicked(const QModelIndex&)), SLOT(sl_selectionChanged()));
}

void InSilicoPcrProductsTable::replaceContext(ADVSequenceObjectContext* newSequenceContext) {
    if (sequenceContext != nullptr) {
        sequenceContext->disconnect(this);
    }
    sequenceContext = newSequenceContext;
}

void InSilicoPcrProductsTable::showProducts(const QList<InSilicoPcrProduct>& products, ADVSequenceObjectContext* newSequenceContext) {
    replaceContext(newSequenceContext);
    setCurrentProducts(products);
}

ADVSequenceObjectContext* InSilicoPcrProductsTable::getCurrentSequenceContext() const {
    return sequenceContext;
}

void InSilicoPcrProductsTable::setCurrentProducts(const QList<InSilicoPcrProduct>& products) {
    const qint64 seqLength = sequenceContext == nullptr ? 0 : sequenceContext->getSequenceLength();
    SAFE_POINT(products.isEmpty() || seqLength > 0, "Invalid sequence length", );
    productsModel->setProducts(products, seqLength);
    if (!products.isEmpty()) {
        selectRow(0);
    }
    emit si_productsChanged();
}

ADVSequenceObjectContext* InSilicoPcrProductsTable::productsContext() const {
    return sequenceContext;
}

QList<InSilicoPcrProduct> InSilicoPcrProductsTable::getSelectedProducts() const {
    QList<InSilicoPcrProduct> result;
    const auto& currentProducts = getAllProducts();
    foreach (const QModelIndex& index, selectedIndexes()) {
        if (index.column() != 0) {
            continue;
        }
        SAFE_POINT(currentProducts.size() > index.row(), "Out of range", result);

        result << currentProducts[index.row()];
    }
    return result;
}

int InSilicoPcrProductsTable::selectedProductsCount() const {
    int count = 0;
    const auto selection = selectionModel()->selection();
    for (const auto& range : selection) {
        if (range.left() == 0) {
            count += range.height();
        }
    }
    return count;
}

const QList<InSilicoPcrProduct>& InSilicoPcrProductsTable::getAllProducts() const {
    return productsModel->getProducts();
}

QVector<U2Region> InSilicoPcrProductsTable::getSelection() const {
    QVector<U2Region> result;
    CHECK(sequenceContext != nullptr, result);

    CHECK(selectedProductsCount() == 1, result);
    const auto rows = selectionModel()->selectedRows();
    CHECK(rows.size() == 1, result);
    const auto& product = getAllProducts().at(rows.first().row());
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
    CHECK(sequenceContext != nullptr, );
    QVector<U2Region> selection = getSelection();
    CHECK(!selection.isEmpty(), );

    sequenceContext->getSequenceSelection()->setSelectedRegions(selection);
    foreach (ADVSequenceWidget* sequenceWidget, sequenceContext->getSequenceWidgets()) {
        sequenceWidget->centerPosition(selection.first().startPos);
    }
}

bool InSilicoPcrProductsTable::onSequenceChanged(ADVSequenceObjectContext* changedContext) {
    if (sequenceContext == changedContext) {
        replaceContext(nullptr);
        setCurrentProducts({});
        return true;
    }
    return false;
}

}  // namespace U2
