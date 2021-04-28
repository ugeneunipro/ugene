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

#ifndef _U2_BUNDLE_INFO_MAC_H_
#define _U2_BUNDLE_INFO_MAC_H_

#include <QCoreApplication>

#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>

namespace U2 {

class BundleInfoMac {
public:
#ifdef Q_OS_DARWIN
    static QString getDBundlePath();
    static QString getExtraTranslationSearchPath(CMDLineRegistry *);
    static QString getDataSearchPath();
    static QString getPluginsSearchPath();
    static QString getToolsSearchPath();
#else
    static QString getDBundlePath() { return ""; }
    static QString getExtraTranslationSearchPath(CMDLineRegistry *) { return ""; }
    static QString getDataSearchPath() { return ""; }
    static QString getPluginsSearchPath() { return ""; }
    static QString getToolsSearchPath() { return ""; }
#endif

};

}    // namespace U2

#endif    // _U2_BUNDLE_INFO_MAC_H_
