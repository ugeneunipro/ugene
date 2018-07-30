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

#include <U2Lang/Dataset.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowUtils.h>

#include "BaseTypes.h"

namespace U2 {

static const QString DNA_SEQUENCE_TYPE_ID("seq");
static const QString ANNOTATION_TABLE_LIST_TYPE_ID("ann-table-list");
static const QString ANNOTATION_TABLE_TYPE_ID("ann_table");
static const QString MULTIPLE_ALIGNMENT_TYPE_ID("malignment");
static const QString VARIATION_TRACK_TYPE_ID("variation");
static const QString ASSEMBLY_TYPE_ID("assembly");
static const QString STRING_TYPE_ID("string");
static const QString STRING_LIST_TYPE_ID("string-list");
static const QString BOOL_TYPE_ID("bool");
static const QString MAP_TYPE_ID("map");
static const QString NUM_TYPE_ID("number");
static const QString URL_DATASETS_TYPE_ID("url-datasets");
static const QString ANY_TYPE_ID("void");

using namespace Workflow;

DataTypePtr BaseTypes::DNA_SEQUENCE_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new DataType(DNA_SEQUENCE_TYPE_ID, tr("Sequence"), tr("A sequence"))));
        startup = false;
    }
    return dtr->getById(DNA_SEQUENCE_TYPE_ID);
}

DataTypePtr BaseTypes::ANNOTATION_TABLE_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new DataType(ANNOTATION_TABLE_TYPE_ID, tr("Set of annotations"), tr("A set of annotated features in a sequence"))));
        startup = false;
    }
    return dtr->getById(ANNOTATION_TABLE_TYPE_ID);
}

DataTypePtr BaseTypes::ANNOTATION_TABLE_LIST_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new ListDataType(
            Descriptor(ANNOTATION_TABLE_LIST_TYPE_ID, tr("List of annotations"), tr("A list of sequence annotations")),
            BaseTypes::ANNOTATION_TABLE_TYPE())));
        startup = false;
    }
    return dtr->getById(ANNOTATION_TABLE_LIST_TYPE_ID);
}

DataTypePtr BaseTypes::MULTIPLE_ALIGNMENT_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new DataType(MULTIPLE_ALIGNMENT_TYPE_ID, tr("Multiple alignment"), tr("Set of aligned sequences"))));
        startup = false;
    }
    return dtr->getById(MULTIPLE_ALIGNMENT_TYPE_ID);
}

DataTypePtr BaseTypes::VARIATION_TRACK_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new DataType(VARIATION_TRACK_TYPE_ID, tr("Variation track"), tr("Set of variations"))));
        startup = false;
    }
    return dtr->getById(VARIATION_TRACK_TYPE_ID);
}

DataTypePtr BaseTypes::ASSEMBLY_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        dtr->registerEntry(DataTypePtr(new DataType(ASSEMBLY_TYPE_ID, tr("Assembly data"), tr("Assembly data"))));
        startup = false;
    }
    return dtr->getById(ASSEMBLY_TYPE_ID);
}

DataTypePtr BaseTypes::STRING_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(STRING_TYPE_ID, tr("String"), tr("A string of characters"))));
        startup = false;
    }
    return dtr->getById(STRING_TYPE_ID);
}

DataTypePtr BaseTypes::STRING_LIST_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new ListDataType(STRING_LIST_TYPE_ID, BaseTypes::STRING_TYPE())));
        startup = false;
    }
    return dtr->getById(STRING_LIST_TYPE_ID);
}

DataTypePtr BaseTypes::BOOL_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(BOOL_TYPE_ID, tr("Boolean"), tr("A boolean value (true/false)"))));
        startup = false;
    }
    return dtr->getById(BOOL_TYPE_ID);
}

DataTypePtr BaseTypes::MAP_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup) {
        QMap<Descriptor, DataTypePtr> map;
        map.insert(Descriptor("A map of datatypes"), BaseTypes::STRING_TYPE());
        dtr->registerEntry(DataTypePtr(new MapDataType(MAP_TYPE_ID, map)));
        startup = false;
    }
    return dtr->getById(MAP_TYPE_ID);
}

DataTypePtr BaseTypes::NUM_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(NUM_TYPE_ID, tr("Number"), tr("A number"))));
        startup = false;
    }
    return dtr->getById(NUM_TYPE_ID);
}

DataTypePtr BaseTypes::ANY_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(ANY_TYPE_ID, tr("Undefined"), tr("A void type"))));
        startup = false;
    }
    return dtr->getById(ANY_TYPE_ID);
}

DataTypePtr BaseTypes::URL_DATASETS_TYPE() {
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(URL_DATASETS_TYPE_ID, tr("Url datasets"), tr("A list of urls grouped into datasets"))));
        startup = false;
    }
    return dtr->getById(URL_DATASETS_TYPE_ID);
}

U2DataType BaseTypes::toDataType(const QString &typeId) {
    if (typeId == DNA_SEQUENCE_TYPE()->getId()) {
        return U2Type::Sequence;
    } else if (typeId == ANNOTATION_TABLE_TYPE()->getId()) {
        return U2Type::AnnotationTable;
    } else if (typeId == MULTIPLE_ALIGNMENT_TYPE()->getId()) {
        return U2Type::Msa;
    } else if (typeId == VARIATION_TRACK_TYPE()->getId()) {
        return U2Type::VariantTrack;
    } else if (typeId == ASSEMBLY_TYPE()->getId()) {
        return U2Type::Assembly;
    } else if (typeId == STRING_TYPE()->getId()) {
        return U2Type::Text;
    } else {
        return U2Type::Unknown;
    }
}

QString BaseTypes::toTypeId(const U2DataType &dataType) {
    switch (dataType) {
    case U2Type::Sequence :
        return DNA_SEQUENCE_TYPE()->getId();
    case U2Type::AnnotationTable :
        return ANNOTATION_TABLE_TYPE()->getId();
    case U2Type::Msa :
        return MULTIPLE_ALIGNMENT_TYPE()->getId();
    case U2Type::VariantTrack :
        return VARIATION_TRACK_TYPE()->getId();
    case U2Type::Assembly :
        return ASSEMBLY_TYPE()->getId();
    case U2Type::Text :
        return STRING_TYPE()->getId();
    default:
        return ANY_TYPE()->getId();
    }
}

static void setIfNotNull( bool * to, bool val ) {
    if( to != NULL ) {
        *to = val;
    }
}

/****************************************
* StringTypeValueFactory
****************************************/
QVariant StringTypeValueFactory::getValueFromString( const QString & str, bool * ok ) const {
    setIfNotNull( ok, true );
    return qVariantFromValue( str );
}

/****************************************
* StringListTypeValueFactory
****************************************/
QVariant StringListTypeValueFactory::getValueFromString(const QString &str, bool *ok) const {
    setIfNotNull(ok, true);
    return QVariant::fromValue<QStringList>(StrPackUtils::unpackStringList(str, StrPackUtils::SingleQuotes));
}

/****************************************
* MapTypeValueFactory
****************************************/
QVariant MapTypeValueFactory::getValueFromString(const QString &str, bool *ok) const {
    setIfNotNull(ok, true);
    StrStrMap map = StrPackUtils::unpackMap(str, StrPackUtils::SingleQuotes);
    QVariantMap variantMap;
    foreach (const QString& key, map.keys()) {
        variantMap.insert(key, map.value(key));
    }
    return variantMap;
}

/****************************************
* BoolTypeValueFactory
****************************************/
const QString BoolTypeValueFactory::TRUE_STR    = "true";
const QString BoolTypeValueFactory::FALSE_STR   = "false";
const QString BoolTypeValueFactory::YES_STR     = "yes";
const QString BoolTypeValueFactory::NO_STR      = "no";
const QString BoolTypeValueFactory::ONE_STR     = "1";
const QString BoolTypeValueFactory::NIL_STR     = "0";

QVariant BoolTypeValueFactory::getValueFromString( const QString & s, bool * ok ) const {
    QString str = s.toLower();
    if( str == TRUE_STR || str == YES_STR || str == ONE_STR ) {
        setIfNotNull( ok, true );
        return qVariantFromValue( true );
    }
    if( str == FALSE_STR || str == NO_STR || str == NIL_STR ) {
        setIfNotNull( ok, true );
        return qVariantFromValue( false );
    }
    setIfNotNull( ok, false );
    return QVariant();
}

/****************************************
* NumTypeValueFactory
****************************************/
QVariant NumTypeValueFactory::getValueFromString( const QString & str, bool * okArg ) const {
    bool ok = false;

    qint64 longIntCandidate = str.toLongLong(&ok);
    if(ok) {
        setIfNotNull(okArg, true);
        return qVariantFromValue(longIntCandidate);
    }

    int intCandidate = str.toInt(&ok);
    if(ok) {
        setIfNotNull(okArg, true);
        return qVariantFromValue(intCandidate);
    }

    double doubleCandidate = str.toDouble(&ok);
    if(ok) {
        setIfNotNull(okArg, true);
        return qVariantFromValue(doubleCandidate);
    }

    setIfNotNull(okArg, false);
    return QVariant();
}

/************************************************************************/
/* UrlTypeValueFactory */
/************************************************************************/
QVariant UrlTypeValueFactory::getValueFromString(const QString &str, bool *ok) const {
    const QString splitter = WorkflowUtils::getDatasetSplitter(str);
    QStringList datasetStrs = str.split(splitter+splitter, QString::SkipEmptyParts);
    QList<Dataset> sets;
    int count = 0;
    foreach (const QString &datasetStr, datasetStrs) {
        QStringList urls = datasetStr.split(splitter, QString::SkipEmptyParts);
        count++;
        Dataset dSet(QString("Dataset %1").arg(count));
        foreach (const QString url, urls) {
            dSet.addUrl(URLContainerFactory::createUrlContainer(url));
        }
        sets << dSet;
    }
    *ok = true;
    return qVariantFromValue< QList<Dataset> >(sets);
}

QString UrlTypeValueFactory::getId() const {
    return BaseTypes::URL_DATASETS_TYPE()->getId();
}

} // U2
