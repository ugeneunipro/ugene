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

#ifndef _U2_RESULT_TABLE_H_
#define _U2_RESULT_TABLE_H_

#include <QTableWidget>

#include <U2Core/U2Region.h>

namespace U2 {

class AnnotatedDNAView;
class AnnotationGroup;
class Annotation;

struct ResultTableData {
    AnnotatedDNAView *associatedView = nullptr;
    AnnotationGroup *associatedGroup = nullptr;
    QList<U2Region> currentProducts;
};

class ResultTable : public QTableWidget {
    Q_OBJECT
public:
    ResultTable(QWidget *parent);

    void setCurrentProducts(const QList<U2Region> &currentProducts, AnnotatedDNAView *associatedView);
    void setAnnotationGroup(AnnotationGroup *associatedGroup);
    Annotation* getSelectedAnnotation() const;
    ResultTableData getPCRPrimerProductTableData() const;

    static constexpr int MAXIMUM_ROW_COUNT = 8;
private slots:
    void sl_selectionChanged();

private:
    AnnotatedDNAView *associatedView = nullptr;
    AnnotationGroup *associatedGroup = nullptr;
    QList<U2Region> currentProducts;
};

}    // namespace U2

#endif
