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

#include "BundleInfo.h"

#include <QDir>

#ifdef Q_OS_DARWIN
#    include <CoreFoundation/CoreFoundation.h>

#    include <U2Core/CMDLineCoreOptions.h>
#    include <U2Core/CMDLineRegistry.h>
#endif

namespace U2 {

#ifdef Q_OS_DARWIN
static QString getMacBundlePath() {
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                                  kCFURLPOSIXPathStyle);
    const char* bundlePath = CFStringGetCStringPtr(macPath,
                                                   CFStringGetSystemEncoding());
    CFRelease(appUrlRef);
    CFRelease(macPath);

    return QString(bundlePath);
}
#endif

QString BundleInfo::getExtraTranslationSearchPath(CMDLineRegistry* cmdLineRegistry) {
#ifdef Q_OS_DARWIN
    QString translationFileDir = getMacBundlePath() + "/Contents/Resources";
    QString transl = "transl_en";
    QString cmdlineTransl = cmdLineRegistry->getParameterValue(CMDLineCoreOptions::TRANSLATION);
    if (!cmdlineTransl.isEmpty()) {
        transl = QString("transl_") + cmdlineTransl;
    }
    return translationFileDir + "/" + transl;
#else
    Q_UNUSED(cmdLineRegistry);
    return "";
#endif
}

QString BundleInfo::getDataSearchPath() {
#ifdef Q_OS_DARWIN
    QString dir = getMacBundlePath() + "/Contents/Resources/data";
    return QDir(dir).exists() ? dir : "";
#else
    return "";
#endif
}

QString BundleInfo::getPluginsSearchPath() {
#ifdef Q_OS_DARWIN
    QString dir = getMacBundlePath() + "/Contents/Resources/plugins";
    return QDir(dir).exists() ? dir : "";
#else
    return "";
#endif
}

QString BundleInfo::getToolsSearchPath() {
#ifdef Q_OS_DARWIN
    QString dir = getMacBundlePath() + "/Contents/Resources/tools";
    return QDir(dir).exists() ? dir : "";
#else
    return "";
#endif
}

}  // namespace U2
