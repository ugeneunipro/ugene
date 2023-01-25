/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
        XMLTestFormat* xmlTestFormat = qobject_cast<XMLTestFormat*>(testFormatRegistry->findFormat("XML"));
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

GTest* XMLTestFormat::createTest(const QString& name, GTest* cp, const GTestEnvironment* env, const QByteArray& testData, QString& err) {
    QDomDocument doc;
    int line = 0;
    int col = 0;
    bool res = doc.setContent(testData, &err, &line, &col);
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
