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

#ifndef _U2_CUSTOM_EXTERNAL_TOOL_H_
#define _U2_CUSTOM_EXTERNAL_TOOL_H_

#include "ExternalToolRegistry.h"

namespace U2 {

class U2CORE_EXPORT CustomExternalTool : public ExternalTool {
    Q_OBJECT
public:
    CustomExternalTool();

    void setId(const QString& id);
    void setName(const QString &name);
    void setIcon(const QIcon &icon);
    void setGrayIcon(const QIcon &icon);
    void setWarnIcon(const QIcon &icon);
    void setDescription(const QString &description);
    void setLauncher(const QString &launcherId);
    void setBinaryName(const QString &binaryName);
    void setValidationArguments(const QStringList &arguments);
    void setValidationExpectedText(const QString &text);
    void setPredefinedVersion(const QString &version);
    void setVersionRegExp(const QRegExp &versionRegExp);
    void setToolkitName(const QString &toolkitName);
    void setDependencies(const QStringList &dependencies);
    void setConfigFilePath(const QString &configFilePath);

    const QString &getConfigFilePath() const;

private:
    QString configFilePath;
};

}   // namespace U2

#endif // _U2_CUSTOM_EXTERNAL_TOOL_H_
