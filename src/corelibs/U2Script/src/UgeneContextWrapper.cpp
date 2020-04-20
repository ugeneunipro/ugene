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

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>
#include <U2Algorithm/CDSearchTaskFactoryRegistry.h>
#include <U2Algorithm/CudaGpuRegistry.h>
#include <U2Algorithm/DnaAssemblyAlgRegistry.h>
#include <U2Algorithm/GenomeAssemblyRegistry.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/MolecularSurfaceFactoryRegistry.h>
#include <U2Algorithm/OpenCLGpuRegistry.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Algorithm/RepeatFinderTaskFactoryRegistry.h>
#include <U2Algorithm/SWMulAlignResultNamesTagsRegistry.h>
#include <U2Algorithm/SWResultFilterRegistry.h>
#include <U2Algorithm/SecStructPredictAlgRegistry.h>
#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppFileStorage.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/ConsoleShutdownTask.h>
#include <U2Core/DBXRefRegistry.h>
#include <U2Core/DNAAlphabetRegistryImpl.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DataBaseRegistry.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/LogCache.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/Timer.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/VirtualFileSystem.h>

#include <U2Formats/DocumentFormatUtils.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/QueryDesignerRegistry.h>
#include <U2Lang/ScriptContext.h>
#include <U2Lang/WorkflowEnvImpl.h>
#include <U2Lang/WorkflowSettings.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <AppContextImpl.h>
#include <AppSettingsImpl.h>
#include <ConsoleLogDriver.h>
#include <DocumentFormatRegistryImpl.h>
#include <IOAdapterRegistryImpl.h>
#include <PluginSupportImpl.h>
#include <ServiceRegistryImpl.h>
#include <SettingsImpl.h>
#include <TaskSchedulerImpl.h>
#include <crash_handler/CrashHandler.h>

#include "UgeneContextWrapper.h"

int ARGC = 0;

namespace U2 {

static void setDataSearchPaths( ) {
    //set search paths for data files
    QStringList dataSearchPaths;
    const char *relativeDataDir = "/data";
    const char *relativeDevDataDir = "/../../data";

    if( QDir( AppContext::getWorkingDirectoryPath() + relativeDataDir ).exists( ) ) {
        dataSearchPaths.push_back( AppContext::getWorkingDirectoryPath( ) + relativeDataDir );
    } else if( QDir( AppContext::getWorkingDirectoryPath() + relativeDevDataDir ).exists( ) ) {
        coreLog.info( "Added path: " + AppContext::getWorkingDirectoryPath( )
            + relativeDevDataDir );
        dataSearchPaths.push_back( AppContext::getWorkingDirectoryPath( ) + relativeDevDataDir );
    }

#if (defined(Q_OS_UNIX)) && defined( UGENE_DATA_DIR )
    //using folder which is set during installation process on Linux
    const QString ugene_data_dir( UGENE_DATA_DIR );
    if( QDir( ugene_data_dir ).exists( ) ) {
        dataSearchPaths.push_back( QString( UGENE_DATA_DIR ) );
    }
#endif

    if( dataSearchPaths.empty( ) ) {
        dataSearchPaths.push_back("/");
    }

    QDir::setSearchPaths( PATH_PREFIX_DATA, dataSearchPaths );
    //now data files may be opened using QFile( "data:some_data_file" )
}

static void setSearchPaths( ) {
    setDataSearchPaths( );
}

UgeneContextWrapper::UgeneContextWrapper( const QString &workingDirectoryPath )
    : app( ARGC, NULL )
{
    const char* buildVersion = QT_VERSION_STR;
    const char* runtimeVersion = qVersion( );
    if ( strcmp( buildVersion, runtimeVersion ) > 0 ) {
        printf( "Installed Qt version must be %s or greater \r\n", QT_VERSION_STR );
        return;
    }

    GTIMER( c1, t1, "initialization of UGENE Context" );

    appContext = AppContextImpl::getApplicationContext( );
    appContext->setWorkingDirectoryPath( workingDirectoryPath );

    QCoreApplication::addLibraryPath( workingDirectoryPath );
    const QString devPluginsPath = QDir( workingDirectoryPath + "/../../installer/windows" )
        .absolutePath( );
    QCoreApplication::addLibraryPath( devPluginsPath );

    setSearchPaths( );
    cmdLineRegistry = new CMDLineRegistry( QStringList( ) );
    appContext->setCMDLineRegistry(cmdLineRegistry);

    globalSettings = new SettingsImpl( QSettings::SystemScope );
    appContext->setGlobalSettings( globalSettings );

    settings = new SettingsImpl( QSettings::UserScope );
    appContext->setSettings( settings );

    appSettings = new AppSettingsImpl( );
    appContext->setAppSettings( appSettings );

    userAppSettings = AppContext::getAppSettings( )->getUserAppsSettings( );

    ConsoleLogDriver logs;
    coreLog.details( AppContextImpl::tr( "UGENE script environment initialization started" ) );
    coreLog.trace( QString( "UGENE script environment run at dir %1" ).arg( workingDirectoryPath ) );

    resTrack = new ResourceTracker( );
    appContext->setResourceTracker( resTrack );

    ts = new TaskSchedulerImpl( appSettings->getAppResourcePool( ) );
    appContext->setTaskScheduler( ts );

    asr = new AnnotationSettingsRegistry( DocumentFormatUtils::predefinedSettings( ) );
    appContext->setAnnotationSettingsRegistry( asr );

    tf = new TestFramework( );
    appContext->setTestFramework( tf );

    rfr = new RepeatFinderTaskFactoryRegistry( );
    appContext->setRepeatFinderTaskFactoryRegistry( rfr );

    str = new ScriptingToolRegistry( );
    appContext->setScriptingToolRegistry( str );

    qpr = new QDActorPrototypeRegistry( );
    appContext->setQDActorFactoryRegistry( qpr );

    phyreg = new PhyTreeGeneratorRegistry( );
    appContext->setPhyTreeGeneratorRegistry( phyreg );

    etr = new ExternalToolRegistry( );
    appContext->setExternalToolRegistry( etr );

    dbiRegistry = new U2DbiRegistry( );
    appContext->setDbiRegistry( dbiRegistry );

    dfr = new DocumentFormatRegistryImpl( );
    appContext->setDocumentFormatRegistry( dfr );

    psp = new PluginSupportImpl( );
    appContext->setPluginSupport( psp );

    sreg = new ServiceRegistryImpl( );
    appContext->setServiceRegistry( sreg );

    io = new IOAdapterRegistryImpl( );
    appContext->setIOAdapterRegistry( io );

    dtr = new DNATranslationRegistry( );
    appContext->setDNATranslationRegistry( dtr );

    dal = new DNAAlphabetRegistryImpl( dtr );
    appContext->setDNAAlphabetRegistry( dal );

    dbxrr = new DBXRefRegistry( );
    appContext->setDBXRefRegistry( dbxrr );

    msaConsReg = new MSAConsensusAlgorithmRegistry( );
    appContext->setMSAConsensusAlgorithmRegistry( msaConsReg );

    msaDistReg = new MSADistanceAlgorithmRegistry( );
    appContext->setMSADistanceAlgorithmRegistry( msaDistReg );

    assemblyConsReg = new AssemblyConsensusAlgorithmRegistry( );
    appContext->setAssemblyConsensusAlgorithmRegistry( assemblyConsReg );

    pwmConvReg = new PWMConversionAlgorithmRegistry( );
    appContext->setPWMConversionAlgorithmRegistry( pwmConvReg );

    smr = new SubstMatrixRegistry( );
    appContext->setSubstMatrixRegistry( smr );

    swar = new SmithWatermanTaskFactoryRegistry( );
    appContext->setSmithWatermanTaskFactoryRegistry( swar );

    msfr = new MolecularSurfaceFactoryRegistry( );
    appContext->setMolecularSurfaceFactoryRegistry( msfr );

    swrfr = new SWResultFilterRegistry( );
    appContext->setSWResultFilterRegistry( swrfr );

    sspar = new SecStructPredictAlgRegistry( );
    appContext->setSecStructPedictAlgRegistry( sspar );

    cgr = new CudaGpuRegistry( );
    appContext->setCudaGpuRegistry( cgr );

    alignmentAlgorithmRegistry = new AlignmentAlgorithmsRegistry( );
    appContext->setAlignmentAlgorithmsRegistry(alignmentAlgorithmRegistry);

#ifdef OPENCL_SUPPORT
    oclgr = new OpenCLGpuRegistry( );
    appContext->setOpenCLGpuRegistry( oclgr );
#endif

    rdc = new RecentlyDownloadedCache( );
    appContext->setRecentlyDownloadedCache( rdc );

    vfsReg = new VirtualFileSystemRegistry( );
    appContext->setVirtualFileSystemRegistry( vfsReg );

    Workflow::WorkflowEnv::init( new Workflow::WorkflowEnvImpl( ) );
    Workflow::WorkflowEnv::getDomainRegistry( )->registerEntry(
        new LocalWorkflow::LocalDomainFactory( ) );

    assemblyReg = new DnaAssemblyAlgRegistry( );
    appContext->setDnaAssemblyAlgRegistry( assemblyReg );

    genomeAssemblyReg = new GenomeAssemblyAlgRegistry( );
    appContext->setGenomeAssemblyAlgRegistry( genomeAssemblyReg );

    dbr = new DataBaseRegistry( );
    appContext->setDataBaseRegistry( dbr );

    cdsfr = new CDSearchFactoryRegistry( );
    appContext->setCDSearchFactoryRegistry( cdsfr );

    saar = new StructuralAlignmentAlgorithmRegistry( );
    appContext->setStructuralAlignmentAlgorithmRegistry( saar );

    splicedAlignmentTaskRegistry = new SplicedAlignmentTaskRegistry( );
    appContext->setSplicedAlignmentTaskRegistry( splicedAlignmentTaskRegistry );

    workflowScriptRegistry = new WorkflowScriptRegistry( );
    appContext->setWorkflowScriptRegistry( workflowScriptRegistry );

    appFileStorage = new AppFileStorage( );
    U2OpStatusImpl os;
    appFileStorage->init( os );
    if ( os.hasError( ) ) {
        coreLog.error( os.getError( ) );
        delete appFileStorage;
    } else {
        appContext->setAppFileStorage( appFileStorage );
    }

    dpr = new U2DataPathRegistry( );
    appContext->setDataPathRegistry( dpr );

    GReportableCounter launchCounter( "U2Script is ready", "", 1 );
    ++launchCounter.totalCount;

    t1.stop( );
    QObject::connect( psp, SIGNAL( si_allStartUpPluginsLoaded( ) ), &app, SLOT( quit( ) ) );
    app.exec();
}

UgeneContextWrapper::~UgeneContextWrapper( ) {
    Workflow::WorkflowEnv::shutdown( );

    delete dpr;
    appContext->setDataPathRegistry( NULL );

    delete genomeAssemblyReg;
    appContext->setGenomeAssemblyAlgRegistry( NULL);

    delete vfsReg;
    appContext->setVirtualFileSystemRegistry( NULL );

    delete rdc;
    appContext->setRecentlyDownloadedCache( NULL );

    delete swrfr;
    appContext->setSWResultFilterRegistry( NULL );

    delete swar;
    appContext->setSmithWatermanTaskFactoryRegistry( NULL );

    delete msfr;
    appContext->setMolecularSurfaceFactoryRegistry( NULL );

    delete smr;
    appContext->setSubstMatrixRegistry( NULL );

    delete sreg;
    appContext->setServiceRegistry( NULL );

    delete psp;
    appContext->setPluginSupport( NULL );

    delete dal;
    appContext->setDNAAlphabetRegistry( NULL );

    delete dbxrr;
    appContext->setDBXRefRegistry( NULL );

    delete dtr;
    appContext->setDNATranslationRegistry( NULL );

    delete io;
    appContext->setIOAdapterRegistry( NULL );

    delete dfr;
    appContext->setDocumentFormatRegistry( NULL );

    delete dbiRegistry;
    appContext->setDbiRegistry( NULL );

    delete ts;
    appContext->setTaskScheduler( NULL );

    delete asr;
    appContext->setAnnotationSettingsRegistry( NULL );

    delete resTrack;
    appContext->setResourceTracker( NULL );

    delete cgr;
    appContext->setCudaGpuRegistry( NULL );

#ifdef OPENCL_SUPPORT
    delete oclgr;
    appContext->setOpenCLGpuRegistry( NULL );
#endif

    delete appSettings;
    appContext->setAppSettings( NULL );

    delete settings;
    appContext->setSettings( NULL );

    delete globalSettings;
    appContext->setGlobalSettings( NULL );

    delete sspar;
    appContext->setSecStructPedictAlgRegistry( NULL );

    appContext->setExternalToolRegistry( NULL );
    delete etr;

    delete str;
    appContext->setScriptingToolRegistry( NULL );

    delete msaConsReg;
    appContext->setMSAConsensusAlgorithmRegistry( NULL );

    delete msaDistReg;
    appContext->setMSADistanceAlgorithmRegistry( NULL );

    delete assemblyConsReg;
    appContext->setAssemblyConsensusAlgorithmRegistry( NULL );

    appContext->setPWMConversionAlgorithmRegistry( NULL );
    delete pwmConvReg;

    delete dbr;
    appContext->setDataBaseRegistry( NULL );

    appContext->setQDActorFactoryRegistry( NULL );
    delete qpr;

    delete cdsfr;
    appContext->setCDSearchFactoryRegistry( NULL );

    appContext->setStructuralAlignmentAlgorithmRegistry( NULL );
    delete saar;

    delete splicedAlignmentTaskRegistry;
    appContext->setSplicedAlignmentTaskRegistry( NULL );

    appContext->setAlignmentAlgorithmsRegistry( NULL );
    delete alignmentAlgorithmRegistry;
}

int UgeneContextWrapper::processTask(Task *task) {
    coreLog.info( QObject::tr( "%1-bit version of UGENE started" )
        .arg( Version::appArchitecture ) );
    ts->registerTopLevelTask( task );
    QObject::connect( ts, SIGNAL( si_topLevelTaskUnregistered( Task * ) ), &app, SLOT( quit( ) ) );
    return app.exec( );
}

// TODO: fix this dummy check
bool UgeneContextWrapper::isAppContextInitialized( ) {
    return AppContextImpl::getApplicationContext( )->getWorkingDirectoryPath( ).isEmpty( );
}

} // namespace U2
