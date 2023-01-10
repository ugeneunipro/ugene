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

#ifndef _U2_XML_TEST_FORMAT_H_
#define _U2_XML_TEST_FORMAT_H_

#include "../GTest.h"

class QDomElement;

namespace U2 {

class XMLTestFormat;

class U2TEST_EXPORT XMLTestFactory {
public:
    XMLTestFactory(const QString& _tagName)
        : tagName(_tagName) {
    }
    virtual ~XMLTestFactory();

    virtual GTest* createTest(XMLTestFormat* tf, const QString& testName, GTest* cp, const GTestEnvironment* env, const QList<GTest*>& subtasks, const QDomElement& el) = 0;

    const QString& getTagName() const {
        return tagName;
    }

private:
    QString tagName;
};

class U2TEST_EXPORT XMLTestFormat : public GTestFormat {
    Q_OBJECT
public:
    XMLTestFormat();
    virtual ~XMLTestFormat();

    virtual GTest* createTest(const QString& name, GTest* cp, const GTestEnvironment* env, const QByteArray& testData, QString& err);

    virtual GTest* createTest(const QString& name, GTest* cp, const GTestEnvironment* env, const QDomElement& el, QString& err);

    virtual bool registerTestFactory(XMLTestFactory* tf);

    /** Registers all factories from the list. Asserts that there are no duplicates in the registry as the result. */
    void registerTestFactories(const QList<XMLTestFactory*>& factoryList);

    virtual bool unregisterTestFactory(XMLTestFactory* tf);

private:
    QMap<QString, XMLTestFactory*> testFactories;
};

}  // namespace U2

#endif
