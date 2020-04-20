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

#ifndef _U2_RESTRICTION_MAP_WIDGET_H_
#define _U2_RESTRICTION_MAP_WIDGET_H_

#include <QTreeWidget>

namespace U2 {

class Annotation;
class AnnotationGroup;
class ADVSequenceObjectContext;

class EnzymeItem : public QTreeWidgetItem
{
public:
    EnzymeItem(const QString& locationStr, Annotation *a);
    Annotation * getEnzymeAnnotation() const {return annotation; }
private:
    Annotation *annotation;
};

class EnzymeFolderItem : public QTreeWidgetItem
{
    QString enzymeName;
public:
    EnzymeFolderItem(const QString& name);
    void addEnzymeItem(Annotation *enzAnn);
    void removeEnzymeItem(Annotation *enzAnn);
    const QString& getName() const { return enzymeName; }
};

class RestrctionMapWidget : public QWidget
{
    Q_OBJECT
public:
    RestrctionMapWidget(ADVSequenceObjectContext* ctx, QWidget *p);

private slots:
    void sl_onAnnotationsAdded(const QList<Annotation *> &anns);
    void sl_onAnnotationsRemoved(const QList<Annotation *> &anns);
    void sl_onAnnotationsInGroupRemoved(const QList<Annotation *> &anns, AnnotationGroup *group);
    void sl_onAnnotationsGroupCreated(AnnotationGroup *g);
    void sl_itemSelectionChanged();
private:
    ADVSequenceObjectContext* ctx;
    QTreeWidget* treeWidget;
    EnzymeFolderItem* findEnzymeFolderByName(const QString& enzymeName);
    void registerAnnotationObjects();
    void updateTreeWidget();
    void initTreeWidget();
};

} //namespace U2

#endif //  _U2_RESTRICTION_MAP_WIDGET_H_
