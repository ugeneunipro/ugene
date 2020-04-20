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

#include "CufflinksSupport.h"

#include <U2Core/AppContext.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

const QString  CufflinksSupport::ET_CUFFCOMPARE = "Cuffcompare";
const QString  CufflinksSupport::ET_CUFFCOMPARE_ID = "USUPP_CUFFCOMPARE";
const QString  CufflinksSupport::ET_CUFFDIFF = "Cuffdiff";
const QString  CufflinksSupport::ET_CUFFDIFF_ID = "USUPP_CUFFDIFF";
const QString  CufflinksSupport::ET_CUFFLINKS = "Cufflinks";
const QString  CufflinksSupport::ET_CUFFLINKS_ID = "USUPP_CUFFLINKS";
const QString  CufflinksSupport::ET_CUFFMERGE = "Cuffmerge";
const QString  CufflinksSupport::ET_CUFFMERGE_ID = "USUPP_CUFFMERGE";
const QString  CufflinksSupport::ET_GFFREAD = "Gffread";
const QString  CufflinksSupport::ET_GFFREAD_ID = "USUPP_GFFREAD";

const QString  CufflinksSupport::CUFFLINKS_TMP_DIR = "cufflinks";
const QString  CufflinksSupport::CUFFDIFF_TMP_DIR = "cuffdiff";
const QString  CufflinksSupport::CUFFMERGE_TMP_DIR = "cuffmerge";

CufflinksSupport::CufflinksSupport(const QString& id,  const QString& name, const QString& path)
    : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "Cufflinks";
    versionRegExp=QRegExp("v(\\d+\\.\\d+\\.\\d+)");

    // Cuffcompare
    if (name == ET_CUFFCOMPARE) {
#ifdef Q_OS_WIN
        executableFileName = "cuffcompare.exe";
#else
#if defined(Q_OS_UNIX)
        executableFileName = "cuffcompare";
#endif
#endif

        validMessage = "cuffcompare";
        description = CufflinksSupport::tr("<i>Cuffcompare</i> helps"
            " comparing assembled transcripts to a reference annotation,"
            " and also tracking transcripts across multiple experiments.");
    }

    // Cuffdiff
    else if (name == ET_CUFFDIFF) {
#ifdef Q_OS_WIN
        executableFileName = "cuffdiff.exe";
#else
#if defined(Q_OS_UNIX)
        executableFileName = "cuffdiff";
#endif
#endif

        validMessage = "cuffdiff";
        description = CufflinksSupport::tr("<i>Cuffdiff</i> &nbsp;tests for"
            " differential expression and regulation in RNA-Seq samples.");
    }

    // Cufflinks
    else if (name == ET_CUFFLINKS) {
#ifdef Q_OS_WIN
        executableFileName = "cufflinks.exe";
#else
#if defined(Q_OS_UNIX)
        executableFileName = "cufflinks";
#endif
#endif

        validMessage = "cufflinks";
        description = CufflinksSupport::tr("<i>Cufflinks</i> assembles transcripts"
            " and estimates their abundances.");
    }


    // Cuffmerge
    else if (name == ET_CUFFMERGE) {
#ifdef Q_OS_WIN
        executableFileName = "cuffmerge.py";
#else
#if defined(Q_OS_UNIX)
        executableFileName = "cuffmerge";
#endif
#endif

        validMessage = "cuffmerge";
        description = CufflinksSupport::tr("<i>Cuffmerge</i> merges together several assemblies.");
    }


    // Gffread
    else if (name == ET_GFFREAD) {
#ifdef Q_OS_WIN
        executableFileName = "gffread.exe";
#elif defined(Q_OS_UNIX)
        executableFileName = "gffread";
#endif
        validMessage = "gffread <input_gff>";
        validationArguments << "--help";
        description = CufflinksSupport::tr("<i>Gffread</i> is used to verify or perform various operations on GFF files.");
    }

    muted = true;
}

} // namespace
