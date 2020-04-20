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

#include "VcfConsensusSupport.h"
#include "perl/PerlSupport.h"
#include "samtools/TabixSupport.h"

#include <U2Core/AppContext.h>
#include <U2Gui/MainWindow.h>

namespace U2 {

const QString VcfConsensusSupport::ET_VCF_CONSENSUS = "vcf-consensus";
const QString VcfConsensusSupport::ET_VCF_CONSENSUS_ID = "USUPP_VCF_CONSENSUS";
const QString VcfConsensusSupport::VCF_CONSENSUS_TMP_DIR = "vcf-consensus";

VcfConsensusSupport::VcfConsensusSupport(const QString& id, const QString &name, const QString &path)
    : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow() != NULL) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName = "vcf-consensus";

    muted = true;
    validationArguments << "-help";
    validMessage = "vcf-consensus";
    description = tr("Apply VCF variants to a fasta file to create consensus sequence.");
    versionRegExp = QRegExp("Version: (\\d+.\\d+.\\d+)");
    toolKitName = "VCFtools";

    toolRunnerProgram = PerlSupport::ET_PERL_ID;
    dependencies << PerlSupport::ET_PERL_ID << TabixSupport::ET_TABIX_ID;
}

} // namespace U2
