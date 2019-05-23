/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include <QApplication>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U1AnnotationUtils.h>

#include "AssemblyAnnotationsAreaUtils.h"
#include "tree_view/AssemblyAnnotationsTreeViewModel.h"

namespace U2 {

QItemSelection AssemblyAnnotationsAreaUtils::getSelectionFromIndexList(const QModelIndexList& indexList) {
    QItemSelection selection;
    foreach(const QModelIndex& index, indexList) {
        selection.merge(QItemSelection(index, index), QItemSelectionModel::Select);
    }

    return selection;
}

QItemSelection AssemblyAnnotationsAreaUtils::rowSelection(AssemblyAnnotationsTreeViewModel* treeViewModel,
                                                          const QModelIndex& rowIndex) {
    int rowNum = rowIndex.row();
    QModelIndex rowIndexParent = rowIndex.parent();
    QModelIndex firstColumnIndex = treeViewModel->index(rowNum,
                                                        0,
                                                        rowIndexParent);
    QModelIndex lastColumnIndex = treeViewModel->index(rowNum,
                                                       treeViewModel->columnCount() - 1,
                                                       rowIndexParent);

    return QItemSelection (firstColumnIndex, lastColumnIndex);
}


QVariantList AssemblyAnnotationsAreaUtils::getTableObjData(AnnotationTableObject* obj) {
    const QString annTableObjName = obj->getGObjectName();
    const QString docShortName = obj->getDocument()->getName();
    QVariantList tableObjData = { annTableObjName + " [" + docShortName + "]", "" };

    return tableObjData;
}

QVariantList AssemblyAnnotationsAreaUtils::getAnnotationData(Annotation* ann) {
    U2Location location = ann->getLocation();
    U2LocationData* annLocationData = location.data();
    QString annRegionString = U1AnnotationUtils::buildLocationString(*annLocationData);
QVariantList annData = { ann->getName(), annRegionString };

return annData;
}

QVariantList AssemblyAnnotationsAreaUtils::getQualifierData(const U2Qualifier& qualifier) {
    QVariantList qualifierData = { qualifier.name, qualifier.value };

    return qualifierData;
}


} // U2
