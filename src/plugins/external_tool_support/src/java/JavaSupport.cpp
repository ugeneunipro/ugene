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

#include "JavaSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString JavaSupport::ET_JAVA_ID = "USUPP_JAVA";

const QStringList JavaSupport::RUN_PARAMETERS = {"-jar"};

JavaSupport::JavaSupport()
    : RunnerTool(RUN_PARAMETERS, JavaSupport::ET_JAVA_ID, "java", "java") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
    executableFileName = isOsWindows() ? "java.exe" : "java";
    validMessage = "version \"\\d+.\\d+.\\d+(_\\d+)?";
    validationArguments << "-version";

    description += tr("Java Platform lets you develop and deploy Java applications on desktops and servers.<br><i>(Requires Java 8 or higher)</i>.<br>"
                      "Java can be freely downloaded on the official web-site: https://www.java.com/en/download/");
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+(_\\d+)?)");
    toolKitName = "Java";

    muted = true;
}

}  // namespace U2
