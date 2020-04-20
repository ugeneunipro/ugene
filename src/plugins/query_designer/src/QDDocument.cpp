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

#include "QDDocument.h"
#include "QDSceneIOTasks.h"

#include <QStringList>
#include <QRegExp>


namespace U2 {

#define STRING_MAX_LEN 80
#define TEXT_OFFSET "    "
#define DOUBLE_TEXT_OFFSET "        "

static const QString GRAPH_KEYWORD = "query";
static const QString IMPORT_KEYWORD = "import";
static const QString BLOCK_START = "{";
static const QString BLOCK_END = "}";
static const QString META_KEYWORD = ".meta";
static const QString VISUAL_KEYWORD = "visual";

QDDocStatement::~QDDocStatement() {

}

QString QDDocStatement::getAttribute(const QString& name) const {
    foreach(const StringAttribute& attr, attributes) {
        if (attr.first==name) {
            return attr.second;
        }
    }
    return QString();
}

void QDDocStatement::setAttribute(const QString& name, const QString& value) {
    assert(!name.contains(' '));
    for (int i=0; i<attributes.size(); i++) {
        StringAttribute& attr = attributes[i];
        if (attr.first==name) {
            attr.second = value;
            return;
        }
    }
    QPair<QString,QString> newAttr = qMakePair(name, value);
    attributes.append(newAttr);
}

QString QDDocStatement::toString() const {
    int strLen = evalStringLen();
    QString res;
    foreach(const StringAttribute& attr, attributes) {
        if (strLen>STRING_MAX_LEN) {
            res+="\n";
            res+=DOUBLE_TEXT_OFFSET;
        }
        res += " " + attr.first + ": ";
        QString val;
        if (attr.second.contains(';') || attr.second.contains('#')) {
            val = "\""+attr.second+"\"";
        }
        else {
            val = attr.second;
        }
        res+=val + ';';
    }
    return res;
}

int QDDocStatement::evalStringLen() const {
    int res=0;
    foreach(const StringAttribute& attr, attributes) {
        res+=attr.first.length() + attr.second.length() + 2;
    }
    return res;
}

const QString QDElementStatement::GEOMETRY_ATTR_NAME("geometry");
const QString QDElementStatement::ALGO_ATTR_NAME("type");

QString QDElementStatement::toString() const {
    QString res = QDDocStatement::toString();
    if (evalStringLen()>STRING_MAX_LEN) {
        res = BLOCK_START + res + "\n" + TEXT_OFFSET + BLOCK_END;
    }
    else {
        res = BLOCK_START + res + " " + BLOCK_END +" ";
    }

    res = TEXT_OFFSET + id + " " + res;
    return res;
}

QString QDElementStatement::definedIn() const {
    return QDDocument::definedIn(id);
}

const QString QDLinkStatement::TYPE_ATTR_NAME("type");
QString QDLinkStatement::toString() const {
    QString res = " " + BLOCK_START + QDDocStatement::toString() + " " + BLOCK_END;
    QString prefix = elementIds.at(0);
    for (int i=1; i<elementIds.size(); i++) {
        prefix+="--" + elementIds.at(i);
    }
    res = TEXT_OFFSET + prefix + res;
    return res;
}

//QDDocument
//////////////////////////////////////////////////////////////////////////
static const QString COMMENT_PATTERN = "\\\\\\\\|#([^\n]*)\n";
static const QString SINGLE_ID_PATTERN = "(?:[a-zA-Z]+)(?:[a-zA-Z0-9]|_|(?:-(?!-)))*";
const QString QDDocument::ID_PATTERN = "(" + SINGLE_ID_PATTERN + "(?:\\." + SINGLE_ID_PATTERN + ")*)";
//static const QString VAL_PATTERN = "((?:\"[^\"]+\")|[^\\s\"]+)";
static const QString VAL_PATTERN = "((?:\"[^\"]+\")|[^\\s;]+)";
static const QString IMPORT_PATTERN = IMPORT_KEYWORD + "\\s+" + VAL_PATTERN;
static const QString DOC_NAME_PATTERN = GRAPH_KEYWORD + "\\s*" + QDDocument::ID_PATTERN;// + "\\s*\\{{1,1}" + "[^\\{]*(?:\\{{1,1}[^\\{\\}]\\}{1,1})*" + "}{1,1}";
static const QString ELEMENT_ATTRS_PATTERN = "\\"+BLOCK_START+"{1,1}([^\\"+ BLOCK_START
+"\\"+BLOCK_END+"]*)\\"+BLOCK_END+"{1,1}";
//static const QString GROUP_ATTRS_PATTERN = "\\{{1,1}([^\\{\\}]*)\\}{1,1}";
static const QString ELEMENT_STATEMENT_PATTERN = "[^--](?:\n|\\s)+" + QDDocument::ID_PATTERN + "\\s*" + ELEMENT_ATTRS_PATTERN;
//static const QString GROUP_STATEMENT_PATTERN ="[^--]" + ID_PATTERN + "\\s*" + GROUP_ATTRS_PATTERN;
static const QString LINK_STATEMENT_PATTERN = "(" + QDDocument::ID_PATTERN + "(?:\\s*--\\s*" + QDDocument::ID_PATTERN + ")+)\\s*" + ELEMENT_ATTRS_PATTERN;

static const QString ORDER_KEYWORD = "order";
static const QString ORDER_PATTERN = ORDER_KEYWORD + "\\{{1,1}([^\\{\\}]*)\\}{1,1}";

static const QString SCHEMA_STRAND_ATTR = "schema-strand";
static const QString SCHEMA_STRAND_PATTERN = SCHEMA_STRAND_ATTR + "\\s*:\\s*(direct|complement|both)\\s*;";

const QString QDDocument::HEADER_LINE("#@UGENE_QUERY");
const QString QDDocument::DEPRECATED_HEADER_LINE("#!UGENE_QUERY");
const QString QDDocument::GROUPS_SECTION("groups");

QDDocument::~QDDocument() {
    qDeleteAll(elements);
    qDeleteAll(links);
}

QDElementStatement* QDDocument::getElement(const QString& id) const {
    foreach(QDElementStatement* stmt, elements) {
        if (stmt->getId()==id) {
            return stmt;
        }
    }
    return NULL;
}

QList<QDElementStatement*> QDDocument::getElements(QDStatementType type) const {
    QList<QDElementStatement*> res;
    foreach(QDElementStatement* stmt, elements) {
        if (stmt->getType()==type) {
            res.append(stmt);
        }
    }
    return res;
}

QByteArray QDDocument::toByteArray() const {
    QString content;
    foreach(QDElementStatement* el, getElements(Group)) {
        if (el->getId()==GROUPS_SECTION) {
            continue;
        }
        content += el->toString() + "\n";
    }
    content+="\n";
    foreach(QDLinkStatement* lnk, getLinks()) {
        content += lnk->toString() + "\n";
    }
    content+="\n";
    QString txtOffset = QString(TEXT_OFFSET);
    content+=txtOffset + META_KEYWORD + BLOCK_START + "\n";
    content+=txtOffset + txtOffset + VISUAL_KEYWORD + BLOCK_START + "\n";
    foreach(QDElementStatement* el, getElements(Element)) {
        content+=txtOffset+txtOffset+txtOffset;
        content += el->toString() + "\n";
    }
    content+=txtOffset + txtOffset + BLOCK_END + "\n";

    if (!order.isEmpty()) {
        content+=txtOffset + txtOffset + ORDER_KEYWORD + " " + BLOCK_START + " ";
        foreach(QString s, order) {
            content+=s+';';
        }
        content+= " " + BLOCK_END + "\n";
    }

    if (schemaStrand!=QDStrand_Both) {
        content+=txtOffset + txtOffset + SCHEMA_STRAND_ATTR + " : ";
        content+= QDSchemeSerializer::STRAND_MAP.value(schemaStrand) + ';';
        content+= " \n";
    }

    QDElementStatement* grEl = getElement(GROUPS_SECTION);
    if (grEl) {
        content+=txtOffset + grEl->toString() + "\n";
    }

    content+=txtOffset + BLOCK_END + "\n";
    QString desc = docDesc;
    desc = "#" + desc.replace("\n", "\n#") + "\n\n";
    content = HEADER_LINE + "\n" + desc + GRAPH_KEYWORD + " " + docName + " {\n\n\n" + content + "}";
    return content.toUtf8();
}

bool QDDocument::setContent(const QString& content) {
    QRegExp reg(DOC_NAME_PATTERN);
    reg.indexIn(content);
    docName = reg.cap(1);
    findImportedUrls(content);
    findComments(content);
    parseSchemaStrand(content);
    if(!findElementStatements(content)) {
        return false;
    }
    if (!findLinkStatements(content)) {
        return false;
    }
    return true;
}

void QDDocument::parseSchemaStrand(const QString& str) {
    QRegExp reg(SCHEMA_STRAND_PATTERN);
    int pos = reg.indexIn(str);
    if (pos>=0) {
        QString strandStr = reg.cap(1);
        if (QDSchemeSerializer::STRAND_MAP.values().contains(strandStr)) {
            schemaStrand = QDSchemeSerializer::STRAND_MAP.key(strandStr);
        }
    }
}

void QDDocument::findComments(const QString& str) {
    QRegExp reg("((?:#[^\n]*\n{1,1})+)\\s*"+GRAPH_KEYWORD);
    int pos = reg.indexIn(str);
    if (pos>=0) {
        QString coms = reg.cap(1);
        coms.remove(HEADER_LINE);
        docDesc = coms.replace("#","");
        int lastIdx = docDesc.length()-1;
        assert(QString(docDesc.at(lastIdx))=="\n");
        docDesc.remove(lastIdx,1);
    }
}

void QDDocument::parseOrder(const QString& str) {
    order.clear();
    order = str.trimmed().split(QRegExp("\\s*;\\s*"));
}

bool QDDocument::addElement(QDElementStatement* el) {
    QDStatementType elType = el->getType();
    const QString& elId = el->getId();
    foreach(QDElementStatement* stmt, elements) {
        if (stmt->getId()==elId && stmt->getType()==elType) {
            return false;
        }
    }
    elements.append(el);
    assert(el->document==NULL);
    el->document = this;
    return true;
}

void QDDocument::findImportedUrls(const QString& str) {
    QRegExp reg(IMPORT_PATTERN);
    int pos=0;
    while(pos>=0) {
        pos = reg.indexIn(str, pos);
        if (pos>=0) {
            pos+=reg.matchedLength();
            QString toImport = reg.cap(1);
            importedUrls.append(toImport);
        }
    }
}

bool QDDocument::findElementStatements(const QString& str) {
    QRegExp reg;
    reg.setPattern(ELEMENT_STATEMENT_PATTERN);
    int pos=0;
    while(pos>=0) {
        pos = reg.indexIn(str, pos);
        if (pos>=0) {
            /*QString ch1 = QString(str.at(pos));
            QString ch2 = QString(str.at(pos-1));
            QString ch3 = QString(str.at(pos+1));*/
            pos+=reg.matchedLength();
            const QString& id = reg.cap(1);
            const QString& attrs = reg.cap(2);
            if (id==ORDER_KEYWORD) {
                parseOrder(attrs);
                continue;
            }
            const QMap<QString, QString>& attrsMap = string2attributesMap(attrs);
            QDStatementType type;
            if (id.contains('.')) {
                type = Element;
            }
            else {
                type = Group;
            }
            QDElementStatement* element = new QDElementStatement(id, type);
            foreach(const QString& attrName, attrsMap.keys()) {
                const QString& val = attrsMap.value(attrName);
                element->setAttribute(attrName, val);
            }
            addElement(element);
        }
    }
    return true;
}

bool QDDocument::findLinkStatements(const QString& str) {
    QRegExp reg(LINK_STATEMENT_PATTERN);
    int pos=0;
    while (pos>=0) {
        pos = reg.indexIn(str, pos);
        if (pos>=0) {
            pos+=reg.matchedLength();
            const QString& elemS = reg.cap(1);
            const QList<QString>& elIds = idsFromString(elemS);
            QDLinkStatement* link = new QDLinkStatement(elIds);
            int capCount = reg.captureCount();
            const QString& attrs = reg.cap(capCount);
            const QMap<QString, QString>& attrsMap = string2attributesMap(attrs);
            foreach(const QString& attrName, attrsMap.keys()) {
                const QString& val = attrsMap.value(attrName);
                link->setAttribute(attrName, val);
            }
            addLink(link);
        }
    }
    return true;
}

QMap<QString, QString> QDDocument::string2attributesMap(const QString& str) {
    QMap<QString, QString> res;
    QRegExp reg(ID_PATTERN + "\\s*:\\s*" + VAL_PATTERN);
    int pos = 0;
    while (pos>=0) {
        pos = reg.indexIn(str, pos);
        if (pos >= 0) {
            const QString& attrName = reg.cap(1);
            QString attrVal = reg.cap(2);
            attrVal.remove('"');
            res[attrName] = attrVal;
            pos+=reg.matchedLength();
        }
    }
    return res;
}

QList<QString> QDDocument::idsFromString(const QString& str) {
    QStringList l = str.split(QRegExp("\\s*--\\s*"));
    return l;
}

QString QDDocument::definedIn(const QString &id) {
    const QStringList& chunks = id.split('.');
    if (chunks.size()<3) {
        return QString();
    }
    else {
        assert(chunks.size()==3);
        QString defIn = chunks.at(0);
        return defIn;
    }
}

QString QDDocument::getLocalName(const QString& id) {
    if (id.contains('.')) {
        int dotPos = id.indexOf('.');
        return id.mid(dotPos+1);
    }
    return id;
}

QDElementStatement* QDDocument::findElementByUnitName(QDElementStatement* parent, const QString& unitName) const {
    return getElement(parent->getId()+"."+unitName);
}

void QDDocument::saveOrder(const QList<QDActor*>& actors) {
    foreach(QDActor* a, actors) {
        order.append(a->getParameters()->getLabel());
    }
}

bool QDDocument::isHeaderLine(const QString &line) {
    return (line.startsWith(HEADER_LINE) ||
            line.startsWith(DEPRECATED_HEADER_LINE));
}

//Mapper
//////////////////////////////////////////////////////////////////////////
QString QDIdMapper::distance2string(QDDistanceType type) {
    switch (type) {
        case E2S:
            return "end-to-start";
        case E2E:
            return "end-to-end";
        case S2S:
            return "start-to-start";
        case S2E:
            return "start-to-end";
        default:
            return QString();
    }
}

int QDIdMapper::string2distance(const QString& str) {
    if (str=="end-to-start") {
        return 0;
    }
    if (str=="end-to-end") {
        return 1;
    }
    if (str=="start-to-start") {
        return 2;
    }
    if (str=="start-to-end") {
        return 3;
    }
    return -1;
}

QDConstraintType QDIdMapper::string2constraintType(const QString& str) {
    if (str=="distance") {
        return QDConstraintTypes::DISTANCE;
    }
    return QString();
}

QString QDIdMapper::constraintType2string(const QDConstraintType& type) {
    if (type==QDConstraintTypes::DISTANCE) {
        return "distance";
    }
    return QString();
}

}//namespace
