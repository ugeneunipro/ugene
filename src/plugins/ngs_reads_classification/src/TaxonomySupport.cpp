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

#include <QAbstractItemModel>

#include <QPushButton>
#include <QVBoxLayout>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>


#include <U2Designer/DelegateEditors.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/FastaFormat.h>
#include <U2Formats/FastqFormat.h>


#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Lang/WorkflowEnv.h>

#include <U2Gui/HelpButton.h>

#include "TaxonomySupport.h"

namespace U2 {
namespace LocalWorkflow {

const QString TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT_ID("tax-data");
static const QString CLASSIFICATION_SLOT_TYPE_ID("tax-classification");

const TaxID TaxonomyTree::UNDEFINED_ID = (TaxID)-1;
const TaxID TaxonomyTree::UNCLASSIFIED_ID = 0;

TaxonomyTree *TaxonomyTree::the_tree = NULL;
const int RANK_SHIFT = (sizeof(TaxID) - sizeof(char))* 8;
const TaxID RANK_MASK = 0xFF << RANK_SHIFT;

/********************************************************************/
/*TaxonomySupport*/
/********************************************************************/

const Descriptor TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()
{
    return Descriptor(TAXONOMY_CLASSIFICATION_SLOT_ID, tr("Taxonomy classification data"), tr("Taxonomy classification data"));
}

DataTypePtr TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE() {
    DataTypeRegistry* dtr = Workflow::WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new DataType(CLASSIFICATION_SLOT_TYPE_ID, tr("Taxonomy classification data"), tr("Taxonomy classification data"))));
        startup = false;
    }
    return dtr->getById(CLASSIFICATION_SLOT_TYPE_ID);
}

TaxonomyTree *TaxonomyTree::getInstance()
{
    if (the_tree == NULL) {
        //fixme data race???
        the_tree = load(new TaxonomyTree);
    }
    return the_tree;
}

QString TaxonomyTree::getName(TaxID id) const
{
    if (unsigned(names.size()) > id) {
        return names.at(id);
    }
    algoLog.info(QString("Unknown taxon ID requested: %1").arg(id));
    return QString("Unknown taxon ID");
}

QString TaxonomyTree::getRank(TaxID id) const
{
    if (unsigned(nodes.size()) > id) {
        return ranks.at((nodes.at(id) & RANK_MASK) >> RANK_SHIFT);
    }
    algoLog.info(QString("Unknown taxon ID requested: %1").arg(id));
    return QString("Unknown taxon ID");
}

TaxID TaxonomyTree::getParent(TaxID id) const
{
    if (unsigned(nodes.size()) > id) {
        return nodes.at(id) & ~RANK_MASK;
    }
    algoLog.info(QString("Unknown taxon ID requested: %1").arg(id));
    return UNDEFINED_ID;
}

QList<TaxID> TaxonomyTree::getChildren(TaxID id) const
{
    return childs.values(id);
}

int TaxonomyTree::getNamesListSize() const {
    return names.size();
}

bool TaxonomyTree::contains(TaxID id) const {
    return unsigned(nodes.size()) > id;
}

bool TaxonomyTree::isValid() const {
    return valid;
}

TaxID TaxonomyTree::match(TaxID id, QSet<TaxID> filter)
{
    // first try fastpath
    if (id >= unsigned(nodes.size())) {
        return UNDEFINED_ID;
    }
    if (filter.contains(id)) {
        return id;
    }
    // then go searching by hierarchy
    QList<TaxID> parents;
    TaxID parent = id;
    while (parent > 1) {
        if (unsigned(nodes.size()) > parent) {
            parents << parent;
            parent = getParent(parent);
        } else {
            algoLog.error(QString("Broken taxonomy tree: %1").arg(id));
            break;
        }
    }
    foreach (TaxID parent, parents) {
        if (filter.contains(parent)) {
            return parent;
        }
    }
    return UNDEFINED_ID;
}

TaxonomyTree::TaxonomyTree()
    : valid(false)
{

}

class TaxonNameComparator
{
public:
    TaxonNameComparator( TaxonomyTree *tree) : tree(tree) {}
    bool operator()(const TaxID left, const TaxID right ) const {
        return tree->getName(left).compare(tree->getName(right));
    }

private:
TaxonomyTree *tree;
};

TaxonomyTree *TaxonomyTree::load(TaxonomyTree *tree)
{
    U2DataPathRegistry *dataPathRegistry = AppContext::getDataPathRegistry();
    SAFE_POINT(NULL != dataPathRegistry, "U2DataPathRegistry is NULL", tree);

    U2DataPath *taxonomyDataPath = dataPathRegistry->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(), algoLog.details(QString("Taxonomy data is not configured")), tree);

    bool hasError = false;

    QString nodesUrl = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID);
    QFile nodesFile(nodesUrl);
    if (!nodesFile.open(QIODevice::ReadOnly)) {
        algoLog.error(QString("Cannot open taxonomy classification data: %1").arg(nodesUrl.isEmpty() ? NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID : nodesUrl));
        hasError = true;
    } else {
        GTIMER(cvar, tvar, "TaxonomyTree::nodes");
        QList<TaxID> &nodes = tree->nodes;
        nodes.reserve(2000000);
        QByteArray line;
        while ((line = nodesFile.readLine()).size() != 0) {
            QList<QByteArray> row = line.split('|');
            if (row.size() > 3) {
                bool ok = true;
                TaxID taxID = row[0].trimmed().toUInt(&ok);
                if (ok) {
                    TaxID parentID = row[1].trimmed().toUInt(&ok);
                    if (ok) {
                        if (unsigned(nodes.size()) <= taxID) {
                            nodes.reserve(taxID + 1000);
                        }
                        while (unsigned(nodes.size()) <= taxID) {
                            nodes.append(0);
                        }

                        QString rank = row[2].trimmed();
                        int rankID = tree->ranks.indexOf(rank);
                        if (rankID < 0) {
                            rankID = tree->ranks.size();
                            tree->ranks << rank;
                            assert(tree->ranks.size() < 0xFF);
                            assert(rankID == tree->ranks.indexOf(rank));
                        }

                        //Hack to skip "cellular organisms" node, "to follow NCBI taxonomy browser"
                        const TaxID CELL_ORGMS_ID = 131567;
                        if (parentID == CELL_ORGMS_ID) {
                            parentID = 1;
                        }
                        if (taxID == CELL_ORGMS_ID) {
                            assert(rank == "no rank");
                            assert(parentID == 1);
                        }

                        assert (parentID == (parentID & ~RANK_MASK));
                        nodes[taxID] = parentID | (rankID << RANK_SHIFT);
                        assert(tree->getParent(taxID) == parentID);
                        assert(tree->getRank(taxID) == rank);

                        if (taxID != 1 && taxID != CELL_ORGMS_ID) {
                            tree->childs.insert(parentID, taxID);
                        }
                        continue;
                    }
                }
            }
            algoLog.error(QString("Broken nodes.dmp file : %1").arg(nodesUrl));
            hasError = true;
            break;
        }
        nodesFile.close();
    }

    QString namesUrl = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID);
    QFile namesFile(namesUrl);
    if (!namesFile.open(QIODevice::ReadOnly)) {
        algoLog.error(QString("Cannot open taxonomy classification data: %1").arg(namesUrl.isEmpty() ? NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID : namesUrl));
        hasError = true;
    } else {
        GTIMER(cvar, tvar, "TaxonomyTree::names");
        QStringList &names = tree->names;
        QByteArray line;
        names.reserve(2000000);
        while ((line = namesFile.readLine()).size() != 0) {
            const QList<QByteArray> row = line.split('|');
            bool ok = true;
            if (row.size() > 3) {
                if (row[3].trimmed() == "scientific name") {
                    TaxID taxID = row[0].trimmed().toUInt(&ok);
                    if (ok) {
                        QByteArray name = row[1].trimmed();
                        if (unsigned(names.size()) <= taxID) {
                            names.reserve(taxID + 1000);
                        }
                        while (unsigned(names.size()) <= taxID) {
                            names.append(QString());
                        }
                        if (!names.at(taxID).isEmpty())
                        {
                            algoLog.error(QString("Non-unique scientific name for taxon ID : %1").arg(taxID));
                        }
                        names[taxID] = name;
                    }
                }
            } else {
                ok = false;
            }
            if (!ok) {
                algoLog.error(QString("Broken names.dmp file : %1").arg(namesUrl));
                hasError = true;
                break;
            }
        }
        namesFile.close();
    }
    tree->valid = !hasError;
    return tree;
}


////////////////////////// WD GUI ////////////////////////

class TreeItem;

/********************************************************************/
/*TaxonomyTreeModel*/
/********************************************************************/

namespace {

bool taxIdLessThan(const TaxID a, const TaxID b) {
    return TaxonomyTree::getInstance()->getName(a) < TaxonomyTree::getInstance()->getName(b);
}

}

TaxonomyTreeModel::TaxonomyTreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent), tree(TaxonomyTree::getInstance()) {
    QStringList taxons = data.split(";", QString::SkipEmptyParts);
    foreach (const QString &idStr, taxons) {
        selected.insert(idStr.toInt());
    }
    foreach (TaxID id, selected) {
        TaxID parent = tree->getParent(id);
        while (parent > 1) {
            tristate.insert(parent, id);
            parent = tree->getParent(parent);
        }
    }
}

QList<TaxID> TaxonomyTreeModel::getChildrenSorted(TaxID id) const {
    QList<TaxID> values = tree->getChildren(id);
    if (values.size() > 1) {
        //qSort(values.begin(), values.end(), TaxonNameComparator(tree));
        qSort(values.begin(), values.end(), taxIdLessThan);
    }
    return values;
}

int TaxonomyTreeModel::columnCount(const QModelIndex &) const {
    return 3;
}

QString TaxonomyTreeModel::getSelected() const {
    QString res;
    foreach (TaxID id, selected) {
        res += QString::number(id) + ";";
    }
    if (!res.isEmpty()) {
        res.chop(1);
    }
    return res;
}

bool TaxonomyTreeModel::setData(const QModelIndex &index, const QVariant &v, int role) {
    if (role != Qt::CheckStateRole || index.column() != 0)
        return false;

    TaxID item = static_cast<TaxID>(index.internalId());
    bool result = false;
    bool old = selected.contains(item);
    int value = v.toInt();
    if (value == Qt::PartiallyChecked) {
        // workaround for Qt5.4: https://bugreports.qt.io/browse/QTBUG-43473
        value = Qt::Checked;
    }
    algoLog.info(QString("check %1 for %2 ").arg(value).arg(item));

    if (value == Qt::Unchecked && old) {
        selected.remove(item);
        result = true;
    } else if (value == Qt::Checked && !old) {
        selected << item;
        result = true;
    }

    if (result) {
        QVector<int> checkRole(1, Qt::CheckStateRole);
        emit dataChanged(index, index, checkRole);

        QList<TaxID> children = getChildrenSorted(item);
        if (children.size() != 0) {
            emit dataChanged(createIndex(0,0,children.first()), createIndex(children.size()-1,3,children.last()));
        }
        TaxID parent = tree->getParent(item);
        while (parent > 1) {
            if (value == Qt::Checked) {
                tristate.insert(parent, item);
            } else {
                tristate.remove(parent, item);
            }
            emit dataChanged(createIndex(0,0,parent), createIndex(0,3,parent), checkRole);
            parent = tree->getParent(parent);
        }
    }
    return result;
}


QVariant TaxonomyTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    TaxID item = static_cast<TaxID>(index.internalId());
    if ( role == Qt::CheckStateRole && index.column() == 0 ) {
        while (item > 1) {
            if (selected.contains(item)) {
                return Qt::Checked;
            }
            item = tree->getParent(item);
        }
        item = static_cast<TaxID>(index.internalId());
        return tristate.contains(item) ? Qt::PartiallyChecked : Qt::Unchecked;
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return tree->getName(item);
        case 1:
            return tree->getRank(item);
        case 2:
            return item;
        }
    }
    return QVariant();

}

Qt::ItemFlags TaxonomyTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    TaxID item = static_cast<TaxID>(index.internalId());

    if (!selected.contains(item)) {
        while (item > 1) {
            item = tree->getParent(item);
            if (selected.contains(item)) {
                flags &= ~Qt::ItemIsEnabled;
                break;
            }
        }
    }

    if ( index.column() == 0 ) {
        flags |= Qt::ItemIsUserCheckable | Qt::ItemIsTristate;
    }

    return flags;
}

QVariant TaxonomyTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section) {
        case 0:
            return TaxonomySupport::tr("Taxon name");
        case 1:
            return TaxonomySupport::tr("Rank");
        case 2:
            return TaxonomySupport::tr("Taxon ID");
        }
    }

    return QVariant();
}

QModelIndex TaxonomyTreeModel::index(int row, int column, const QModelIndex &parent)
            const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TaxID parentItem;

    if (!parent.isValid())
        parentItem = 1;
    else
        parentItem = static_cast<TaxID>(parent.internalId());

    QList<TaxID> children = getChildrenSorted(parentItem);
    if (row < children.size())
        return createIndex(row, column, children.at(row));
    else
        return QModelIndex();
}

QModelIndex TaxonomyTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    TaxID childItem = static_cast<TaxID>(index.internalId());
    TaxID parentItem = tree->getParent(childItem);

    if (parentItem == 1)
        return QModelIndex();

    QList<TaxID> siblings = getChildrenSorted(tree->getParent(parentItem));
    int row = siblings.indexOf(parentItem);
    if (row >= 0)
        return createIndex(row, 0, parentItem);
    //else todo assert

    return QModelIndex();
}

int TaxonomyTreeModel::rowCount(const QModelIndex &parent) const {
    TaxID parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = 1;
    else
        parentItem = static_cast<TaxID>(parent.internalId());

    return tree->getChildren(parentItem).size();
}

/********************************************************************/
/*TaxonomyDelegate*/
/********************************************************************/

static const QString PLACEHOLDER("Select IDs...");

TaxonomyDelegate::TaxonomyDelegate(QObject *parent)
    : PropertyDelegate(parent)
{
}

QVariant TaxonomyDelegate::getDisplayValue(const QVariant &value) const {
    QString str = value.value<QString>();
    return str.isEmpty() ? PLACEHOLDER : str;
}

QWidget *TaxonomyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    TaxonomyPropertyWidget* editor = new TaxonomyPropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget *TaxonomyDelegate::createWizardWidget(U2OpStatus &, QWidget *parent) const {
    return new TaxonomyPropertyWidget(parent);
}

void TaxonomyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    const QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    TaxonomyPropertyWidget *propertyWidget = qobject_cast<TaxonomyPropertyWidget *>(editor);
    propertyWidget->setValue(value);
}

void TaxonomyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    TaxonomyPropertyWidget *propertyWidget = qobject_cast<TaxonomyPropertyWidget *>(editor);
    model->setData(index, propertyWidget->value(), ConfigurationEditor::ItemValueRole);
}

PropertyDelegate *TaxonomyDelegate::clone() {
    return new TaxonomyDelegate(parent());
}

void TaxonomyDelegate::sl_commit() {
    TaxonomyPropertyWidget* editor = qobject_cast<TaxonomyPropertyWidget *>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

/********************************************************************/
/*TaxonomyPropertyWidget*/
/********************************************************************/

TaxonomyPropertyWidget::TaxonomyPropertyWidget(QWidget *parent, DelegateTags *tags)
    : PropertyWidget(parent, tags)
{
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(PLACEHOLDER);
    lineEdit->setReadOnly(true);
    lineEdit->setObjectName("lineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("toolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_showDialog()));
    layout()->addWidget(toolButton);

    setObjectName("TaxonomyPropertyWidget");
}

QVariant TaxonomyPropertyWidget::value() {
    return text;
}

void TaxonomyPropertyWidget::setValue(const QVariant &value) {
    text = value.value<QString>();
    lineEdit->setText(text);
}

void TaxonomyPropertyWidget::sl_showDialog() {
    QObjectScopedPointer<TaxonSelectionDialog> dialog(new TaxonSelectionDialog(text, this));
    if (QDialog::Accepted == dialog->exec()) {
        CHECK(!dialog.isNull(), );
        text = dialog->getValue();
        lineEdit->setText(text);
        emit si_valueChanged(value());
    }
}

/********************************************************************/
/*TaxonSelectionDialog*/
/********************************************************************/

TaxonSelectionDialog::TaxonSelectionDialog(const QString &value, QWidget *parent)
    : QDialog(parent)
{
    if (objectName().isEmpty()) {
        setObjectName(QStringLiteral("TaxonSelectionDialog"));
    }
    mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    treeView = new QTreeView(this);
    treeModel = new TaxonomyTreeModel(value); //fixme delete
    treeView->setModel(treeModel);
    for (int column = 0; column < treeModel->columnCount(); ++column) {
        treeView->resizeColumnToContents(column);
    }
    treeView->header()->resizeSection(0, 370);
    treeView->header()->resizeSection(1, 120);
    mainLayout->addWidget(treeView);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    mainLayout->addWidget(buttonBox);

    setWindowTitle(QApplication::translate("TaxonSelectionDialog", "Select Taxa", 0));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);

    new HelpButton(this, buttonBox, "43");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Select"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

  //  adjustSize();
    resize(580, 440);
    setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Expanding);
}

QString TaxonSelectionDialog::getValue() const {
    return treeModel->getSelected();
}

}   // namespace LocalWorkflow
}   // namespace U2
