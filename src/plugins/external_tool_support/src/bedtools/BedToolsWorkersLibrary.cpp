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

#include <QTextStream>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/DbiDataStorage.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "BedtoolsIntersectTask.h"
#include "BedtoolsSupport.h"
#include "BedToolsWorkersLibrary.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//Slopbed
const QString SlopbedWorkerFactory::ACTOR_ID("slopbed");
static const QString GENOME_ID( "genome-id" );
static const QString B_ID( "b-id" );
static const QString L_ID( "l-id" );
static const QString R_ID( "r-id" );
static const QString S_ID( "s-id" );
static const QString PCT_ID( "pct-id" );
static const QString HEADER_ID( "header-id" );
static const QString FILTER_ID( "filter-id" );

/************************************************************************/
/* SlopbedPrompter */
/************************************************************************/
QString SlopbedPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BaseNGSWorker::INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Increases the size of each feature in files from %1 with bedtool slop.").arg(producerName);
    return doc;
}

/************************************************************************/
/* SlopbedWorkerFactory */
/************************************************************************/
void SlopbedWorkerFactory::init() {
    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(BedtoolsSupport::GENOMES_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }
    Descriptor desc( ACTOR_ID, SlopbedWorker::tr("slopBed"),
        SlopbedWorker::tr("Increases the size of each feature in files using bedtools slop.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(BaseNGSWorker::INPUT_PORT, SlopbedWorker::tr("Input File"),
            SlopbedWorker::tr("Set of files to bedtools slop"));
        Descriptor outD(BaseNGSWorker::OUTPUT_PORT, SlopbedWorker::tr("Output File"),
            SlopbedWorker::tr("Output file"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("sb.input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("sb.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor outDir(BaseNGSWorker::OUT_MODE_ID, SlopbedWorker::tr("Output folder"),
            SlopbedWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(BaseNGSWorker::CUSTOM_DIR_ID, SlopbedWorker::tr("Custom folder"),
            SlopbedWorker::tr("Select the custom output folder."));

        Descriptor outName(BaseNGSWorker::OUT_NAME_ID, SlopbedWorker::tr("Output file name"),
            SlopbedWorker::tr("A name of an output file. If default of empty value is provided the output name is the name of the first file with additional extension."));

        Descriptor genomeAttrDesc(GENOME_ID, SlopbedWorker::tr("Genome"),
            SlopbedWorker::tr("In order to prevent the extension of intervals beyond chromosome boundaries, bedtools slop requires a genome file defining the length of each chromosome or contig. The format of the file is: <chromName><TAB><chromSize> (-g)."));

        Descriptor bAttr(B_ID, SlopbedWorker::tr("Each direction increase"),
            SlopbedWorker::tr("Increase the BED/GFF/VCF entry by the same number base pairs in each direction. If this parameter is used -l and -l are ignored. Enter 0 to disable. (-b)"));

        Descriptor lAttr(L_ID, SlopbedWorker::tr("Subtract from start"),
            SlopbedWorker::tr("The number of base pairs to subtract from the start coordinate. Enter 0 to disable. (-l)"));

        Descriptor rAttr(R_ID, SlopbedWorker::tr("Add to end"),
            SlopbedWorker::tr("The number of base pairs to add to the end coordinate. Enter 0 to disable. (-r)"));

        Descriptor sAttr(S_ID, SlopbedWorker::tr("Strand-based"),
            SlopbedWorker::tr("Define -l and -r based on strand. For example. if used, -l 500 for a negative-stranded feature, it will add 500 bp to the end coordinate. (-s)"));

        Descriptor pctAttr(PCT_ID, SlopbedWorker::tr("As fraction"),
            SlopbedWorker::tr("Define -l and -r as a fraction of the feature’s length. E.g. if used on a 1000bp feature, -l 0.50, will add 500 bp “upstream”. (-pct)"));

        Descriptor headerAttr(HEADER_ID, SlopbedWorker::tr("Print header"),
            SlopbedWorker::tr("Print the header from the input file prior to results. (-header)"));

        Descriptor filterAttr(FILTER_ID, SlopbedWorker::tr("Filter start>end fields"),
            SlopbedWorker::tr("Remove lines with start position greater than end position"));


        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(BaseNGSWorker::OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;
        a << new Attribute( outName, BaseTypes::STRING_TYPE(), false, QVariant(BaseNGSWorker::DEFAULT_NAME));

        Attribute* genomeAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
            }else{
                genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true);
            }
        }else{
            genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true);
        }
        a << genomeAttr;

        a << new Attribute( bAttr, BaseTypes::NUM_TYPE(), false, QVariant(0));
        a << new Attribute( lAttr, BaseTypes::NUM_TYPE(), false, QVariant(0));
        a << new Attribute( rAttr, BaseTypes::NUM_TYPE(), false, QVariant(0));
        a << new Attribute( sAttr, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( pctAttr, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( headerAttr, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( filterAttr, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap directoryMap;
        QString fileDir = SlopbedWorker::tr("Input file");
        QString workflowDir = SlopbedWorker::tr("Workflow");
        QString customD = SlopbedWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[BaseNGSWorker::OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[BaseNGSWorker::CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);

        QVariantMap vm;
        if (dataPath){
            vm = dataPath->getDataItemsVariantMap();
        }
        delegates[GENOME_ID] = new ComboBoxWithUrlsDelegate(vm);

        QVariantMap lenMap; lenMap["minimum"] = QVariant(0); lenMap["maximum"] = QVariant(INT_MAX);
        delegates[B_ID] = new SpinBoxDelegate(lenMap);
        delegates[L_ID] = new SpinBoxDelegate(lenMap);
        delegates[R_ID] = new SpinBoxDelegate(lenMap);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new SlopbedPrompter());
    proto->addExternalTool(BedtoolsSupport::ET_BEDTOOLS_ID);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new SlopbedWorkerFactory());
}

/************************************************************************/
/* SlopbedWorker */
/************************************************************************/
SlopbedWorker::SlopbedWorker(Actor *a)
:BaseNGSWorker(a)
{

}

QVariantMap SlopbedWorker::getCustomParameters() const{
    QVariantMap res;
    const QString genomePath = getValue<QString>(GENOME_ID);
    if(!genomePath.isEmpty()){
        res["-g"] = genomePath;
    }
    const int b = getValue<int>(B_ID);
    if(b!=0){
        res["-b"] = b;
    }else{
        const int l = getValue<int>(L_ID);
        const int r = getValue<int>(R_ID);
        res["-l"] = l;
        res["-r"] = r;
}
    const bool s = getValue<bool>(S_ID);
    if(s){
        res["-s"] = "";
    }
    const int pct = getValue<bool>(PCT_ID);
    if(pct){
        res["-pct"] = "";
    }
    const bool header = getValue<bool>(HEADER_ID);
    if(header){
        res["-header"] = "";
    }
    const bool filter = getValue<bool>(FILTER_ID);
    if(filter){
        res["-filter"] = "";
    }
    return res;
}

QString SlopbedWorker::getDefaultFileName() const{
    return ".sb.bed";
}

Task *SlopbedWorker::getTask(const BaseNGSSetting &settings) const {
    return new SlopbedTask(settings);
}

//////////////////////////////////////////////////////
//SlopbedTask
SlopbedTask::SlopbedTask(const BaseNGSSetting &settings)
    :BaseNGSTask(settings)
    ,filterLines(false)
{
    GCOUNTER(cvar, tvar, "NGS:SlopBedTask");
}

void SlopbedTask::prepareStep(){
    Task* etTask = getExternalToolTask(BedtoolsSupport::ET_BEDTOOLS_ID);
    CHECK(etTask != NULL, );

    addSubTask(etTask);
}

void SlopbedTask::finishStep(){
    if(filterLines){
        QFile f(getResult());
        if(f.open(QIODevice::ReadWrite | QIODevice::Text)){
            QString s;
            QTextStream t(&f);
            while(!t.atEnd()){
                QString line = t.readLine();
                QStringList fields = line.split('\t');
                if(fields.length() >= 3){
                    bool parsed = true;
                    qint64 start = fields.at(1).toInt(&parsed);
                    if(parsed){
                        qint64 end = fields.at(2).toInt(&parsed);
                        if(parsed){
                            if (start>end){
                                continue;
                            }
                        }
                    }
                }
                s.append(line + "\n");
            }
            f.resize(0);
            t << s;
            f.close();
        }
    }
}

QStringList SlopbedTask::getParameters(U2OpStatus &os) {
    QStringList res;
    res << "slop";
    res << "-i";
    res << settings.inputUrl;



    if(!settings.customParameters.contains("-g")){
        os.setError("No genome file");
        return res;
    }else{
        res << "-g";
        res << settings.customParameters["-g"].toString();
    }

    if(!(settings.customParameters.contains("-b") || (settings.customParameters.contains("-r") && settings.customParameters.contains("-l")))){
        os.setError("Need -l and -r together or -b alone.");
        return res;
    }

    if(settings.customParameters.contains("-b")){
        res << "-b";
        res << settings.customParameters["-b"].toString();
    }else{
        if(settings.customParameters.contains("-r")){
            res << "-r";
            res << settings.customParameters["-r"].toString();
        }

        if(settings.customParameters.contains("-l")){
            res << "-l";
            res << settings.customParameters["-l"].toString();
        }
    }

    if(settings.customParameters.contains("-s")){
        res << "-s";
    }

    if(settings.customParameters.contains("-pct")){
        res << "-pct";
    }

    if(settings.customParameters.contains("-header")){
        res << "-header";
    }

    if(settings.customParameters.contains("-filter")){
        filterLines = true;
    }


    return res;
}

///////////////////////////////////////////////////////////////
//Genomecov
const QString GenomecovWorkerFactory::ACTOR_ID("genomecov");

static const QString MODE_ID( "mode-id" );
static const QString SPLIT_ID( "split-id" );
static const QString STRAND_ID( "strand-id" );
static const QString PRIME5_ID( "prime5-id" );
static const QString PRIME3_ID( "prime3-id" );
static const QString MAX_ID( "max-id" );
static const QString SCALE_ID( "scale-id" );
static const QString TRACKLINE_ID( "trackline-id" );
static const QString TRACKOPTS_ID( "trackopts-id" );

namespace{

enum GenomecovMode{
    Histograms = 0,
    EachPos1Based,
    EachPos0Based,
    BedGraph,
    BedGraphIncludeZero
};

QString getDescriptionByMode(GenomecovMode mode){
    QString res = "";
    if(mode == Histograms){
        res = QString("Compute a histogram of coverage.");

    }else if(mode == EachPos0Based){
        res = QString("Compute the depth of feature coverage for each base on each chromosome (0-based).");

    }else if(mode == EachPos1Based){
        res = QString("Compute the depth of feature coverage for each base on each chromosome (1-based).");

    }else if(mode == BedGraph){
        res = QString("Produces genome-wide coverage output in BEDGRAPH format.");

    }else if(mode == BedGraphIncludeZero){
        res = QString("Produces genome-wide coverage output in BEDGRAPH format (including uncovered).");
    }

    return res;
}

QString getNameByMode(GenomecovMode mode){
    QString res = "";
    if(mode == Histograms){
        res = QString("Histogram");

    }else if(mode == EachPos0Based){
        res = QString("Per-base (0-based)");

    }else if(mode == EachPos1Based){
        res = QString("Per-base (1-based)");

    }else if(mode == BedGraph){
        res = QString("BEDGRAPH");

    }else if(mode == BedGraphIncludeZero){
        res = QString("BEDGRAPH (including uncoveded)");
    }

    return res;
}

QString getParameterByMode(GenomecovMode mode){
    QString res = "";
    if(mode == Histograms){
        res = QString("");

    }else if(mode == EachPos0Based){
        res = QString("-dz");

    }else if(mode == EachPos1Based){
        res = QString("-d");

    }else if(mode == BedGraph){
        res = QString("-bg");

    }else if(mode == BedGraphIncludeZero){
        res = QString("-bga");
    }

    return res;
}

}
/************************************************************************/
/* GenomecovPrompter */
/************************************************************************/
QString GenomecovPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BaseNGSWorker::INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);
    QString descr = getDescriptionByMode( GenomecovMode(getParameter(MODE_ID).toInt()) );

    QString doc = tr("%1 from %2 with bedtool genomecov.").arg(descr).arg(producerName);
    return doc;
}

/************************************************************************/
/* GenomecovWorkerFactory */
/************************************************************************/
void GenomecovWorkerFactory::init() {
    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(BedtoolsSupport::GENOMES_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }
    Descriptor desc( ACTOR_ID, GenomecovWorker::tr("Genome Coverage"),
        GenomecovWorker::tr("Calculates genome coverage using bedtools genomecov.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(BaseNGSWorker::INPUT_PORT, GenomecovWorker::tr("Input File"),
            GenomecovWorker::tr("Set of files to NGS slop"));
        Descriptor outD(BaseNGSWorker::OUTPUT_PORT, GenomecovWorker::tr("Output File"),
            GenomecovWorker::tr("Output file"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("sb.input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("sb.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor outDir(BaseNGSWorker::OUT_MODE_ID, GenomecovWorker::tr("Output folder"),
            GenomecovWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(BaseNGSWorker::CUSTOM_DIR_ID, GenomecovWorker::tr("Custom folder"),
            GenomecovWorker::tr("Select the custom output folder."));

        Descriptor outName(BaseNGSWorker::OUT_NAME_ID, GenomecovWorker::tr("Output file name"),
            GenomecovWorker::tr("A name of an output file. If default of empty value is provided the output name is the name of the first file with additional extension."));

        Descriptor genomeAttrDesc(GENOME_ID, GenomecovWorker::tr("Genome"),
            GenomecovWorker::tr("In order to prevent the extension of intervals beyond chromosome boundaries, bedtools slop requires a genome file defining the length of each chromosome or contig. The format of the file is: <chromName><TAB><chromSize>. (-g)"));

        Descriptor mAttr(MODE_ID, GenomecovWorker::tr("Report mode"),
            GenomecovWorker::tr("<b>%1 (%2)</b> - %3 \n"
                                "<b>%4 (%5)</b> - %6 \n"
                                "<b>%7 (%8)</b> - %9 \n"
                                "<b>%10 (%11)</b> - %12 \n"
                                "<b>%13 (%14)</b> - %15 \n")
                         .arg(getNameByMode(Histograms)).arg(getParameterByMode(Histograms)).arg(getDescriptionByMode(Histograms))
                         .arg(getNameByMode(EachPos0Based)).arg(getParameterByMode(EachPos0Based)).arg(getDescriptionByMode(EachPos0Based))
                         .arg(getNameByMode(EachPos1Based)).arg(getParameterByMode(EachPos1Based)).arg(getDescriptionByMode(EachPos1Based))
                         .arg(getNameByMode(BedGraph)).arg(getParameterByMode(BedGraph)).arg(getDescriptionByMode(BedGraph))
                         .arg(getNameByMode(BedGraphIncludeZero)).arg(getParameterByMode(BedGraphIncludeZero)).arg(getDescriptionByMode(BedGraphIncludeZero))
                                                            );

        Descriptor splitAttrDesc(SPLIT_ID, GenomecovWorker::tr("Split"),
            GenomecovWorker::tr("Treat “split” BAM or BED12 entries as distinct BED intervals when computing coverage. For BAM files, this uses the CIGAR “N” and “D” operations to infer the blocks for computing coverage. For BED12 files, this uses the BlockCount, BlockStarts, and BlockEnds fields (i.e., columns 10,11,12). (-split)"));

        Descriptor strandAttrDesc(STRAND_ID, GenomecovWorker::tr("Strand"),
            GenomecovWorker::tr("Calculate coverage of intervals from a specific strand. With BED files, requires at least 6 columns (strand is column 6). (-strand)"));

        Descriptor prime5AttrDesc(PRIME5_ID, GenomecovWorker::tr("5 prime"),
            GenomecovWorker::tr("Calculate coverage of 5’ positions (instead of entire interval). (-5)"));

        Descriptor prime3AttrDesc(PRIME3_ID, GenomecovWorker::tr("3 prime"),
            GenomecovWorker::tr("Calculate coverage of 3’ positions (instead of entire interval). (-3)"));

        Descriptor maxAttrDesc(MAX_ID, GenomecovWorker::tr("Max"),
            GenomecovWorker::tr("Combine all positions with a depth >= max into a single bin in the histogram. (-max)"));

        Descriptor scaleAttrDesc(SCALE_ID, GenomecovWorker::tr("Scale"),
            GenomecovWorker::tr("Scale the coverage by a constant factor.Each coverage value is multiplied by this factor before being reported. Useful for normalizing coverage by, e.g., reads per million (RPM). Default is 1.0; i.e., unscaled. (-scale)"));

        Descriptor tracklineAttrDesc(TRACKLINE_ID, GenomecovWorker::tr("Trackline"),
            GenomecovWorker::tr("Adds a UCSC/Genome-Browser track line definition in the first line of the output. (-trackline)"));

        Descriptor trackoptsAttrDesc(TRACKOPTS_ID, GenomecovWorker::tr("Trackopts"),
            GenomecovWorker::tr("Writes additional track line definition parameters in the first line. (-trackopts)"));

        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(BaseNGSWorker::OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;
        a << new Attribute( outName, BaseTypes::STRING_TYPE(), false, QVariant(BaseNGSWorker::DEFAULT_NAME));

        Attribute* genomeAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
            }else{
                genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true);
            }
        }else{
            genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true);
        }
        a << genomeAttr;

        a << new Attribute( mAttr, BaseTypes::NUM_TYPE(), false, QVariant(Histograms));
        a << new Attribute( splitAttrDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( strandAttrDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( prime5AttrDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( prime3AttrDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( maxAttrDesc, BaseTypes::NUM_TYPE(), false, QVariant(INT_MAX));
        a << new Attribute( scaleAttrDesc, BaseTypes::NUM_TYPE(), false, QVariant(1.0));
        a << new Attribute( tracklineAttrDesc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( trackoptsAttrDesc, BaseTypes::STRING_TYPE(), false, QVariant(""));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap directoryMap;
        QString fileDir = GenomecovWorker::tr("Input file");
        QString workflowDir = GenomecovWorker::tr("Workflow");
        QString customD = GenomecovWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[BaseNGSWorker::OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[BaseNGSWorker::CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);

        QVariantMap vm;
        if (dataPath){
            vm = dataPath->getDataItemsVariantMap();
        }
        delegates[GENOME_ID] = new ComboBoxWithUrlsDelegate(vm);

        QVariantMap modes;
        modes[getNameByMode(Histograms)] = Histograms;
        modes[getNameByMode(EachPos0Based)] = EachPos0Based;
        modes[getNameByMode(EachPos1Based)] = EachPos1Based;
        modes[getNameByMode(BedGraph)] = BedGraph;
        modes[getNameByMode(BedGraphIncludeZero)] = BedGraphIncludeZero;
        delegates[MODE_ID] = new ComboBoxDelegate(modes);

        QVariantMap lenMap; lenMap["minimum"] = QVariant(0); lenMap["maximum"] = QVariant(INT_MAX);
        delegates[MAX_ID] = new SpinBoxDelegate(lenMap);

        QVariantMap lenFMap; lenFMap["minimum"] = QVariant(0); lenFMap["maximum"] = QVariant(1);
        delegates[SCALE_ID] = new DoubleSpinBoxDelegate(lenFMap);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new GenomecovPrompter());
    proto->addExternalTool(BedtoolsSupport::ET_BEDTOOLS_ID);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new GenomecovWorkerFactory());
}

/************************************************************************/
/* GenomecovWorker */
/************************************************************************/
GenomecovWorker::GenomecovWorker(Actor *a)
:BaseNGSWorker(a)
{

}

QVariantMap GenomecovWorker::getCustomParameters() const{
    QVariantMap res;
    const QString genomePath = getValue<QString>(GENOME_ID);
    if(!genomePath.isEmpty()){
        res["-g"] = genomePath;
    }
    const QString mode = getParameterByMode( GenomecovMode(getValue<int>(MODE_ID)));
    if(!mode.isEmpty()){
        res[mode] = "";
    }

    if(getValue<bool>(SPLIT_ID)){
        res["-split"] = "";
    }
    if(getValue<bool>(STRAND_ID)){
        res["-strand"] = "";
    }
    if(getValue<bool>(PRIME5_ID)){
        res["-5"] = "";
    }
    if(getValue<bool>(PRIME3_ID)){
        res["-3"] = "";
    }
    if(getValue<bool>(TRACKLINE_ID)){
        res["-trackline"] = "";
        const QString trackopts = getValue<QString>(TRACKOPTS_ID);
        if(!trackopts.isEmpty()){
            res["-trackopts"] = trackopts;
        }
    }

    const int max = getValue<int>(MAX_ID);
    if(max != INT_MAX){
        res["-max"] = max;
    }
    const float scale = getValue<float>(SCALE_ID);
    if(scale != 1.0){
        res["-scale"] = scale;
    }
    return res;
}

QString GenomecovWorker::getDefaultFileName() const{
    return ".gc";
}

Task *GenomecovWorker::getTask(const BaseNGSSetting &settings) const {
    return new GenomecovTask(settings);
}

//////////////////////////////////////////////////////
//GenomecovTask
GenomecovTask::GenomecovTask(const BaseNGSSetting &settings)
    :BaseNGSTask(settings){
    GCOUNTER(cvar, tvar, "NGS:GenomeCovTask");
}

void GenomecovTask::prepareStep(){
    Task* etTask = getExternalToolTask(BedtoolsSupport::ET_BEDTOOLS_ID);
    CHECK(etTask != NULL, );

    addSubTask(etTask);
}

QStringList GenomecovTask::getParameters(U2OpStatus &os){
    QStringList res;
    res << "genomecov";

    const QString detectedFormat = FileAndDirectoryUtils::detectFormat(settings.inputUrl);
    if(detectedFormat.isEmpty()){
        os.setError(tr("Unknown file format: ") + settings.inputUrl);
        return res;
    }

    if(detectedFormat == BaseDocumentFormats::BAM){
        res << "-ibam";
        res << settings.inputUrl;
    }else{
        res << "-i";
        res << settings.inputUrl;

        if(!settings.customParameters.contains("-g")){
            os.setError("No genome file");
            return res;
        }else{
            res << "-g";
            res << settings.customParameters["-g"].toString();
        }
    }

    if(settings.customParameters.contains("-d")){
        res << "-d";
    }

    if(settings.customParameters.contains("-dz")){
        res << "-dz";
    }

    if(settings.customParameters.contains("-bg")){
        res << "-bg";
    }

    if(settings.customParameters.contains("-bga")){
        res << "-bga";
    }

    if(settings.customParameters.contains("-split")){
        res << "-split";
    }

    if(settings.customParameters.contains("-strand")){
        res << "-strand";
    }

    if(settings.customParameters.contains("-5")){
        res << "-5";
    }

    if(settings.customParameters.contains("-3")){
        res << "-3";
    }

    if(settings.customParameters.contains("-trackline")){
        res << "-trackline";
        if(settings.customParameters.contains("-trackopts")){
            res << "-trackopts";
            res << settings.customParameters["-trackopts"].toString();
        }
    }

    if(settings.customParameters.contains("-max")){
        res << "-max";
        res << settings.customParameters["-max"].toString();
    }

    if(settings.customParameters.contains("-scale")){
        res << "-scale";
        res << settings.customParameters["-scale"].toString();
    }
    return res;
}


/************************************************************************/
/* IntersectAnnotationsWorker */
/************************************************************************/

const QString BedtoolsIntersectWorkerFactory::ACTOR_ID("intersect-annotations");

const static QString IN_PORT_A_ID("input-annotations-a");
const static QString IN_PORT_B_ID("input-annotations-b");
const static QString OUT_PORT_ID("output-intersect-annotations");

const static QString MIN_OVERLAP("minimum-overlap");
const static QString REPORT("report");
const static QString UNIQUE("unique");

BedtoolsIntersectWorker::BedtoolsIntersectWorker(Actor *a)
    : BaseWorker(a, false),
      inputA(NULL),
      inputB(NULL),
      output(NULL)
{
}

void BedtoolsIntersectWorker::init() {
    inputA = ports.value(IN_PORT_A_ID);
    inputB = ports.value(IN_PORT_B_ID);
    output = ports.value(OUT_PORT_ID);
}

Task* BedtoolsIntersectWorker::tick() {
    storeMessages(inputA, storeA);
    storeMessages(inputB, storeB);

    if (inputA->isEnded() && inputB->isEnded()) {
        return createTask();
    }

    return NULL;
}

bool BedtoolsIntersectWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    int hasA = inputA->hasMessage();
    bool endedA = inputA->isEnded();

    int hasB = inputB->hasMessage();
    bool endedB = inputB->isEnded();

    return hasA || hasB || (endedA && endedB);
}

void BedtoolsIntersectWorker::sl_taskFinished(Task *task) {
    if (task->isCanceled() || task->hasError()) {
        return;
    }
    BedtoolsIntersectAnnotationsByEntityTask* intersectTask = qobject_cast<BedtoolsIntersectAnnotationsByEntityTask*>(task);
    if (intersectTask == NULL) {
        return;
    }
    setDone();

    QList<GObject*> objList = intersectTask->getResult();
    CHECK_EXT(!objList.isEmpty(), output->setEnded(), );

    foreach(GObject* gObj, objList) {
        AnnotationTableObject* obj = qobject_cast<AnnotationTableObject*>(gObj);
        CHECK_EXT(obj != NULL, output->setEnded(), );
        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(obj);
        output->put( Message( BaseTypes::ANNOTATION_TABLE_TYPE(),
                              qVariantFromValue<SharedDbiDataHandler>( tableId ) ) );
    }

    output->setEnded();
}

Task* BedtoolsIntersectWorker::createTask() {
    BedtoolsIntersectByEntityRefSettings settings;

    settings.minOverlap = actor->getParameter(MIN_OVERLAP)->getAttributeValue<double>(context) / 100;
    settings.report = (BedtoolsIntersectSettings::Report) (actor->getParameter(REPORT)->getAttributeValue<int>(context));
    settings.unique = actor->getParameter(UNIQUE)->getAttributeValue<bool>(context);

    settings.entitiesA = getAnnotationsEntityRefFromMessages(storeA, IN_PORT_A_ID);
    settings.entitiesB = getAnnotationsEntityRefFromMessages(storeB, IN_PORT_B_ID);

    BedtoolsIntersectAnnotationsByEntityTask* t = new BedtoolsIntersectAnnotationsByEntityTask(settings);
    t->addListeners(createLogListeners());
    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
    return t;
}

QList<U2EntityRef> BedtoolsIntersectWorker::getAnnotationsEntityRefFromMessages(const QList<Message> &mList, const QString& portId) {
    QList<U2EntityRef> res;

    U2OpStatusImpl os;
    foreach (const Message& m, mList) {
        CHECK(!m.isEmpty(), res);
        U2EntityRef ref = getAnnotationsEntityRef(m, portId, os);
        res << ref;
    }
    return res;
}

U2EntityRef BedtoolsIntersectWorker::getAnnotationsEntityRef(const Message &m, const QString& portId, U2OpStatus &os) {
    const QVariantMap data = m.getData().toMap();
    CHECK_EXT(data.contains(portId), os.setError(tr("Data not found by %1 id").arg(portId)), U2EntityRef());

    const SharedDbiDataHandler dbiHandler = data[portId].value<SharedDbiDataHandler>();
    const AnnotationTableObject *obj = StorageUtils::getAnnotationTableObject(context->getDataStorage(), dbiHandler);
    CHECK_EXT(obj != NULL, os.setError(tr("Can not get annotation table object")), U2EntityRef());

    return obj->getEntityRef();
}

void BedtoolsIntersectWorker::storeMessages(IntegralBus *bus, QList<Message> &store) {
    while (bus->hasMessage()) {
        store << getMessageAndSetupScriptValues(bus);
    }
}

void BedtoolsIntersectWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    {
        Descriptor inDescA( IN_PORT_A_ID, BedtoolsIntersectWorker::tr("Annotations A"), BedtoolsIntersectWorker::tr("Annotations A"));
        QMap<Descriptor, DataTypePtr> inM_A;
        inM_A[inDescA] = BaseTypes::ANNOTATION_TABLE_TYPE();
        portDescs << new PortDescriptor( inDescA, DataTypePtr(new MapDataType("in.anns.a", inM_A)), /*input*/ true);

        Descriptor inDescB( IN_PORT_B_ID, BedtoolsIntersectWorker::tr("Annotations B"), BedtoolsIntersectWorker::tr("Annotations B"));
        QMap<Descriptor, DataTypePtr> inM_B;
        inM_B[inDescB] = BaseTypes::ANNOTATION_TABLE_TYPE();
        portDescs << new PortDescriptor( inDescB, DataTypePtr(new MapDataType("in.anns.b", inM_B)), /*input*/ true);

        Descriptor outDesc( OUT_PORT_ID, BedtoolsIntersectWorker::tr("Annotations"), BedtoolsIntersectWorker::tr("Result annotations"));
        QMap<Descriptor, DataTypePtr> outM;
        outM[outDesc] = BaseTypes::ANNOTATION_TABLE_TYPE();
        portDescs << new PortDescriptor( outDesc, DataTypePtr(new MapDataType("out.anns", outM)), /*intput*/ false);
    }

    QList<Attribute*> attribs;
    {
        Descriptor minOverlapDesc( MIN_OVERLAP,
                                   BedtoolsIntersectWorker::tr( "Minimum overlap"),
                                   BedtoolsIntersectWorker::tr( "Minimum overlap required as a fraction of an annotation from set A."
                                                                "<br/>By default, even 1 bp overlap between annotations from set A and set B is taken into account."
                                                                " Yet sometimes you may want to restrict reported overlaps to cases where the annotations in B overlaps"
                                                                " at least X% (e.g. 50%) of the A annotation. ") );

        Descriptor reportDesc ( REPORT,
                                BedtoolsIntersectWorker::tr("Result annotations"),
                                BedtoolsIntersectWorker::tr("Select one of the following:"
                                                            "<ul>"
                                                            "<li><i>Shared interval</i> to report intervals shared"
                                                            " between overlapped annotations from set A and set B."
                                                            "</li>"
                                                            "<li><i>Overlapped annotations from A</i> to report annotations"
                                                            " from set A that have an overlap with annotations from set B."
                                                            "</li>"
                                                            "<li><i>Non-overlapped annotations from A</i> to report annotations"
                                                            " from set A that have NO overlap with annotations from set B."
                                                            "</li></ul>"));
        Descriptor uniqueDesc ( UNIQUE,
                                BedtoolsIntersectWorker::tr( "Unique overlaps"),
                                BedtoolsIntersectWorker::tr( "If the parameter value is \"True\", write original A entry once if any overlaps found in B."
                                                             " In other words, just report the fact at least one overlap was found in B.<br/>"
                                                             "The minimum overlap number is ignored in this case.<br/><br/>"
                                                             "If the parameter value is \"False\", the A annotation is reported for every overlap found."));

        attribs << new Attribute( reportDesc, BaseTypes::NUM_TYPE(), /*required*/ false, QVariant(BedtoolsIntersectSettings::Report_OverlapedA));

        Attribute* uniqueAttr = new Attribute( uniqueDesc, BaseTypes::BOOL_TYPE(), /*required*/ false, QVariant(true));
        uniqueAttr->addRelation(new VisibilityRelation(REPORT, QVariantList() << BedtoolsIntersectSettings::Report_OverlapedA));
        attribs << uniqueAttr;

        Attribute* minOverlapAttr = new Attribute( minOverlapDesc, BaseTypes::NUM_TYPE(), /*required*/ false, QVariant(BedtoolsIntersectSettings::DEFAULT_MIN_OVERLAP * 100) );
        minOverlapAttr->addRelation(new VisibilityRelation(REPORT, QVariantList() << BedtoolsIntersectSettings::Report_Intervals
                                                           << BedtoolsIntersectSettings::Report_OverlapedA));
        minOverlapAttr->addRelation(new VisibilityRelation(UNIQUE, QVariantList() << false));
        attribs << minOverlapAttr;
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap spinMap;
        spinMap["minimum"] = QVariant(BedtoolsIntersectSettings::DEFAULT_MIN_OVERLAP * 100);
        spinMap["maximum"] = QVariant(100);
        spinMap["suffix"] = QVariant("%");
        spinMap["decimals"] = 7;
        delegates[MIN_OVERLAP] = new DoubleSpinBoxDelegate(spinMap);

        QVariantMap comboMap;
        comboMap["Shared interval"] = BedtoolsIntersectSettings::Report_Intervals;
        comboMap["Overlapped annotations from A"] = BedtoolsIntersectSettings::Report_OverlapedA;
        comboMap["Non-overlapped annotations from A"] = BedtoolsIntersectSettings::Report_NonOverlappedA;
        delegates[REPORT] = new ComboBoxDelegate(comboMap);

        delegates[UNIQUE] = new ComboBoxWithBoolsDelegate();
    }

    Descriptor desc( BedtoolsIntersectWorkerFactory::ACTOR_ID,
                     BedtoolsIntersectWorker::tr("Intersect Annotations"),
                     BedtoolsIntersectWorker::tr("Intersects two sets of annotations denoted as A and B.") );
    ActorPrototype * proto = new IntegralBusActorPrototype( desc, portDescs, attribs );
    proto->setPrompter( new BedtoolsIntersectPrompter() );
    proto->setEditor(new DelegateEditor(delegates));
    proto->addExternalTool(BedtoolsSupport::ET_BEDTOOLS_ID);

    WorkflowEnv::getProtoRegistry()->registerProto( BaseActorCategories::CATEGORY_BASIC(), proto );
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById( LocalDomainFactory::ID );
    localDomain->registerEntry( new BedtoolsIntersectWorkerFactory() );
}

QString BedtoolsIntersectPrompter::composeRichDoc() {
    QString a = getProducersOrUnset(IN_PORT_A_ID, IN_PORT_A_ID);
    QString b = getProducersOrUnset(IN_PORT_B_ID, IN_PORT_B_ID);

    QString res = QString(tr("Intersect annotations from <u>%1</u> (<b>set A</b>) with annotations from <u>%2</u> (<b>set B</b>). Report ")
                          .arg(a)
                          .arg(b));

    BedtoolsIntersectSettings::Report r = (BedtoolsIntersectSettings::Report)target->getParameter(REPORT)->getAttributePureValue().toInt();
    QString reportHyperlinkText;
    switch (r) {
    case BedtoolsIntersectSettings::Report_Intervals:
        reportHyperlinkText = "intervals shared between annotations from <b>set A</b> and <b>set B</b>.";
        break;
    case BedtoolsIntersectSettings::Report_OverlapedA:
        reportHyperlinkText = "overlapped annotations from <b>set A</b>.";
        break;
    case BedtoolsIntersectSettings::Report_NonOverlappedA:
        reportHyperlinkText = "non-overlapped annotations from <b>set A</b>.";
    }

    if (target->getParameter(UNIQUE)->getAttributePureValue().toBool() && r == BedtoolsIntersectSettings::Report_OverlapedA) {
        res.append(getHyperlink(UNIQUE, "<u>unique</u> "));
    }

    res.append(getHyperlink(REPORT, "<u>" + reportHyperlinkText +"</u>"));
    return res;
}

} //LocalWorkflow
} //U2
