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

#ifndef _U2_ASSEMBLY_ANNOTATIONS_TREE_VIEW_H_
#define _U2_ASSEMBLY_ANNOTATIONS_TREE_VIEW_H_

#include <QObject>
#include <QTreeView>

namespace U2 {

class Annotation;
class AnnotationSelection;
class AssemblyAnnotationsTreeViewModel;

class AssemblyAnnotationsTreeView : public QTreeView {
Q_OBJECT
public:
    AssemblyAnnotationsTreeView(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

private slots:
    void sl_onAnnotationSelectionChanged(AnnotationSelection *as,
                                         const QList<Annotation *>& added,
                                         const QList<Annotation *>& removed);
    void sl_clearSelectedAnnotations();

private:
    AssemblyAnnotationsTreeViewModel* getModel() const;
    void expandList(const QModelIndexList& indexList);

    bool selectionChangedFromOutside;
};

}

#endif
