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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/FastaFormat.h>
#include <U2Formats/FastqFormat.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/ActorValidator.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/PairedReadsPortValidator.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ClassificationFilterWorker.h"
#include "NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//ClassificationFilter

//const QString ClassificationFilterSettings::SPECIES("species");
//const QString ClassificationFilterSettings::GENUS("genus");
//const QString ClassificationFilterSettings::FAMILY("family");
//const QString ClassificationFilterSettings::ORDER("order");
//const QString ClassificationFilterSettings::CLASS("class");
//const QString ClassificationFilterSettings::PHYLUM("phylum");



QString ClassificationFilterPrompter::composeRichDoc() {
    return tr("Put input sequences that belong to the specified taxonomic group(s) to separate file(s).");
}

/************************************************************************/
/* ClassificationFilterValidator */
/************************************************************************/

bool ClassificationFilterValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &) const {
    const bool taxaListAttributeValid = validateTaxaListAttribute(actor, notificationList);
    const bool taxonomyTreeValid = validateTaxonomyTree(actor, notificationList);
    return taxaListAttributeValid && taxonomyTreeValid;
}

bool ClassificationFilterValidator::validateTaxaListAttribute(const Actor *actor, NotificationsList &notificationList) const {
    const bool saveUnspecificSequences = actor->getParameter(ClassificationFilterWorkerFactory::SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID)->getAttributeValueWithoutScript<bool>();

    const QStringList taxonsTokens = actor->getParameter(ClassificationFilterWorkerFactory::TAXONS)->getAttributeValueWithoutScript<QString>().split(";", QString::SkipEmptyParts);
    QSet<TaxID> taxons;
    foreach(const QString &idStr, taxonsTokens) {
        bool OK = true;
        TaxID id = idStr.toInt(&OK);
        if (OK) {
            taxons.insert(id);
        } else {
            notificationList << WorkflowNotification(tr("Invalid taxon ID: %1").arg(idStr), actor->getId());
            return false;
        }
    }

    if (!saveUnspecificSequences && taxons.isEmpty()) {
        notificationList << WorkflowNotification(tr("Set \"%1\" to \"True\" or select a taxon in \"%2\".")
            .arg(actor->getParameter(ClassificationFilterWorkerFactory::SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID)->getDisplayName())
            .arg(actor->getParameter(ClassificationFilterWorkerFactory::TAXONS)->getDisplayName()), actor->getId());
        return false;
    }

    return true;
}

bool ClassificationFilterValidator::validateTaxonomyTree(const Actor *actor, NotificationsList &notificationList) const {
    bool valid = true;
    if (!TaxonomyTree::getInstance()->isValid()) {
        notificationList << WorkflowNotification(tr("Taxonomy classification data from NCBI are not available."), actor->getId());
        valid = false;
    }
    return valid;
}

/************************************************************************/
/* ClassificationFilterWorkerFactory */
/************************************************************************/

const QString ClassificationFilterWorkerFactory::ACTOR_ID = "classification-filter";

const QString ClassificationFilterWorkerFactory::INPUT_PORT = "in";
const QString ClassificationFilterWorkerFactory::OUTPUT_PORT = "out";

// Slots should be the same as in GetReadsListWorkerFactory
const QString ClassificationFilterWorkerFactory::INPUT_SLOT = "reads-url1";
const QString ClassificationFilterWorkerFactory::PAIRED_INPUT_SLOT = "reads-url2";

const QString ClassificationFilterWorkerFactory::OUTPUT_SLOT = "reads-url1";
const QString ClassificationFilterWorkerFactory::PAIRED_OUTPUT_SLOT = "reads-url2";

const QString ClassificationFilterWorkerFactory::SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID = "save-unspecific-sequences";
const QString ClassificationFilterWorkerFactory::TAXONOMY_RANK = "taxonomy-rank";
const QString ClassificationFilterWorkerFactory::SEQUENCING_READS = "sequencing-reads";
const QString ClassificationFilterWorkerFactory::TAXONS = "tax-ids";

const QString ClassificationFilterWorkerFactory::SINGLE_END = "single-end";
const QString ClassificationFilterWorkerFactory::PAIRED_END = "paired-end";

void ClassificationFilterWorkerFactory::init() {

    Descriptor desc( ACTOR_ID, ClassificationFilterWorker::tr("Filter by Classification"),
        ClassificationFilterWorker::tr("The filter takes files with NGS reads or contigs, classified by one of the tools: "
                                       "Kraken, CLARK, DIAMOND, WEVOTE. For each input file it outputs a file with unspecific "
                                       "sequences (i.e. sequences not classified by the tools, taxID = 0) and/or one or several "
                                       "files with sequences that belong to specific taxonomic group(s).") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, ClassificationFilterWorker::tr("Input sequences and tax IDs"),
                       ClassificationFilterWorker::tr("The following input should be provided: <ul>"
                                                      "<li>URL(s) to FASTQ or FASTA file(s)."
                                                      "<li>Corresponding taxonomy classification of sequences in the files."
                                                      "</ul>"
                                                      "To process single-end reads or contigs, pass the URL(s) to  the \"Input URL 1\" slot.<br><br>"
                                                      "To process paired-end reads, pass the URL(s) to files with the \"left\" and \"right\" reads to the \"Input URL 1\" and \"Input URL 2\" slots correspondingly.<br><br>"
                                                      "The taxonomy classification data are received by one of the classification tools (Kraken, CLARK, or DIAMOND) and should correspond to the input files."
                ));
//        Descriptor inD2(PAIRED_INPUT_PORT, ClassificationFilterWorker::tr("Input sequences 2"), ClassificationFilterWorker::tr("URL(s) to FASTQ or FASTA file(s) should be provided."
//                    "<br>The port is used, if paired-end sequencing was done. The input files should contain the \"right\" reads (see \"Input data\" parameter of the element)."));
        Descriptor outD(OUTPUT_PORT, ClassificationFilterWorker::tr("Output File(s)"),
                        ClassificationFilterWorker::tr("The port outputs URLs to files with NGS reads, classified by taxon IDs: one file per each specified taxon ID per each input file (or pair of files in case of PE reads).\n\n"
                                                       "Either one (for SE reads or contigs) or two (for PE reads) output slots are used depending on the input data.\n\n"
                                                       "See also the \"Input data\" parameter of the element."));

//        Descriptor outD2(OUTPUT_PORT2, ClassificationFilterWorker::tr("Output sequences 2"),
//                        ClassificationFilterWorker::tr("URL(s) to the filtered FASTQ or FASTA file(s). The files contain \"right\" reads in case of paired-end sequencing (see \"Input data\" parameter of the element)."));

        Descriptor inSlot1Descriptor(INPUT_SLOT, ClassificationFilterWorker::tr("Input URL 1"), ClassificationFilterWorker::tr("Input URL 1."));
        Descriptor inSlot2Descriptor(PAIRED_INPUT_SLOT, ClassificationFilterWorker::tr("Input URL 2"), ClassificationFilterWorker::tr("Input URL 2."));

        QMap<Descriptor, DataTypePtr> inM;
        inM[inSlot1Descriptor] = BaseTypes::STRING_TYPE();
        inM[inSlot2Descriptor] = BaseTypes::STRING_TYPE();
        inM[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("filter.input", inM)), true);

        Descriptor outSlot1Descriptor(OUTPUT_SLOT, ClassificationFilterWorker::tr("Output URL 1"), ClassificationFilterWorker::tr("Output URL 1."));
        Descriptor outSlot2Descriptor(PAIRED_OUTPUT_SLOT, ClassificationFilterWorker::tr("Output URL 2"), ClassificationFilterWorker::tr("Output URL 2."));

        QMap<Descriptor, DataTypePtr> outM;
        //outM[Descriptor(OUTPUT_SLOT, ClassificationFilterWorker::tr("Output URL(s)"), ClassificationFilterWorker::tr("Output URL(s)"))] = BaseTypes::STRING_TYPE();
        outM[outSlot1Descriptor] = BaseTypes::STRING_TYPE();
        outM[outSlot2Descriptor] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("filter.output-url", outM)), false, true);
        //p << new PortDescriptor(outD2, DataTypePtr(new MapDataType("filter.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor saveUnspecificSequencesDescription(SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID, ClassificationFilterWorker::tr("Save unspecific sequences"),
            ClassificationFilterWorker::tr("Select \"True\" to put all unspecific input sequences (i. e. sequences with tax ID = 0) into a separate file.<br>"
                                           "Select \"False\" to skip unspecific sequences. At least one specific taxon should be selected in the \"Save sequences with taxID\" parameter in this case."));

        Descriptor sequencingReadsDesc(SEQUENCING_READS, ClassificationFilterWorker::tr("Input data"),
                                             ClassificationFilterWorker::tr("To filter single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to \"SE reads or contigs\". Use the \"Input URL 1\" slot of the input port.<br><br>"
                                                                            "To filter paired-end (PE) reads, set the value to \"PE reads\". Use the \"\"Input URL 1\" and \"Input URL 2\" slots of the input port to input the NGS reads data.<br><br>"
                                                                            "Also, input the classification data, received from Kraken, CLARK, or DIAMOND, to the \"Taxonomy classification data\" input slot.<br><br>"
                                                                            "Either one or two slots of the output port are used depending on the input data."));

        Descriptor saveSequencesWithTaxidDescription(TAXONS, ClassificationFilterWorker::tr("Save sequences with taxID"),
            ClassificationFilterWorker::tr("Select a taxID to put all sequences that belong to this taxonomic group "
                                           "(i. e. the specified taxID and all children in the taxonomy tree) into a separate file."));

        Attribute *sequencingReadsAttribute = new Attribute(sequencingReadsDesc, BaseTypes::STRING_TYPE(), false, SINGLE_END);

        sequencingReadsAttribute->addSlotRelation(new SlotRelationDescriptor(INPUT_PORT, PAIRED_INPUT_SLOT, QVariantList() << PAIRED_END));
        sequencingReadsAttribute->addSlotRelation(new SlotRelationDescriptor(OUTPUT_PORT, PAIRED_OUTPUT_SLOT, QVariantList() << PAIRED_END));

        a << sequencingReadsAttribute;
        a << new Attribute(saveUnspecificSequencesDescription, BaseTypes::BOOL_TYPE(), false, true);
        a << new Attribute(saveSequencesWithTaxidDescription, BaseTypes::STRING_TYPE());
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap sequencingReadsMap;
        sequencingReadsMap[ClassificationFilterWorker::tr("SE reads or contigs")] = SINGLE_END;
        sequencingReadsMap[ClassificationFilterWorker::tr("PE reads")] = PAIRED_END;

        delegates[SEQUENCING_READS] = new ComboBoxDelegate(sequencingReadsMap);
        delegates[SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID] = new ComboBoxWithBoolsDelegate();
        delegates[TAXONS] = new TaxonomyDelegate();
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ClassificationFilterPrompter());
    proto->setValidator(new ClassificationFilterValidator());
    proto->setPortValidator(INPUT_PORT, new PairedReadsPortValidator(INPUT_SLOT, PAIRED_INPUT_SLOT));

    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ClassificationFilterWorkerFactory());
}

void ClassificationFilterWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}


/************************************************************************/
/* ClassificationFilterWorker */
/************************************************************************/
ClassificationFilterWorker::ClassificationFilterWorker(Actor *a)
:BaseWorker(a, false), input(NULL), /*pairedOutput(NULL),*/ output(NULL)
{
}

void ClassificationFilterWorker::init() {
    input = ports.value(ClassificationFilterWorkerFactory::INPUT_PORT);
//    pairedOutput = ports.value(OUTPUT_PORT2);
    output = ports.value(ClassificationFilterWorkerFactory::OUTPUT_PORT);

    SAFE_POINT(NULL != input, QString("Port with id '%1' is NULL").arg(ClassificationFilterWorkerFactory::INPUT_PORT), );
//    SAFE_POINT(NULL != pairedOutput, QString("Port with id '%1' is NULL").arg(OUTPUT_PORT2), );
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(ClassificationFilterWorkerFactory::OUTPUT_PORT), );

    output->addComplement(input);
    input->addComplement(output);
    //FIXME pairedOutput looses complement context

    cfg.paired = (getValue<QString>(ClassificationFilterWorkerFactory::SEQUENCING_READS) == ClassificationFilterWorkerFactory::PAIRED_END);
//    cfg.rank = getValue<QString>(TAXONOMY_RANK);

    cfg.saveUnspecificSequences = getValue<bool>(ClassificationFilterWorkerFactory::SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID);

    QStringList taxons = getValue<QString>(ClassificationFilterWorkerFactory::TAXONS).split(";", QString::SkipEmptyParts);
    foreach (const QString &idStr, taxons) {
        bool OK = true;
        TaxID id = idStr.toInt(&OK);
        if (OK) {
            cfg.taxons.insert(id);
        } else {
            reportError(tr("Invalid taxon ID: %1").arg(idStr));
            return;
        }
    }
    if (!cfg.saveUnspecificSequences && cfg.taxons.isEmpty()) {
        reportError(tr("Set \"%1\" to \"True\" or select a taxon in \"%2\".")
            .arg(getActor()->getParameter(ClassificationFilterWorkerFactory::SAVE_UNSPECIFIC_SEQUENCES_ATTR_ID)->getDisplayName())
            .arg(getActor()->getParameter(ClassificationFilterWorkerFactory::TAXONS)->getDisplayName()));
        return;
    }
    algoLog.trace(QString("Filter taxa num: %1").arg(cfg.taxons.size()));
    //TODO validate ids relations

    cfg.workingDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
}

Task * ClassificationFilterWorker::tick() {
    if (input->hasMessage()) {
        const Message message = getMessageAndSetupScriptValues(input);

        QVariantMap data = message.getData().toMap();
        QString readsUrl = data[ClassificationFilterWorkerFactory::INPUT_SLOT].toString();
        QString pairedReadsUrl = data[ClassificationFilterWorkerFactory::PAIRED_INPUT_SLOT].toString();
        TaxonomyClassificationResult tax = data[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT().getId()/*INPUT_SLOT_CLASSIFICATION*/].value<U2::LocalWorkflow::TaxonomyClassificationResult>();

        if (cfg.paired && pairedReadsUrl.isEmpty()) {
            return new FailTask(tr("No paired read provided"));
        }

        ClassificationFilterTask *task = new ClassificationFilterTask(cfg, readsUrl, pairedReadsUrl, tax);
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    }

    if (input->isEnded()) {
        setDone();
        algoLog.info("Filter worker is done as input has ended");
        output->setEnded();
//        pairedOutput->setEnded();
    }

    return NULL;
}

void ClassificationFilterWorker::sl_taskFinished(Task *t) {
    ClassificationFilterTask *task = qobject_cast<ClassificationFilterTask *>(t);
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (!task->isFinished() || task->hasError() || task->isCanceled()) {
        return;
    }
    if (cfg.paired && task->getSeUrls().size() != task->getPeUrls().size()) {
        reportError("Internal Error, mis-paired read files produced!!!");
    }

    QStringListIterator it1(task->getSeUrls());
    QStringListIterator it2(task->getPeUrls());
    while (it1.hasNext()) {
        {
            QVariantMap m;
            const QString url = it1.next();
            algoLog.trace(QString("Classification filter produced SE: %1").arg(url));
            m[ClassificationFilterWorkerFactory::INPUT_SLOT] = url;
//            QString datasetName = "Dataset 1"; //TODO use input url or dataset name???
//            m[BaseSlots::DATASET_SLOT().getId()] = datasetName;
//            MessageMetadata metadata(url, datasetName);
//            context->getMetadataStorage().put(metadata);
            context->getMonitor()->addOutputFile(url, getActor()->getId());
            if (cfg.paired && it2.hasNext()) {
//                QVariantMap m;
                const QString url = it2.next();
                QString datasetName = "Dataset 1"; //TODO use input url or dataset name???
                m[ClassificationFilterWorkerFactory::PAIRED_INPUT_SLOT] = url;
//                m[BaseSlots::DATASET_SLOT().getId()] = datasetName;
//                MessageMetadata metadata(url, datasetName);
//                context->getMetadataStorage().put(metadata);
//                pairedOutput->put(Message(output->getBusType(), m, metadata.getId()));
                context->getMonitor()->addOutputFile(url, getActor()->getId());
                algoLog.trace(QString("Classification filter produced PE: %1").arg(url));
            }
            output->put(Message(output->getBusType(), m/*, metadata.getId()*/));
        }
    }

    const QMap<QString, TaxID> &found = task->getFoundIDs();
    foreach (QString inputFile, found.uniqueKeys()) {
        QList<TaxID> ids = found.values(inputFile);
        if (cfg.taxons.size() != ids.size()) {
            foreach (const TaxID &id, cfg.taxons) {
                if (!ids.contains(id)) {
                    QString taxName = TaxonomyTree::getInstance()->getName(id);
                    QString msg;
                    if (cfg.paired) {
                        QStringList pair = inputFile.split(";");
                        msg = tr("There are no sequences that belong to taxon ‘%1 (ID: %2)’"
                                 " in the input ‘%3’ and ‘%4’ files.").arg(taxName).arg(id).arg(pair.first()).arg(pair.last());
                    } else {
                        msg = tr("There are no sequences that belong to taxon ‘%1 (ID: %2)’ "
                                 "in the input ‘%3’ file.").arg(taxName).arg(id).arg(inputFile);
                    }
                    algoLog.info(msg);
                    monitor()->addInfo(msg, getActorId(), WorkflowNotification::U2_INFO);
                }
            }
        }
    }

    if (task->hasMissed()) {
        QString dashboardMsg = tr("Some input sequences have been skipped, as there was no classification data for them. See log for details.");
        monitor()->addInfo(dashboardMsg, getActorId(), WorkflowNotification::U2_WARNING);
    }
}

ClassificationFilterTask::ClassificationFilterTask(const ClassificationFilterSettings &settings, const QString &readsUrl, const QString &pairedReadsUrl, const TaxonomyClassificationResult &report)
    : Task(tr("Filter classified reads"), TaskFlag_None),
      cfg(settings), readsUrl(readsUrl), pairedReadsUrl(pairedReadsUrl), report(report), missed(false)
{
    GCOUNTER(cvar, tvar, "ClassificationFilterTask");

    SAFE_POINT_EXT(!readsUrl.isEmpty(), setError("Reads URL is empty"), );
    SAFE_POINT_EXT(!cfg.paired || !pairedReadsUrl.isEmpty(), setError("Classification report URL is empty"), );
    SAFE_POINT_EXT(cfg.saveUnspecificSequences || !cfg.taxons.isEmpty(), setError("Taxon filter is empty"), );
    SAFE_POINT_EXT(!settings.workingDir.isEmpty(), setError("Working dir is not specified"), );
}

static QString composeOutputName(GUrl input, QString suffix, QString dir) {
    QString ext = input.fileName();
    QString prefix = GUrlUtils::getUncompressedCompleteBaseName(ext);
    ext = ext.right(ext.size() - prefix.size());
    return QString("%1/%2_taxid%3%4").arg(dir).arg(prefix).arg(suffix).arg(ext);
}

void ClassificationFilterTask::run()
{

    StreamSequenceReader reader, pairedReader;
    if (!reader.init(QStringList(readsUrl))){
        stateInfo.setError(reader.getErrorMessage());
        return;
    }
    if (cfg.paired && !pairedReader.init(QStringList(pairedReadsUrl))){
        stateInfo.setError(pairedReader.getErrorMessage());
        return;
    }

    algoLog.trace(QString("Going to filter file: %1").arg(readsUrl));

    dir = GUrlUtils::createDirectory(cfg.workingDir + "Filter", "_", stateInfo);
    CHECK_OP(stateInfo, );

    while(reader.hasNext()) {
        CHECK_OP(stateInfo, );

        DNASequence *seq = reader.getNextSequenceObject(), *pairedSeq;
        algoLog.trace(QString("Got seq: %1").arg(seq->getName()));
        if (cfg.paired) {
            if (!pairedReader.hasNext()) {
                stateInfo.setError(tr("Missing pair read for '%1', input files: %2 and %3.").arg(seq->getName()).arg(readsUrl).arg(pairedReadsUrl));
                return;
            }
            pairedSeq = pairedReader.getNextSequenceObject();
//            if (seq->getName() != pairedSeq->getName()) {
//                stateInfo.setError(tr("Missing pair read for '%1', input files: %2 and %3.").arg(seq->getName()).arg(readsUrl).arg(pairedReadsUrl));
//                return;
//            }
        }
        QString fName = reader.getIO()->getURL().fileName();
        if (cfg.paired) {
            fName += ";" + pairedReader.getIO()->getURL().fileName();
        }
        QString suffix = filter(seq, fName);
        algoLog.trace(QString("Filter result: %1").arg(suffix));
        if (!suffix.isEmpty()) {
            QString name = composeOutputName(reader.getIO()->getURL(), suffix, dir);
            if (write(seq, name, reader) && !seUrls.contains(name)) {
                seUrls << name;
            }
            if (cfg.paired) {
                QString peName = composeOutputName(pairedReader.getIO()->getURL(), suffix, dir);
                if (write(pairedSeq, peName, pairedReader)&& !peUrls.contains(peName)) {
                    peUrls << peName;
                }
            }
        }
    }
}

QString ClassificationFilterTask::filter(DNASequence *seq, QString inputName)
{
    QString seqName = seq->getName().split(QRegExp("\\s+")).first();
    TaxID id = report.value(seqName, TaxonomyTree::UNDEFINED_ID);
    if (id == TaxonomyTree::UNDEFINED_ID) {
            algoLog.info(tr("Warning: classification result for the ‘%1’ (from '%2') hasn’t been found.").arg(seq->getName()).arg(inputName));
            missed = true;
    } else if (id != TaxonomyTree::UNCLASSIFIED_ID) {
        id = TaxonomyTree::getInstance()->match(id, cfg.taxons);
        if (id != TaxonomyTree::UNDEFINED_ID) {
            foundIDs.insertMulti(inputName, id);
            QString taxName = TaxonomyTree::getInstance()->getName(id);
            return QString("%1_%2").arg(id).arg(GUrlUtils::fixFileName(taxName));
        }
        foundIDs.insertMulti(inputName, 0); // save anyway to track inputs for dashboard
    } else {
        // Unclassified
        foundIDs.insertMulti(inputName, 0); // save anyway to track inputs for dashboard
        if (cfg.saveUnspecificSequences) {
            return QString("0_unclassified");
        }
    }
    return QString();
}

bool ClassificationFilterTask::write(DNASequence *seq, QString fileName, const StreamSequenceReader &original)
{
    DocumentFormat *format = original.getFormat();
    if (format->getFormatId() != BaseDocumentFormats::FASTA && format->getFormatId() != BaseDocumentFormats::FASTQ) {
        setError(tr("Format %1 is not supported by this task.").arg(format->getFormatName()));
        return false;
    }

    IOAdapter* io = original.getIO()->getFactory()->createIOAdapter();
    if (!io->open(fileName, IOAdapterMode_Append)) {
        algoLog.error(tr("Failed writing sequence to ‘%1’.").arg(fileName));
        return false;
    }
    if (format->getFormatId() == BaseDocumentFormats::FASTA) {
        FastaFormat *fasta = qobject_cast<FastaFormat*>(format);
        fasta->storeSequence(*seq, io, stateInfo);
        //if (stateInfo.hasError())
    } else if (format->getFormatId() == BaseDocumentFormats::FASTQ) {
        QString err = tr("Failed writing sequence to ‘%1’.").arg(io->getURL().getURLString());
        FastqFormat::writeEntry(seq->getName(), *seq, io, err, stateInfo, false);
    }
    io->close();
    delete io;
    return true;
}

ClassificationFilterSettings::ClassificationFilterSettings()
    : /*rank(ClassificationFilterSettings::SPECIES),*/ saveUnspecificSequences(false), paired(false)
{
}

} //LocalWorkflow
} //U2
