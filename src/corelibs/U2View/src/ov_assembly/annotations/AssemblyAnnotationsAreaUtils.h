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

#ifndef _ASSEMBLY_ANNOTATIONS_AREA_UTILS_
#define _ASSEMBLY_ANNOTATIONS_AREA_UTILS_

#include <QItemSelection>
#include <QItemSelectionModel>
#include <QList>

namespace U2 {

class Annotation;
class AnnotationTableObject;
class AssemblyAnnotationsTreeViewModel;
class U2Qualifier;

class AssemblyAnnotationsAreaUtils {
public:
    static QItemSelection getSelectionFromIndexList(const QModelIndexList& inedexList);
    static QItemSelection rowSelection(AssemblyAnnotationsTreeViewModel* treeViewModel,
                                       const QModelIndex& rowIndex);
    static QVariantList getTableObjData(AnnotationTableObject* obj);
    static QVariantList getAnnotationData(Annotation* ann);
    static QVariantList getQualifierData(const U2Qualifier& qualifier);

    template <class T>
    static inline void collectSelectionInfo(T clickedItem,
                                            const QList<T>& selectedItemList,
                                            QItemSelectionModel::SelectionFlag& clickedItemFlag,
                                            QList<T>& toDeselectItemList);

};

template <class T>
inline void AssemblyAnnotationsAreaUtils::collectSelectionInfo(T clickedItem,
                                                               const QList<T>& selectedItemList,
                                                               QItemSelectionModel::SelectionFlag& clickedItemFlag,
                                                               QList<T>& toDeselectItemList) {
    const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
    const bool controlOrShiftPressed = modifiers.testFlag(Qt::ControlModifier)
                                    || modifiers.testFlag(Qt::ShiftModifier);

    if (!selectedItemList.contains(clickedItem)) {
        clickedItemFlag = QItemSelectionModel::Select;
        if (!controlOrShiftPressed) {
            toDeselectItemList = selectedItemList;
        }
    } else {
        if (controlOrShiftPressed) {
            clickedItemFlag = QItemSelectionModel::Deselect;
        } else {
            toDeselectItemList = selectedItemList;
            toDeselectItemList.removeOne(clickedItem);
        }
    }
}


} // U2

#endif // _ASSEMBLY_ANNOTATIONS_AREA_UTILS_
