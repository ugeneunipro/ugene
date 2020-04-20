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

#include "FastqcSupport.h"
#include "java/JavaSupport.h"
#include "perl/PerlSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/ScriptingToolRegistry.h>

#include <U2Formats/ConvertFileTask.h>

namespace U2 {

const QString FastQCSupport::ET_FASTQC = "FastQC";
const QString FastQCSupport::ET_FASTQC_ID = "USUPP_FASTQC";

FastQCSupport::FastQCSupport(const QString& id, const QString& name, const QString& path) : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName="fastqc";

    validMessage = "FastQC v";
    description = tr("<i>FastQC</i>: A quality control tool for high throughput sequence data.");

    versionRegExp = QRegExp("FastQC v(\\d+.\\d+.\\d+)");
    toolKitName = "FastQC";

    toolRunnerProgram = PerlSupport::ET_PERL_ID;
    dependencies << JavaSupport::ET_JAVA_ID;
    dependencies << PerlSupport::ET_PERL_ID;

    ExternalTool *java = getJava();
    CHECK(NULL != java, );
    connect(java, SIGNAL(si_pathChanged()), SLOT(sl_javaPathChanged()));
    sl_javaPathChanged();
}

void FastQCSupport::sl_javaPathChanged() {
    ExternalTool *java = getJava();
    CHECK(NULL != java, );

    validationArguments.clear();
    validationArguments << "-v";
    validationArguments << "-java";
    validationArguments << java->getPath();
}

ExternalTool * FastQCSupport::getJava() {
    ExternalToolRegistry *registry = AppContext::getExternalToolRegistry();
    SAFE_POINT(NULL != registry, L10N::nullPointerError("External tool registry"), NULL);
    ExternalTool *java = registry->getById(JavaSupport::ET_JAVA_ID);
    SAFE_POINT(NULL != java, L10N::nullPointerError("Java tool"), NULL);
    return java;
}

}//namespace

