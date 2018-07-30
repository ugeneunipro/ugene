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

#ifndef _U2_TAXONOMY_SUPPORT_H_
#define _U2_TAXONOMY_SUPPORT_H_

#include <QMap>
#include <QMultiMap>
#include <QSet>
#include <QTreeView>

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

#include <U2Core/U2Type.h>

#include <U2Designer/DelegateEditors.h>
#include <U2Designer/PropertyWidget.h>

#include <U2Lang/Datatype.h>
#include <U2Lang/Descriptor.h>

#include "NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

typedef uint TaxID;
typedef QSet<TaxID> TaxIDSet;
typedef QMap<QString, TaxID> TaxonomyClassificationResult;
//typedef QVariantMap TaxonomyClassificationResult;

class TaxonomyTreeModel;

class U2NGS_READS_CLASSIFICATION_EXPORT TaxonomyTree {
public:
    /** Id to mark unknown or undefined value (e.g. search result or default initialization). */
    static const TaxID UNDEFINED_ID;
    /** Id to mark unclassified reads in results. */
    static const TaxID UNCLASSIFIED_ID;

    static TaxonomyTree *getInstance();
    QString getName(TaxID id) const;
    QString getRank(TaxID id) const;
    TaxID getParent(TaxID id) const;
    QList<TaxID> getChildren(TaxID id) const;

    /*
     * Result counter also counts empty invalid entries.
     */
    int getNamesListSize() const;

    bool contains(TaxID id) const;

    bool isValid() const;

    /**
     * @param id
     * @param filter
     * @return Closest match from the filter set or UNDEFINED_ID if the id does not belong to the specified taxonomy
     */
    TaxID match(TaxID id, QSet<TaxID> filter);
private:
    TaxonomyTree();

    static TaxonomyTree *the_tree;
    static TaxonomyTree *load(TaxonomyTree *);

    /**
     * Index array of taxons. Value at a given index keeps parent ID (+ rank index in the upper byte).
     * Used for down-top traversal.
     */
    QList<TaxID> nodes;

    /**
     * Index array of scientific taxon names.
     * List contains empty invalid entries, for performance purposes.
     */
    QStringList  names;

    /**
     * Names of ranks
     */
    QStringList  ranks;

    /**
     * Keeps parent-children relation for top-down traversal.
     */
    QMultiMap<TaxID, TaxID> childs;

    bool valid;
};

class TaxonomyTreeModel : public QAbstractItemModel {
public:
    TaxonomyTreeModel(const QString &data, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QString getSelected() const;

private:
    QList<TaxID> getChildrenSorted(TaxID id) const;

    TaxonomyTree *tree;
    /**
    * Set of actually selected items
    */
    QSet<TaxID> selected;
    /**
    * Keeps all (grand) parents of actually selected items.
    * Used to compute partially checked state.
    */
    QMultiMap<TaxID, TaxID> tristate;
};

class U2NGS_READS_CLASSIFICATION_EXPORT TaxonomySupport : public QObject {
    Q_OBJECT
public:
    static const QString TAXONOMY_CLASSIFICATION_SLOT_ID;
    static const Descriptor TAXONOMY_CLASSIFICATION_SLOT();
    static DataTypePtr TAXONOMY_CLASSIFICATION_TYPE();
};

class U2NGS_READS_CLASSIFICATION_EXPORT TaxonomyDelegate : public PropertyDelegate {
    Q_OBJECT
public:
    TaxonomyDelegate(QObject *parent = 0);

    QVariant getDisplayValue(const QVariant &value) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    PropertyWidget *createWizardWidget(U2OpStatus &os, QWidget *parent) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    PropertyDelegate *clone();

private slots:
    void sl_commit();
};

class TaxonomyPropertyWidget : public PropertyWidget {
    Q_OBJECT
public:
    TaxonomyPropertyWidget(QWidget *parent = NULL, DelegateTags *tags = NULL);

    QVariant value();

public slots:
    void setValue(const QVariant &value);

private slots:
    void sl_showDialog();

private:
    QLineEdit *lineEdit;
    QToolButton *toolButton;
    QString text;
};

class TaxonSelectionDialog : public QDialog {
public:
    TaxonSelectionDialog(const QString &value, QWidget *parent);

    QString getValue() const;

private:
    QVBoxLayout *mainLayout;
    QDialogButtonBox *buttonBox;
    QTreeView *treeView;
    TaxonomyTreeModel *treeModel;
};

}   // namespace LocalWorkflow
}   // namespace U2

Q_DECLARE_METATYPE(U2::LocalWorkflow::TaxID)
Q_DECLARE_METATYPE(U2::LocalWorkflow::TaxIDSet)
Q_DECLARE_METATYPE(U2::LocalWorkflow::TaxonomyClassificationResult)

#endif
