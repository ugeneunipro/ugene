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

#pragma once

#include <QCoreApplication>

namespace U2 {

class Task;
class AppContextImpl;
class CMDLineRegistry;
class SettingsImpl;
class AppSettings;
class UserAppsSettings;
class ResourceTracker;
class TaskSchedulerImpl;
class AnnotationSettingsRegistry;
class TestFramework;
class RepeatFinderTaskFactoryRegistry;
class QDActorPrototypeRegistry;
class PhyTreeGeneratorRegistry;
class ExternalToolRegistry;
class U2DbiRegistry;
class DocumentFormatRegistryImpl;
class PluginSupportImpl;
class ServiceRegistryImpl;
class IOAdapterRegistryImpl;
class DNATranslationRegistry;
class DNAAlphabetRegistry;
class DBXRefRegistry;
class MsaConsensusAlgorithmRegistry;
class MsaDistanceAlgorithmRegistry;
class AssemblyConsensusAlgorithmRegistry;
class PWMConversionAlgorithmRegistry;
class SubstMatrixRegistry;
class SmithWatermanTaskFactoryRegistry;
class MolecularSurfaceFactoryRegistry;
class SWResultFilterRegistry;
class SecStructPredictAlgRegistry;
class AlignmentAlgorithmsRegistry;
class RecentlyDownloadedCache;
class VirtualFileSystemRegistry;
class DnaAssemblyAlgRegistry;
class GenomeAssemblyAlgRegistry;
class DataBaseRegistry;
class CDSearchFactoryRegistry;
class StructuralAlignmentAlgorithmRegistry;
class SplicedAlignmentTaskRegistry;
class WorkflowScriptRegistry;
class AppFileStorage;
class U2DataPathRegistry;
class ScriptContext;
class ScriptingToolRegistry;

class UgeneContextWrapper {
public:
    UgeneContextWrapper(const QString& workingDirectoryPath);
    ~UgeneContextWrapper();

    int processTask(Task* task);

    static bool isAppContextInitialized();

private:
    QCoreApplication app;

    AppContextImpl* appContext;
    CMDLineRegistry* cmdLineRegistry;
    SettingsImpl* globalSettings;
    SettingsImpl* settings;
    AppSettings* appSettings;
    UserAppsSettings* userAppSettings;
    ResourceTracker* resTrack;
    TaskSchedulerImpl* ts;
    AnnotationSettingsRegistry* asr;
    TestFramework* tf;
    RepeatFinderTaskFactoryRegistry* rfr;
    QDActorPrototypeRegistry* qpr;
    PhyTreeGeneratorRegistry* phyreg;
    ExternalToolRegistry* etr;
    U2DbiRegistry* dbiRegistry;
    DocumentFormatRegistryImpl* dfr;
    PluginSupportImpl* psp;
    ServiceRegistryImpl* sreg;
    IOAdapterRegistryImpl* io;
    DNATranslationRegistry* dtr;
    DNAAlphabetRegistry* dal;
    DBXRefRegistry* dbxrr;
    MsaConsensusAlgorithmRegistry* msaConsReg;
    MsaDistanceAlgorithmRegistry* msaDistReg;
    AssemblyConsensusAlgorithmRegistry* assemblyConsReg;
    PWMConversionAlgorithmRegistry* pwmConvReg;
    SubstMatrixRegistry* smr;
    SmithWatermanTaskFactoryRegistry* swar;
    MolecularSurfaceFactoryRegistry* msfr;
    SWResultFilterRegistry* swrfr;
    SecStructPredictAlgRegistry* sspar;
    AlignmentAlgorithmsRegistry* alignmentAlgorithmRegistry;
    RecentlyDownloadedCache* rdc;
    VirtualFileSystemRegistry* vfsReg;
    DnaAssemblyAlgRegistry* assemblyReg;
    GenomeAssemblyAlgRegistry* genomeAssemblyReg;
    DataBaseRegistry* dbr;
    CDSearchFactoryRegistry* cdsfr;
    StructuralAlignmentAlgorithmRegistry* saar;
    SplicedAlignmentTaskRegistry* splicedAlignmentTaskRegistry;
    WorkflowScriptRegistry* workflowScriptRegistry;
    AppFileStorage* appFileStorage;
    U2DataPathRegistry* dpr;
    ScriptingToolRegistry* str;
};

}  // namespace U2
