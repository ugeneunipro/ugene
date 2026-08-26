/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "XMLTestFormat.h"

#include <QDomDocument>
#include <QHash>
#include <QRegularExpression>

#define TEST_FACTORIES_AUTO_CLEANUP

#ifdef TEST_FACTORIES_AUTO_CLEANUP
#    include <U2Core/AppContext.h>

#    include <U2Test/GTestFrameworkComponents.h>
#endif

#include <U2Test/XMLTestUtils.h>

namespace U2 {

XMLTestFactory::~XMLTestFactory() {
#ifdef TEST_FACTORIES_AUTO_CLEANUP
    GTestFormatRegistry* testFormatRegistry = AppContext::getTestFramework()->getTestFormatRegistry();
    if (testFormatRegistry != nullptr) {
        auto xmlTestFormat = qobject_cast<XMLTestFormat*>(testFormatRegistry->findFormat("XML"));
        if (xmlTestFormat != nullptr) {
            xmlTestFormat->unregisterTestFactory(this);
        }
    }
#endif
}

XMLTestFormat::XMLTestFormat()
    : GTestFormat("XML") {
    // Register built-in factories.
    registerTestFactories(XMLTestUtils::createTestFactories());
}

XMLTestFormat::~XMLTestFormat() {
    const QList<XMLTestFactory*> factoryList = testFactories.values();
    for (XMLTestFactory* f : qAsConst(factoryList)) {
        delete f;
    }
}

namespace {

// Removes duplicate attributes from a single XML start tag, keeping the LAST value for each
// repeated name. Returns the tag unchanged if it has no duplicates. The tag string includes the
// leading '<' and trailing '>'.
QString dedupeTagAttributes(const QString& tag) {
    static const QRegularExpression tagHead(R"(^<\s*/?\s*[\w:.\-]+)");
    const QRegularExpressionMatch headMatch = tagHead.match(tag);
    if (!headMatch.hasMatch()) {
        return tag;
    }
    const QString head = tag.left(headMatch.capturedEnd());

    QString body = tag.mid(headMatch.capturedEnd());
    if (body.endsWith('>')) {
        body.chop(1);
    }
    body = body.trimmed();
    bool selfClose = false;
    if (body.endsWith('/')) {
        body.chop(1);
        selfClose = true;
    }

    // Test XML attribute values never contain raw '"', '<' or '>' (they are entity-encoded),
    // so a simple name="value" / name='value' scan is sufficient here.
    static const QRegularExpression attrRe(R"(([\w:.\-]+)\s*=\s*("[^"]*"|'[^']*'))");
    QStringList order;
    QHash<QString, QString> values;
    bool hadDuplicate = false;
    QRegularExpressionMatchIterator it = attrRe.globalMatch(body);
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        const QString attrName = m.captured(1);
        if (!values.contains(attrName)) {
            order.append(attrName);
        } else {
            hadDuplicate = true;
        }
        values.insert(attrName, m.captured(2));  // last occurrence wins, matching pre-Qt6 QDom
    }
    if (!hadDuplicate) {
        return tag;
    }

    QString out = head;
    for (const QString& attrName : qAsConst(order)) {
        out += ' ' + attrName + '=' + values.value(attrName);
    }
    out += selfClose ? "/>" : ">";
    return out;
}

// Pre-Qt6 QDomDocument silently accepted duplicate attributes (last value won); Qt6 rejects the
// document as not well-formed. UGENE ships hundreds of legacy test files with duplicate attributes,
// so when the strict parse fails we retry on a sanitized copy with duplicates removed.
QByteArray removeDuplicateXmlAttributes(const QByteArray& data) {
    const QString xml = QString::fromUtf8(data);
    QString result;
    result.reserve(xml.size());
    const int n = xml.size();
    int i = 0;
    while (i < n) {
        if (xml[i] != '<') {
            result += xml[i++];
            continue;
        }
        if (xml.mid(i, 4) == "<!--") {  // comment: copy verbatim
            int end = xml.indexOf("-->", i + 4);
            end = (end == -1) ? n : end + 3;
            result += xml.mid(i, end - i);
            i = end;
            continue;
        }
        if (i + 1 < n && (xml[i + 1] == '!' || xml[i + 1] == '?')) {  // doctype / PI: copy verbatim
            int end = xml.indexOf('>', i);
            end = (end == -1) ? n : end + 1;
            result += xml.mid(i, end - i);
            i = end;
            continue;
        }
        int end = xml.indexOf('>', i);  // element tag
        if (end == -1) {
            result += xml.mid(i);
            break;
        }
        result += dedupeTagAttributes(xml.mid(i, end + 1 - i));
        i = end + 1;
    }
    return result.toUtf8();
}

}  // namespace

GTest* XMLTestFormat::createTest(const QString& name, GTest* cp, const GTestEnvironment* env, const QByteArray& testData, QString& err) {
    QDomDocument doc;
    int line = 0;
    int col = 0;
    bool res = doc.setContent(testData, &err, &line, &col);
    if (!res) {
        // Retry tolerating duplicate attributes the way pre-Qt6 QDom did.
        const QByteArray sanitized = removeDuplicateXmlAttributes(testData);
        if (sanitized != testData && doc.setContent(sanitized, &err, &line, &col)) {
            res = true;
        }
    }
    if (!res) {
        err = "Error reading test: " + err;
        err += QString(" line: %1 col: %2").arg(line).arg(col);
        return nullptr;
    }
    QString topLevelElementTag = doc.documentElement().tagName();
    if (topLevelElementTag != "multi-test" && topLevelElementTag != "unittest") {
        err = "Top level element is not <multi-test>: " + topLevelElementTag;
        return nullptr;
    }
    QDomElement testEl = doc.documentElement();
    return createTest(name, cp, env, testEl, err);
}

GTest* XMLTestFormat::createTest(const QString& name, GTest* cp, const GTestEnvironment* env, const QDomElement& el, QString& err) {
    QString tagName = el.tagName();
    XMLTestFactory* factory = testFactories.value(tagName);
    if (factory == nullptr) {
        err = QString("XMLTestFactory not found '%1'").arg(tagName);
        return nullptr;
    }
    QList<GTest*> subs;
    return factory->createTest(this, name, cp, env, subs, el);
}

bool XMLTestFormat::registerTestFactory(XMLTestFactory* tf) {
    const QString& tagName = tf->getTagName();
    if (testFactories.contains(tagName)) {
        return false;
    }
    testFactories[tagName] = tf;
    return true;
}

void XMLTestFormat::registerTestFactories(const QList<XMLTestFactory*>& factoryList) {
    for (XMLTestFactory* factory : qAsConst(factoryList)) {
        bool ok = registerTestFactory(factory);
        Q_UNUSED(ok);
        Q_ASSERT(ok);
    }
}

bool XMLTestFormat::unregisterTestFactory(XMLTestFactory* tf) {
    const QString& tagName = tf->getTagName();
    if (!testFactories.contains(tagName)) {
        return false;
    }
    testFactories.remove(tagName);
    return true;
}

}  // namespace U2
