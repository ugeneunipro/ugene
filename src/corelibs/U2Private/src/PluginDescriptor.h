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

#ifndef _U2_PLUGINDESCRIPTOR_H_
#define _U2_PLUGINDESCRIPTOR_H_

#include <QObject>

#include <U2Core/global.h>
#include <U2Core/Version.h>
#include <U2Core/GUrl.h>

namespace U2 {

#define PLUGIN_FILE_EXT "plugin"

enum PlatformName {
    PlatformName_Unknown,
    PlatformName_Win,
    PlatformName_UnixNotMac,
    PlatformName_Mac
};

enum PlatformArch {
    PlatformArch_Unknown,
    PlatformArch_32,
    PlatformArch_64
};

enum PluginModeFlag {
    PluginMode_Malformed    = 1,
    PluginMode_Console      = 1 << 1,
    PluginMode_UI           = 1 << 2
};

typedef QFlags<PluginModeFlag> PluginMode;

class PlatformInfo {
public:
    PlatformInfo() : name(PlatformName_Unknown), arch(PlatformArch_Unknown) {}

    PlatformName name;
    PlatformArch arch;
    bool operator == (const PlatformInfo& p) const  {return name == p.name && arch == p.arch;}
};

class DependsInfo {
public:
    QString id;
    Version version;
};

class PluginDesc {
public:
    PluginDesc();

    QString             id;
    QString             name;
    Version             pluginVersion;
    Version             ugeneVersion;
    Version             qtVersion;
    QString             pluginVendor;
    GUrl                descriptorUrl;
    GUrl                libraryUrl;
    GUrl                licenseUrl;
    PlatformInfo        platform;
    PluginMode          mode;
    QList<DependsInfo>  dependsList;

    bool isValid() const {
        return !id.isEmpty() && !pluginVersion.text.isEmpty() && !ugeneVersion.text.isEmpty() && !libraryUrl.isEmpty();
    }

    bool operator == (const PluginDesc& pd) const ;
};


class U2PRIVATE_EXPORT PluginDescriptorHelper: public QObject {
    Q_OBJECT
private:
    PluginDescriptorHelper() {}
public:
    static PluginDesc readPluginDescriptor(const QString& url, QString& error);

    //orders plugins by loading order
    static QList<PluginDesc> orderPlugins(const QList<PluginDesc>& unordered, QString& err);

};


} //namespace

#endif

