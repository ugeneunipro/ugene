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

#include <U2Core/AppContext.h>
#include <U2Core/global.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

class U2PRIVATE_EXPORT AppContextImpl : public AppContext {
    Q_OBJECT
public:
    AppContextImpl() {
        instance = this;

        aaSupport = nullptr;
        alignmentAlgorithmsRegistry = nullptr;
        appFileStorage = nullptr;
        as = nullptr;
        asg = nullptr;
        asr = nullptr;
        assemblyConsensusAlgoRegistry = nullptr;
        cdsfr = nullptr;
        cfr = nullptr;
        cmdLineRegistry = nullptr;
        credentialsAsker = nullptr;
        dal = nullptr;
        dashboardInfoRegistry = nullptr;
        dataBaseRegistry = nullptr;
        dbiRegistry = nullptr;
        dbxr = nullptr;
        dfr = nullptr;
        dnaAssemblyAlgRegistry = nullptr;
        dpr = nullptr;
        dtr = nullptr;
        externalToolRegistry = nullptr;
        genomeAssemblyAlgRegistry = nullptr;
        gs = nullptr;
        io = nullptr;
        mcsr = nullptr;
        mhsr = nullptr;
        msaConsensusAlgoRegistry = nullptr;
        msaDistanceAlgoRegistry = nullptr;
        msfr = nullptr;
        mw = nullptr;
        opCommonWidgetFactoryRegistry = nullptr;
        opWidgetFactoryRegistry = nullptr;
        ovfr = nullptr;
        passwordStorage = nullptr;
        pf = nullptr;
        pl = nullptr;
        plv = nullptr;
        prj = nullptr;
        projectFilterTaskRegistry = nullptr;
        protocolInfoRegistry = nullptr;
        prs = nullptr;
        ps = nullptr;
        pv = nullptr;
        pwmConversionAlgoRegistry = nullptr;
        qdafr = nullptr;
        rdc = nullptr;
        remoteMachineMonitor = nullptr;
        rfr = nullptr;
        rt = nullptr;
        saar = nullptr;
        secStructPredictRegistry = nullptr;
        smr = nullptr;
        splicedAlignmentTaskRegistry = nullptr;
        sr = nullptr;
        ss = nullptr;
        str = nullptr;
        swar = nullptr;
        swmarntr = nullptr;
        swrfr = nullptr;
        tcr = nullptr;
        tf = nullptr;
        treeGeneratorRegistry = nullptr;
        ts = nullptr;
        udrSchemaRegistry = nullptr;
        virtualFileSystemRegistry = nullptr;
        welcomePageActionRegistry = nullptr;
        workflowScriptRegistry = nullptr;

        guiMode = false;
        activeWindow = "";
        workingDirectoryPath = "";
    }

    ~AppContextImpl() override;

    void setPluginSupport(PluginSupport* _ps) {
        assert(ps == nullptr || _ps == nullptr);
        ps = _ps;
    }

    void setServiceRegistry(ServiceRegistry* _sr) {
        assert(sr == nullptr || _sr == nullptr);
        sr = _sr;
    }

    void setProjectLoader(ProjectLoader* _pl) {
        assert(pl == nullptr || _pl == nullptr);
        pl = _pl;
    }

    void setProject(Project* _prj) {
        assert(prj == nullptr || _prj == nullptr);
        prj = _prj;
    }

    void setProjectService(ProjectService* _prs) {
        assert(prs == nullptr || _prs == nullptr);
        prs = _prs;
    }

    void setMainWindow(MainWindow* _mw) {
        assert(mw == nullptr || _mw == nullptr);
        mw = _mw;
    }

    void setProjectView(ProjectView* _pv) {
        assert(pv == nullptr || _pv == nullptr);
        pv = _pv;
    }

    void setPluginViewer(PluginViewer* _plv) {
        assert(plv == nullptr || _plv == nullptr);
        plv = _plv;
    }

    void setSettings(Settings* _ss) {
        assert(ss == nullptr || _ss == nullptr);
        ss = _ss;
    }

    void setGlobalSettings(Settings* _gs) {
        assert(gs == nullptr || _gs == nullptr);
        gs = _gs;
    }

    void setAppSettings(AppSettings* _as) {
        assert(as || _as);
        as = _as;
    }

    void setAppSettingsGUI(AppSettingsGUI* _asg) {
        assert(asg == nullptr || _asg == nullptr);
        asg = _asg;
    }

    void setDocumentFormatRegistry(DocumentFormatRegistry* _dfr) {
        assert(dfr == nullptr || _dfr == nullptr);
        dfr = _dfr;
    }

    void setIOAdapterRegistry(IOAdapterRegistry* _io) {
        assert(io == nullptr || _io == nullptr);
        io = _io;
    }

    void setDNATranslationRegistry(DNATranslationRegistry* _dtr) {
        assert(dtr == nullptr || _dtr == nullptr);
        dtr = _dtr;
    }

    void setDNAAlphabetRegistry(DNAAlphabetRegistry* _dal) {
        assert(dal == nullptr || _dal == nullptr);
        dal = _dal;
    }

    void setObjectViewFactoryRegistry(GObjectViewFactoryRegistry* _ovfr) {
        assert(ovfr == nullptr || _ovfr == nullptr);
        ovfr = _ovfr;
    }

    void setTaskScheduler(TaskScheduler* _ts) {
        assert(ts == nullptr || _ts == nullptr);
        ts = _ts;
    }

    void setResourceTracker(ResourceTracker* _rt) {
        assert(rt == nullptr || _rt == nullptr);
        rt = _rt;
    }

    void setAnnotationSettingsRegistry(AnnotationSettingsRegistry* _asr) {
        assert(asr == nullptr || _asr == nullptr);
        asr = _asr;
    }

    void setTestFramework(TestFramework* _tf) {
        assert(tf || _tf);
        tf = _tf;
    }

    void setDBXRefRegistry(DBXRefRegistry* _dbxr) {
        assert(dbxr == nullptr || _dbxr == nullptr);
        dbxr = _dbxr;
    }

    void setSubstMatrixRegistry(SubstMatrixRegistry* _smr) {
        assert(smr == nullptr || _smr == nullptr);
        smr = _smr;
    }

    void setSmithWatermanTaskFactoryRegistry(SmithWatermanTaskFactoryRegistry* _swar) {
        assert(swar == nullptr || _swar == nullptr);
        swar = _swar;
    }

    void setMolecularSurfaceFactoryRegistry(MolecularSurfaceFactoryRegistry* _msfr) {
        assert(msfr == nullptr || _msfr == nullptr);
        msfr = _msfr;
    }

    void setSWResultFilterRegistry(SWResultFilterRegistry* _swrfr) {
        assert(swrfr == nullptr || _swrfr == nullptr);
        swrfr = _swrfr;
    }

    void setSWMulAlignResultNamesTagsRegistry(SWMulAlignResultNamesTagsRegistry* _swmarntr) {
        assert(swmarntr == nullptr || _swmarntr == nullptr);
        swmarntr = _swmarntr;
    }

    void setMsaColorSchemeRegistry(MsaColorSchemeRegistry* _mcsr) {
        assert(mcsr == nullptr || _mcsr == nullptr);
        mcsr = _mcsr;
    }

    void setMsaHighlightingSchemeRegistry(MsaHighlightingSchemeRegistry* _mhsr) {
        assert(mhsr == nullptr || _mhsr == nullptr);
        mhsr = _mhsr;
    }

    void setSecStructPedictAlgRegistry(SecStructPredictAlgRegistry* _sspar) {
        assert(secStructPredictRegistry == nullptr || _sspar == nullptr);
        secStructPredictRegistry = _sspar;
    }

    void setRecentlyDownloadedCache(RecentlyDownloadedCache* _rdc) {
        assert(rdc == nullptr || _rdc == nullptr);
        rdc = _rdc;
    }

    void setDataPathRegistry(U2DataPathRegistry* _dpr) {
        assert(dpr == nullptr || _dpr == nullptr);
        dpr = _dpr;
    }

    void setScriptingToolRegistry(ScriptingToolRegistry* _str) {
        assert(str == nullptr || _str == nullptr);
        str = _str;
    }

    void setPasteFactory(PasteFactory* _pf) {
        assert(pf == nullptr || _pf == nullptr);
        pf = _pf;
    }

    void setDashboardInfoRegistry(DashboardInfoRegistry* _dashboardInfoRegistry) {
        assert(dashboardInfoRegistry == nullptr || _dashboardInfoRegistry == nullptr);
        dashboardInfoRegistry = _dashboardInfoRegistry;
    }

    void setProtocolInfoRegistry(ProtocolInfoRegistry* pr) {
        assert(protocolInfoRegistry == nullptr || nullptr == pr);
        protocolInfoRegistry = pr;
    }

    void setRemoteMachineMonitor(RemoteMachineMonitor* rm) {
        assert(remoteMachineMonitor == nullptr || nullptr == rm);
        remoteMachineMonitor = rm;
    }

    void setPhyTreeGeneratorRegistry(PhyTreeGeneratorRegistry* genRegistry) {
        assert(treeGeneratorRegistry == nullptr || nullptr == genRegistry);
        treeGeneratorRegistry = genRegistry;
    }

    void setMSAConsensusAlgorithmRegistry(MSAConsensusAlgorithmRegistry* reg) {
        assert(reg == nullptr || msaConsensusAlgoRegistry == nullptr);
        msaConsensusAlgoRegistry = reg;
    }

    void setMSADistanceAlgorithmRegistry(MSADistanceAlgorithmRegistry* reg) {
        assert(reg == nullptr || msaDistanceAlgoRegistry == nullptr);
        msaDistanceAlgoRegistry = reg;
    }

    void setAssemblyConsensusAlgorithmRegistry(AssemblyConsensusAlgorithmRegistry* reg) {
        assert(reg == nullptr || assemblyConsensusAlgoRegistry == nullptr);
        assemblyConsensusAlgoRegistry = reg;
    }

    void setPWMConversionAlgorithmRegistry(PWMConversionAlgorithmRegistry* reg) {
        assert(reg == nullptr || pwmConversionAlgoRegistry == nullptr);
        pwmConversionAlgoRegistry = reg;
    }

    void setCMDLineRegistry(CMDLineRegistry* r) {
        assert(cmdLineRegistry == nullptr || r == nullptr);
        cmdLineRegistry = r;
    }

    void setVirtualFileSystemRegistry(VirtualFileSystemRegistry* r) {
        assert(virtualFileSystemRegistry == nullptr || r == nullptr);
        virtualFileSystemRegistry = r;
    }

    void setDnaAssemblyAlgRegistry(DnaAssemblyAlgRegistry* r) {
        assert(dnaAssemblyAlgRegistry == nullptr || r == nullptr);
        dnaAssemblyAlgRegistry = r;
    }

    void setGenomeAssemblyAlgRegistry(GenomeAssemblyAlgRegistry* r) {
        assert(genomeAssemblyAlgRegistry == nullptr || r == nullptr);
        genomeAssemblyAlgRegistry = r;
    }

    void setDataBaseRegistry(DataBaseRegistry* dbr) {
        assert(dataBaseRegistry == nullptr || dbr == nullptr);
        dataBaseRegistry = dbr;
    }

    void setExternalToolRegistry(ExternalToolRegistry* _etr) {
        assert(externalToolRegistry == nullptr || _etr == nullptr);
        externalToolRegistry = _etr;
    }

    void setRepeatFinderTaskFactoryRegistry(RepeatFinderTaskFactoryRegistry* _rfr) {
        assert(rfr == nullptr || _rfr == nullptr);
        rfr = _rfr;
    }

    void setQDActorFactoryRegistry(QDActorPrototypeRegistry* _queryfactoryRegistry) {
        assert(qdafr == nullptr || _queryfactoryRegistry == nullptr);
        qdafr = _queryfactoryRegistry;
    }

    void setAutoAnnotationsSupport(AutoAnnotationsSupport* _aaSupport) {
        assert(aaSupport == nullptr || _aaSupport == nullptr);
        aaSupport = _aaSupport;
    }

    void setDbiRegistry(U2DbiRegistry* _dbiRegistry) {
        assert((dbiRegistry == nullptr) || (nullptr == _dbiRegistry));
        dbiRegistry = _dbiRegistry;
    }

    void setUdrSchemaRegistry(UdrSchemaRegistry* _udrSchemaRegistry) {
        assert((udrSchemaRegistry == nullptr) || (nullptr == _udrSchemaRegistry));
        udrSchemaRegistry = _udrSchemaRegistry;
    }

    void setCDSearchFactoryRegistry(CDSearchFactoryRegistry* _cdsfr) {
        assert((cdsfr == nullptr) || (nullptr == _cdsfr));
        cdsfr = _cdsfr;
    }

    void setSplicedAlignmentTaskRegistry(SplicedAlignmentTaskRegistry* tr) {
        assert((splicedAlignmentTaskRegistry == nullptr) || (nullptr == tr));
        splicedAlignmentTaskRegistry = tr;
    }

    void setOPCommonWidgetFactoryRegistry(OPCommonWidgetFactoryRegistry* _opCommonWidgetFactoryRegistry) {
        assert((opCommonWidgetFactoryRegistry == nullptr) || (nullptr == _opCommonWidgetFactoryRegistry));
        opCommonWidgetFactoryRegistry = _opCommonWidgetFactoryRegistry;
    }

    void setOPWidgetFactoryRegistry(OPWidgetFactoryRegistry* _opWidgetFactoryRegistry) {
        assert((opWidgetFactoryRegistry == nullptr) || (nullptr == _opWidgetFactoryRegistry));
        opWidgetFactoryRegistry = _opWidgetFactoryRegistry;
    }

    void setStructuralAlignmentAlgorithmRegistry(StructuralAlignmentAlgorithmRegistry* _saar) {
        assert(saar == nullptr || _saar == nullptr);
        saar = _saar;
    }

    void setWorkflowScriptRegistry(WorkflowScriptRegistry* _wsr) {
        assert(workflowScriptRegistry == nullptr || _wsr == nullptr);
        workflowScriptRegistry = _wsr;
    }

    void setCredentialsAsker(CredentialsAsker* _credentialsAsker) {
        assert(credentialsAsker == nullptr || _credentialsAsker == nullptr);
        credentialsAsker = _credentialsAsker;
    }

    void setPasswordStorage(PasswordStorage* _passwordStorage) {
        assert(passwordStorage == nullptr || _passwordStorage == nullptr);
        passwordStorage = _passwordStorage;
    }

    void setAppFileStorage(AppFileStorage* afs) {
        assert(appFileStorage == nullptr || afs == nullptr);
        appFileStorage = afs;
    }

    void setAlignmentAlgorithmsRegistry(AlignmentAlgorithmsRegistry* _alignmentAlgorithmsRegistry) {
        assert(alignmentAlgorithmsRegistry == nullptr || _alignmentAlgorithmsRegistry == nullptr);
        alignmentAlgorithmsRegistry = _alignmentAlgorithmsRegistry;
    }

    void setConvertFactoryRegistry(ConvertFactoryRegistry* _cfr) {
        assert(cfr == nullptr || _cfr == nullptr);
        cfr = _cfr;
    }

    void setWelcomePageActionRegistry(IdRegistry<WelcomePageAction>* value) {
        assert(welcomePageActionRegistry == nullptr || value == nullptr);
        welcomePageActionRegistry = value;
    }

    void setProjectFilterTaskRegistry(ProjectFilterTaskRegistry* value) {
        assert(projectFilterTaskRegistry == nullptr || value == nullptr);
        projectFilterTaskRegistry = value;
    }

    void setTmCalculatorRegistry(TmCalculatorRegistry* value) {
        SAFE_POINT(tcr == nullptr || value == nullptr, "TmCalculatorRegistry and tcr aren't nullptr", );
        tcr = value;
    }

    void setGUIMode(bool v) {
        guiMode = v;
    }

    void _setActiveWindowName(const QString& name) override {
        activeWindow = name;
    }

    void setWorkingDirectoryPath(const QString& path) {
        assert(!path.isEmpty());
        workingDirectoryPath = path;
    }

    static AppContextImpl* getApplicationContext();

protected:
    PluginSupport* _getPluginSupport() const override {
        return ps;
    }
    ServiceRegistry* _getServiceRegistry() const override {
        return sr;
    }
    ProjectLoader* _getProjectLoader() const override {
        return pl;
    }
    Project* _getProject() const override {
        return prj;
    }
    ProjectService* _getProjectService() const override {
        return prs;
    }
    MainWindow* _getMainWindow() const override {
        return mw;
    }
    ProjectView* _getProjectView() const override {
        return pv;
    }
    PluginViewer* _getPluginViewer() const override {
        return plv;
    }
    Settings* _getSettings() const override {
        return ss;
    }
    Settings* _getGlobalSettings() const override {
        return gs;
    }
    AppSettings* _getAppSettings() const override {
        return as;
    }
    AppSettingsGUI* _getAppSettingsGUI() const override {
        return asg;
    }

    DocumentFormatRegistry* _getDocumentFormatRegistry() const override {
        return dfr;
    }
    IOAdapterRegistry* _getIOAdapterRegistry() const override {
        return io;
    }
    DNATranslationRegistry* _getDNATranslationRegistry() const override {
        return dtr;
    }
    DNAAlphabetRegistry* _getDNAAlphabetRegistry() const override {
        return dal;
    }
    GObjectViewFactoryRegistry* _getObjectViewFactoryRegistry() const override {
        return ovfr;
    }
    TaskScheduler* _getTaskScheduler() const override {
        return ts;
    }
    ResourceTracker* _getResourceTracker() const override {
        return rt;
    }
    AnnotationSettingsRegistry* _getAnnotationsSettingsRegistry() const override {
        return asr;
    }
    TestFramework* _getTestFramework() const override {
        return tf;
    }
    DBXRefRegistry* _getDBXRefRegistry() const override {
        return dbxr;
    }
    SubstMatrixRegistry* _getSubstMatrixRegistry() const override {
        return smr;
    }
    SmithWatermanTaskFactoryRegistry* _getSmithWatermanTaskFactoryRegistry() const override {
        return swar;
    }
    PhyTreeGeneratorRegistry* _getPhyTreeGeneratorRegistry() const override {
        return treeGeneratorRegistry;
    }

    MolecularSurfaceFactoryRegistry* _getMolecularSurfaceFactoryRegistry() const override {
        return msfr;
    }
    SWResultFilterRegistry* _getSWResultFilterRegistry() const override {
        return swrfr;
    }
    SWMulAlignResultNamesTagsRegistry* _getSWMulAlignResultNamesTagsRegistry() const override {
        return swmarntr;
    }
    MsaColorSchemeRegistry* _getMsaColorSchemeRegistry() const override {
        return mcsr;
    }
    MsaHighlightingSchemeRegistry* _getMsaHighlightingSchemeRegistry() const override {
        return mhsr;
    }
    SecStructPredictAlgRegistry* _getSecStructPredictAlgRegistry() const override {
        return secStructPredictRegistry;
    }
    RecentlyDownloadedCache* _getRecentlyDownloadedCache() const override {
        return rdc;
    }
    ProtocolInfoRegistry* _getProtocolInfoRegistry() const override {
        return protocolInfoRegistry;
    }
    RemoteMachineMonitor* _getRemoteMachineMonitor() const override {
        return remoteMachineMonitor;
    }
    CMDLineRegistry* _getCMDLineRegistry() const override {
        return cmdLineRegistry;
    }
    MSAConsensusAlgorithmRegistry* _getMSAConsensusAlgorithmRegistry() const override {
        return msaConsensusAlgoRegistry;
    }
    MSADistanceAlgorithmRegistry* _getMSADistanceAlgorithmRegistry() const override {
        return msaDistanceAlgoRegistry;
    }
    AssemblyConsensusAlgorithmRegistry* _getAssemblyConsensusAlgorithmRegistry() const override {
        return assemblyConsensusAlgoRegistry;
    }
    PWMConversionAlgorithmRegistry* _getPWMConversionAlgorithmRegistry() const override {
        return pwmConversionAlgoRegistry;
    }
    VirtualFileSystemRegistry* _getVirtualFileSystemRegistry() const override {
        return virtualFileSystemRegistry;
    }
    DnaAssemblyAlgRegistry* _getDnaAssemblyAlgRegistry() const override {
        return dnaAssemblyAlgRegistry;
    }
    GenomeAssemblyAlgRegistry* _getGenomeAssemblyAlgRegistry() const override {
        return genomeAssemblyAlgRegistry;
    }
    DataBaseRegistry* _getDataBaseRegistry() const override {
        return dataBaseRegistry;
    }
    ExternalToolRegistry* _getExternalToolRegistry() const override {
        return externalToolRegistry;
    }
    RepeatFinderTaskFactoryRegistry* _getRepeatFinderTaskFactoryRegistry() const override {
        return rfr;
    }
    QDActorPrototypeRegistry* _getQDActorFactoryRegistry() const override {
        return qdafr;
    }
    StructuralAlignmentAlgorithmRegistry* _getStructuralAlignmentAlgorithmRegistry() const override {
        return saar;
    }
    AutoAnnotationsSupport* _getAutoAnnotationsSupport() const override {
        return aaSupport;
    }
    CDSearchFactoryRegistry* _getCDSFactoryRegistry() const override {
        return cdsfr;
    }
    U2DbiRegistry* _getDbiRegistry() const override {
        return dbiRegistry;
    }
    UdrSchemaRegistry* _getUdrSchemaRegistry() const override {
        return udrSchemaRegistry;
    }
    SplicedAlignmentTaskRegistry* _getSplicedAlignmentTaskRegistry() const override {
        return splicedAlignmentTaskRegistry;
    }
    OPCommonWidgetFactoryRegistry* _getOPCommonWidgetFactoryRegistry() const override {
        return opCommonWidgetFactoryRegistry;
    }
    OPWidgetFactoryRegistry* _getOPWidgetFactoryRegistry() const override {
        return opWidgetFactoryRegistry;
    }
    WorkflowScriptRegistry* _getWorkflowScriptRegistry() const override {
        return workflowScriptRegistry;
    }
    AppFileStorage* _getAppFileStorage() const override {
        return appFileStorage;
    }
    AlignmentAlgorithmsRegistry* _getAlignmentAlgorithmsRegistry() const override {
        return alignmentAlgorithmsRegistry;
    }
    U2DataPathRegistry* _getDataPathRegistry() const override {
        return dpr;
    }
    ScriptingToolRegistry* _getScriptingToolRegistry() const override {
        return str;
    }
    CredentialsAsker* _getCredentialsAsker() const override {
        return credentialsAsker;
    }
    PasswordStorage* _getPasswordStorage() const override {
        return passwordStorage;
    }
    ConvertFactoryRegistry* _getConvertFactoryRegistry() const override {
        return cfr;
    }
    IdRegistry<WelcomePageAction>* _getWelcomePageActionRegistry() const override {
        return welcomePageActionRegistry;
    }
    ProjectFilterTaskRegistry* _getProjectFilterTaskRegistry() const override {
        return projectFilterTaskRegistry;
    }
    PasteFactory* _getPasteFactory() const override {
        return pf;
    }
    DashboardInfoRegistry* _getDashboardInfoRegistry() const override {
        return dashboardInfoRegistry;
    }
    TmCalculatorRegistry* _getTmCalculatorRegistry() const override {
        return tcr;
    }

    void _registerGlobalObject(AppGlobalObject* go) override;
    void _unregisterGlobalObject(const QString& id) override;
    AppGlobalObject* _getGlobalObjectById(const QString& id) const override;
    bool _isGUIMode() const override {
        return guiMode;
    }
    QString _getActiveWindowName() const override {
        return activeWindow;
    }
    QString _getWorkingDirectoryPath() const override {
        return workingDirectoryPath;
    }

private:
    AlignmentAlgorithmsRegistry* alignmentAlgorithmsRegistry;
    AnnotationSettingsRegistry* asr;
    AppFileStorage* appFileStorage;
    AppSettings* as;
    AppSettingsGUI* asg;
    AssemblyConsensusAlgorithmRegistry* assemblyConsensusAlgoRegistry;
    AutoAnnotationsSupport* aaSupport;
    CDSearchFactoryRegistry* cdsfr;
    CMDLineRegistry* cmdLineRegistry;
    ConvertFactoryRegistry* cfr;
    CredentialsAsker* credentialsAsker;
    DBXRefRegistry* dbxr;
    DNAAlphabetRegistry* dal;
    DNATranslationRegistry* dtr;
    DashboardInfoRegistry* dashboardInfoRegistry;
    DataBaseRegistry* dataBaseRegistry;
    DnaAssemblyAlgRegistry* dnaAssemblyAlgRegistry;
    DocumentFormatRegistry* dfr;
    ExternalToolRegistry* externalToolRegistry;
    GObjectViewFactoryRegistry* ovfr;
    GenomeAssemblyAlgRegistry* genomeAssemblyAlgRegistry;
    IOAdapterRegistry* io;
    IdRegistry<WelcomePageAction>* welcomePageActionRegistry;
    MSAConsensusAlgorithmRegistry* msaConsensusAlgoRegistry;
    MSADistanceAlgorithmRegistry* msaDistanceAlgoRegistry;
    MainWindow* mw;
    MolecularSurfaceFactoryRegistry* msfr;
    MsaColorSchemeRegistry* mcsr;
    MsaHighlightingSchemeRegistry* mhsr;
    OPCommonWidgetFactoryRegistry* opCommonWidgetFactoryRegistry;
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry;
    PWMConversionAlgorithmRegistry* pwmConversionAlgoRegistry;
    PasswordStorage* passwordStorage;
    PasteFactory* pf;
    PhyTreeGeneratorRegistry* treeGeneratorRegistry;
    PluginSupport* ps;
    PluginViewer* plv;
    Project* prj;
    ProjectFilterTaskRegistry* projectFilterTaskRegistry;
    ProjectLoader* pl;
    ProjectService* prs;
    ProjectView* pv;
    ProtocolInfoRegistry* protocolInfoRegistry;
    QDActorPrototypeRegistry* qdafr;
    RecentlyDownloadedCache* rdc;
    RemoteMachineMonitor* remoteMachineMonitor;
    RepeatFinderTaskFactoryRegistry* rfr;
    ResourceTracker* rt;
    SWMulAlignResultNamesTagsRegistry* swmarntr;
    SWResultFilterRegistry* swrfr;
    ScriptingToolRegistry* str;
    SecStructPredictAlgRegistry* secStructPredictRegistry;
    ServiceRegistry* sr;
    Settings* gs;
    Settings* ss;
    SmithWatermanTaskFactoryRegistry* swar;
    SplicedAlignmentTaskRegistry* splicedAlignmentTaskRegistry;
    StructuralAlignmentAlgorithmRegistry* saar;
    SubstMatrixRegistry* smr;
    TaskScheduler* ts;
    TmCalculatorRegistry* tcr;
    TestFramework* tf;
    U2DataPathRegistry* dpr;
    U2DbiRegistry* dbiRegistry;
    UdrSchemaRegistry* udrSchemaRegistry;
    VirtualFileSystemRegistry* virtualFileSystemRegistry;
    WorkflowScriptRegistry* workflowScriptRegistry;

    bool guiMode;
    QString activeWindow;
    QString workingDirectoryPath;

    QList<AppGlobalObject*> appGlobalObjects;
};

}  // namespace U2
