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

#ifndef _U2_CMDLINE_TESTS_H_
#define _U2_CMDLINE_TESTS_H_

#include <QProcess>
#include <U2Test/XMLTestUtils.h>

namespace U2 {

/***********************************
 * arguments for cmdline ugene are set in the following way:
 * tag="value" -> --tag=value
 * if value is existing file in _common_data we will resolve this
 * if you want to place file in _tmp you should write filename as "!tmp_out!filename"
 ***********************************/
class GTest_RunCMDLine : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY( GTest_RunCMDLine, "run-cmdline" );
    virtual void prepare();
    virtual ReportResult report();
    virtual void cleanup();

private:
    void setUgeneclPath();
    void setArgs( const QDomElement & owner );
    QString getVal( const QString & val );
    QString splitVal(const QString & val, QString prefValue, const QString & prefix, bool isTmp);

private:
    QString expectedMessage;
    QString unexpectedMessage;
    QStringList args;
    QProcess *  proc;
    QString     ugeneclPath;
    QStringList tmpFiles;
    QString     workingDir;
    bool        autoRemoveWorkingDir;
    bool        customIniSet;

    static const QString UGENECL_PATH;
}; // GTest_RunCMDLine

class CMDLineTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
}; // CMDLineTests

} // U2

#endif // _U2_CMDLINE_TESTS_H_
