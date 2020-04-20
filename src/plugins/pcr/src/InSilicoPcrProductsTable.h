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

#ifndef _U2_IN_SILICO_PCR_PRODUCTS_TABLE_H_
#define _U2_IN_SILICO_PCR_PRODUCTS_TABLE_H_

#include <QTableWidget>

#include <U2Core/U2Region.h>

#include "InSilicoPcrTask.h"

namespace U2 {

class ADVSequenceObjectContext;

class InSilicoPcrProductsTable : public QTableWidget {
    Q_OBJECT
public:
    InSilicoPcrProductsTable(QWidget *parent);

    void showProducts(const QList<InSilicoPcrProduct> &products, ADVSequenceObjectContext *sequenceContext);
    /* Returns if current context is the modified one */
    bool onSequenceChanged(ADVSequenceObjectContext *sequenceContext);

    ADVSequenceObjectContext * productsContext() const;

    QList<InSilicoPcrProduct> getSelectedProducts() const;
    const QList<InSilicoPcrProduct> & getAllProducts() const;

    ADVSequenceObjectContext * getCurrentSequenceContext() const;

private slots:
    void sl_selectionChanged();

private:
    QVector<U2Region> getSelection() const;
    void replaceContext(ADVSequenceObjectContext *sequenceContext);
    void setCurrentProducts(const QList<InSilicoPcrProduct> &products);

private:
    ADVSequenceObjectContext *sequenceContext;
    QList<InSilicoPcrProduct> currentProducts;
};

} // U2

#endif // _U2_IN_SILICO_PCR_PRODUCTS_TABLE_H_
