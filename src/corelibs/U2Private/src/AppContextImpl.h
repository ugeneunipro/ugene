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

#ifndef _U2_APPCONTEXT_IMPL_
#define _U2_APPCONTEXT_IMPL_

#include <U2Core/global.h>
#include <U2Core/AppContext.h>

namespace U2 {

class U2PRIVATE_EXPORT AppContextImpl : public AppContext {
    Q_OBJECT
public:
    AppContextImpl()  {
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
        cgr = nullptr;
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
        oclgr = nullptr;
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
        tb = nullptr;
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

    ~AppContextImpl();

    void setPluginSupport(PluginSupport* _ps) {assert(ps == nullptr || _ps == nullptr); ps = _ps;}

    void setServiceRegistry(ServiceRegistry* _sr) {assert(sr == nullptr || _sr == nullptr); sr = _sr;}

    void setProjectLoader(ProjectLoader* _pl) {assert(pl == nullptr || _pl == nullptr); pl = _pl;}

    void setProject(Project* _prj) {assert(prj == nullptr || _prj == nullptr); prj = _prj;}

    void setProjectService(ProjectService* _prs) {assert(prs == nullptr || _prs == nullptr); prs = _prs;}

    void setMainWindow(MainWindow* _mw) {assert(mw == nullptr || _mw == nullptr); mw = _mw;}

    void setProjectView(ProjectView* _pv) {assert(pv == nullptr || _pv == nullptr); pv = _pv;}

    void setPluginViewer(PluginViewer* _plv) {assert(plv == nullptr || _plv == nullptr); plv = _plv;}

    void setSettings(Settings* _ss) {assert(ss == nullptr || _ss == nullptr); ss= _ss;}

    void setGlobalSettings(Settings* _gs) {assert(gs == nullptr || _gs == nullptr); gs= _gs;}

    void setAppSettings( AppSettings* _as) { assert( as|| _as); as= _as; }

    void setAppSettingsGUI( AppSettingsGUI* _asg) { assert( asg == nullptr || _asg == nullptr ); asg= _asg; }

    void setDocumentFormatRegistry(DocumentFormatRegistry* _dfr) {assert(dfr == nullptr || _dfr == nullptr); dfr = _dfr;}

    void setIOAdapterRegistry(IOAdapterRegistry* _io) {assert(io == nullptr || _io == nullptr); io = _io;}

    void setDNATranslationRegistry(DNATranslationRegistry* _dtr) {assert(dtr == nullptr || _dtr == nullptr);dtr = _dtr;}

    void setDNAAlphabetRegistry(DNAAlphabetRegistry* _dal) {assert(dal == nullptr || _dal == nullptr);dal = _dal;}

    void setObjectViewFactoryRegistry(GObjectViewFactoryRegistry* _ovfr) {assert(ovfr == nullptr || _ovfr == nullptr); ovfr = _ovfr;}

    void setTaskScheduler(TaskScheduler* _ts) {assert(ts == nullptr || _ts == nullptr); ts = _ts;}

    void setResourceTracker(ResourceTracker* _rt) {assert(rt == nullptr || _rt == nullptr); rt = _rt;}

    void setAnnotationSettingsRegistry(AnnotationSettingsRegistry* _asr)  {assert(asr == nullptr || _asr == nullptr); asr = _asr;}

    void setTestFramework( TestFramework* _tf) { assert( tf || _tf ); tf = _tf; }

    void setDBXRefRegistry( DBXRefRegistry* _dbxr) { assert( dbxr == nullptr || _dbxr == nullptr ); dbxr = _dbxr; }

    void setSubstMatrixRegistry(SubstMatrixRegistry* _smr) { assert( smr == nullptr || _smr == nullptr ); smr = _smr; }

    void setSmithWatermanTaskFactoryRegistry (SmithWatermanTaskFactoryRegistry* _swar) { assert( swar == nullptr || _swar == nullptr ); swar = _swar; }

    void setMolecularSurfaceFactoryRegistry (MolecularSurfaceFactoryRegistry* _msfr) { assert( msfr == nullptr || _msfr == nullptr ); msfr = _msfr; }

    void setSWResultFilterRegistry (SWResultFilterRegistry* _swrfr) { assert( swrfr == nullptr || _swrfr == nullptr ); swrfr = _swrfr; }

    void setSWMulAlignResultNamesTagsRegistry (SWMulAlignResultNamesTagsRegistry * _swmarntr) { assert( swmarntr == nullptr || _swmarntr == nullptr ); swmarntr = _swmarntr; }

    void setMsaColorSchemeRegistry(MsaColorSchemeRegistry* _mcsr) {assert( mcsr == nullptr || _mcsr == nullptr ); mcsr = _mcsr;}

    void setMsaHighlightingSchemeRegistry(MsaHighlightingSchemeRegistry* _mhsr) {assert( mhsr == nullptr || _mhsr == nullptr ); mhsr = _mhsr;}

    void setSecStructPedictAlgRegistry(SecStructPredictAlgRegistry* _sspar) {assert( secStructPredictRegistry == nullptr || _sspar == nullptr ); secStructPredictRegistry = _sspar;}

    void setCudaGpuRegistry( CudaGpuRegistry * _cgr ) { assert( cgr == nullptr || _cgr == nullptr ); cgr = _cgr; }

    void setOpenCLGpuRegistry( OpenCLGpuRegistry* _oclgr ) { assert( oclgr == nullptr || _oclgr == nullptr ); oclgr = _oclgr; }

    void setRecentlyDownloadedCache( RecentlyDownloadedCache* _rdc) { assert( rdc == nullptr || _rdc == nullptr ); rdc = _rdc;}

    void setDataPathRegistry( U2DataPathRegistry* _dpr) { assert( dpr == nullptr || _dpr == nullptr ); dpr = _dpr;}

    void setScriptingToolRegistry( ScriptingToolRegistry* _str) { assert( str == nullptr || _str == nullptr ); str = _str;}

    void setPasteFactory( PasteFactory* _pf) { assert( pf == nullptr || _pf == nullptr ); pf = _pf;}

    void setDashboardInfoRegistry(DashboardInfoRegistry *_dashboardInfoRegistry) { assert(dashboardInfoRegistry == nullptr || _dashboardInfoRegistry == nullptr); dashboardInfoRegistry = _dashboardInfoRegistry; }

    void setProtocolInfoRegistry( ProtocolInfoRegistry * pr ) { assert( nullptr == protocolInfoRegistry || nullptr == pr );
        protocolInfoRegistry = pr; }

    void setRemoteMachineMonitor( RemoteMachineMonitor * rm ) { assert( nullptr == remoteMachineMonitor || nullptr == rm );
        remoteMachineMonitor = rm; }

    void setPhyTreeGeneratorRegistry(PhyTreeGeneratorRegistry* genRegistry) {
        assert(nullptr == treeGeneratorRegistry || nullptr == genRegistry);
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

    void setCMDLineRegistry(CMDLineRegistry* r) { assert(cmdLineRegistry == nullptr || r == nullptr); cmdLineRegistry = r; }

    void setVirtualFileSystemRegistry( VirtualFileSystemRegistry * r ) {
        assert( virtualFileSystemRegistry == nullptr || r == nullptr );
        virtualFileSystemRegistry = r;
    }

    void setDnaAssemblyAlgRegistry( DnaAssemblyAlgRegistry * r ) {
        assert( dnaAssemblyAlgRegistry == nullptr || r == nullptr );
        dnaAssemblyAlgRegistry = r;
    }

    void setGenomeAssemblyAlgRegistry( GenomeAssemblyAlgRegistry * r ) {
        assert( genomeAssemblyAlgRegistry == nullptr || r == nullptr );
        genomeAssemblyAlgRegistry = r;
    }

    void setDataBaseRegistry( DataBaseRegistry *dbr) {
        assert (dataBaseRegistry == nullptr || dbr == nullptr );
        dataBaseRegistry = dbr;
    }

    void setExternalToolRegistry( ExternalToolRegistry * _etr) {
        assert( externalToolRegistry == nullptr || _etr == nullptr );
        externalToolRegistry = _etr;
    }

    void setRepeatFinderTaskFactoryRegistry (RepeatFinderTaskFactoryRegistry* _rfr) {
        assert( rfr == nullptr || _rfr == nullptr ); rfr = _rfr;
    }

    void setQDActorFactoryRegistry(QDActorPrototypeRegistry* _queryfactoryRegistry) {
        assert( qdafr == nullptr || _queryfactoryRegistry == nullptr );
        qdafr = _queryfactoryRegistry;
    }

    void setAutoAnnotationsSupport(AutoAnnotationsSupport* _aaSupport) {
        assert( aaSupport == nullptr || _aaSupport == nullptr );
        aaSupport = _aaSupport;
    }

    void setDbiRegistry(U2DbiRegistry *_dbiRegistry) {
        assert((nullptr == dbiRegistry) || (nullptr == _dbiRegistry));
        dbiRegistry = _dbiRegistry;
    }

    void setUdrSchemaRegistry(UdrSchemaRegistry *_udrSchemaRegistry) {
        assert((nullptr == udrSchemaRegistry) || (nullptr == _udrSchemaRegistry));
        udrSchemaRegistry = _udrSchemaRegistry;
    }

    void setCDSearchFactoryRegistry(CDSearchFactoryRegistry* _cdsfr) {
        assert((nullptr == cdsfr) || (nullptr == _cdsfr));
        cdsfr= _cdsfr;
    }

    void setSplicedAlignmentTaskRegistry(SplicedAlignmentTaskRegistry* tr) {
        assert((nullptr == splicedAlignmentTaskRegistry) || (nullptr == tr));
        splicedAlignmentTaskRegistry = tr;
    }

    void setOPCommonWidgetFactoryRegistry(OPCommonWidgetFactoryRegistry *_opCommonWidgetFactoryRegistry) {
        assert((nullptr == opCommonWidgetFactoryRegistry) || (nullptr == _opCommonWidgetFactoryRegistry));
        opCommonWidgetFactoryRegistry = _opCommonWidgetFactoryRegistry;
    }

    void setOPWidgetFactoryRegistry(OPWidgetFactoryRegistry* _opWidgetFactoryRegistry) {
        assert((nullptr == opWidgetFactoryRegistry) || (nullptr == _opWidgetFactoryRegistry));
        opWidgetFactoryRegistry = _opWidgetFactoryRegistry;
    }

    void setStructuralAlignmentAlgorithmRegistry(StructuralAlignmentAlgorithmRegistry *_saar) {
        assert(saar == nullptr || _saar == nullptr);
        saar = _saar;
    }

    void setWorkflowScriptRegistry(WorkflowScriptRegistry *_wsr) {
        assert(workflowScriptRegistry == nullptr || _wsr == nullptr);
        workflowScriptRegistry = _wsr;
    }

    void setCredentialsAsker(CredentialsAsker* _credentialsAsker) {
        assert(credentialsAsker == nullptr || _credentialsAsker == nullptr);
        credentialsAsker = _credentialsAsker;
    }

    void setPasswordStorage(PasswordStorage *_passwordStorage) {
        assert(passwordStorage == nullptr || _passwordStorage == nullptr);
        passwordStorage = _passwordStorage;
    }

    void setAppFileStorage(AppFileStorage *afs) {
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

    void setWelcomePageActionRegistry(IdRegistry<WelcomePageAction> *value) {
        assert(welcomePageActionRegistry == nullptr || value == nullptr);
        welcomePageActionRegistry = value;
    }

    void setProjectFilterTaskRegistry(ProjectFilterTaskRegistry *value) {
        assert(projectFilterTaskRegistry == nullptr || value == nullptr);
        projectFilterTaskRegistry = value;
    }

    void setGUIMode(bool v) {
        guiMode = v;
    }

    void _setActiveWindowName(const QString& name) {
        activeWindow = name;
    }

    void setWorkingDirectoryPath(const QString &path) {
        assert(!path.isEmpty());
        workingDirectoryPath = path;
    }

    void setGUITestBase(UGUITestBase *_tb) {assert(tb == nullptr || _tb == nullptr); tb = _tb;}

    static AppContextImpl* getApplicationContext();

protected:
    virtual PluginSupport*  _getPluginSupport() const {return ps;}
    virtual ServiceRegistry*  _getServiceRegistry() const {return sr;}
    virtual ProjectLoader*  _getProjectLoader() const {return pl;}
    virtual Project*        _getProject() const {return prj;}
    virtual ProjectService* _getProjectService() const {return prs;}
    virtual MainWindow*     _getMainWindow() const {return mw;}
    virtual ProjectView*    _getProjectView() const {return pv;}
    virtual PluginViewer*   _getPluginViewer() const {return plv;}
    virtual Settings*       _getSettings() const {return ss;}
    virtual Settings*       _getGlobalSettings() const {return gs;}
    virtual AppSettings*    _getAppSettings() const{return as;}
    virtual AppSettingsGUI* _getAppSettingsGUI() const{return asg;}

    virtual DocumentFormatRegistry*         _getDocumentFormatRegistry() const {return dfr;}
    virtual IOAdapterRegistry*              _getIOAdapterRegistry() const  {return io;}
    virtual DNATranslationRegistry*         _getDNATranslationRegistry() const  {return dtr;}
    virtual DNAAlphabetRegistry*            _getDNAAlphabetRegistry() const {return dal;}
    virtual GObjectViewFactoryRegistry*     _getObjectViewFactoryRegistry() const  {return ovfr;}
    virtual TaskScheduler*                  _getTaskScheduler() const  {return ts;}
    virtual ResourceTracker*                _getResourceTracker() const {return rt;}
    virtual AnnotationSettingsRegistry*     _getAnnotationsSettingsRegistry() const {return asr;}
    virtual TestFramework*                  _getTestFramework() const {return tf;}
    virtual DBXRefRegistry*                 _getDBXRefRegistry() const {return dbxr;}
    virtual SubstMatrixRegistry*            _getSubstMatrixRegistry() const {return smr;}
    virtual SmithWatermanTaskFactoryRegistry*   _getSmithWatermanTaskFactoryRegistry() const {return swar;}
    virtual PhyTreeGeneratorRegistry*         _getPhyTreeGeneratorRegistry() const {return treeGeneratorRegistry;}

    virtual MolecularSurfaceFactoryRegistry*   _getMolecularSurfaceFactoryRegistry() const {return msfr;}
    virtual SWResultFilterRegistry*     _getSWResultFilterRegistry() const {return swrfr;}
    virtual SWMulAlignResultNamesTagsRegistry * _getSWMulAlignResultNamesTagsRegistry() const {return swmarntr;}
    virtual MsaColorSchemeRegistry*     _getMsaColorSchemeRegistry() const {return mcsr;}
    virtual MsaHighlightingSchemeRegistry* _getMsaHighlightingSchemeRegistry() const {return mhsr;}
    virtual SecStructPredictAlgRegistry* _getSecStructPredictAlgRegistry() const {return secStructPredictRegistry;}
    virtual CudaGpuRegistry *            _getCudaGpuRegistry() const { return cgr; }
    virtual OpenCLGpuRegistry *          _getOpenCLGpuRegistry() const { return oclgr; }
    virtual RecentlyDownloadedCache*     _getRecentlyDownloadedCache() const {return rdc;}
    virtual ProtocolInfoRegistry *          _getProtocolInfoRegistry() const { return protocolInfoRegistry; }
    virtual RemoteMachineMonitor *          _getRemoteMachineMonitor() const { return remoteMachineMonitor; }
    virtual CMDLineRegistry*                _getCMDLineRegistry() const {return cmdLineRegistry;}
    virtual MSAConsensusAlgorithmRegistry*  _getMSAConsensusAlgorithmRegistry() const {return msaConsensusAlgoRegistry;}
    virtual MSADistanceAlgorithmRegistry*  _getMSADistanceAlgorithmRegistry() const {return msaDistanceAlgoRegistry;}
    virtual AssemblyConsensusAlgorithmRegistry*  _getAssemblyConsensusAlgorithmRegistry() const {return assemblyConsensusAlgoRegistry;}
    virtual PWMConversionAlgorithmRegistry* _getPWMConversionAlgorithmRegistry() const {return pwmConversionAlgoRegistry;}
    virtual VirtualFileSystemRegistry *     _getVirtualFileSystemRegistry() const { return virtualFileSystemRegistry; }
    virtual DnaAssemblyAlgRegistry*         _getDnaAssemblyAlgRegistry() const {return dnaAssemblyAlgRegistry; }
    virtual GenomeAssemblyAlgRegistry*         _getGenomeAssemblyAlgRegistry() const {return genomeAssemblyAlgRegistry; }
    virtual DataBaseRegistry *              _getDataBaseRegistry() const {return dataBaseRegistry;}
    virtual ExternalToolRegistry *          _getExternalToolRegistry() const {return externalToolRegistry;}
    virtual RepeatFinderTaskFactoryRegistry*   _getRepeatFinderTaskFactoryRegistry() const {return rfr;}
    virtual QDActorPrototypeRegistry*            _getQDActorFactoryRegistry() const { return qdafr; }
    virtual StructuralAlignmentAlgorithmRegistry* _getStructuralAlignmentAlgorithmRegistry() const { return saar; }
    virtual AutoAnnotationsSupport*         _getAutoAnnotationsSupport() const { return aaSupport; }
    virtual CDSearchFactoryRegistry*        _getCDSFactoryRegistry() const { return cdsfr; }
    virtual U2DbiRegistry *                 _getDbiRegistry() const { return dbiRegistry; }
    virtual UdrSchemaRegistry *             _getUdrSchemaRegistry() const { return udrSchemaRegistry; }
    virtual UGUITestBase*                    _getGUITestBase() const {return tb;}
    virtual SplicedAlignmentTaskRegistry*   _getSplicedAlignmentTaskRegistry() const { return splicedAlignmentTaskRegistry; }
    virtual OPCommonWidgetFactoryRegistry*  _getOPCommonWidgetFactoryRegistry() const { return opCommonWidgetFactoryRegistry; }
    virtual OPWidgetFactoryRegistry*        _getOPWidgetFactoryRegistry() const { return opWidgetFactoryRegistry; }
    virtual WorkflowScriptRegistry*         _getWorkflowScriptRegistry() const { return workflowScriptRegistry; }
    virtual AppFileStorage*                 _getAppFileStorage() const { return appFileStorage; }
    virtual AlignmentAlgorithmsRegistry*      _getAlignmentAlgorithmsRegistry() const { return alignmentAlgorithmsRegistry; }
    virtual U2DataPathRegistry*             _getDataPathRegistry() const { return dpr; }
    virtual ScriptingToolRegistry*          _getScriptingToolRegistry() const { return str; }
    virtual CredentialsAsker*               _getCredentialsAsker() const { return credentialsAsker; }
    virtual PasswordStorage*                _getPasswordStorage() const { return passwordStorage; }
    virtual ConvertFactoryRegistry*         _getConvertFactoryRegistry() const { return cfr; }
    virtual IdRegistry<WelcomePageAction>* _getWelcomePageActionRegistry() const { return welcomePageActionRegistry; }
    virtual ProjectFilterTaskRegistry *    _getProjectFilterTaskRegistry() const { return projectFilterTaskRegistry; }
    virtual PasteFactory *                 _getPasteFactory() const { return pf; }
    virtual DashboardInfoRegistry *         _getDashboardInfoRegistry() const { return dashboardInfoRegistry; }

    virtual void _registerGlobalObject(AppGlobalObject* go);
    virtual void _unregisterGlobalObject(const QString& id);
    virtual AppGlobalObject* _getGlobalObjectById(const QString& id) const;
    virtual bool _isGUIMode() const {return guiMode;}
    virtual QString _getActiveWindowName() const {return activeWindow;}
    virtual QString _getWorkingDirectoryPath() const { return workingDirectoryPath; }

private:
    AlignmentAlgorithmsRegistry* alignmentAlgorithmsRegistry;
    AnnotationSettingsRegistry* asr;
    AppFileStorage *appFileStorage;
    AppSettings * as;
    AppSettingsGUI* asg;
    AssemblyConsensusAlgorithmRegistry* assemblyConsensusAlgoRegistry;
    AutoAnnotationsSupport* aaSupport;
    CDSearchFactoryRegistry* cdsfr;
    CMDLineRegistry* cmdLineRegistry;
    ConvertFactoryRegistry *cfr;
    CredentialsAsker* credentialsAsker;
    CudaGpuRegistry * cgr;
    DBXRefRegistry* dbxr;
    DNAAlphabetRegistry* dal;
    DNATranslationRegistry* dtr;
    DashboardInfoRegistry *dashboardInfoRegistry;
    DataBaseRegistry* dataBaseRegistry;
    DnaAssemblyAlgRegistry* dnaAssemblyAlgRegistry;
    DocumentFormatRegistry* dfr;
    ExternalToolRegistry * externalToolRegistry;
    GObjectViewFactoryRegistry* ovfr;
    GenomeAssemblyAlgRegistry* genomeAssemblyAlgRegistry;
    IOAdapterRegistry* io;
    IdRegistry<WelcomePageAction> *welcomePageActionRegistry;
    MSAConsensusAlgorithmRegistry* msaConsensusAlgoRegistry;
    MSADistanceAlgorithmRegistry* msaDistanceAlgoRegistry;
    MainWindow* mw;
    MolecularSurfaceFactoryRegistry* msfr;
    MsaColorSchemeRegistry* mcsr;
    MsaHighlightingSchemeRegistry *mhsr;
    OPCommonWidgetFactoryRegistry* opCommonWidgetFactoryRegistry;
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry;
    OpenCLGpuRegistry * oclgr;
    PWMConversionAlgorithmRegistry* pwmConversionAlgoRegistry;
    PasswordStorage* passwordStorage;
    PasteFactory* pf;
    PhyTreeGeneratorRegistry *treeGeneratorRegistry;
    PluginSupport* ps;
    PluginViewer* plv;
    Project*    prj;
    ProjectFilterTaskRegistry *projectFilterTaskRegistry;
    ProjectLoader* pl;
    ProjectService* prs;
    ProjectView* pv;
    ProtocolInfoRegistry * protocolInfoRegistry;
    QDActorPrototypeRegistry* qdafr;
    RecentlyDownloadedCache* rdc;
    RemoteMachineMonitor * remoteMachineMonitor;
    RepeatFinderTaskFactoryRegistry* rfr;
    ResourceTracker* rt;
    SWMulAlignResultNamesTagsRegistry * swmarntr;
    SWResultFilterRegistry*  swrfr;
    ScriptingToolRegistry *str;
    SecStructPredictAlgRegistry* secStructPredictRegistry;
    ServiceRegistry* sr;
    Settings* gs;
    Settings* ss;
    SmithWatermanTaskFactoryRegistry* swar;
    SplicedAlignmentTaskRegistry* splicedAlignmentTaskRegistry;
    StructuralAlignmentAlgorithmRegistry* saar;
    SubstMatrixRegistry* smr;
    TaskScheduler* ts;
    TestFramework* tf;
    U2DataPathRegistry *dpr;
    U2DbiRegistry *dbiRegistry;
    UGUITestBase *tb;
    UdrSchemaRegistry *udrSchemaRegistry;
    VirtualFileSystemRegistry * virtualFileSystemRegistry;
    WorkflowScriptRegistry* workflowScriptRegistry;

    bool guiMode;
    QString activeWindow;
    QString workingDirectoryPath;

    QList<AppGlobalObject*> appGlobalObjects;
};

}//namespace

#endif

