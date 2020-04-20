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

#include <QAbstractItemModel>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>

#include <U2Core/Log.h>

#include <U2Gui/HelpButton.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowSettings.h>

#include "CreateScriptWorker.h"
#include "WorkflowEditorDelegates.h"
#include "library/ScriptWorker.h"
#include "util/WorkerNameValidator.h"

namespace U2 {

#define WORKFLOW_DOC "GB2WORKFLOW"
#define ACTOR_ELEMENT "Actor"
#define INPUT_PORT_ELEMENT "Input-port"
#define OUTPUT_PORT_ELEMENT "Output-port"
#define ATTRIBUTE_ELEMENT "Attributes"
#define IN_SLOT_ELEMENT "In-Slots"
#define OUT_SLOT_ELEMENT "Out-Slots"
#define SLOT_ID "Slot"
#define ATTR_ELEMENT "Attribute"
#define NAME_ID "Name"
#define TYPE_ID "Type"
#define NAME_ELEMENT "Element-name"
#define DESCR_ELEMENT "Element-description"
#define DESCR_ID "Description"

//Q_DECLARE_METATYPE(DataTypePtr);

class MapForTypesDelegate {
public:
    MapForTypesDelegate() {}

    static QVariantMap getAttrTypes() {
        QVariantMap res;
        DataTypePtr ptr = BaseTypes::BOOL_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::BOOL_TYPE()->getId();

        ptr = BaseTypes::NUM_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::NUM_TYPE()->getId();

        ptr = BaseTypes::STRING_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::STRING_TYPE()->getId();

        return res;
    }

    static QVariantMap getPortTypes() {
        QVariantMap res;
        DataTypePtr ptr = BaseTypes::DNA_SEQUENCE_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::DNA_SEQUENCE_TYPE()->getId();

        ptr = BaseTypes::ANNOTATION_TABLE_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::ANNOTATION_TABLE_TYPE()->getId();

        ptr = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE()->getId();

        ptr = BaseTypes::STRING_TYPE();
        res[ptr->getDisplayName()] = BaseTypes::STRING_TYPE()->getId();
        /*Descriptor ptr = BaseSlots::DNA_SEQUENCE_SLOT();
        res[ptr.getDisplayName()] = BaseSlots::DNA_SEQUENCE_SLOT().getId();

        ptr = BaseSlots::ANNOTATION_TABLE_SLOT();
        res[ptr.getDisplayName()] = BaseSlots::ANNOTATION_TABLE_SLOT().getId();

        ptr = BaseSlots::MULTIPLE_ALIGNMENT_SLOT();
        res[ptr.getDisplayName()] = BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId();

        ptr = BaseSlots::TEXT_SLOT();
        res[ptr.getDisplayName()] = BaseSlots::TEXT_SLOT().getId();*/
        return res;
    }

    static QVariantMap portMap;
    static QVariantMap attrMap;
};

QVariantMap MapForTypesDelegate::attrMap = MapForTypesDelegate::getAttrTypes();
QVariantMap MapForTypesDelegate::portMap = MapForTypesDelegate::getPortTypes();


enum DelegateType {
    DelegateForPort,
    DelegateForAttribute
};

class CfgListItem {
    Q_DISABLE_COPY(CfgListItem)
public:
    CfgListItem(DelegateType t)  {
        if(t == DelegateForPort) {
            delegate = new ComboBoxDelegate(MapForTypesDelegate::portMap);
            dataTypeId = BaseTypes::DNA_SEQUENCE_TYPE()->getId();
        }
        else if(t == DelegateForAttribute) {
            delegate = new ComboBoxDelegate(MapForTypesDelegate::attrMap);
            dataTypeId = BaseTypes::STRING_TYPE()->getId();
        }
        else {
            assert(0);
        }
    }
    ~CfgListItem() {delete delegate;}
    PropertyDelegate *getDelegate() const {return delegate;}
    QString getDataType() const {return dataTypeId;}
    void setDataType(const QString& id) {
        dataTypeId = id;
    }

    QString getName() const {return name;}
    void setName(const QString &_name) {name = _name;}

private:
    PropertyDelegate *delegate;
    QString dataTypeId;
    QString name;
    //DataTypePtr dataType;
};


class CfgListModel: public QAbstractListModel {
public:
    // Hint for row height. We use non-default row to make combo-boxes fit.
    int itemHeight;
    CfgListModel(int rowHeightHint, QObject *obj = NULL): QAbstractListModel(obj), itemHeight(rowHeightHint) {
        items.append(new CfgListItem(DelegateForPort));
    }

    ~CfgListModel() {
        foreach(CfgListItem *item, items) {
            delete item;
        }
    }

    QStringList getItems() const {
        QStringList result;
        foreach(CfgListItem *item, items) {
            result.append(item->getDataType());
        }
        return result;
    }

    int rowCount(const QModelIndex & parent = QModelIndex()) const {
        Q_UNUSED(parent);
        return items.count();
    }

    Qt::ItemFlags flags(const QModelIndex &) const{
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    CfgListItem* getItem(const QModelIndex &index) const {
        //CfgListItem *item  = static_cast<CfgListItem*>(index.internalPointer());
        return items.at(index.row());
    }

    QModelIndex parent(const QModelIndex &) const{
        return QModelIndex();
    }

    QVariant data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const {
        CfgListItem *item = getItem(index);
        PropertyDelegate *dg = item->getDelegate();
        switch(role) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return dg->getDisplayValue(item->getDataType());
            case DelegateRole:
                return qVariantFromValue<PropertyDelegate*>(dg);
            case Qt::EditRole:
            case ConfigurationEditor::ItemValueRole:
                return item->getDataType();
            case Qt::SizeHintRole:
                return QSize(0, itemHeight);
            default:
                return QVariant();
        }
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) {
        switch (role)
        {
            case Qt::EditRole:
            case ConfigurationEditor::ItemValueRole:
                CfgListItem * item = getItem(index);
                if(item->getDataType() != value.toString()) {
                    if(!value.toString().isEmpty()) {
                        item->setDataType(value.toString());
                    }
                }
                emit dataChanged(index, index);
        }
        return true;
    }

    bool insertRows ( int row, int count = 0, const QModelIndex & parent = QModelIndex() )  {
        Q_UNUSED(row);
        beginInsertRows(parent,items.size(), items.size() + count);
        for(int i = 0;i < count; i++) {
            items.append(new CfgListItem(DelegateForPort));
        }
        endInsertRows();
        return true;
    }

    bool removeRows(int row, int count, const QModelIndex &parent /* = QModelIndex */) {
        Q_UNUSED(count);
        if(rowCount() == 0 || row <0 || row > rowCount()) {
            return false;
        }

        beginRemoveRows(parent, row, row);
        items.removeAt(row);
        endRemoveRows();
        return true;
    }


private:
    QList<CfgListItem*> items;
};

class CfgTableModel: public QAbstractTableModel {
public:
    CfgTableModel(QObject *obj = NULL): QAbstractTableModel(obj) {
        //attrs.append(new CfgListItem());
    }

    int rowCount(const QModelIndex & /* = QModelIndex */) const{
        return attrs.size();
    }

    int columnCount(const QModelIndex & /* = QModelIndex */) const {
        return 2;
    }

    Qt::ItemFlags flags(const QModelIndex &) const{
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    CfgListItem* getItem(const QModelIndex &index) const {
        //CfgListItem *item  = static_cast<CfgListItem*>(index.internalPointer());
        return attrs.at(index.row());
    }

    QList<CfgListItem*> getItems() const {
        return attrs;
    }

    QVariant data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const {
        CfgListItem *item = getItem(index);
        int col = index.column();
        PropertyDelegate *dg = item->getDelegate();

        switch(role) {
            case Qt::DisplayRole:
                if(col == 0) return item->getName();
                else return dg->getDisplayValue(item->getDataType());
            case DelegateRole:
                if(col == 1)return qVariantFromValue<PropertyDelegate*>(dg);
                else return QVariant();
            case Qt::EditRole:
            case ConfigurationEditor::ItemValueRole:
                if(col == 1) return item->getDataType();
                else return item->getName();
            default:
                return QVariant();
        }
    }

    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch(section) {
                case 0: return CreateScriptElementDialog::tr("Name");
                case 1: return CreateScriptElementDialog::tr("Type");
                default: return QVariant();
            }
        }
        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) {
        int col = index.column();
        CfgListItem * item = getItem(index);
        switch (role)
        {
            case Qt::EditRole:
            case ConfigurationEditor::ItemValueRole:
                if(col == 1) {
                    if(item->getDataType() != value.toString()) {
                        if(!value.toString().isEmpty()) {
                            item->setDataType(value.toString());
                        }
                    }
                }
                else {
                    if(item->getName() != value.toString()) {
                        item->setName(value.toString());
                    }
                }
                emit dataChanged(index, index);
        }
        return true;
    }

    bool insertRows ( int row, int count = 0, const QModelIndex & parent = QModelIndex() )  {
        Q_UNUSED(row);
        Q_UNUSED(count);
        beginInsertRows(parent, attrs.size(), attrs.size());
        attrs.append(new CfgListItem(DelegateForAttribute));
        endInsertRows();
        return true;
    }

    bool removeRows(int row, int count = 0, const QModelIndex & parent = QModelIndex()) {
        Q_UNUSED(count);
        if(row >= 0 && row < attrs.size()) {
            beginRemoveRows(parent, row,row);
            attrs.removeAt(row);
            endRemoveRows();
            return true;
        } else {
            return false;
        }
    }

private:
    QList<CfgListItem*>attrs;
};

CreateScriptElementDialog::CreateScriptElementDialog(QWidget *p, ActorPrototype* proto): QDialog(p), editing(false) {
    setupUi(this);
    new HelpButton(this, buttonBox, "28967031");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    // make list rows have the same size as buttons -> this must be enought to fit combo-boxes
    int comboboxHeightHint = addInputButton->sizeHint().height();
    inputList->setModel(new CfgListModel(comboboxHeightHint, this));
    inputList->setItemDelegate(new ProxyDelegate());
    outputList->setModel(new CfgListModel(comboboxHeightHint, this));
    outputList->setItemDelegate(new ProxyDelegate());

    attributeTable->setModel(new CfgTableModel());
    attributeTable->setItemDelegate(new ProxyDelegate());

    errorBox->hide();

    connect(addInputButton, SIGNAL(clicked()), SLOT(sl_addInputClicked()));
    connect(addOutputButton, SIGNAL(clicked()), SLOT(sl_addOutputClicked()));
    connect(addAttributeButton, SIGNAL(clicked()), SLOT(sl_addAttribute()));

    connect(deleteInputButton, SIGNAL(clicked()), SLOT(sl_deleteInputClicked()));
    connect(deleteOutputButton, SIGNAL(clicked()), SLOT(sl_deleteOutputClicked()));
    connect(deleteAttributeButton, SIGNAL(clicked()), SLOT(sl_deleteAttributeClicked()));

    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    connect(fileButton, SIGNAL(clicked()), SLOT(sl_getDirectory()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_okClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_cancelClicked()));

    attributeTable->horizontalHeader()->setStretchLastSection(true);
    attributeTable->verticalHeader()->hide();

    nameEdit->setValidator(new DeprecatedWorkerNameValidator(this));

    if(proto) {
        fillFields(proto);
        editing = true;
    }

}

static DataTypePtr getDatatypeOfSlotDesc(const Descriptor & dt) {
    QString dtId = dt.getId();
    if(dtId == BaseSlots::DNA_SEQUENCE_SLOT().getId()) {
        return BaseTypes::DNA_SEQUENCE_TYPE();
    }
    if(dtId == BaseSlots::ANNOTATION_TABLE_SLOT().getId()) {
        return BaseTypes::ANNOTATION_TABLE_TYPE();
    }
    if(dtId == BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()) {
        return BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    }
    if(dtId == BaseSlots::TEXT_SLOT().getId()) {
        return BaseTypes::STRING_TYPE();
    }
    assert(false);
    return DataTypePtr();
}

void CreateScriptElementDialog::fillFields(ActorPrototype *proto) {
    proto->getPortDesciptors();
    int inputInd = 0;
    int outputInd = 0;
    foreach(const PortDescriptor *desc, proto->getPortDesciptors()) {
        if(desc->isInput()) {
            inputList->model()->insertRows(0,desc->getType()->getAllDescriptors().size() - 1, QModelIndex());
            foreach(const Descriptor &d, desc->getType()->getAllDescriptors()) {
                QModelIndex mi = inputList->model()->index(inputInd,0);
                inputList->model()->setData(mi,getDatatypeOfSlotDesc(d)->getId());
                inputInd++;
            }

        } else {
            outputList->model()->insertRows(0,desc->getType()->getAllDescriptors().size() - 1, QModelIndex());
            foreach(const Descriptor &d, desc->getType()->getAllDescriptors()) {
                QModelIndex mi = outputList->model()->index(outputInd,0);
                outputList->model()->setData(mi,getDatatypeOfSlotDesc(d)->getId());
                outputInd++;
            }
        }
    }

    int ind = 0;
    foreach(const Attribute*attr, proto->getAttributes()) {
        attributeTable->model()->insertRow(1, QModelIndex());
        QModelIndex mi1 = attributeTable->model()->index(ind,0);
        QModelIndex mi2 = attributeTable->model()->index(ind,1);
        attributeTable->model()->setData(mi1, attr->getId());
        attributeTable->model()->setData(mi2, attr->getAttributeType()->getId());
        ind++;
    }

    nameEdit->setText(proto->getDisplayName());
    descriptionEdit->setText(proto->getDocumentation());
}

void CreateScriptElementDialog::sl_getDirectory() {
    QString url = WorkflowSettings::getUserDirectory();

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::List);
    dialog.setDirectory(url);
    if(dialog.exec() == QDialog::Accepted) {
        QString dir = dialog.selectedFiles().first();
        fileEdit->setText(dir);
    }
}

void CreateScriptElementDialog::sl_addInputClicked() {
    inputList->model()->insertRow(0,QModelIndex());
}

void CreateScriptElementDialog::sl_addOutputClicked() {
    outputList->model()->insertRow(0,QModelIndex());
}

void CreateScriptElementDialog::sl_addAttribute() {
    attributeTable->model()->insertRow(0,QModelIndex());
}

void CreateScriptElementDialog::sl_deleteInputClicked() {
    QModelIndex index = inputList->currentIndex();
    inputList->model()->removeRow(index.row());
}

void CreateScriptElementDialog::sl_deleteOutputClicked() {
    QModelIndex index = outputList->currentIndex();
    outputList->model()->removeRow(index.row());
}

void CreateScriptElementDialog::sl_deleteAttributeClicked() {
    QModelIndex index = attributeTable->currentIndex();
    attributeTable->model()->removeRow(index.row());
}

void CreateScriptElementDialog::sl_cancelClicked() {
    reject();
}

void CreateScriptElementDialog::sl_okClicked() {
    CfgListModel * inputPorts = static_cast<CfgListModel*>(inputList->model());
    QList<QString> typeIds = inputPorts->getItems();
    DataTypeRegistry *dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    input.clear();
    foreach(const QString & id, typeIds) {
        DataTypePtr ptr = dtr->getById(id);
        if(input.contains(ptr)) {
            QMessageBox::critical(this, tr("error"), tr("Two identical types for input port"));
            coreLog.error(tr("two identical types for input port"));
            return;
        }
        input << ptr;
    }

    CfgListModel * outputPorts = static_cast<CfgListModel*>(outputList->model());
    typeIds = outputPorts->getItems();
    assert(dtr);
    output.clear();
    foreach(const QString & id, typeIds) {
        DataTypePtr ptr = dtr->getById(id);
        if(output.contains(ptr)) {
            QMessageBox::critical(this, tr("error"),tr("Two identical types for output port"));
            coreLog.error(tr("two identical types for output port"));
            return;
        }
        output << ptr;
    }

    CfgTableModel * attrTableModel = static_cast<CfgTableModel*>(attributeTable->model());
    QList<CfgListItem*> attributes = attrTableModel->getItems();
    attrs.clear();
    foreach(CfgListItem *item, attributes) {
        QString itemName = item->getName();
        if(itemName.isEmpty()) {
            QMessageBox::critical(this, tr("error"),tr("Name for some attributes is empty"));
            coreLog.error(tr("Name for some attributes is empty"));
            return;
        }
        foreach(const Attribute *attr, attrs) {
            if(attr->getId() == itemName) {
                QMessageBox::critical(this, tr("error"),tr("Two attributes with name %1").arg(itemName));
                coreLog.error(tr("Two attributes with name %1").arg(itemName));
                return;
            }
        }

        DataTypePtr ptr = dtr->getById(item->getDataType());
        Descriptor desc(itemName, itemName, ptr->getDisplayName());
        if(ptr == BaseTypes::BOOL_TYPE()) {
            attrs << new Attribute(desc, ptr, false, QVariant(false));
        }
        else {
            attrs << new Attribute(desc, ptr);
        }
    }

    name = nameEdit->text();
    if(name.isEmpty()) {
        QMessageBox::critical(this, tr("error"),tr("Name for block is empty"));
        coreLog.error(tr("Name for block is empty"));
        return;
    }

    ActorPrototypeRegistry *pr = WorkflowEnv::getProtoRegistry();
    if(pr) {
        if(pr->getProto(LocalWorkflow::ScriptWorkerFactory::ACTOR_ID+name) && !editing) {
            QMessageBox::critical(this, tr("error"),tr("Actor with this name already registered"));
            coreLog.error(tr("Actor with this name already registered"));
            return;
        }
    }

    description = descriptionEdit->toPlainText();
    if(description.isEmpty()) {
        QMessageBox::critical(this, tr("error"),tr("Description for block is empty"));
        coreLog.error(tr("Description for block is empty"));
        return;
    }

    if(!fileEdit->text().isEmpty()) {
        changeDirectoryForActors();
    }

    if(!saveParams()) {
        errorBox->show();
        return;
    }

    accept();
}

void CreateScriptElementDialog::changeDirectoryForActors() {
    QString url = WorkflowSettings::getUserDirectory();
    QString newUrl = fileEdit->text() + "/";

    if(url != newUrl) {
        WorkflowSettings::setUserDirectory(newUrl);

        QDir dir(url);
        if(!dir.exists()) {
            //coreLog.info(tr("There isn't folder with users workflow elements"));
            return;
        }
        dir.setNameFilters(QStringList() << "*.usa");
        QFileInfoList fileList = dir.entryInfoList();
        foreach(const QFileInfo &fileInfo, fileList) {
            QString newFileUrl = newUrl + fileInfo.fileName();
            QFile::copy(fileInfo.filePath(), newFileUrl);
        }
    }
}

bool CreateScriptElementDialog::saveParams() {
    QDomDocument doc = saveXml();
    QString url = WorkflowSettings::getUserDirectory();
    QDir dir(url);
    if(!dir.exists()) {
        dir.mkpath(url);
    }

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    IOAdapter *io = iof->createIOAdapter();
    actorFilePath = url + name + ".usa";
    if(io->open(actorFilePath, IOAdapterMode_Write)) {
        io->writeBlock(doc.toByteArray());
        io->close();
        return true;
    }
    else {
        coreLog.error(tr("Can't save user's workflow element"));
        return false;
    }
}

QDomDocument CreateScriptElementDialog::saveXml() {
    QDomDocument xml(WORKFLOW_DOC);
    QDomElement actor = xml.createElement(ACTOR_ELEMENT);
    xml.appendChild(actor);

    CfgListModel * inputPorts = static_cast<CfgListModel*>(inputList->model());
    QList<QString> typeIds = inputPorts->getItems();
    QDomElement inputPort = xml.createElement(INPUT_PORT_ELEMENT);
    actor.appendChild(inputPort);
    foreach(const QString & str, typeIds) {
        QDomElement slot = xml.createElement(IN_SLOT_ELEMENT);
        slot.setAttribute(SLOT_ID, str);
        inputPort.appendChild(slot);
    }

    CfgListModel * outputPorts = static_cast<CfgListModel*>(outputList->model());
    typeIds = outputPorts->getItems();
    QDomElement outputPort = xml.createElement(OUTPUT_PORT_ELEMENT);
    actor.appendChild(outputPort);
    foreach(const QString & str, typeIds) {
        QDomElement slot = xml.createElement(OUT_SLOT_ELEMENT);
        slot.setAttribute(SLOT_ID, str);
        outputPort.appendChild(slot);
    }

    CfgTableModel * attrTableModel = static_cast<CfgTableModel*>(attributeTable->model());
    QList<CfgListItem*> attributes = attrTableModel->getItems();
    QDomElement attribute = xml.createElement(ATTRIBUTE_ELEMENT);
    actor.appendChild(attribute);
    foreach(CfgListItem *item, attributes) {
        QString itemName = item->getName();
        QString itemId = item->getDataType();
        QDomElement attr = xml.createElement(ATTR_ELEMENT);
        attr.setAttribute(NAME_ID, itemName);
        attr.setAttribute(TYPE_ID, itemId);
        attribute.appendChild(attr);
    }

    QDomElement nameEl = xml.createElement(NAME_ELEMENT);
    nameEl.setAttribute(NAME_ID, name);
    actor.appendChild(nameEl);

    QDomElement descriptionEl = xml.createElement(DESCR_ELEMENT);
    descriptionEl.setAttribute(DESCR_ID, description);
    actor.appendChild(descriptionEl);

    return xml;
}

QList<DataTypePtr > CreateScriptElementDialog::getInput() const{
    return input;
}
QList<DataTypePtr > CreateScriptElementDialog::getOutput() const{
    return output;
}
QList<Attribute *> CreateScriptElementDialog::getAttributes() const {
    return attrs;
}
const QString CreateScriptElementDialog::getName() const {
    return name;
}

const QString CreateScriptElementDialog::getDescription() const {
    return description;
}

const QString CreateScriptElementDialog::getActorFilePath() const {
    return actorFilePath;
}

} //namespace
