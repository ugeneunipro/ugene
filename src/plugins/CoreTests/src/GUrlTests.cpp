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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppFileStorage.h>
#include <U2Core/GHints.h>
#include <U2Core/GObject.h>
#include <U2Core/GUrl.h>
#include <U2Core/U2SafePoints.h>

#include "GUrlTests.h"

namespace U2 {

#define ORIGINAL_URL_ATTR       "original"
#define EXPECTED_RESULT_ATTR    "expected-result"
#define PLATFORM_ATTR           "platform"
#define PLATFORM_WIN            "win"
#define PLATFORM_UNIX           "unix"

void GTest_ConvertPath::init(XMLTestFormat*, const QDomElement& el) {
    originalUrl = el.attribute(ORIGINAL_URL_ATTR);
    expectedResult = el.attribute(EXPECTED_RESULT_ATTR);
    platform = el.attribute(PLATFORM_ATTR);

#ifdef Q_OS_WIN
    QString currPlatform = PLATFORM_WIN;
#else
    QString currPlatform = PLATFORM_UNIX;
#endif

    runThisTest = (platform == currPlatform);
    if(runThisTest) {
        GUrl gurl(originalUrl);
        result = gurl.getURLString();
        isFileUrl = (gurl.getType() == GUrl_File);
    }
}

Task::ReportResult GTest_ConvertPath::report() {
    if(runThisTest) {
        if(!isFileUrl) {
            stateInfo.setError(tr("%1 isn't a File URL.").arg(originalUrl));
        } else if (expectedResult != result) {
            stateInfo.setError(tr("%1 was converted into %2, while %3 was expected").arg(originalUrl).arg(result).arg(expectedResult));
        }
    }
    return ReportResult_Finished;
}

/************************************************************************/
/* GTest_CreateTmpDir */
/************************************************************************/
#define TEMP_DATA_DIR_ENV_ID "TEMP_DATA_DIR"
#define URL_ATTR "url"
#define DATA_ATTR "data"
#define EXISTS_ATTR "exists"

void GTest_CreateTmpDir::init(XMLTestFormat * /*tf*/, const QDomElement &el) {
    url = el.attribute(URL_ATTR);
}

Task::ReportResult GTest_CreateTmpDir::report() {
    QDir tmpDir(env->getVar(TEMP_DATA_DIR_ENV_ID));
    bool exists = tmpDir.exists(url);
    if (!exists) {
        bool created = tmpDir.mkdir(url);
        if (!created) {
            setError("Can not create a folder: " + url);
        }
    }
    return ReportResult_Finished;
}

/************************************************************************/
/* GTest_RemoveTmpDir */
/************************************************************************/
void GTest_RemoveTmpDir::init(XMLTestFormat * /*tf*/, const QDomElement &el) {
    url = env->getVar( TEMP_DATA_DIR_ENV_ID ) + "/" + el.attribute(URL_ATTR);
}

Task::ReportResult GTest_RemoveTmpDir::report() {
    removeDir(url);
    return ReportResult_Finished;
}

void GTest_RemoveTmpDir::removeDir(const QString &url) {
    QDir dir(url);
    if (!dir.exists()) {
        return;
    }
    foreach (const QFileInfo &entry, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (entry.isDir()) {
            removeDir(entry.absoluteFilePath());
            CHECK_OP(stateInfo, );
        } else {
            bool removed = QFile::remove(entry.absoluteFilePath());
            if (!removed) {
                setError(QString("Can not remove a file: %1").arg(entry.absoluteFilePath()));
            }
        }
    }
    bool removed = dir.rmdir(url);
    if (!removed) {
        setError(QString("Can not remove a dir: %1").arg(url));
    }
}

/************************************************************************/
/* GTest_RemoveTmpFile */
/************************************************************************/
void GTest_RemoveTmpFile::init(XMLTestFormat * /*tf*/, const QDomElement &el) {
    url = env->getVar( TEMP_DATA_DIR_ENV_ID ) + "/" + el.attribute(URL_ATTR);
}

Task::ReportResult GTest_RemoveTmpFile::report() {
    bool removed = QFile::remove(url);
    if (!removed) {
        setError(QString("Can not remove a file: %1").arg(url));
    }
    return ReportResult_Finished;
}

/************************************************************************/
/* GTest_CreateTmpFile */
/************************************************************************/
void GTest_CreateTmpFile::init(XMLTestFormat * /*tf*/, const QDomElement &el) {
    url = env->getVar( TEMP_DATA_DIR_ENV_ID ) + "/" + el.attribute(URL_ATTR);
    data = el.attribute(DATA_ATTR);
}

Task::ReportResult GTest_CreateTmpFile::report() {
    QFile file(url);
    bool created = file.open(QIODevice::WriteOnly);
    if (!created) {
        setError(QString("Can not create a file: %1").arg(url));
        return ReportResult_Finished;
    }
    file.write(data.replace("\\n", "\n").toLatin1());
    file.close();
    return ReportResult_Finished;
}

/************************************************************************/
/* GTest_CheckTmpFile */
/************************************************************************/
void GTest_CheckTmpFile::init(XMLTestFormat * /*tf*/, const QDomElement &el) {
    url = env->getVar( TEMP_DATA_DIR_ENV_ID ) + "/" + el.attribute(URL_ATTR);
    exists = bool(el.attribute(EXISTS_ATTR).toInt());
}

Task::ReportResult GTest_CheckTmpFile::report() {
    bool actual = QFile::exists(url);
    if (exists != actual) {
        setError(QString("File exist state failed. Expected: %1. Actual: %2").arg(exists).arg(actual));
    }
    return ReportResult_Finished;
}

/************************************************************************/
/* GTest_CheckStorageFile */
/************************************************************************/
void GTest_CheckStorageFile::init(XMLTestFormat *tf, const QDomElement &el) {
    Q_UNUSED(tf);
    storageUrl = AppContext::getAppFileStorage()->getStorageDir();
    fileName = el.attribute(URL_ATTR);
    exists = bool(el.attribute(EXISTS_ATTR).toInt());
}

Task::ReportResult GTest_CheckStorageFile::report() {
    bool actual = findRecursive(storageUrl);
    if (exists != actual) {
        setError(QString("File exist state failed. Expected: %1. Actual: %2").arg(exists).arg(actual));
    }
    return ReportResult_Finished;
}

bool GTest_CheckStorageFile::findRecursive(const QString& currentDirUrl) {
    QDir currentDir(currentDirUrl);
    QFileInfoList subDirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList fileList = currentDir.entryInfoList(QDir::Files);

    foreach (QFileInfo fileInfo, fileList) {
        if (fileName == fileInfo.fileName()) {
            return true;
        }
    }

    foreach (QFileInfo dirInfo, subDirList) {
        if (true == findRecursive(dirInfo.filePath())) {
            return true;
        }
    }

    return false;
}

/************************************************************************/
/* GTest_CheckCreationTime */
/************************************************************************/
#define URL_ATTR "url"
#define LESS_ATTR "lessThen"
#define MORE_ATTR "moreThen"
void GTest_CheckCreationTime::init(XMLTestFormat *tf, const QDomElement &el){
    Q_UNUSED(tf)
    url = el.attribute(URL_ATTR);
    XMLTestUtils::replacePrefix(env, url);

    QString lessThen_string = el.attribute(LESS_ATTR);
    QString moreThen_string = el.attribute(MORE_ATTR);
    if(lessThen_string.isEmpty() && moreThen_string.isEmpty()){
        setError("lessThen or moreThen tag should be set, but neither was set");
        return;
    }
    if(!lessThen_string.isEmpty() && !moreThen_string.isEmpty()){
        setError("lessThen or moreThen tag should be set, but both were set");
        return;
    }

    int lessThen_int = -1;
    int moreThen_int = -1;
    if(!lessThen_string.isEmpty()){
        bool ok;
        lessThen_int = lessThen_string.toInt(&ok);
        if(!ok){
            setError("lessThen tag is not a number");
            return;
        }
    }
    if(!moreThen_string.isEmpty()){
        bool ok;
        moreThen_int = moreThen_string.toInt(&ok);
        if(!ok){
            setError("moreThen tag is not a number");
            return;
        }
    }
    lessThen = lessThen_int;
    moreThen = moreThen_int;
}

Task::ReportResult GTest_CheckCreationTime::report(){

    QFile f(url);
    if(!f.exists()){
        setError("file " + url + " not found");
        return Task::ReportResult_Finished;
    }

    QFileInfo info(f);
    QDateTime created = info.created();
    QDateTime now = QDateTime::currentDateTime();
    int seconds = created.secsTo(now);

    if(lessThen != -1){
        if(seconds>lessThen){
            setError(QString("time is more then expected: %1").arg(seconds));
            return Task::ReportResult_Finished;
        }
    }
    if(moreThen != -1){
        if(seconds<moreThen){
            setError(QString("time is less then expected: %1").arg(seconds));
            return Task::ReportResult_Finished;
        }
    }
    return Task::ReportResult_Finished;
}

/************************************************************************/
/* GTest_CheckFilesNum */
/************************************************************************/
#define FOLDER_ATTR "folder"
#define EXP_NUM "expected"
void GTest_CheckFilesNum::init(XMLTestFormat *tf, const QDomElement &el){
    Q_UNUSED(tf)
    folder = el.attribute(FOLDER_ATTR);
    QString num_string = el.attribute(EXP_NUM);
    if(num_string.isEmpty()){
        setError("<expected> tag should be set");
        return;
    }
    bool ok;
    expectedNum = num_string.toInt(&ok);
    if(!ok){
        setError("<expected> tab sould be integer");
    }

}

Task::ReportResult GTest_CheckFilesNum::report(){
    XMLTestUtils::replacePrefix(env, folder);
    QDir d(folder);
    if(!d.exists()){
        setError("file " + d.absolutePath());
        return Task::ReportResult_Finished;
    }

    QFileInfoList list = d.entryInfoList();
    int actualNum = list.size();
    if(actualNum != expectedNum){
        setError(QString("Unexpected files number: %1").arg(actualNum));
        return Task::ReportResult_Finished;
    }
    return Task::ReportResult_Finished;
}

/*******************************
* GUrlTests
*******************************/
QList<XMLTestFactory*> GUrlTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_ConvertPath::createFactory());
    res.append(GTest_CreateTmpDir::createFactory());
    res.append(GTest_RemoveTmpDir::createFactory());
    res.append(GTest_RemoveTmpFile::createFactory());
    res.append(GTest_CreateTmpFile::createFactory());
    res.append(GTest_CheckTmpFile::createFactory());
    res.append(GTest_CheckStorageFile::createFactory());
    res.append(GTest_CheckCreationTime::createFactory());
    res.append(GTest_CheckFilesNum::createFactory());
    return res;
}


}//namespace
