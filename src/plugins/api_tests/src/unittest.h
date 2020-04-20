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

#ifndef _U2_UNIT_TEST_H_
#define _U2_UNIT_TEST_H_

#include <QString>


#define _STR(c) #c
#define TEST_CLASS(suite, name)          suite##_##name
#define TEST_CLASS_STR(suite, name) _STR(suite##_##name)

/** Macros for writing tests without repeating too much */

/** For each test place this in header file, inside namespace U2 to declare test class */
#define DECLARE_TEST(suite, name) \
    class TEST_CLASS(suite, name) : public UnitTest { \
    public: \
        virtual void Test(); \
    }

/** For each test place this in header file, outside (!) namespace U2 to register Qt meta type */
#define DECLARE_METATYPE(suite, name) \
    Q_DECLARE_METATYPE(U2::suite##_##name)

/** Place this in cpp file and define test body in subsequent {} block */
#define IMPLEMENT_TEST(suite, name) \
    static const int _##suite##_##name##_type ATTR_UNUSED = qRegisterMetaType<U2::TEST_CLASS(suite, name)>(TEST_CLASS_STR(suite, name)); \
    void TEST_CLASS(suite, name)::Test()

/** Macros to be used in test body: they all check for some condition,
    and if check fails then set test error and break its execution */

#define CHECK_NO_ERROR(os) CHECK_OP_EXT(os, SetError(os.getError()),)
#define CHECK_TRUE(condition, error) CHECK_EXT(condition, SetError(error),)
#define CHECK_FALSE(condition, error) CHECK_EXT(!(condition), SetError(error),)

/** To use CHECK_EQUAL and CHECK_NOT_EQUAL for class that is not convertible to QString,
    define partial specialization of template function toString<T>(const T &t) */
#define CHECK_EQUAL(expected, actual, what) CHECK_TRUE(expected == actual, QString("unexpected %1: expected '%2', got '%3'").arg(what).arg(toString(expected)).arg(toString(actual)))
#define CHECK_NOT_EQUAL(notExpected, actual, what) CHECK_TRUE(notExpected != actual, QString("unexpected %1: expected not to be '%2', but got '%3'").arg(what).arg(toString(notExpected)).arg(toString(actual)))

namespace U2 {

class UnitTest {
public:
    virtual ~UnitTest() {}

    virtual void Test() = 0;
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual void SetError(const QString& err) { error = err; }
    virtual QString GetError() { return error; }
protected:
    QString error;
};

template<class T> QString toString(const T &t) { return QString("%1").arg(t); }

} // namespace

#endif
