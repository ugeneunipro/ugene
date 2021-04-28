/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include "BundleInfoMac.h"

namespace U2 {

#ifdef Q_OS_DARWIN

QString BundleInfoMac::getDBundlePath() {
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                                  kCFURLPOSIXPathStyle);
    const char *bundlePath = CFStringGetCStringPtr(macPath,
                                                   CFStringGetSystemEncoding());
    CFRelease(appUrlRef);
    CFRelease(macPath);

    return QString(bundlePath);
}

QString BundleInfoMac::getExtraTranslationSearchPath(CMDLineRegistry *cmdLineRegistry) {
    QString translationFileDir = getDBundlePath() + "/Contents/Resources";
    QString transl = "transl_en";
    QString cmdlineTransl = cmdLineRegistry->getParameterValue(CMDLineCoreOptions::TRANSLATION);
    if (!cmdlineTransl.isEmpty()) {
        transl = QString("transl_") + cmdlineTransl;
    }
    return translationFileDir + "/" + transl;
}

QString BundleInfoMac::getDataSearchPath() {
    QString dir = getDBundlePath() + "/Contents/Resources/data";
    if (!QDir(dir).exists()) {    //data location in Resources
        dir = "";
    }
    return dir;
}

QString BundleInfoMac::getPluginsSearchPath() {
    QString dir = getDBundlePath() + "/Contents/Resources/plugins";
    if (!QDir(dir).exists()) {    //data location in Resources
        dir = "";
    }
    return dir;
}

QString BundleInfoMac::getToolsSearchPath() {
    QString dir = getDBundlePath() + "/Contents/Resources/tools";
    if (!QDir(dir).exists()) {    //data location in Resources
        dir = "";
    }
    return dir;
}

#endif

}    // namespace U2
