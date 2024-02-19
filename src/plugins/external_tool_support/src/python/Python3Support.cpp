/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Python3Support.h"

#include <U2Core/AppContext.h>

namespace U2 {

const QString Python3Support::ET_PYTHON_ID = "USUPP_PYTHON3";

Python3Support::Python3Support()
    : RunnerTool(QStringList(), Python3Support::ET_PYTHON_ID, "python3", "Python 3") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/python.png");
        grayIcon = QIcon(":external_tool_support/images/python_gray.png");
        warnIcon = QIcon(":external_tool_support/images/python_warn.png");
    }
    executableFileName = isOsWindows() ? "python.exe" : "python3";

    static const QString PYTHON_VERSION_REGEXP = "(\\d+.\\d+.\\d+)";

    validationMessageRegExp = "Python " + PYTHON_VERSION_REGEXP;
    validationArguments << "--version";

    description += tr("Python scripts interpreter");
    versionRegExp = QRegExp(PYTHON_VERSION_REGEXP);
    toolKitName = "python3";

    muted = true;
}

}  // namespace U2
