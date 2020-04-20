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

#ifdef OPENCL_SUPPORT

#include <QDomElement>
#include <QStringList>

#include <U2Core/AppContext.h>

#include "BinaryFindOpenCLTests.h"

namespace U2 {

#define NUMBERS "numbers"
#define FIND_NUMBERS "find_numbers"
#define EXPECTED_RESULTS "expected_res"

QList< XMLTestFactory* > BinaryFindOpenCLTests::createTestFactories(){
    QList< XMLTestFactory* > res;
    res.append( GTest_BinaryFindOpenCL::createFactory() );
    return res;
}

void GTest_BinaryFindOpenCL::init(XMLTestFormat *tf, const QDomElement& el){
    Q_UNUSED(tf);

    QString buf;

    buf = el.attribute(NUMBERS);
    if (!buf.isEmpty()){
        QStringList numsStr = buf.split(",");
        foreach(QString s, numsStr) {
            numbers.append(s.toLongLong());
        }
    } else {
        failMissingValue(NUMBERS);
    }

    buf = el.attribute(FIND_NUMBERS);
    if (!buf.isEmpty()){
        QStringList numsStr = buf.split(",");
        foreach(QString s, numsStr) {
            findNumbers.append(s.toLongLong());
        }
    } else {
        failMissingValue(FIND_NUMBERS);
    }

    buf = el.attribute(EXPECTED_RESULTS);
    if (!buf.isEmpty()){
        QStringList numsStr = buf.split(",");
        foreach(QString s, numsStr) {
            expectedResults.append(s.toInt());
        }
    } else {
        failMissingValue(EXPECTED_RESULTS);
    }
}

void GTest_BinaryFindOpenCL::prepare() {
    if (nullptr == AppContext::getOpenCLGpuRegistry()->getEnabledGpu()) {
        stateInfo.setError(QString("No enabled gpu's found"));
    } else {
        BinaryFindOpenCL bf(numbers.constData(), numbers.size(), findNumbers.constData(), findNumbers.size(), windowSizes.constData());
        results = bf.launch();
    }
}

Task::ReportResult GTest_BinaryFindOpenCL::report(){
//    QString buf = "";
//    for (int i = 0; i < findNumbers.size(); i ++ ) {
//        buf += QString::number(results[i]) + " ";
//    }
//    coreLog.info("!!!!!!!!!!!!! result !!!!!!!!!!!!!");
//    coreLog.info(buf);
    for (int i = 0; i < findNumbers.size(); i ++ ) {
        if ( (expectedResults.at(i) == 0 && results[i] != -1) ||
             (expectedResults.at(i) != 0 && results[i] == -1) ||
             (results[i] != -1 && numbers[results[i]] != findNumbers.at(i))) {
            stateInfo.setError(QString("expectedIsContains: %1 value: %2 does not satisfy the resulting position: %3").arg(expectedResults.at(i)).arg(findNumbers.at(i)).arg(results[i]));
            break;
        }
    }
    delete[] results;
    return ReportResult_Finished;
}

} //namespace

#endif /*OPENCL_SUPPORT*/
