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

#ifndef _U2_ASSEMBLY_SAMTOOLS_MPILEUP_H_
#define _U2_ASSEMBLY_SAMTOOLS_MPILEUP_H_

#include <U2Core/global.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

#include <U2Lang/LocalDomain.h>

namespace U2 {

class LoadDocumentTask;

namespace LocalWorkflow{

class CallVariantsTaskSettings{
    public:

    QString variationsUrl;
    QList<QString> assemblyUrls;
    QString refSeqUrl; // TODO: change to sequence ID

    //mpileup
    bool    illumina13;
    bool    use_orphan;
    bool    disable_baq;
    int     capq_thres;
    int     max_depth;
    bool    ext_baq;
    QByteArray bed;
    QByteArray reg;
    int     min_mq;
    int     min_baseq;
    int     extq;
    int     tandemq;
    bool    no_indel;
    int     max_indel_depth;
    int     openq;
    QByteArray pl_list;
    
    //bcf view
    bool    keepalt;
    bool    fix_pl;
    bool    no_geno;
    bool    acgt_only;
    QByteArray bcf_bed;
    bool    qcall;
    QByteArray samples;
    float   min_smpl_frac;
    bool    call_gt;
    float   indel_frac;
    float   pref;
    QByteArray ptype;
    float   theta;
    QByteArray ccall;
    int     n1;
    int     n_perm;
    float   min_perm_p;

    //varFilter
    int minQual;
    int minDep;
    int maxDep;
    int minAlt;
    int gapSize;
    int window;
    float pvalue1;
    double pvalue2;
    float pvalue3;
    float pvalue4;
    float pvalueHwe;
    bool printFiltered;

    QStringList getMpiliupArgs() const;
    QStringList getBcfViewArgs() const;
    QStringList getVarFilterArgs() const;
};

class SamtoolsMpileupTask : public ExternalToolSupportTask{
    Q_OBJECT
public:
    SamtoolsMpileupTask(const CallVariantsTaskSettings &settings);

    void prepare();
    void run();

private:
    void start(const ProcessRun &pRun, const QString &toolName);
    void checkExitCode(QProcess *process, const QString &toolName);

private:
    CallVariantsTaskSettings settings;

    // The following constants are taken from the appropriate files in external_tool_support plugin
    static const QString SAMTOOLS_ID;
    static const QString BCFTOOLS_ID;
    static const QString VCFUTILS_ID;
};

class CallVariantsTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    CallVariantsTask(const CallVariantsTaskSettings& _settings, DbiDataStorage* _store);

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    const QList<QVariantMap>& getResults(){return results;}
    QString getResultUrl(){return settings.variationsUrl;}
    void clearResults(){results.clear();}

    static QString tmpFilePath(const QString &baseName, const QString &ext, U2OpStatus &os);

private:
    enum FileType {Reference, Assembly};
    static QString toString(FileType type);
    bool ensureFileExists(const QString &url, FileType type);

private:
    CallVariantsTaskSettings    settings;
    LoadDocumentTask*       loadTask;
    SamtoolsMpileupTask*    mpileupTask;
    DbiDataStorage*         storage;
    QList<QVariantMap>      results;
};

}
}//namespace

#endif //_U2_ASSEMBLY_SAMTOOLS_MPILEUP_H_
