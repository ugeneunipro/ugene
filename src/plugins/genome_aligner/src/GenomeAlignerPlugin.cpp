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

#include "GenomeAlignerPlugin.h"
#include "GenomeAlignerTask.h"
#include "GenomeAlignerSettingsWidget.h"
#include "GenomeAlignerCMDLineTask.h"
#include "BuildSArraySettingsWidget.h"

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineHelpProvider.h>
#include <U2Core/TaskStarter.h>
#include <U2Gui/MainWindow.h>
#include <U2Algorithm/DnaAssemblyAlgRegistry.h>
#include <U2Lang/WorkflowEnv.h>

#include "GenomeAlignerSettingsController.h"
#include "GenomeAlignerTask.h"
#include "GenomeAlignerWorker.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin * U2_PLUGIN_INIT_FUNC() {
    GenomeAlignerPlugin * plug = new GenomeAlignerPlugin();
    return plug;
}

const QString GenomeAlignerPlugin::GENOME_ALIGNER_INDEX_TYPE_ID("gai");
const QString GenomeAlignerPlugin::RUN_GENOME_ALIGNER("genome-aligner");

DataTypePtr GenomeAlignerPlugin::GENOME_ALIGNER_INDEX_TYPE()
{
    DataTypeRegistry* dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    static bool startup = true;
    if (startup)
    {
        dtr->registerEntry(DataTypePtr(new DataType(GENOME_ALIGNER_INDEX_TYPE_ID, QString("Genome aligner index"), QString("Index for genome aligner"))));
        startup = false;
    }
    return dtr->getById(GENOME_ALIGNER_INDEX_TYPE_ID);
}

class GenomeAlignerGuiExtFactory : public DnaAssemblyGUIExtensionsFactory {
public:
    GenomeAlignerGuiExtFactory(){};
    DnaAssemblyAlgorithmMainWidget* createMainWidget(QWidget* parent) {return new GenomeAlignerSettingsWidget(parent);}
    DnaAssemblyAlgorithmBuildIndexWidget* createBuildIndexWidget(QWidget* parent) {return new BuildSArraySettingsWidget(parent);}
    bool hasMainWidget() {return true;}
    bool hasBuildIndexWidget() {return true;}
};

GenomeAlignerPlugin::GenomeAlignerPlugin() : Plugin( tr("UGENE Genome Aligner"), tr("Assembly DNA to reference sequence") ) {

    // Register GenomeAligner algorithm
    DnaAssemblyAlgRegistry* registry = AppContext::getDnaAssemblyAlgRegistry();

    bool guiMode = AppContext::getMainWindow();
    DnaAssemblyGUIExtensionsFactory* guiFactory = guiMode ? new GenomeAlignerGuiExtFactory(): NULL;
    QStringList referenceFormats(BaseDocumentFormats::FASTA);
    referenceFormats << BaseDocumentFormats::PLAIN_GENBANK;
    referenceFormats << BaseDocumentFormats::FASTQ;
    QStringList readsFormats;
    readsFormats << BaseDocumentFormats::FASTA;
    readsFormats << BaseDocumentFormats::FASTQ;
    readsFormats << BaseDocumentFormats::PLAIN_GENBANK;
    DnaAssemblyAlgorithmEnv* algo = new DnaAssemblyAlgorithmEnv("UGENE Genome Aligner", new GenomeAlignerTask::Factory, guiFactory, true, true, false, referenceFormats, readsFormats);
    bool res = registry->registerAlgorithm(algo);
    Q_UNUSED(res);
    assert(res);

    LocalWorkflow::GenomeAlignerWorkerFactory::init();

    registerCMDLineHelp();
    processCMDLineOptions();
}

GenomeAlignerPlugin::~GenomeAlignerPlugin() {
}

void GenomeAlignerPlugin::processCMDLineOptions()
{
    CMDLineRegistry * cmdlineReg = AppContext::getCMDLineRegistry();
    assert(cmdlineReg != NULL);

    if (cmdlineReg->hasParameter( RUN_GENOME_ALIGNER ) ) {
        Task * t = new GenomeAlignerCMDLineTask();
        connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(t), SLOT(registerTask()));
    }
}

void GenomeAlignerPlugin::registerCMDLineHelp()
{
    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();
    assert( NULL != cmdLineRegistry );

    CMDLineHelpProvider * taskSection = new CMDLineHelpProvider(
        RUN_GENOME_ALIGNER,
        tr("UGENE Short Reads Aligner"),
        tr("UGENE Genome Aligner is an efficient and fast tool for short read alignment."
        "It has 2 work modes: build index and align short reads (default mode).\nIf there is no "
        "index available for reference sequence it will be built on the fly.\n"
        "\nUsage: ugene --genome-aligner { --option[=argument] }\n"
        "\nOptions\n--------\n\n%1"
        "\nExamples\n--------\n\n"
        "Build index for reference sequence:\n"
        "ugene --genome-aligner --build-index --reference=/path/to/ref\n"
        "\nAlign short reads using existing index:\n"
        "ugene --genome-aligner --reference=/path/to/ref"
        " --short-reads=/path/to/reads --result=/path/to/result\n")
        .arg( GenomeAlignerCMDLineTask::getArgumentsDescritption() )
          );

    cmdLineRegistry->registerCMDLineHelpProvider( taskSection );
}



} //namespace
