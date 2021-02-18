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

#include "PythonSupport.h"

#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "seqpos/SeqPosSupport.h"

namespace U2 {

const QString PythonSupport::ET_PYTHON_ID = "USUPP_PYTHON2";
const QString PythonModuleDjangoSupport::ET_PYTHON_DJANGO_ID = "DJANGO";
const QString PythonModuleNumpySupport::ET_PYTHON_NUMPY_ID = "NUMPY";
const QString PythonModuleBioSupport::ET_PYTHON_BIO_ID = "BIO";

PythonSupport::PythonSupport()
    : RunnerTool(QStringList(), PythonSupport::ET_PYTHON_ID, "python2", "python") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/python.png");
        grayIcon = QIcon(":external_tool_support/images/python_gray.png");
        warnIcon = QIcon(":external_tool_support/images/python_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "python.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "python2.7";
#    endif
#endif
    static const QString PYTHON_VERSION_REGEXP = "(\\d+.\\d+.\\d+)";

    validMessage = "Python " + PYTHON_VERSION_REGEXP;
    validationArguments << "--version";

    description += tr("Python scripts interpreter");
    versionRegExp = QRegExp(PYTHON_VERSION_REGEXP);
    toolKitName = "python";

    muted = true;
}

PythonModuleSupport::PythonModuleSupport(const QString &id, const QString &name)
    : ExternalToolModule(id, "python2", name) {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/python.png");
        grayIcon = QIcon(":external_tool_support/images/python_gray.png");
        warnIcon = QIcon(":external_tool_support/images/python_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "python.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "python2.7";
#    endif
#endif

    validationArguments << "-c";

    toolKitName = "python";
    dependencies << PythonSupport::ET_PYTHON_ID;

    errorDescriptions.insert("No module named", tr("Python module is not installed. "
                                                   "Install module or set path "
                                                   "to another Python scripts interpreter "
                                                   "with installed module in "
                                                   "the External Tools settings"));

    muted = true;
}

PythonModuleDjangoSupport::PythonModuleDjangoSupport()
    : PythonModuleSupport(PythonModuleDjangoSupport::ET_PYTHON_DJANGO_ID, "django") {
    description += "django" + tr(": Python module for the %1 tool").arg(SeqPosSupport::ET_SEQPOS);

    validationArguments << "import django;print(\"django version: \", django.VERSION);";
    validMessage = "django version:";
    versionRegExp = QRegExp("(\\d+,\\s\\d+,\\s\\d+)");
}

PythonModuleNumpySupport::PythonModuleNumpySupport()
    : PythonModuleSupport(PythonModuleNumpySupport::ET_PYTHON_NUMPY_ID, "numpy") {
    description += "numpy" + tr(": Python module for the %1 tool").arg(SeqPosSupport::ET_SEQPOS);

    validationArguments << "import numpy;print(\"numpy version: \", numpy.__version__);";
    validMessage = "numpy version:";
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+)");
}

PythonModuleBioSupport::PythonModuleBioSupport()
    : PythonModuleSupport(PythonModuleBioSupport::ET_PYTHON_BIO_ID, "Bio") {
    description += "Bio" + tr(" (or biopython) is a python module for biological computations.");

    validationArguments << "import Bio;print(\"Bio version: \", Bio.__version__);";
    validMessage = "Bio version:";
    versionRegExp = QRegExp("(\\d+.\\d+)");
}

}    // namespace U2
