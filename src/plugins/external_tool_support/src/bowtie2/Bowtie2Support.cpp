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

#include "Bowtie2Support.h"

#include <U2Core/AppContext.h>

#include <U2Gui/MainWindow.h>

#include "perl/PerlSupport.h"
#include "python/PythonSupport.h"

namespace U2 {

const QString Bowtie2Support::BOWTIE2_TMP_DIR = "bowtie2";
const QString Bowtie2Support::ET_BOWTIE2_ALIGN = "Bowtie 2 aligner";
const QString Bowtie2Support::ET_BOWTIE2_ALIGN_ID = "USUPP_BOWTIE2";
const QString Bowtie2Support::ET_BOWTIE2_BUILD = "Bowtie 2 build indexer";
const QString Bowtie2Support::ET_BOWTIE2_BUILD_ID = "USUPP_BOWTIE2_BUILD";
const QString Bowtie2Support::ET_BOWTIE2_INSPECT = "Bowtie 2 index inspector";
const QString Bowtie2Support::ET_BOWTIE2_INSPECT_ID = "USUPP_BOWTIE2_INSPECT";

Bowtie2Support::Bowtie2Support(const QString& id, const QString& name, const QString& path /* = */ )
    : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "Bowtie2";
    versionRegExp = QRegExp("version (\\d+\\.\\d+\\.\\d+[.]{0,1}[\\d+]{0,1})");

    if (id == ET_BOWTIE2_ALIGN_ID) { // Bowtie2-align
        toolRunnerProgram = PerlSupport::ET_PERL_ID;
        dependencies << PerlSupport::ET_PERL_ID;
        executableFileName = "bowtie2";
        validationArguments << "--help";
        validMessage = "bowtie2";
        description = tr("<i>Bowtie 2 aligner</i> takes a Bowtie 2 index"
                         " and a set of sequencing read files and outputs a set of alignments.");
    } else if (id == ET_BOWTIE2_BUILD_ID) { // Bowtie2-build
        toolRunnerProgram = PythonSupport::ET_PYTHON_ID;
        dependencies << PythonSupport::ET_PYTHON_ID;
        executableFileName = "bowtie2-build";
        validationArguments << "--version";
        validMessage = "bowtie2-build";
        description = tr("<i>Bowtie 2 build indexer</i> "
                         " builds a Bowtie index from a set of DNA sequences. It outputs"
                         " a set of 6 files with suffixes .1.bt2, .2.bt2, .3.bt2, .4.bt2,"
                         " .rev.1.bt2, and .rev.2.bt2. These files together constitute the index:"
                         " they are all that is needed to align reads to that reference."
                         " The original sequence files are no longer used by <i>Bowtie 2</i>"
                         " once the index is built.");
    } else if (id == ET_BOWTIE2_INSPECT_ID) { // Bowtie2-inspect
        toolRunnerProgram = PythonSupport::ET_PYTHON_ID;
        dependencies << PythonSupport::ET_PYTHON_ID;
        executableFileName = "bowtie2-inspect";
        validationArguments << "--version";
        validMessage = "bowtie2-inspect";
        description = tr("<i>Bowtie 2 index inspector</i>"
                         " extracts information from a Bowtie index about what kind"
                         " of index it is and what reference sequence were used to build it.");
    }
}

} // namespace
