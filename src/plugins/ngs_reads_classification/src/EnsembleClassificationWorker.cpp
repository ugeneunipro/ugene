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
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/ActorValidator.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "EnsembleClassificationWorker.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationUtils.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//EnsembleClassification
const QString EnsembleClassificationWorkerFactory::ACTOR_ID("ensemble-classification");

static const QString INPUT_PORT1("tax-data1");
static const QString INPUT_PORT2("tax-data2");
static const QString INPUT_PORT3("tax-data3");
static const QString INPUT_SLOT1("in");
static const QString INPUT_SLOT2("in");
static const QString INPUT_SLOT3("in");

static const QString OUTPUT_PORT("out");
static const QString OUTPUT_SLOT = BaseSlots::URL_SLOT().getId();

static const QString NUMBER_OF_TOOLS("number-tools");
static const QString OUT_FILE("out-file");

static const QString DEFAULT_OUT_FILE_BASE_NAME("ensemble");
static const QString DEFAULT_OUT_FILE_EXTENSION("csv");
static const QString DEFAULT_OUT_FILE_NAME(DEFAULT_OUT_FILE_BASE_NAME + "." + DEFAULT_OUT_FILE_EXTENSION);

QString EnsembleClassificationPrompter::composeRichDoc() {
    const QString outFile = getHyperlink(OUT_FILE, getURL(OUT_FILE, (bool*)0, "", DEFAULT_OUT_FILE_NAME));
    return tr("Ensemble classification data from other elements into %1").arg(outFile);
}

/************************************************************************/
/* EnsembleClassificationWorkerFactory */
/************************************************************************/
void EnsembleClassificationWorkerFactory::init() {

    Descriptor desc(ACTOR_ID, EnsembleClassificationWorker::tr("Ensemble Classification Data"),
        EnsembleClassificationWorker::tr("The element ensembles data, produced by classification tools "
                                         "(Kraken, CLARK, DIAMOND), into a single file in CSV format. "
                                         "This file can be used as input for the WEVOTE classifier.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inputPortDescriptor1(INPUT_PORT1, EnsembleClassificationWorker::tr("Input taxonomy data 1"),
                       EnsembleClassificationWorker::tr("An input slot for taxonomy classification data."));

        Descriptor inputPortDescriptor2(INPUT_PORT2, EnsembleClassificationWorker::tr("Input taxonomy data 2"),
                       EnsembleClassificationWorker::tr("An input slot for taxonomy classification data."));

        Descriptor inputPortDescriptor3(INPUT_PORT3, EnsembleClassificationWorker::tr("Input taxonomy data 3"),
                       EnsembleClassificationWorker::tr("An input slot for taxonomy classification data."));

        Descriptor outD(OUTPUT_PORT, EnsembleClassificationWorker::tr("Ensembled classification"),
                        EnsembleClassificationWorker::tr("URL to the CSV file with ensembled classification data."));


        Descriptor inSlot1(INPUT_SLOT1, EnsembleClassificationWorker::tr("Input tax data 1"), EnsembleClassificationWorker::tr("Input tax data 1."));
        Descriptor inSlot2(INPUT_SLOT2, EnsembleClassificationWorker::tr("Input tax data 2"), EnsembleClassificationWorker::tr("Input tax data 2."));
        Descriptor inSlot3(INPUT_SLOT3, EnsembleClassificationWorker::tr("Input tax data 3"), EnsembleClassificationWorker::tr("Input tax data 3."));

        QMap<Descriptor, DataTypePtr> inputMap1;
        inputMap1[inSlot1] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();

        QMap<Descriptor, DataTypePtr> inputMap2;
        inputMap2[inSlot2] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();

        QMap<Descriptor, DataTypePtr> inputMap3;
        inputMap3[inSlot3] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();

        p << new PortDescriptor(inputPortDescriptor1, DataTypePtr(new MapDataType("ensemble.input", inputMap1)), true);
        p << new PortDescriptor(inputPortDescriptor2, DataTypePtr(new MapDataType("ensemble.input", inputMap2)), true);
        p << new PortDescriptor(inputPortDescriptor3, DataTypePtr(new MapDataType("ensemble.input", inputMap3)), true);

        Descriptor outSlot(OUTPUT_SLOT, EnsembleClassificationWorker::tr("Output URL"), EnsembleClassificationWorker::tr("Output URL."));

        QMap<Descriptor, DataTypePtr> outM;
        outM[outSlot] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("filter.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor numberOfToolsDescriptor(NUMBER_OF_TOOLS, EnsembleClassificationWorker::tr("Number of tools"),
                                           EnsembleClassificationWorker::tr("Specify the number of classification tools. The corresponding data should be provided using the input ports."));

        Descriptor outFileDesc(OUT_FILE, EnsembleClassificationWorker::tr("Output file"),
            EnsembleClassificationWorker::tr("Specify the output file. The classification data are stored in CSV format with the following columns:"
                                             "<ol><li> a sequence name"
                                             "<li>taxID from the first tool"
                                             "<li>taxID from the second tool"
                                             "<li>optionally, taxID from the third tool</ol>"
                                           ));

        Attribute *numberOfTools = new Attribute(numberOfToolsDescriptor, BaseTypes::NUM_TYPE(), Attribute::None, 2);
        Attribute *outFileAttribute = new Attribute(outFileDesc, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);
        a << numberOfTools;
        a << outFileAttribute;

        numberOfTools->addPortRelation(new PortRelationDescriptor(INPUT_PORT3, QVariantList() << 3));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap numberOfToolsMap;
        numberOfToolsMap["2"] = 2;
        numberOfToolsMap["3"] = 3;
        delegates[NUMBER_OF_TOOLS] = new ComboBoxDelegate(numberOfToolsMap);

        const URLDelegate::Options options = URLDelegate::SelectFileToSave;
        DelegateTags tags;
        tags.set(DelegateTags::PLACEHOLDER_TEXT, EnsembleClassificationWorker::tr("Auto"));
        tags.set(DelegateTags::FILTER, DialogUtils::prepareFileFilter("CSV", QStringList("csv"), false, QStringList()));

        delegates[OUT_FILE] = new URLDelegate(tags, "classification/ensemble", options);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new EnsembleClassificationPrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new EnsembleClassificationWorkerFactory());
}

void EnsembleClassificationWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}


/************************************************************************/
/* EnsembleClassificationWorker */
/************************************************************************/
EnsembleClassificationWorker::EnsembleClassificationWorker(Actor *a)
    : BaseWorker(a, false),
      input1(NULL),
      input2(NULL),
      input3(NULL),
      output(NULL),
      tripleInput(false)
{

}

bool EnsembleClassificationWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    const int hasMessage1 = input1->hasMessage();
    const bool isEnded1 = input1->isEnded();

    const int hasMessage2 = input2->hasMessage();
    const bool isEnded2 = input2->isEnded();

    const int hasMessage3 = input3->hasMessage();
    const bool isEnded3 = input3->isEnded();

    const bool allPortsHaveMessage = hasMessage1 && hasMessage2 && (!tripleInput || hasMessage3);
    const bool nobodyHasMessage = isEnded1 && isEnded2 && (!tripleInput || isEnded3);

    const bool firstPortHasExtraMessage = hasMessage1 && isEnded2 && (!tripleInput || isEnded3);
    const bool secondPortHasExtraMessage = isEnded1 && hasMessage2 && (!tripleInput || isEnded3);
    const bool thirdPortHasExtraMessage = isEnded1 && isEnded2 && tripleInput && hasMessage3;

    const bool firstPortLackMessage = isEnded1 && hasMessage2 && tripleInput && hasMessage3;
    const bool secondPortLackMessage = hasMessage1 && isEnded2 && tripleInput && hasMessage3;
    const bool thirdPortLackMessage = hasMessage1 && hasMessage2 && tripleInput && isEnded3;

    const bool somethingWrongWithMessages = firstPortHasExtraMessage ||
                                            secondPortHasExtraMessage ||
                                            thirdPortHasExtraMessage ||
                                            firstPortLackMessage ||
                                            secondPortLackMessage ||
                                            thirdPortLackMessage;

    return allPortsHaveMessage || nobodyHasMessage || somethingWrongWithMessages;
}

void EnsembleClassificationWorker::init() {
    input1 = ports.value(INPUT_PORT1);
    input2 = ports.value(INPUT_PORT2);
    input3 = ports.value(INPUT_PORT3);
    output = ports.value(OUTPUT_PORT);

    SAFE_POINT(NULL != input1, QString("Port with id '%1' is NULL").arg(INPUT_PORT1), );
    SAFE_POINT(NULL != input2, QString("Port with id '%1' is NULL").arg(INPUT_PORT2), );
    SAFE_POINT(NULL != input3, QString("Port with id '%1' is NULL").arg(INPUT_PORT3), );
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(OUTPUT_PORT), );

    tripleInput = getValue<int>(NUMBER_OF_TOOLS) == 3;
}

namespace {

QVariantMap uniteUniquely(const QVariantMap &first, const QVariantMap &second) {
    QVariantMap result;
    foreach (const QString &key, first.keys()) {
        result[key] = first.value(key);
    }

    foreach (const QString &key, second.keys()) {
        result[key] = second.value(key);
    }
    return result;
}

}

Task * EnsembleClassificationWorker::tick() {
    if (isReadyToRun()) {
        QList<TaxonomyClassificationResult> taxData;

        QString sourceFileUrl;
        QString sourceFileUrl1;
        QString sourceFileUrl2;
        QString sourceFileUrl3;

        const Message message1 = getMessageAndSetupScriptValues(input1);
        taxData << message1.getData().toMap()[INPUT_SLOT1].value<TaxonomyClassificationResult>();
        const MessageMetadata metadata1 = context->getMetadataStorage().get(message1.getMetadataId());
        sourceFileUrl1 = metadata1.getFileUrl();

        const Message message2 = getMessageAndSetupScriptValues(input2);
        taxData << message2.getData().toMap()[INPUT_SLOT2].value<TaxonomyClassificationResult>();
        const MessageMetadata metadata2 = context->getMetadataStorage().get(message2.getMetadataId());
        sourceFileUrl2 = metadata2.getFileUrl();

        if (tripleInput) {
            const Message message3 = getMessageAndSetupScriptValues(input3);
            taxData << message3.getData().toMap()[INPUT_SLOT3].value<TaxonomyClassificationResult>();
            const MessageMetadata metadata3 = context->getMetadataStorage().get(message3.getMetadataId());
            sourceFileUrl3 = metadata3.getFileUrl();
        }


        QVariantMap unitedContext;
        unitedContext = uniteUniquely(unitedContext, input1->getLastMessageContext());
        unitedContext = uniteUniquely(unitedContext, input2->getLastMessageContext());
        if (tripleInput) {
            unitedContext = uniteUniquely(unitedContext, input3->getLastMessageContext());
        }

        int metadataId = MessageMetadata::INVALID_ID;
        if (sourceFileUrl1 == sourceFileUrl2 &&
                (!tripleInput || (tripleInput && sourceFileUrl1 == sourceFileUrl3))) {
            sourceFileUrl = sourceFileUrl1;
            metadataId = metadata1.getId();
        }
        output->setContext(unitedContext, metadataId);

        outputFile = getValue<QString>(OUT_FILE);
        if (outputFile.isEmpty()) {
            outputFile = DEFAULT_OUT_FILE_NAME;
            if (!sourceFileUrl.isEmpty()) {
                QString prefix = GUrlUtils::getPairedFastqFilesBaseName(sourceFileUrl, true);
                if (!prefix.isEmpty()) {
                    outputFile = NgsReadsClassificationUtils::getBaseFileNameWithPrefixes(outputFile,
                                                                                          QStringList() << prefix,
                                                                                          DEFAULT_OUT_FILE_EXTENSION,
                                                                                          false);
                }
            }
        }

        Task* t = new EnsembleClassificationTask(taxData, tripleInput, outputFile, context->workingDir());
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return t;
    }

    if (dataFinished()) {
        setDone();
        algoLog.info("Ensemble worker is done as input has ended");
        output->setEnded();
    }

    const QString error = checkSimultaneousFinish();
    if (!error.isEmpty()) {
        setDone();
        output->setEnded();
        return new FailTask(error);
    }

    return NULL;
}

void EnsembleClassificationWorker::sl_taskFinished(Task *t) {
    EnsembleClassificationTask *task = qobject_cast<EnsembleClassificationTask *>(t);
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (!task->isFinished() || task->hasError() || task->isCanceled()) {
        return;
    }
    QString reportUrl = task->getOutputFile();
    QVariantMap m;
    m[OUTPUT_SLOT] = reportUrl;
    output->put(Message(output->getBusType(), m/*, metadata.getId()*/));
    monitor()->addOutputFile(reportUrl, getActor()->getId());
    if (task->foundMismatches()) {
        QString msg = tr("Different taxonomy data do not match. Some sequence names were skipped.");
        algoLog.info(msg);
        monitor()->addInfo(msg, getActorId(), WorkflowNotification::U2_WARNING);
    }
}

bool EnsembleClassificationWorker::isReadyToRun() const {
    return input1->hasMessage() && input2->hasMessage() && (!tripleInput || input3->hasMessage());
}

bool EnsembleClassificationWorker::dataFinished() const {
    return input1->isEnded() || input2->isEnded() || (tripleInput && input3->isEnded());
}

QString EnsembleClassificationWorker::checkSimultaneousFinish() const {
    if (!input1->isEnded() && input2->isEnded() && (!tripleInput || input3->isEnded())) {
        if (tripleInput) {
            return tr("Not enough classified data in the ports '%1' and '%2'").arg(input2->getPortId()).arg(input3->getPortId());
        } else {
            return tr("Not enough classified data in the port '%1'").arg(input2->getPortId());
        }
    }

    if (input1->isEnded() && !input2->isEnded() && (!tripleInput || input3->isEnded())) {
        if (tripleInput) {
            return tr("Not enough classified data in the ports '%1' and '%2'").arg(input1->getPortId()).arg(input3->getPortId());
        } else {
            return tr("Not enough classified data in the port '%1'").arg(input1->getPortId());
        }
    }

    if (input1->isEnded() && input2->isEnded() && tripleInput && !input3->isEnded()) {
        return tr("Not enough classified data in the ports '%1' and '%2'").arg(input1->getPortId()).arg(input2->getPortId());
    }

    if (!input1->isEnded() && !input2->isEnded() && tripleInput && input3->isEnded()) {
        return tr("Not enough classified data in the port '%1'").arg(input3->getPortId());
    }

    if (!input1->isEnded() && input2->isEnded() && tripleInput && !input3->isEnded()) {
        return tr("Not enough classified data in the port '%1'").arg(input2->getPortId());
    }

    if (input1->isEnded() && !input2->isEnded() && tripleInput && !input3->isEnded()) {
        return tr("Not enough classified data in the port '%1'").arg(input1->getPortId());
    }

    return "";
}

void EnsembleClassificationTask::run() {
    QStringList seqs = taxData[0].keys();
    seqs << taxData[1].keys();
    if (tripleInput) {
        seqs << taxData[2].keys();
    }
    CHECK_OP(stateInfo, );
    seqs.removeDuplicates();
    CHECK_OP(stateInfo, );
    seqs.sort();
    int counter = 0;

    outputFile = GUrlUtils::rollFileName(outputFile, "_");
    if (!QFileInfo(outputFile).isAbsolute()) {
        QString tmpDir = FileAndDirectoryUtils::createWorkingDir(workingDir, FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", workingDir);
        outputFile = tmpDir + '/' + outputFile;
    }

    QFile csvFile(outputFile);
    if (csvFile.open(QIODevice::Append)) {

        foreach(QString seq, seqs) {
            CHECK_OP(stateInfo, );
            stateInfo.setProgress(++counter * 100 / seqs.size());

            TaxID id1 = taxData[0].value(seq, TaxonomyTree::UNDEFINED_ID);
            TaxID id2 = taxData[1].value(seq, TaxonomyTree::UNDEFINED_ID);
            TaxID id3 = taxData[2].value(seq, TaxonomyTree::UNDEFINED_ID);
            if (id1 == TaxonomyTree::UNDEFINED_ID) {
                QString msg = tr("Taxonomy classification for '%1' is missing from %2 slot").arg(seq).arg(INPUT_SLOT1);
                algoLog.trace(msg);
                hasMissing = true;
                continue;
            }
            if (id2 == TaxonomyTree::UNDEFINED_ID) {
                QString msg = tr("Taxonomy classification for '%1' is missing from %2 slot").arg(seq).arg(INPUT_SLOT2);
                algoLog.trace(msg);
                hasMissing = true;
                continue;
            }
            if (tripleInput && id3 == TaxonomyTree::UNDEFINED_ID) {
                QString msg = tr("Taxonomy classification for '%1' is missing from %2 slot").arg(seq).arg(INPUT_SLOT3);
                algoLog.trace(msg);
                hasMissing = true;
                continue;
            }
            QString csvString;
            csvString.append(seq).append(',').append(QString::number(id1)).append(',').append(QString::number(id2));
            if (tripleInput) {
                csvString.append(',').append(QString::number(id3));
            }
            csvString.append("\n");
            csvFile.write(csvString.toLocal8Bit());
        }
        csvFile.close();
    } else {
        setError(csvFile.errorString());
    }
}

EnsembleClassificationTask::EnsembleClassificationTask(const QList<TaxonomyClassificationResult> &_taxData,
                                                       const bool _tripleInput,
                                                       const QString &_outputFile,
                                                       const QString &_workingDir)
    : Task(tr("Ensemble different classifications"), TaskFlag_None),
      taxData(_taxData),
      tripleInput(_tripleInput),
      workingDir(_workingDir),
      outputFile(_outputFile),
      hasMissing(false)
{
    SAFE_POINT_EXT(taxData.size() == 2 || (taxData.size() == 3 && tripleInput), setError("Incorrect size on input data"), );
    if (!tripleInput) {
        taxData << TaxonomyClassificationResult();
    }
}

} //LocalWorkflow
} //U2
