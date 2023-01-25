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

#include "SpadesSupport.h"
#include <python/PythonSupport.h>

#include <U2Algorithm/GenomeAssemblyRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GAutoDeleteList.h>

#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include "SpadesWorker.h"
#include "spades/SpadesSettingsWidget.h"
#include "spades/SpadesTask.h"
#include "spades/SpadesTaskTest.h"

namespace U2 {

// SpadesSupport
const QString SpadesSupport::ET_SPADES = "SPAdes";
const QString SpadesSupport::ET_SPADES_ID = "USUPP_SPADES";

SpadesSupport::SpadesSupport()
    : ExternalTool(SpadesSupport::ET_SPADES_ID, "spades", SpadesSupport::ET_SPADES) {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName = "spades.py";
    validMessage = "SPAdes";
    description = tr("<i>SPAdes</i> - St. Petersburg genome assembler - is intended for both standard isolates and single-cell MDA bacteria assemblies. Official site: http://bioinf.spbau.ru/spades");
    validationArguments << "--version";
    versionRegExp = QRegExp("SPAdes.* v(\\d+.\\d+.\\d+)");
    toolKitName = "SPAdes";

    toolRunnerProgram = PythonSupport::ET_PYTHON_ID;
    dependencies << PythonSupport::ET_PYTHON_ID;
}

void SpadesSupport::checkIn() {
    AppContext::getExternalToolRegistry()->registerEntry(new SpadesSupport());

    QStringList genomeReadsFormats;
    genomeReadsFormats << BaseDocumentFormats::FASTA;
    genomeReadsFormats << BaseDocumentFormats::FASTQ;
    GenomeAssemblyAlgorithmEnv* spadesAlgorithmEnv = new GenomeAssemblyAlgorithmEnv(SpadesSupport::ET_SPADES, new SpadesTaskFactory(), new SpadesGUIExtensionsFactory(), genomeReadsFormats);
    AppContext::getGenomeAssemblyAlgRegistry()->registerAlgorithm(spadesAlgorithmEnv);

    LocalWorkflow::SpadesWorkerFactory::init();

    GTestFormatRegistry* testFormatRegistry = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat* xmlTestFormat = qobject_cast<XMLTestFormat*>(testFormatRegistry->findFormat("XML"));
    xmlTestFormat->registerTestFactories(SpadesTaskTest::createTestFactories());
}

}  // namespace U2
