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

#ifndef _U2_ASSEMBLY_ANNOTATIONS_TREE_VIEW_MODEL_H_
#define _U2_ASSEMBLY_ANNOTATIONS_TREE_VIEW_MODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

namespace U2 {

class Annotation;
class AnnotationTableObject;
class AssemblyAnnotationsTreeItem;
class SequenceObjectContext;
class U2Qualifier;

class AssemblyAnnotationsTreeViewModel : public QAbstractItemModel {
    Q_OBJECT
public:
    AssemblyAnnotationsTreeViewModel(QObject *parent);
    ~AssemblyAnnotationsTreeViewModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private slots:
    void sl_annotationObjectAdded(AnnotationTableObject* obj);
    void sl_annotationObjectRemoved(AnnotationTableObject* obj);
    void sl_contextChanged(SequenceObjectContext* ctx);

private:
    void addAnnotationTableObject(AnnotationTableObject *newObj);
    void addAnnotations(const QList<Annotation*>& annotations, AssemblyAnnotationsTreeItem* parentItem);
    void addQualifiers(const QList<U2Qualifier>& qualifiers, AssemblyAnnotationsTreeItem* parentItem);
    QVariantList getTableObjData(AnnotationTableObject* obj) const;
    QVariantList getAnnotationData(Annotation* ann) const;
    QVariantList getQualifierData(const U2Qualifier& qualifier) const;
    void cleanAnnotationTree();

    AssemblyAnnotationsTreeItem* rootItem;
    SequenceObjectContext* ctx;
};

}

#endif