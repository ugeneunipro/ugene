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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/VariantTrackObject.h>

#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/WorkflowEnv.h>

#include "AssemblySamtoolsMpileup.h"

#define CALL_VARIANTS_DIR "variants"

namespace U2 {
namespace LocalWorkflow{

CallVariantsTask::CallVariantsTask( const CallVariantsTaskSettings& _settings, DbiDataStorage* _store )
:ExternalToolSupportTask(tr("Call variants for %1").arg(_settings.refSeqUrl), TaskFlag_NoRun)
,settings(_settings)
,loadTask(NULL)
,mpileupTask(NULL)
,storage(_store)
{
    GCOUNTER(cvar, tvar, "NGS:CallVariantsTask");
    setMaxParallelSubtasks(1);
}

QString CallVariantsTask::toString(FileType type) {
    switch (type) {
        case Reference:
            return tr("reference");
        case Assembly:
            return tr("assembly");
        default:
            FAIL("Unknown file type", "");
    }
}

bool CallVariantsTask::ensureFileExists(const QString &url, FileType type) {
    if (!QFile::exists(url)) {
        setError(tr("The %1 file does not exist: %2").arg(toString(type)).arg(url));
        return false;
    }
    return true;
}

void CallVariantsTask::prepare(){
    CHECK(ensureFileExists(settings.refSeqUrl, Reference), );
    foreach (const QString &url, settings.assemblyUrls) {
        CHECK(ensureFileExists(url, Assembly), );
    }

    if (settings.assemblyUrls.size() < 1){
        stateInfo.setError(tr("No assembly files"));
        return;
    }

    if (storage == NULL){
        stateInfo.setError(tr("No dbi storage"));
        return;
    }
    if (settings.refSeqUrl.isEmpty()){
        stateInfo.setError(tr("No sequence URL"));
        return;
    }

    mpileupTask = new SamtoolsMpileupTask(settings);
    mpileupTask->addListeners(getListeners());
    addSubTask(mpileupTask );
}

QList<Task*> CallVariantsTask::onSubTaskFinished( Task* subTask ){
    QList<Task*>res;

    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if(hasError() || isCanceled()) {
        return res;
    }

    if (subTask == mpileupTask) {
        const GUrl url(settings.variationsUrl);
        IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById( IOAdapterUtils::url2io( url ) );
        if ( iof == NULL ) {
            return res;
        }
        QList<FormatDetectionResult> dfs = DocumentUtils::detectFormat(url);
        if( dfs.isEmpty() ) {
            return res;
        }
        DocumentFormat * df = dfs.first().format;
        QVariantMap cfg;
        cfg.insert(DocumentFormat::DBI_REF_HINT, qVariantFromValue(storage->getDbiRef()));
        loadTask =  new LoadDocumentTask( df->getFormatId(), url, iof, cfg );
        res.append(loadTask);

    } else if(subTask == loadTask){
        QScopedPointer<Document> doc (loadTask->takeDocument(false));
        SAFE_POINT(doc!=NULL, tr("No document loaded"), res);
        doc->setDocumentOwnsDbiResources(false);
        foreach(GObject* go, doc->findGObjectByType(GObjectTypes::VARIANT_TRACK)) {
            VariantTrackObject *varObj = dynamic_cast<VariantTrackObject*>(go);
            CHECK_EXT(NULL != varObj, taskLog.error(tr("Incorrect variant track object in %1").arg(doc->getURLString())), res);

            QVariantMap m;
            SharedDbiDataHandler handler = storage->getDataHandler(varObj->getEntityRef());
            m[BaseSlots::VARIATION_TRACK_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);
            m[BaseSlots::URL_SLOT().getId()] = settings.variationsUrl;
            results.append(m);
        }
   }

    return res;
}

QString CallVariantsTask::tmpFilePath(const QString &baseName, const QString &ext, U2OpStatus &os) {
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(CALL_VARIANTS_DIR);
    return GUrlUtils::prepareTmpFileLocation(tmpDirPath, baseName, ext, os);
}

/************************************************************************/
/* SamtoolsMpileupTask */
/************************************************************************/

const QString SamtoolsMpileupTask::SAMTOOLS_ID = "USUPP_SAMTOOLS";
const QString SamtoolsMpileupTask::BCFTOOLS_ID = "USUPP_BCFTOOLS";
const QString SamtoolsMpileupTask::VCFUTILS_ID = "USUPP_VCFUTILS";

SamtoolsMpileupTask::SamtoolsMpileupTask(const CallVariantsTaskSettings &_settings)
:ExternalToolSupportTask(tr("Samtool mpileup for %1 ").arg(_settings.refSeqUrl), TaskFlags(TaskFlag_None)),settings(_settings)
{

}

void SamtoolsMpileupTask::prepare(){
    if (settings.refSeqUrl.isEmpty()){
        setError(tr("No reference sequence URL to do pileup"));
        return ;
    }

    if (settings.assemblyUrls.isEmpty()){
        setError(tr("No assembly URL to do pileup"));
        return ;
    }

    foreach(const QString& aUrl, settings.assemblyUrls){
        if (aUrl.isEmpty()){
            setError(tr("There is an assembly with an empty path"));
            return ;
        }
    }

    const QDir outDir = QFileInfo(settings.variationsUrl).absoluteDir();
    if (!outDir.exists()) {
        const bool created = outDir.mkpath(outDir.absolutePath());
        if (!created) {
            setError(tr("Can not create the folder: ") + outDir.absolutePath());
        }
    }
}

void SamtoolsMpileupTask::run() {
    ProcessRun samtools = ExternalToolSupportUtils::prepareProcess(SAMTOOLS_ID, settings.getMpiliupArgs(), "", QStringList(), stateInfo, getListener(0));
    CHECK_OP(stateInfo, );
    QScopedPointer<QProcess> sp(samtools.process);
    ExternalToolLogParser sLogParser;
    ExternalToolRunTaskHelper sHelper(samtools.process, &sLogParser, stateInfo);
    setListenerForHelper(&sHelper, 0);

    ProcessRun bcftools = ExternalToolSupportUtils::prepareProcess(BCFTOOLS_ID, settings.getBcfViewArgs(), "", QStringList(), stateInfo, getListener(1));
    CHECK_OP(stateInfo, );
    QScopedPointer<QProcess> bp(bcftools.process);
    ExternalToolLogParser bLogParser;
    ExternalToolRunTaskHelper bHelper(bcftools.process, &bLogParser, stateInfo);
    setListenerForHelper(&bHelper, 1);

    ProcessRun vcfutils = ExternalToolSupportUtils::prepareProcess(VCFUTILS_ID, settings.getVarFilterArgs(), "", QStringList(), stateInfo, getListener(2));
    CHECK_OP(stateInfo, );
    QScopedPointer<QProcess> vp(vcfutils.process);
    ExternalToolLogParser vLogParser;
    ExternalToolRunTaskHelper vHelper(vcfutils.process, &vLogParser, stateInfo);
    setListenerForHelper(&vHelper, 2);

    samtools.process->setStandardOutputProcess(bcftools.process);
    bcftools.process->setStandardOutputProcess(vcfutils.process);
    vcfutils.process->setStandardOutputFile(settings.variationsUrl);

    start(samtools, "SAMtools");
    CHECK_OP(stateInfo, );
    start(bcftools, "BCFtools");
    CHECK_OP(stateInfo, );
    start(vcfutils, "vcfutils");
    CHECK_OP(stateInfo, );

    while(!vcfutils.process->waitForFinished(1000)){
        if (isCanceled()) {
            CmdlineTaskRunner::killProcessTree(samtools.process);
            CmdlineTaskRunner::killProcessTree(bcftools.process);
            CmdlineTaskRunner::killProcessTree(vcfutils.process);
            return;
        }
    }

    checkExitCode(vcfutils.process, "vcfutils");
    checkExitCode(bcftools.process, "BCFtools");
    checkExitCode(samtools.process, "SAMtools");
}

void SamtoolsMpileupTask::start(const ProcessRun &pRun, const QString &toolName) {
    pRun.process->start(pRun.program, pRun.arguments);
    bool started = pRun.process->waitForStarted();
    CHECK_EXT(started, setError(tr("Can not run %1 tool").arg(toolName)), );
}

void SamtoolsMpileupTask::checkExitCode(QProcess *process, const QString &toolName) {
    int exitCode = process->exitCode();
    if (exitCode != EXIT_SUCCESS && !hasError()) {
        setError(tr("%1 tool exited with code %2").arg(toolName).arg(exitCode));
    } else {
        algoLog.details(tr("Tool %1 finished successfully").arg(toolName));
    }
}

/************************************************************************/
/* CallVariantsTaskSettings */
/************************************************************************/
QStringList CallVariantsTaskSettings::getMpiliupArgs() const {
    QStringList result;
    result << "mpileup"
        << "-uf"
        << refSeqUrl
        << "-C"
        << QString::number(capq_thres)
        << "-d"
        << QString::number(max_depth)
        << "-q"
        << QString::number(min_mq)
        << "-Q"
        << QString::number(min_baseq)
        << "-e"
        << QString::number(extq)
        << "-h"
        << QString::number(tandemq)
        << "-L"
        << QString::number(max_indel_depth)
        << "-o"
        << QString::number(openq);

    if (illumina13) {
        result << "-6";
    }
    if (use_orphan) {
        result << "-A";
    }
    if (disable_baq) {
        result << "-B";
    }
    if (ext_baq) {
        result << "-E";
    }
    if (!bed.isEmpty()) {
        result << "-l";
        result << bed;
    }
    if (!reg.isEmpty()) {
        result << "-r";
        result << reg;
    }
    if (no_indel) {
        result << "-I";
    }
    if (!pl_list.isEmpty()) {
        result << "-P";
        result << pl_list;
    }
    result << assemblyUrls;
    return result;
}

QStringList CallVariantsTaskSettings::getBcfViewArgs() const {
    QStringList result;
    result << "view"
        << "-vc"
        << "-d"
        << QString::number(min_smpl_frac)
        << "-i"
        << QString::number(indel_frac)
        << "-p"
        << QString::number(pref)
        << "-t"
        << QString::number(theta)
        << "-1"
        << QString::number(n1)
        << "-U"
        << QString::number(n_perm)
        << "-X"
        << QString::number(min_perm_p);

    if (keepalt) {
        result << "-A";
    }
    if (fix_pl) {
        result << "-F";
    }
    if (no_geno) {
        result << "-G";
    }
    if (!bcf_bed.isEmpty()) {
        result<< "-l";
        result << bcf_bed;
    }
    if (acgt_only) {
        result << "-N";
    }
    if (qcall) {
        result << "-Q";
    }
    if (!samples.isEmpty()) {
        result << "-s";
        result << samples;
    }
    if (call_gt) {
        result << "-g";
    }
    if (!ptype.isEmpty()) {
        result<< "-P";
        result << ptype;
    }
    if (!ccall.isEmpty()) {
        result<< "-T";
        result << ccall;
    }
    result << "-";
    return result;
}

QStringList CallVariantsTaskSettings::getVarFilterArgs() const {
    QStringList result;
    result << "varFilter"
        << "-Q"
        << QString::number(minQual)
        << "-d"
        << QString::number(minDep)
        << "-D"
        << QString::number(maxDep)
        << "-a"
        << QString::number(minAlt)
        << "-w"
        << QString::number(gapSize)
        << "-W"
        << QString::number(window)
        << "-1"
        << QString::number(pvalue1)
        << "-2"
        << QString::number(pvalue2)
        << "-3"
        << QString::number(pvalue3)
        << "-4"
        << QString::number(pvalue4)
        << "-e"
        << QString::number(pvalueHwe);
    if (printFiltered) {
        result << "-p";
    }
    return result;
}

}
} //namespace
