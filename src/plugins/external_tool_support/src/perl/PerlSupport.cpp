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

#include "PerlSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/ScriptingToolRegistry.h>

namespace U2 {

const QString PerlSupport::ET_PERL_ID = "USUPP_PERL";

PerlSupport::PerlSupport()
    : RunnerTool(QStringList(), PerlSupport::ET_PERL_ID, "perl5", "perl") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/perl.png");
        grayIcon = QIcon(":external_tool_support/images/perl_gray.png");
        warnIcon = QIcon(":external_tool_support/images/perl_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "perl.exe";
#elif defined(Q_OS_UNIX)
    executableFileName = "perl";
#endif
    validMessage = "This is perl";
    validationArguments << "--version";

    description += tr("Perl scripts interpreter");
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+)");
    toolKitName = "perl";

    muted = true;
}

}  // namespace U2
