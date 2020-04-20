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

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowUtils.h>

#include "CfgExternalToolModel.h"
#include "../WorkflowEditorDelegates.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// CfgExternalToolModel
//////////////////////////////////////////////////////////////////////////

CfgExternalToolItem::CfgExternalToolItem()  {
    dfr = AppContext::getDocumentFormatRegistry();
    dtr = Workflow::WorkflowEnv::getDataTypeRegistry();

    delegateForNames = nullptr;
    delegateForIds = nullptr;
    delegateForTypes = nullptr;
    delegateForFormats = nullptr;
    itemData.type = BaseTypes::DNA_SEQUENCE_TYPE()->getId();
    itemData.format = BaseDocumentFormats::FASTA;
}

CfgExternalToolItem::~CfgExternalToolItem() {
    delete delegateForNames;
    delete delegateForIds;
    delete delegateForTypes;
    delete delegateForFormats;
}

const QString &CfgExternalToolItem::getDataType() const {
    return itemData.type;
}

void CfgExternalToolItem::setDataType(const QString& id) {
    itemData.type = id;
}

const QString &CfgExternalToolItem::getId() const {
    return itemData.attributeId;
}

void CfgExternalToolItem::setId(const QString &_id) {
    itemData.attributeId = _id;
}

const QString &CfgExternalToolItem::getName() const {
    return itemData.attrName;
}

void CfgExternalToolItem::setName(const QString &_name) {
    itemData.attrName = _name;
}

const QString &CfgExternalToolItem::getFormat() const {
    return itemData.format;
}

void CfgExternalToolItem::setFormat(const QString & f) {
    itemData.format = f;
}

const QString &CfgExternalToolItem::getDescription() const {
    return itemData.description;
}

void CfgExternalToolItem::setDescription(const QString & _descr) {
    itemData.description = _descr;
}

//////////////////////////////////////////////////////////////////////////
/// CfgExternalToolModel
//////////////////////////////////////////////////////////////////////////

CfgExternalToolModel::CfgExternalToolModel(CfgExternalToolModel::ModelType _modelType, QObject *_obj)
    : QAbstractTableModel(_obj),
      isInput(Input == _modelType)
{
    init();
}

int CfgExternalToolModel::rowCount(const QModelIndex & /*index*/) const{
    return items.size();
}

int CfgExternalToolModel::columnCount(const QModelIndex & /*index*/) const {
    return COLUMNS_COUNT;
}

Qt::ItemFlags CfgExternalToolModel::flags(const QModelIndex & /*index*/) const{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

CfgExternalToolItem* CfgExternalToolModel::getItem(const QModelIndex &index) const {
    return items.at(index.row());
}

QList<CfgExternalToolItem*> CfgExternalToolModel::getItems() const {
    return items;
}

QVariant CfgExternalToolModel::data(const QModelIndex &index, int role) const {
    CfgExternalToolItem *item = getItem(index);
    int col = index.column();

    switch (role) {
    case Qt::DisplayRole: // fallthrough
    case Qt::ToolTipRole:
        switch (col) {
        case COLUMN_NAME:
            return item->getName();
        case COLUMN_ID:
            return item->getId();
        case COLUMN_DATA_TYPE:
            return item->delegateForTypes->getDisplayValue(item->getDataType());
        case COLUMN_FORMAT:
            return item->delegateForFormats->getDisplayValue(item->getFormat());
        case COLUMN_DESCRIPTION:
            return item->getDescription();
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
            return QVariant();
        }
    case DelegateRole:
        switch (col) {
        case COLUMN_NAME:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForNames);
        case COLUMN_ID:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForIds);
        case COLUMN_DATA_TYPE:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForTypes);
        case COLUMN_FORMAT:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForFormats);
        default:
            return QVariant();
        }
    case Qt::EditRole: // fallthrough
    case ConfigurationEditor::ItemValueRole:
        switch (col) {
        case COLUMN_NAME:
            return item->getName();
        case COLUMN_ID:
            return item->getId();
        case COLUMN_DATA_TYPE:
            return item->getDataType();
        case COLUMN_FORMAT:
            return item->getFormat();
        case COLUMN_DESCRIPTION:
            return item->getDescription();
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
            return QVariant();
        }
    default:
        return QVariant();
    }
}

void CfgExternalToolModel::createFormatDelegate(const QString &newType, CfgExternalToolItem *item) {
    PropertyDelegate *delegate;
    QString format;
    if (newType == BaseTypes::DNA_SEQUENCE_TYPE()->getId()) {
        delegate = new ComboBoxDelegate(seqFormatsW);
        format = seqFormatsW.values().first().toString();
    } else if (newType == BaseTypes::MULTIPLE_ALIGNMENT_TYPE()->getId()) {
        delegate = new ComboBoxDelegate(msaFormatsW);
        format = msaFormatsW.values().first().toString();
    } else if (newType == BaseTypes::ANNOTATION_TABLE_TYPE()->getId()) {
        delegate = new ComboBoxDelegate(annFormatsW);
        format = annFormatsW.values().first().toString();
    } else if (newType == SEQ_WITH_ANNS){
        delegate = new ComboBoxDelegate(annSeqFormatsW);
        format = annSeqFormatsW.values().first().toString();
    } else if (newType == BaseTypes::STRING_TYPE()->getId()) {
        delegate = new ComboBoxDelegate(textFormat);
        format = textFormat.values().first().toString();
    } else {
        return;
    }
    item->setFormat(format);
    item->delegateForFormats = delegate;
}

bool CfgExternalToolModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    int col = index.column();
    CfgExternalToolItem * item = getItem(index);
    switch (role) {
    case Qt::EditRole: // fall through
    case ConfigurationEditor::ItemValueRole:
        switch (col) {
        case COLUMN_NAME:
            if (item->getName() != value.toString()) {
                const QString oldGeneratedId = WorkflowUtils::generateIdFromName(item->getName());
                const bool wasIdEditedByUser = (oldGeneratedId != item->getId());
                item->setName(value.toString());
                if (!wasIdEditedByUser) {
                    item->setId(WorkflowUtils::generateIdFromName(item->getName()));
                }
            }
        break;
        case COLUMN_ID:
            if (item->getId() != value.toString()) {
                item->setId(value.toString());
            }
            break;
        case COLUMN_DATA_TYPE: {
            QString newType = value.toString();
            if (item->getDataType() != newType) {
                if (!newType.isEmpty()) {
                    item->setDataType(newType);
                    createFormatDelegate(newType, item);
                }
            }
            break;
        }
        case COLUMN_FORMAT:
            if (item->getFormat() != value.toString() && !value.toString().isEmpty())  {
                item->setFormat(value.toString());
            }
            break;
        case COLUMN_DESCRIPTION:
            if (item->getDescription() != value.toString()) {
                item->setDescription(value.toString());
            }
            break;
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
        }
        emit dataChanged(index, index);
        break;
    default:
        ; // do nothing
    }
    return true;
}

QVariant CfgExternalToolModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case COLUMN_NAME:
            return tr("Display name");
        case COLUMN_ID:
            return tr("Argument name");
        case COLUMN_DATA_TYPE:
            return tr("Type");
        case COLUMN_FORMAT:
            if (isInput) {
                return tr("Argument value");
            } else {
                return tr("Argument value");
            }
        case COLUMN_DESCRIPTION:
            return tr("Description");
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
            return QVariant();
        }
    }
    return QVariant();
}

bool CfgExternalToolModel::insertRows(int /*row*/, int /*count*/, const QModelIndex &parent) {
    beginInsertRows(parent, items.size(), items.size());
    CfgExternalToolItem *newItem = new CfgExternalToolItem();
    newItem->delegateForNames = new LineEditWithValidatorDelegate(WorkflowEntityValidator::ACCEPTABLE_NAME);
    newItem->delegateForIds = new LineEditWithValidatorDelegate(WorkflowEntityValidator::ACCEPTABLE_ID);
    newItem->delegateForTypes = new ComboBoxDelegate(types);
    newItem->delegateForFormats = new ComboBoxDelegate(seqFormatsW);
    items.append(newItem);
    endInsertRows();
    return true;
}

bool CfgExternalToolModel::removeRows(int row, int count, const QModelIndex &parent) {
    CHECK(0 <= row && row < items.size(), false);
    CHECK(0 <= row + count - 1 && row + count - 1 < items.size(), false);
    CHECK(0 < count, false);

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row + count - 1; i >= row; --i) {
        delete items.takeAt(i);
    }
    endRemoveRows();
    return true;
}

void CfgExternalToolModel::init() {
    initTypes();
    initFormats();
}

void CfgExternalToolModel::initFormats() {
    QList<DocumentFormatId> ids = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();

    DocumentFormatConstraints commonConstraints;
    commonConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    commonConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    commonConstraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);

    DocumentFormatConstraints seqWrite(commonConstraints);
    seqWrite.supportedObjectTypes += GObjectTypes::SEQUENCE;

    DocumentFormatConstraints seqRead(commonConstraints);
    seqRead.supportedObjectTypes += GObjectTypes::SEQUENCE;

    DocumentFormatConstraints msaWrite(commonConstraints);
    msaWrite.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;

    DocumentFormatConstraints msaRead(commonConstraints);
    msaRead.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;

    DocumentFormatConstraints annWrite(commonConstraints);
    annWrite.supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;

    DocumentFormatConstraints annRead(commonConstraints);
    annRead.supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;

    DocumentFormatConstraints annSeqWrite(commonConstraints);
    annSeqWrite.supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
    annSeqWrite.supportedObjectTypes += GObjectTypes::SEQUENCE;

    DocumentFormatConstraints annSeqRead(commonConstraints);
    annSeqRead.supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
    annSeqRead.supportedObjectTypes += GObjectTypes::SEQUENCE;

    QString argumentValue(tr("URL to %1 file with data"));
    foreach(const DocumentFormatId& id, ids) {
        DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(id);

        QString formatNameKey = argumentValue.arg(df->getFormatName());
        QString formatId = df->getFormatId();
        if (df->checkConstraints(seqWrite)) {
            seqFormatsW[formatNameKey] = formatId;
        }

        if (df->checkConstraints(seqRead)) {
            seqFormatsR[formatNameKey] = formatId;
        }

        if (df->checkConstraints(msaWrite)) {
            msaFormatsW[formatNameKey] = formatId;
        }

        if (df->checkConstraints(msaRead)) {
            msaFormatsR[formatNameKey] = formatId;
        }

        if (df->checkConstraints(annWrite)) {
            annFormatsW[formatNameKey] = formatId;
        }

        if (df->checkConstraints(annRead)) {
            annFormatsR[formatNameKey] = formatId;
        }

        if (df->checkConstraints(annSeqWrite)) {
            annSeqFormatsW[formatNameKey] = formatId;
        }

        if (df->checkConstraints(annSeqRead)) {
            annSeqFormatsR[formatNameKey] = formatId;
        }
    }

    DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_TEXT);
    if (isInput) {
        textFormat[tr("String data value")] = DataConfig::STRING_VALUE;
    } else {
        textFormat[tr("Output URL")] = DataConfig::OUTPUT_FILE_URL;
    }
    textFormat[argumentValue.arg("TXT")] = df->getFormatId();
}

void CfgExternalToolModel::initTypes() {
    DataTypePtr ptr = BaseTypes::DNA_SEQUENCE_TYPE();
    types[ptr->getDisplayName()] = ptr->getId();

    ptr = BaseTypes::ANNOTATION_TABLE_TYPE();
    types[tr("Annotations")] = ptr->getId();

    ptr = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    types[tr("Alignment")] = ptr->getId();

    ptr = BaseTypes::STRING_TYPE();
    types[ptr->getDisplayName()] = ptr->getId();

    types[tr("Annotated sequence")] = SEQ_WITH_ANNS;
}

//////////////////////////////////////////////////////////////////////////
/// AttributeItem
//////////////////////////////////////////////////////////////////////////

AttributeItem::AttributeItem()
    : delegateForNames(nullptr),
      delegateForIds(nullptr),
      delegateForDefaultValues(nullptr)
{

}

AttributeItem::~AttributeItem() {
    delete delegateForNames;
    delete delegateForIds;
    delete delegateForDefaultValues;
}

const QString &AttributeItem::getId() const {
    return id;
}

void AttributeItem::setId(const QString &_id) {
    id = _id;
}

const QString &AttributeItem::getName() const {
    return name;
}

void AttributeItem::setName(const QString& _name) {
    name = _name;
}

const QString &AttributeItem::getDataType() const {
    return type;
}

void AttributeItem::setDataType(const QString &_type) {
    type = _type;
}

const QVariant&AttributeItem::getDefaultValue() const {
    return defaultValue;
}

void AttributeItem::setDefaultValue(const QVariant&_defaultValue) {
    defaultValue = _defaultValue;
}

const QString &AttributeItem::getDescription() const {
    return description;
}

void AttributeItem::setDescription(const QString &_description) {
    description = _description;
}

//////////////////////////////////////////////////////////////////////////
/// CfgExternalToolModelAttributes
//////////////////////////////////////////////////////////////////////////

CfgExternalToolModelAttributes::CfgExternalToolModelAttributes(SchemaConfig* _schemaConfig, QObject *_parent)
    : QAbstractTableModel(_parent),
    schemaConfig(_schemaConfig)
{
    types.append(QPair<QString, QVariant>(tr("Boolean"), AttributeConfig::BOOLEAN_TYPE));
    types.append(QPair<QString, QVariant>(tr("Integer"), AttributeConfig::INTEGER_TYPE));
    types.append(QPair<QString, QVariant>(tr("Double"), AttributeConfig::DOUBLE_TYPE));
    types.append(QPair<QString, QVariant>(tr("String"), AttributeConfig::STRING_TYPE));
    types.append(QPair<QString, QVariant>(tr("Input file URL"), AttributeConfig::INPUT_FILE_URL_TYPE));
    types.append(QPair<QString, QVariant>(tr("Input folder URL"), AttributeConfig::INPUT_FOLDER_URL_TYPE));
    types.append(QPair<QString, QVariant>(tr("Output file URL"), AttributeConfig::OUTPUT_FILE_URL_TYPE));
    types.append(QPair<QString, QVariant>(tr("Output folder URL"), AttributeConfig::OUTPUT_FOLDER_URL_TYPE));
    typesDelegate = new ComboBoxDelegate(types);
}

CfgExternalToolModelAttributes::~CfgExternalToolModelAttributes() {
    foreach(AttributeItem* item, items) {
        delete item;
    }
}

void CfgExternalToolModelAttributes::changeDefaultValueDelegate(const QString& newType, AttributeItem* item) {
    PropertyDelegate* propDelegate = nullptr;
    QVariant defaultValue;
    if (newType == AttributeConfig::BOOLEAN_TYPE) {
        propDelegate = new ComboBoxWithBoolsDelegate();
        defaultValue = true;
    } else if (newType == AttributeConfig::STRING_TYPE) {
        propDelegate = new LineEditWithValidatorDelegate(QRegularExpression("([^\"]*)"));
    } else if (newType == AttributeConfig::INTEGER_TYPE) {
        QVariantMap integerValues;
        integerValues["minimum"] = QVariant(std::numeric_limits<int>::min());
        integerValues["maximum"] = QVariant(std::numeric_limits<int>::max());
        propDelegate = new SpinBoxDelegate(integerValues);
        defaultValue = QVariant(0);
    } else if (newType == AttributeConfig::DOUBLE_TYPE) {
        QVariantMap doubleValues;
        doubleValues["singleStep"] = 0.1;
        doubleValues["minimum"] = QVariant(std::numeric_limits<double>::lowest());
        doubleValues["maximum"] = QVariant(std::numeric_limits<double>::max());
        doubleValues["decimals"] = 6;
        propDelegate = new DoubleSpinBoxDelegate(doubleValues);
        defaultValue = QVariant(0.0);
    } else if (newType == AttributeConfig::INPUT_FILE_URL_TYPE) {
        propDelegate = new URLDelegate("", "", false, false, false, nullptr, "", false, true);
    } else if (newType == AttributeConfig::OUTPUT_FILE_URL_TYPE) {
        propDelegate = new URLDelegate("", "", false, false, true, nullptr, "", false, false);
    } else if (newType == AttributeConfig::INPUT_FOLDER_URL_TYPE) {
        propDelegate = new URLDelegate("", "", false, true, false, nullptr, "", false, true);
    } else if (newType == AttributeConfig::OUTPUT_FOLDER_URL_TYPE) {
        propDelegate = new URLDelegate("", "", false, true, true, nullptr, "", false, false);
    } else {
        return;
    }

    propDelegate->setSchemaConfig(schemaConfig);
    item->setDefaultValue(defaultValue);
    delete item->delegateForDefaultValues;
    item->delegateForDefaultValues = propDelegate;
}

int CfgExternalToolModelAttributes::rowCount(const QModelIndex & /*index*/) const{
    return items.size();
}

int CfgExternalToolModelAttributes::columnCount(const QModelIndex & /*index*/) const {
    return COLUMNS_COUNT;
}

Qt::ItemFlags CfgExternalToolModelAttributes::flags(const QModelIndex & /*index*/) const{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

AttributeItem* CfgExternalToolModelAttributes::getItem(const QModelIndex &index) const {
    return items.at(index.row());
}

QList<AttributeItem*> CfgExternalToolModelAttributes::getItems() const {
    return items;
}

QVariant CfgExternalToolModelAttributes::data(const QModelIndex &index, int role) const {
    AttributeItem *item = getItem(index);
    int col = index.column();

    switch (role) {
    case Qt::DisplayRole: // fallthrough
    case Qt::ToolTipRole:
        switch (col) {
        case COLUMN_NAME:
            return item->getName();
        case COLUMN_ID:
            return item->getId();
        case COLUMN_DATA_TYPE:
            return typesDelegate->getDisplayValue(item->getDataType());
        case COLUMN_DEFAULT_VALUE:
            return item->delegateForDefaultValues->getDisplayValue(item->getDefaultValue());
        case COLUMN_DESCRIPTION:
            return item->getDescription();
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
            return QVariant();
        }
    case DelegateRole:
        switch (col) {
        case COLUMN_NAME:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForNames);
        case COLUMN_ID:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForIds);
        case COLUMN_DATA_TYPE:
            return qVariantFromValue<PropertyDelegate*>(typesDelegate);
        case COLUMN_DEFAULT_VALUE:
            return qVariantFromValue<PropertyDelegate*>(item->delegateForDefaultValues);
        default:
            return QVariant();
        }
    case Qt::EditRole: // fallthrough
    case ConfigurationEditor::ItemValueRole:
        switch (col) {
        case COLUMN_NAME:
            return item->getName();
        case COLUMN_ID:
            return item->getId();
        case COLUMN_DATA_TYPE:
            return item->getDataType();
        case COLUMN_DEFAULT_VALUE:
            return item->getDefaultValue();
        case COLUMN_DESCRIPTION:
            return item->getDescription();
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
            return QVariant();
        }
    default:
        return QVariant();
    }
}

bool CfgExternalToolModelAttributes::setData(const QModelIndex &index, const QVariant &value, int role) {
    int col = index.column();
    AttributeItem * item = getItem(index);
    switch (role) {
    case Qt::EditRole: // fallthrough
    case ConfigurationEditor::ItemValueRole:
        switch (col) {
        case COLUMN_NAME:
            if (item->getName() != value.toString()) {
                const QString oldGeneratedId = WorkflowUtils::generateIdFromName(item->getName());
                const bool wasIdEditedByUser = (oldGeneratedId != item->getId());
                item->setName(value.toString());
                if (!wasIdEditedByUser) {
                    item->setId(WorkflowUtils::generateIdFromName(item->getName()));
                }
            }
            break;
        case COLUMN_ID:
            if (item->getId() != value.toString()) {
                item->setId(value.toString());
            }
            break;
        case COLUMN_DATA_TYPE: {
            QString newType = value.toString();
            if (item->getDataType() != newType) {
                if (!newType.isEmpty()) {
                    item->setDataType(newType);
                    changeDefaultValueDelegate(newType, item);
                }
            }
            break;
        }
        case COLUMN_DEFAULT_VALUE: {
            if (item->getDefaultValue() != value.toString()) {
                item->setDefaultValue(value.toString());
            }
            break;
        }
        case COLUMN_DESCRIPTION:
            if (item->getDescription() != value.toString()) {
                item->setDescription(value.toString());
            }
            break;
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
        }

        emit dataChanged(index, index);
        break;
    default:
        ; // do nothing
    }
    return true;
}

QVariant CfgExternalToolModelAttributes::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case COLUMN_NAME:
            return tr("Display name");
        case COLUMN_ID:
            return tr("Argument name");
        case COLUMN_DATA_TYPE:
            return tr("Type");
        case COLUMN_DEFAULT_VALUE:
            return tr("Default value");
        case COLUMN_DESCRIPTION:
            return tr("Description");
        default:
            // do nothing, inaccessible code
            Q_ASSERT(false);
            return QVariant();
        }
    }
    return QVariant();
}

bool CfgExternalToolModelAttributes::insertRows(int /*row*/, int /*count*/, const QModelIndex & parent)  {
    beginInsertRows(parent, items.size(), items.size());
    AttributeItem *newItem = new AttributeItem();
    newItem->delegateForNames = new LineEditWithValidatorDelegate(WorkflowEntityValidator::ACCEPTABLE_NAME);
    newItem->delegateForIds = new LineEditWithValidatorDelegate(WorkflowEntityValidator::ACCEPTABLE_ID);
    newItem->setDataType(AttributeConfig::STRING_TYPE);
    changeDefaultValueDelegate(newItem->getDataType(), newItem);
    items.append(newItem);
    endInsertRows();
    return true;
}

bool CfgExternalToolModelAttributes::removeRows(int row, int count, const QModelIndex & parent) {
    CHECK(0 <= row && row < items.size(), false);
    CHECK(0 <= row + count - 1 && row + count - 1 < items.size(), false);
    CHECK(0 < count, false);

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row + count - 1; i >= row; --i) {
        delete items.takeAt(i);
    }
    endRemoveRows();
    return true;
}

} // U2
