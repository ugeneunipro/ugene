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
#include <QItemSelection>
#include <QKeyEvent>

#include "../AssemblyAnnotationsAreaUtils.h"
#include "AssemblyAnnotationsTreeView.h"
#include "AssemblyAnnotationsTreeViewModel.h"

#include <U2Core/AnnotationSelection.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

AssemblyAnnotationsTreeView::AssemblyAnnotationsTreeView(QWidget *parent) : QTreeView(parent),
                                                                            selectionChangedFromOutside(false) {
    setModel(new AssemblyAnnotationsTreeViewModel(this));
    setSelectionMode(ExtendedSelection);
}

void AssemblyAnnotationsTreeView::keyPressEvent(QKeyEvent *e) {
    int key = e->key();
    bool accepted = false;
    switch (key) {
    case Qt::Key_Escape:
        clearSelection();
        accepted = true;
        break;
    }

    if (accepted) {
        e->accept();
    } else {
        QTreeView::keyPressEvent(e);
    }
}

void AssemblyAnnotationsTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    if (!selectionChangedFromOutside) {
        AssemblyAnnotationsTreeViewModel* treeViewModel = getModel();
        CHECK(nullptr != treeViewModel, );

        treeViewModel->changeSelection(selected.indexes(), deselected.indexes());
    }
    QTreeView::selectionChanged(selected, deselected);
}

void AssemblyAnnotationsTreeView::sl_onAnnotationSelectionChanged(AnnotationSelection *as,
                                                                  const QList<Annotation *>& added,
                                                                  const QList<Annotation *>& removed) {
    AssemblyAnnotationsTreeViewModel* treeViewModel = getModel();
    CHECK(nullptr != treeViewModel, );

    QItemSelectionModel* selModel = selectionModel();
    SAFE_POINT(nullptr != selModel, "Selection Model is missed", );

    QModelIndexList toAdd = treeViewModel->getIndexListByAnnotationList(added);
    QModelIndexList toRemove = treeViewModel->getIndexListByAnnotationList(removed);

    expandList(toAdd);

    QItemSelection selected = AssemblyAnnotationsAreaUtils::getSelectionFromIndexList(toAdd);
    QItemSelection deselected = AssemblyAnnotationsAreaUtils::getSelectionFromIndexList(toRemove);

    selectionChangedFromOutside = true;
    selModel->select(selected, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    selModel->select(deselected, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
    selectionChangedFromOutside = false;
}

void AssemblyAnnotationsTreeView::sl_clearSelectedAnnotations() {
    clearSelection();
}

AssemblyAnnotationsTreeViewModel* AssemblyAnnotationsTreeView::getModel() const {
    QAbstractItemModel* treeViewAbstractModel = model();
    SAFE_POINT(nullptr != treeViewAbstractModel, "Tree View Abstract Model is missed", nullptr);

    AssemblyAnnotationsTreeViewModel* treeViewModel = qobject_cast<AssemblyAnnotationsTreeViewModel*>(treeViewAbstractModel);
    SAFE_POINT(nullptr != treeViewModel, "Assembly Annotations Tree View Model is missed", nullptr);

    return treeViewModel;
}

void AssemblyAnnotationsTreeView::expandList(const QModelIndexList& indexList) {
    foreach(const QModelIndex& index, indexList) {
        expand(index.parent());
    }
}

}