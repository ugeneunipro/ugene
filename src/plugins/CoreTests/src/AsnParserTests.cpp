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

#include <U2Core/IOAdapter.h>
#include <U2Core/AppContext.h>
#include <U2Formats/ASNFormat.h>

#include "AsnParserTests.h"

namespace U2 {

#define VALUE_ATTR              "value"
#define URL_ATTR                "url"
#define INDEX_ATTR              "index"
#define ROOT_ATTR               "root-node"
#define NODE_ATTR               "node"
#define NAME_ATTR               "name"
#define TYPE_ATTR               "type"
#define CHILDREN_COUNT_ATTR     "children-count"

void GTest_LoadAsnTree::init(XMLTestFormat*, const QDomElement& el)
{
    rootElem = NULL;
    contextAdded = false;

    asnTreeContextName = el.attribute(INDEX_ATTR);
    if (asnTreeContextName.isEmpty()) {
        failMissingValue(INDEX_ATTR);
        return;
    }

    QString url = el.attribute(URL_ATTR);
    if (url.isEmpty()) {
        failMissingValue(URL_ATTR);
        return;
    }

    url = env->getVar("COMMON_DATA_DIR") + "/" + url;

    IOAdapterId         ioid = el.attribute("io");
    IOAdapterFactory*   iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioid);

    if (iof == NULL) {
        stateInfo.setError( QString("io_adapter_not_found_%1").arg(ioid));
        return;
    }

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());

    bool ok = io->open(url, IOAdapterMode_Read);
    if (!ok) {
        stateInfo.setError(QString("error_opening_url_for_read '%1'").arg(url));
        return;
    }

    ASNFormat::AsnParser asnParser(io.data(), stateInfo);
    rootElem = asnParser.loadAsnTree();
}

void GTest_LoadAsnTree::cleanup()
{
    if (contextAdded) {
        removeContext(asnTreeContextName);
        delete rootElem;
    }

    XmlTest::cleanup();
}

Task::ReportResult GTest_LoadAsnTree::report() {

    if (rootElem != NULL) {
        addContext(asnTreeContextName, rootElem);
        contextAdded = true;
    }

    return ReportResult_Finished;
}

//--------------------------------------------------------------------------------


void GTest_FindFirstNodeByName::init(XMLTestFormat*, const QDomElement& el)
{
    contextAdded = false;

    nodeContextName = el.attribute(INDEX_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(INDEX_ATTR);
        return;
    }

    rootContextName = el.attribute(ROOT_ATTR);
    if (rootContextName.isEmpty()) {
        failMissingValue(ROOT_ATTR);
        return;
    }

    nodeName = el.attribute(NAME_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(NAME_ATTR);
        return;
    }

}

void GTest_FindFirstNodeByName::cleanup()
{
    if (contextAdded) {
        removeContext(nodeContextName);
    }

    XmlTest::cleanup();
}

Task::ReportResult GTest_FindFirstNodeByName::report() {
    AsnNode* rootElem = getContext<AsnNode>(this, rootContextName);
    if( rootElem == NULL ){
        stateInfo.setError(QString("node is not in the context, wrong value %1").arg(rootContextName));
        return ReportResult_Finished;
    }

    AsnNode* node = ASNFormat::findFirstNodeByName(rootElem, nodeName);
    if (node == NULL) {
        stateInfo.setError(QString("node not found %1").arg(nodeName));
        return ReportResult_Finished;
    }

    addContext(nodeContextName, node);
    contextAdded = true;

    return ReportResult_Finished;
}

//--------------------------------------------------------------------------------

void GTest_CheckNodeType::init(XMLTestFormat*, const QDomElement& el)
{
    nodeContextName = el.attribute(NODE_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(NODE_ATTR);
        return;
    }

    nodeTypeName = el.attribute(TYPE_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(TYPE_ATTR);
        return;
    }

}

Task::ReportResult GTest_CheckNodeType::report()
{
    AsnNode* node = getContext<AsnNode>(this, nodeContextName);
    if( node == NULL ){
        stateInfo.setError(QString("node is in the context, wrong value %1").arg(nodeContextName));
        return ReportResult_Finished;
    }

    QString tmpTypeName(ASNFormat::getAsnNodeTypeName(node));
    if (nodeTypeName != tmpTypeName) {
        stateInfo.setError(QString("type for node (%1) doesn't match: (%2)").arg(nodeContextName).arg(tmpTypeName) +
            QString(", expected (%1) ").arg(nodeTypeName));
    }

    return ReportResult_Finished;
}

//--------------------------------------------------------------------------------

void GTest_CheckNodeValue::init(XMLTestFormat*, const QDomElement& el)
{
    nodeContextName = el.attribute(NODE_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(NODE_ATTR);
        return;
    }

    nodeValue = el.attribute(VALUE_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

}

Task::ReportResult GTest_CheckNodeValue::report()
{
    AsnNode* node = getContext<AsnNode>(this, nodeContextName);
    if( node == NULL ){
        stateInfo.setError(QString("node is not in the context, wrong value %1").arg(nodeContextName));
        return ReportResult_Finished;
    }

    QString tmpValue(node->value);
    if (nodeValue != tmpValue) {
        stateInfo.setError(QString("value for node (%1) doesn't match: (%2)").arg(nodeContextName).arg(tmpValue) +
                QString(", expected (%1) ").arg(nodeValue));
    }

    return ReportResult_Finished;
}

//--------------------------------------------------------------------------------

void GTest_CheckNodeChildrenCount::init(XMLTestFormat*, const QDomElement& el)
{
    nodeContextName = el.attribute(NODE_ATTR);
    if (nodeContextName.isEmpty()) {
        failMissingValue(NODE_ATTR);
        return;
    }

    QString count = el.attribute(CHILDREN_COUNT_ATTR);
    if (count.isEmpty()) {
        failMissingValue(CHILDREN_COUNT_ATTR);
        return;
    }

    bool ok = false;
    numChildren = count.toInt(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, int required").arg(CHILDREN_COUNT_ATTR));
    }

}

Task::ReportResult GTest_CheckNodeChildrenCount::report()
{
    AsnNode* node = getContext<AsnNode>(this, nodeContextName);
    if( node == NULL ){
        stateInfo.setError(QString("node is not in the context, wrong value %1").arg(nodeContextName));
        return ReportResult_Finished;
    }

    int tmpNum = node->children.count();
    if (numChildren != tmpNum) {
        stateInfo.setError(QString("children count for node (%1) doesn't match: (%2)").arg(nodeContextName).arg(tmpNum) +
            QString(", expected (%1) ").arg(numChildren));
    }

    return ReportResult_Finished;

}


//--------------------------------------------------------------------------------




QList<XMLTestFactory*> AsnParserTests::createTestFactories()
{
    QList<XMLTestFactory*> res;
    res.append(GTest_LoadAsnTree::createFactory());
    res.append(GTest_FindFirstNodeByName::createFactory());
    res.append(GTest_CheckNodeType::createFactory());
    res.append(GTest_CheckNodeValue::createFactory());
    res.append(GTest_CheckNodeChildrenCount::createFactory());

    return res;
}
} //namespace
