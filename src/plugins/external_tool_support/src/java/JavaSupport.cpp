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

#include <U2Core/AppContext.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "JavaSupport.h"

namespace U2 {

const QString JavaSupport::ARCHITECTURE = "architecture";
const QString JavaSupport::ARCHITECTURE_X32 = "x32";
const QString JavaSupport::ARCHITECTURE_X64 = "x64";

JavaSupport::JavaSupport(const QString &name, const QString &path)
    : ExternalTool(name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "java.exe";
#elif defined(Q_OS_UNIX)
    executableFileName = "java";
#endif

    validMessage = "version \"\\d+.\\d+.\\d+(_\\d+)?";
    validationArguments << "-version";

    description += tr("Java Platform lets you develop and deploy Java applications on desktops and servers.<br><i>(Requires Java 1.7 or higher)</i>.<br>"
                      "Java can be freely downloaded on the official web-site: https://www.java.com/en/download/");
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+(_\\d+)?)");
    toolKitName="Java";

    muted = true;

    connect(this, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolValidationStatusChanged(bool)));
}

void JavaSupport::getAdditionalParameters(const QString& output) {
    Architecture architecture = x32;
    if (output.contains("64-Bit")) {
        architecture = x64;
    }
    additionalInfo.insert(ARCHITECTURE, architecture2string(architecture));
}

JavaSupport::Architecture JavaSupport::getArchitecture() const {
    return string2architecture(additionalInfo.value(ARCHITECTURE));
}

void JavaSupport::sl_toolValidationStatusChanged(bool isValid) {
    Q_UNUSED(isValid);
    ScriptingTool::onPathChanged(this, QStringList() << "-jar");
}

QString JavaSupport::architecture2string(Architecture architecture) {
    switch (architecture) {
    case JavaSupport::x32:
        return ARCHITECTURE_X32;
    case JavaSupport::x64:
        return ARCHITECTURE_X64;
    default:
        FAIL("An unknown architecture", "");
    }
}

JavaSupport::Architecture JavaSupport::string2architecture(const QString &string) {
    if (ARCHITECTURE_X32 == string) {
        return x32;
    } else if (ARCHITECTURE_X64 == string) {
        return x64;
    } else {
        return x32;
    }
}

} // U2
