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

#ifndef _U2_HMMER_BUILD_TASK_TEST_H_
#define _U2_HMMER_BUILD_TASK_TEST_H_

#include "HmmerBuildTask.h"

#include <U2Test/GTest.h>
#include <U2Test/XMLTestFormat.h>
#include <U2Test/XMLTestUtils.h>

namespace U2 {

/*****************************************
* Builds hmm profile from input file
*****************************************/
class GTest_UHMMER3Build : public XmlTest {
    Q_OBJECT
public:
    static const QString INPUT_FILE_TAG;
    static const QString OUTPUT_FILE_TAG;
    static const QString OUTPUT_DIR_TAG;
    static const QString DEL_OUTPUT_TAG;

    static const QString RELATIVE_WEIGHTING_OPTION_TAG; /* --wgsc, --wblosum, --wpb, --wnone, --wgiven */

    static const QString EFFECTIVE_WEIGHTING_OPTION_TAG;/* --eent, --eclust, --enone, --eset */

    static const QString E_VALUE_CALIBRATION_OPTION_TAG; /*  */

    static const QString MODEL_CONSTRUCTION_OPTION_TAG; /* --fast + symfrac and
                                                        --hand ( requires RF annotation )*/
    static const QString SEED_OPTION_TAG;

    static void setBuildSettings(HmmerBuildSettings & settings, const QDomElement& el, TaskStateInfo & ti);

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_UHMMER3Build, "uhmmer3-build")

    virtual void prepare();
    ReportResult report();
    void cleanup();

private:
    void setAndCheckArgs();

private:
    QString                 inFile;
    QString                 outFile;
    QString                 outputDir;
    bool                    delOutFile;
    HmmerBuildSettings      bldSettings;
    HmmerBuildTask          *buildTask;

}; // GTest_UHMMER3Build

/*****************************************
* Totally compares two hmm files
*****************************************/
class GTest_CompareHmmFiles : public XmlTest {
    Q_OBJECT
public:
    static const QString FILE1_NAME_TAG;
    static const QString FILE2_NAME_TAG;
    static const QString FILE1_TMP_TAG;
    static const QString FILE2_TMP_TAG;

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareHmmFiles, "hmm3-compare-files");
    ReportResult report();

private:
    void setAndCheckArgs();

private:
    QString filename1;
    QString filename2;
    bool    file1Tmp;
    bool    file2Tmp;

}; // GTest_CompareHmmFiles

}

#endif
