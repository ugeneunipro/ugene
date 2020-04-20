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

#ifndef _CFG_EXTERNAL_TOOL_MODEL_H_
#define _CFG_EXTERNAL_TOOL_MODEL_H_

#include <QAbstractTableModel>

#include <U2Designer/PropertyWidget.h>

#include <U2Lang/ExternalToolCfg.h>

namespace U2 {

class DataTypeRegistry;
class DocumentFormatRegistry;
class PropertyDelegate;

class CfgExternalToolItem {
public:
    CfgExternalToolItem();
    ~CfgExternalToolItem();

    const QString &getDataType() const;
    void setDataType(const QString& typeId);

    const QString &getId() const;
    void setId(const QString &id);

    const QString &getName() const;
    void setName(const QString &name);

    const QString &getFormat() const;
    void setFormat(const QString &format);

    const QString &getDescription() const;
    void setDescription(const QString &descr);

    PropertyDelegate *delegateForNames;
    PropertyDelegate *delegateForIds;
    PropertyDelegate *delegateForTypes;
    PropertyDelegate *delegateForFormats;

    DataConfig itemData;

private:
    DocumentFormatRegistry *dfr;
    DataTypeRegistry *dtr;
};

class CfgExternalToolModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum ModelType {
        Input,
        Output
    };

    enum Columns {
        COLUMN_NAME = 0,
        COLUMN_ID = 1,
        COLUMN_DATA_TYPE = 2,
        COLUMN_FORMAT = 3,
        COLUMN_DESCRIPTION = 4,
        COLUMNS_COUNT = COLUMN_DESCRIPTION + 1   // elements count
    };

    CfgExternalToolModel(ModelType modelType, QObject *obj = NULL);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    CfgExternalToolItem* getItem(const QModelIndex &index) const;
    QList<CfgExternalToolItem*> getItems() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void createFormatDelegate(const QString &newType, CfgExternalToolItem *item);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    void init();
    void initFormats();
    void initTypes();

    bool isInput;
    QList<CfgExternalToolItem*> items;
    QVariantMap types;
    QVariantMap seqFormatsW;
    QVariantMap msaFormatsW;
    QVariantMap annFormatsW;
    QVariantMap annSeqFormatsW;
    QVariantMap seqFormatsR;
    QVariantMap msaFormatsR;
    QVariantMap annFormatsR;
    QVariantMap annSeqFormatsR;
    QVariantMap textFormat;
};

class AttributeItem {
public:
    AttributeItem();
    ~AttributeItem();

    const QString &getId() const;
    void setId(const QString &id);

    const QString &getName() const;
    void setName(const QString &name);

    const QString &getDataType() const;
    void setDataType(const QString &type);

    const QVariant&getDefaultValue() const;
    void setDefaultValue(const QVariant&defaultValue);

    const QString &getDescription() const;
    void setDescription(const QString &description);

    PropertyDelegate *delegateForNames;
    PropertyDelegate *delegateForIds;
    PropertyDelegate *delegateForDefaultValues;

private:
    QString id;
    QString name;
    QString type;
    QVariant defaultValue;
    QString description;
};

class CfgExternalToolModelAttributes : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Columns {
        COLUMN_NAME = 0,
        COLUMN_ID = 1,
        COLUMN_DATA_TYPE = 2,
        COLUMN_DEFAULT_VALUE = 3,
        COLUMN_DESCRIPTION = 4,
        COLUMNS_COUNT = COLUMN_DESCRIPTION + 1   // elements count
    };

    CfgExternalToolModelAttributes(SchemaConfig* schemaConfig, QObject *parent = nullptr);
    ~CfgExternalToolModelAttributes();

    void changeDefaultValueDelegate(const QString& newType, AttributeItem* item);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    AttributeItem* getItem(const QModelIndex &index) const;
    QList<AttributeItem*> getItems() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    QList<AttributeItem*> items;
    PropertyDelegate *typesDelegate;
    QList<ComboItem> types;
    SchemaConfig *schemaConfig;
};

} // U2

#endif // _CFG_EXTERNAL_TOOL_MODEL_H_
