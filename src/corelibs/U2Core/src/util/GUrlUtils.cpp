/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/Log.h>
#include <U2Core/Task.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "GUrlUtils.h"

namespace U2 {

QString GUrlUtils::getUncompressedExtension(const GUrl& url) {
    QString ext = url.lastFileSuffix();
    if (ext == "gz") {
        QString completeSuffix = url.completeFileSuffix();
        QStringList suffixList = completeSuffix.split(".");
        ext = (suffixList.size() < 2) ? QString() : suffixList[suffixList.size() - 2];
    }
    return ext;
}

QString GUrlUtils::getUncompressedCompleteBaseName(const GUrl &url) {
    QString filePath = url.getURLString();
    if ("gz" == url.lastFileSuffix())  {
        filePath.chop(QString(".gz").length());
    }
    return QFileInfo(filePath).completeBaseName();
}

GUrl GUrlUtils::ensureFileExt(const GUrl& url, const QStringList& typeExt) {
    SAFE_POINT(!typeExt.isEmpty(), "Type extension is empty!", GUrl());

    if( url.isVFSFile() ) {
        return url;
    }

    if (typeExt.isEmpty()) {
        return url;
    }

    if (typeExt.contains(GUrlUtils::getUncompressedExtension(url))) {
        return url;
    }
    return GUrl(url.getURLString() + "."  + typeExt.first(), url.getType());
}

bool GUrlUtils::containSpaces(const QString &string) {
    return string.contains(QRegExp("\\s"));
}

GUrl GUrlUtils::changeFileExt(const GUrl &url, const DocumentFormatId &oldFormatId, const DocumentFormatId &newFormatId) {
    CHECK(url.isLocalFile(), GUrl());
    DocumentFormatRegistry *dfRegistry = AppContext::getDocumentFormatRegistry();
    CHECK(NULL != dfRegistry, GUrl());
    DocumentFormat *oldFormat = dfRegistry->getFormatById(oldFormatId);
    CHECK(NULL != oldFormat, GUrl());
    DocumentFormat *newFormat = dfRegistry->getFormatById(newFormatId);
    CHECK(NULL != newFormat, GUrl());

    const QString dirPath = url.dirPath();
    const QString baseFileName = url.baseFileName();
    QString completeSuffix = url.completeFileSuffix();

    bool isCompressed = false;
    if (completeSuffix.endsWith("gz")) {
        isCompressed = true;
        completeSuffix.chop(QString(".gz").size());
    }

    const QString currentExtension = completeSuffix.mid(completeSuffix.lastIndexOf(".") + 1);
    if (oldFormat->getSupportedDocumentFileExtensions().contains(currentExtension)) {
        completeSuffix.chop(currentExtension.size() + 1);
    }

    const QStringList newFormatExtensions = newFormat->getSupportedDocumentFileExtensions();
    if (!newFormatExtensions.isEmpty()) {
        completeSuffix += "." + newFormatExtensions.first();
    }

    if (isCompressed) {
        completeSuffix += ".gz";
    }

    const QString dotCompleteSuffix = (completeSuffix.startsWith(".") ? completeSuffix : "." + completeSuffix);
    return GUrl(dirPath + QDir::separator() + baseFileName + dotCompleteSuffix);
}

GUrl GUrlUtils::changeFileExt(const GUrl &url, const DocumentFormatId &newFormatId) {
    CHECK(url.isLocalFile(), GUrl());
    DocumentFormatRegistry *dfRegistry = AppContext::getDocumentFormatRegistry();
    CHECK(NULL != dfRegistry, GUrl());
    DocumentFormat *newFormat = dfRegistry->getFormatById(newFormatId);
    CHECK(NULL != newFormat, GUrl());

    const QString dirPath = url.dirPath();
    const QString baseFileName = url.baseFileName();
    QString completeSuffix = url.completeFileSuffix();

    bool isCompressed = false;
    if (completeSuffix.endsWith("gz")) {
        isCompressed = true;
        completeSuffix.chop(QString(".gz").size());
    }

    const QString currentExtension = completeSuffix.mid(completeSuffix.lastIndexOf(".") + 1);
    completeSuffix.chop(currentExtension.size() + 1);

    const QStringList newFormatExtensions = newFormat->getSupportedDocumentFileExtensions();
    if (!newFormatExtensions.isEmpty()) {
        completeSuffix += "." + newFormatExtensions.first();
    }

    if (isCompressed) {
        completeSuffix += ".gz";
    }

    const QString dotCompleteSuffix = (completeSuffix.startsWith(".") ? completeSuffix : "." + completeSuffix);
    return GUrl(dirPath + QDir::separator() + baseFileName + dotCompleteSuffix);
}

bool GUrlUtils::renameFileWithNameRoll(const QString& original, TaskStateInfo& ti, const QSet<QString>& excludeList, Logger* log) {
    QString rolled = GUrlUtils::rollFileName(original, "_oldcopy", excludeList);
    if (rolled == original) {
        return true;
    }
    if (QFile(original).rename(rolled)) {
        if (log) {
            log->details(tr("Renamed %1 to %2").arg(original).arg(rolled));
        }
        return true;
    } else {
        ti.setError(tr("Failed to rename %1 to %2").arg(original).arg(rolled));
        return false;
    }
}

static void getPreNPost(const QString &originalUrl, QString &pre, QString &post, int &i, const QString &rolledSuffix) {
    i = 0;
    pre = originalUrl;
    int idx = pre.lastIndexOf(".");

    if (idx != -1) {
        QString extSuffix = pre.mid(idx);
        if(extSuffix == ".gz") {
            pre.chop(extSuffix.length());
            idx = pre.lastIndexOf(".");
            if(idx != -1) {
                extSuffix = pre.mid(idx) + extSuffix;
                pre.append(".gz");
            }
        }
        if (!extSuffix.contains("\\") && !extSuffix.contains("/")) {
            post = extSuffix;
            pre.chop(extSuffix.length());
        }
        idx = pre.lastIndexOf(rolledSuffix);
        if (idx != -1) {
            QString possibleNumberString = pre.mid(idx + rolledSuffix.length());
            int possibleNumber = possibleNumberString.toInt();
            if (possibleNumber > 0 && QString::number(possibleNumber) == possibleNumberString) {
                i = possibleNumber;
                pre = pre.left(idx);
            }
        }
    }
}

QString GUrlUtils::insertSuffix(const QString &originalUrl, const QString &suffix) {
    QString pre, post;
    int i = 0;
    getPreNPost(originalUrl, pre, post, i,suffix);
    return pre + suffix + post;
}

QStringList GUrlUtils::getRolledFilesList(const QString& originalUrl, const QString& rolledSuffix) {
    QString pre, post; //pre and post url parts. A number will be placed between
    int i = 0;
    getPreNPost(originalUrl, pre, post, i, rolledSuffix);

    QString resultUrl = originalUrl;
    QStringList urls;
    while (QFile::exists(resultUrl)) {
        urls << resultUrl;
        resultUrl = pre + rolledSuffix + QString("%1").arg(++i) + post;
    }
    return urls;
}

QString GUrlUtils::rollFileName(const QString& originalUrl, const QString& rolledSuffix, const QSet<QString>& excludeList) {
    QString pre, post; //pre and post url parts. A number will be placed between
    int i = 0;
    getPreNPost(originalUrl, pre, post, i, rolledSuffix);

    QString resultUrl = originalUrl;
    while (QFile::exists(resultUrl) || excludeList.contains(resultUrl)) {
        resultUrl = pre + rolledSuffix + QString("%1").arg(++i) + post;
    }
    return resultUrl;
}

QUrl GUrlUtils::gUrl2qUrl( const GUrl& gurl) {
    if( gurl.isVFSFile() ) {
        return QUrl();
    }

    QString str = gurl.getURLString();
    if (str.startsWith("http://") || str.startsWith("https://") || str.startsWith("file://")) {
        return str;
    } else {
        return QUrl("file:///" + QFileInfo(str).absoluteFilePath());
    }
}

QList<QUrl> GUrlUtils::gUrls2qUrls( const QList<GUrl>& gurls) {
    QList<QUrl> urls;
    foreach(const GUrl& gurl, gurls) {
        urls << gUrl2qUrl(gurl);
    }
    return urls;
}

GUrl GUrlUtils::qUrl2gUrl( const QUrl& qurl) {
    QString str = qurl.toString();
    return GUrl(str);
}

QList<GUrl> GUrlUtils::qUrls2gUrls( const QList<QUrl>& qurls) {
    QList<GUrl> urls;
    foreach(const QUrl& qurl, qurls) {
        urls << qUrl2gUrl(qurl);
    }
    return urls;
}


QString GUrlUtils::prepareFileName(const QString& url, int count, const QStringList& typeExt) {
    return prepareFileName(url, QString("%1").arg(count, 3, 10, QChar('0')), typeExt);
}

QString GUrlUtils::prepareFileName(const QString& url, const QString& baseSuffix, const QStringList& typeExt) {
    QFileInfo fi(url);
    QStringList suffixList = fi.completeSuffix().split(".");
    QString ext;
    foreach(const QString& suffix, suffixList) {
        if (typeExt.contains(suffix)) {
            ext = suffix;
            break;
        }
    }
    if (ext.isEmpty()) {
        return url + QString(".%1.%2").arg(baseSuffix).arg(typeExt.first());
    }
    QString base = fi.baseName();
    while(!suffixList.isEmpty()) {
        QString nextSuffix = suffixList.takeFirst();
        if (nextSuffix == ext) {
            break;
        }
        base += "." + nextSuffix;
    }
    QString result = QString("%1/%2.%3.%4").arg(fi.dir().path()).arg(base).arg(baseSuffix).arg(ext);
    if (!suffixList.isEmpty()) {
        result.append(".").append(suffixList.join("."));
    }

    return result;
}

// checks that file path is valid: creates required folder if needed.
// Returns canonical path to file. Does not create nor remove file, affects just folder
// Sample usage: processing URLs in "save file" inputs
QString GUrlUtils::prepareFileLocation(const QString& filePath, U2OpStatus& os) {
    QFileInfo fi(filePath);
    QString dirPath = fi.absoluteDir().absolutePath();
    QString canonicalDirPath = prepareDirLocation(dirPath, os);
    CHECK_OP(os, QString());
    // examples with "/" at the end:
    QString result = canonicalDirPath + (canonicalDirPath.endsWith("/") ? "" : "/") + fi.fileName();
    return result;
}

// checks that dir path is valid. Creates the folder if needed.
// Returns absolute (without "." or ".." but with symlinks) folder path.
// Does not affect folder if already exists.
// Sample usage: processing URLs in "save dir" inputs
QString GUrlUtils::prepareDirLocation(const QString& dirPath, U2OpStatus& os) {
    CHECK_EXT(!dirPath.isEmpty(), os.setError(tr("Folder is not specified")), QString());
    QDir targetDir(dirPath);
    if (!targetDir.exists()) {
        QString absPath = targetDir.absolutePath();
        if (!targetDir.mkpath(absPath)) {
            os.setError(tr("Folder can't be created: %1").arg(absPath));
            return QString();
        }
        targetDir = QDir(absPath); //It looks like QT caches results for QDir? Create new QDir instance in this case!
        if (!targetDir.isReadable()) {
            os.setError(tr("Folder can't be read: %1").arg(absPath));
            return QString();
        }
    }
    QString result = targetDir.absolutePath();
    return result;
}


QString GUrlUtils::prepareTmpFileLocation(const QString& dir, const QString& prefix, const QString& ext, U2OpStatus& os) {
    int i = 0;
    QString result;
    while (true) {
        QString name = QString("%1_%2.%3").arg(prefix).arg(i).arg(ext);
        QString filePath = prepareFileLocation(dir + "/" + name, os);
        CHECK_OP(os, result);
        if (!QFile::exists(filePath)) {
            result = filePath;
            break;
        }
        i++;
    }
    return result;
}


void GUrlUtils::removeDir(const QString& dirPath, U2OpStatus& os) {
    QDir dir(dirPath);
    CHECK(dir.exists(), );
    QList<QFileInfo> entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    foreach(const QFileInfo& info, entries) {
        if (info.isDir()) {
            removeDir(info.absoluteFilePath(), os);
        } else {
            QFile::remove(info.absoluteFilePath());
        }
        CHECK_OP(os, );
    }
    QDir().rmdir(dirPath);
}

void GUrlUtils::removeFile( const QString& filePath, U2OpStatus& os ){
    CHECK_EXT(!filePath.isEmpty(), os.setError(tr("File path is not specified")), );
    QFileInfo info(filePath);

    CHECK_EXT(!info.isDir(), os.setError(tr("Folder path instead of file path")), );

    if(info.exists()){
        QFile::remove(info.absoluteFilePath());
    }

}

bool GUrlUtils::canWriteFile(const QString &path) {
    bool res = false;

#ifdef Q_OS_WIN // a workaround of that files with a colon in names can be created but are not accessible on Windows
    if (QFileInfo(path).fileName().contains(':')) {
        return false;
    }
#endif

    QFile tmpFile(path);
    if (!tmpFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    if (tmpFile.isWritable()) {
        res = true;
        tmpFile.close();
        tmpFile.remove();
    } else {
        res = false;
    }
    return res;
}

QString GUrlUtils::getDefaultDataPath(){
    QString res;

    QString path = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath();

    if (!QDir(path).exists()) {
        if (!QDir().mkpath(path)) {
            return res;
        }
    }
    res = path;

    return res;
}

QString GUrlUtils::getQuotedString( const QString& inString ){
    if (inString.contains(QRegExp("\\s"))) {
        return "\"" + inString + "\"";
    }
    return inString;
}

QString GUrlUtils::createDirectory(const QString &path, const QString &suffix, U2OpStatus &os) {
    QString newPath = rollFileName(path, suffix, QSet<QString>());
    QDir dir(newPath);
    bool created = dir.mkpath(newPath);
    if (!created) {
        os.setError(tr("Can not create a folder: %1").arg(newPath));
    }
    return newPath;
}

namespace {
    QString getDotExtension(const DocumentFormatId &formatId) {
        DocumentFormatRegistry *dfr = AppContext::getDocumentFormatRegistry();
        SAFE_POINT(NULL != dfr, "NULL document format registry", "");

        DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
        CHECK(NULL != format, "");

        QStringList results = format->getSupportedDocumentFileExtensions();
        CHECK(!results.isEmpty(), "");

        return "." + results.first();
    }
}

void GUrlUtils::getLocalPathFromUrl(const GUrl &url, const QString &defaultBaseFileName, QString &dirPath, QString &baseFileName) {
    if (url.isLocalFile()) {
        dirPath = url.dirPath();
        baseFileName = url.baseFileName();
    } else {
        dirPath = getDefaultDataPath();
        baseFileName = fixFileName(defaultBaseFileName);
    }
}

QString GUrlUtils::getLocalUrlFromUrl(const GUrl &url, const QString &defaultBaseFileName, const QString &dotExtention, const QString &suffix) {
    QString dirPath;
    QString baseFileName;
    GUrlUtils::getLocalPathFromUrl(url, defaultBaseFileName, dirPath, baseFileName);
    QString result = dirPath + QDir::separator() + baseFileName + suffix + dotExtention;
    return QDir::toNativeSeparators(result);
}

QString GUrlUtils::getNewLocalUrlByFormat(const GUrl &url, const QString &defaultBaseFileName, const DocumentFormatId &format, const QString &suffix) {
    return getNewLocalUrlByExtention(url, defaultBaseFileName, getDotExtension(format), suffix);
}

QString GUrlUtils::getNewLocalUrlByExtention(const GUrl &url, const QString &defaultBaseFileName, const QString &dotExtention, const QString &suffix) {
    QString result = getLocalUrlFromUrl(url, defaultBaseFileName, dotExtention, suffix);
    return rollFileName(result, DocumentUtils::getNewDocFileNameExcludesHint());
}

void GUrlUtils::validateLocalFileUrl(const GUrl &url, U2OpStatus &os, const QString &urlName) {
    QString urlStr = url.getURLString();
    if (!url.isLocalFile()) {
        os.setError(tr("%1 is not a local file [%2].").arg(urlName).arg(urlStr));
        return;
    }

    QFileInfo info(urlStr);
    if (!info.exists()) {
        QString dirUrl = info.dir().absolutePath();
        bool created = QDir().mkpath(dirUrl);
        if (!created) {
            os.setError(tr("Can not create a folder [%1].").arg(dirUrl));
        }
        return;
    }
    if (info.isDir()) {
        os.setError(tr("%1 is a folder [%2].").arg(urlName).arg(urlStr));
        return;
    }
}

QString GUrlUtils::getPairedFastqFilesBaseName(const QString &sourceFileUrl, bool truncate) {
    static const QStringList pairedSuffixes = QStringList() << "-R1" << "-R2"
                                                            << "_1" << "_2"
                                                            << "_R1_001" << "_R2_001"
                                                            << "_R1" << "_R2";
    QString baseName = QFileInfo(sourceFileUrl).completeBaseName();
    if (truncate) {
        foreach (const QString &suffix, pairedSuffixes) {
            if (baseName.endsWith(suffix)) {
                baseName.chop(suffix.length());
                break;
            }
        }
    }
    return baseName;
}

QString GUrlUtils::fixFileName(const QString &fileName) {
    QString result = fileName;
    result.replace(QRegExp("[^0-9a-zA-Z._\\-]"), "_");
    result.replace(QRegExp("_+"), "_");

    /* We truncate long file names a little bit more to allow suffix adjustments (rolling) later. */
    result.truncate(MAX_OS_FILE_NAME_LENGTH - 50);
    return result;
}

QString GUrlUtils::getSlashEndedPath(const QString &dirPath) {
    return dirPath.endsWith("/") ? dirPath : dirPath + "/";
}

}//namespace
