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

#include "AminoTranslationWorker.h"

#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Designer/DelegateEditors.h>

#include <U2Core/U2SequenceUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/TextUtils.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/U2Attribute.h>
#include <U2Core/U2OpStatusUtils.h>

const QString OFFSET_DELIMITER(",");

namespace U2 {
namespace LocalWorkflow {

const QString AminoTranslationWorkerFactory::ACTOR_ID("sequence-translation");

static const QString AUTO_TRANSLATION_ATTR("auto-translation");
static const QString ID_ATTR("genetic-code");
static const QString POS_2_TRANSLATE_ATTR("pos-2-translate");

TranslateSequence2AminoTask::TranslateSequence2AminoTask(const AminoTranslationSettings& _configs,
    const U2DbiRef &dbiRef)
    : Task("Translate sequence to amino", TaskFlag_None), configs(_configs), dbiRef(dbiRef) {
    SAFE_POINT_EXT(dbiRef.isValid(), stateInfo.setError("Invalid DBI reference supplied!"), );
}

void TranslateSequence2AminoTask::run() {
    assert(configs.seqObj != NULL);

    U2SequenceObject* seqObj = configs.seqObj.data();
    qint64 seqLen = seqObj->getSequenceLength();
    QVector<U2Region> regionsDirect = configs.regionsDirect;
    QVector<U2Region> regionsComplementary = configs.regionsComplementary;
    QString resultName = configs.resultName;

    DNATranslation* aminoTT = configs.aminoTT;
    SAFE_POINT(aminoTT != NULL, QString("Can't produce amino translation"), );

    for (QVector<U2Region>::iterator i = regionsDirect.begin(); i != regionsDirect.end();) {
        if (i->startPos >= seqLen) {
            i = regionsDirect.erase(i);
        } else {
            ++i;
        }
    }

    for (QVector<U2Region>::iterator i = regionsComplementary.begin(); i != regionsComplementary.end();) {
        if (i->startPos >= seqLen) {
            i = regionsComplementary.erase(i);
        } else {
            ++i;
        }
    }

    DNATranslation* complTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(seqObj->getAlphabet());
    SAFE_POINT(complTT != NULL || regionsComplementary.isEmpty(), QString("Can't support complement mode"), );

    static const int NUM_DB_READ = 3 * 1024;

    int currentSeq = 0;

    QByteArray seq;
    QByteArray translatedSeq;
    // direct
    foreach(U2Region directRegion, regionsDirect) {
        seq.clear();
        translatedSeq.clear();

        U2SequenceImporter importer;
        importer.startSequence(stateInfo, dbiRef, U2ObjectDbi::ROOT_FOLDER, seqObj->getSequenceName() + " " + resultName + QString(" %1").arg(currentSeq) + " direct", false);

        int blockCounter = 0;
        qint64 end = directRegion.startPos + directRegion.length / 3 * 3;
        for (qint64 i = directRegion.startPos; i < end; i += 3, blockCounter += 3) {
            if ((blockCounter % NUM_DB_READ) == 0) {
                importer.addBlock(translatedSeq.constData(), translatedSeq.size(), stateInfo);
                seq.clear();
                translatedSeq.clear();
                seq = seqObj->getSequenceData(U2Region(i, qMin(static_cast<qint64>(NUM_DB_READ), end - i)), stateInfo);
                CHECK_OP(stateInfo, );
                blockCounter = 0;
            }
            translatedSeq.append(aminoTT->translate3to1(seq[blockCounter], seq[blockCounter + 1], seq[blockCounter + 2]));
        }

        importer.addBlock(translatedSeq.constData(), translatedSeq.size(), stateInfo);
        U2Sequence u2Seq = importer.finalizeSequence(stateInfo);

        CHECK_OP(stateInfo, );
        results << new U2SequenceObject(u2Seq.visualName, U2EntityRef(dbiRef, u2Seq.id));
        currentSeq++;
    }
    currentSeq = 0;
    // complementary
    foreach(U2Region complementaryRegion, regionsComplementary) {
        translatedSeq.clear();

        U2SequenceImporter importer;
        importer.startSequence(stateInfo, dbiRef, U2ObjectDbi::ROOT_FOLDER, seqObj->getSequenceName() + " " + resultName + QString(" %1").arg(currentSeq) + " complementary", false);

        QByteArray complementarySeq;
        char* complSeq = NULL;

        int blockCounter = 0;
        qint64 end = complementaryRegion.endPos() - 1 - complementaryRegion.length / 3 * 3;

        for (qint64 i = complementaryRegion.endPos() - 1; i > end;i -= 3, blockCounter += 3) {
            if ((blockCounter % NUM_DB_READ) == 0) {
                importer.addBlock(translatedSeq.constData(), translatedSeq.size(), stateInfo);
                complementarySeq.clear();
                seq.clear();
                translatedSeq.clear();
                seq = seqObj->getSequenceData(U2Region(qMax(end + 1, static_cast<qint64>(i - NUM_DB_READ + 1)), qMin(static_cast<qint64>(NUM_DB_READ), i - end)), stateInfo);
                CHECK_OP(stateInfo, );
                complementarySeq.reserve(seq.size());
                complSeq = complementarySeq.data();
                TextUtils::translate(complTT->getOne2OneMapper(), seq, seq.size(), complSeq);
                TextUtils::reverse(complSeq, seq.size());
                blockCounter = 0;

            }
            SAFE_POINT(complSeq != NULL, "complSeq is NULL", );
            translatedSeq.append(aminoTT->translate3to1(complSeq[blockCounter], complSeq[blockCounter + 1], complSeq[blockCounter + 2]));
        }

        importer.addBlock(translatedSeq.constData(), translatedSeq.size(), stateInfo);
        U2Sequence u2Seq = importer.finalizeSequence(stateInfo);

        CHECK_OP(stateInfo, );
        results << new U2SequenceObject(u2Seq.visualName, U2EntityRef(dbiRef, u2Seq.id));
        currentSeq++;
    }
}


void AminoTranslationWorkerFactory::init() {

    QMap<Descriptor, DataTypePtr> m;
    m[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    DataTypePtr inSet(new MapDataType(Descriptor("regioned.sequence"), m));
    DataTypeRegistry* dr = WorkflowEnv::getDataTypeRegistry();
    assert(dr);
    dr->registerEntry(inSet);

    QList<PortDescriptor*> p;
    {
        Descriptor ind(BasePorts::IN_SEQ_PORT_ID(),
            AminoTranslationWorker::tr("Input Data"),
            AminoTranslationWorker::tr("An input sequence to translate in amino."));

        Descriptor oud(BasePorts::OUT_SEQ_PORT_ID(),
            AminoTranslationWorker::tr("Amino sequence"),
            AminoTranslationWorker::tr("Sequence has been translated."));

        p << new PortDescriptor(ind, inSet, true);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        outM[BaseSlots::TEXT_SLOT()] = BaseTypes::STRING_TYPE();

        p << new PortDescriptor(oud, DataTypePtr(new MapDataType("translate.sequence", outM)), false, true);
    }
    QList<Attribute*> a;

    Descriptor pt(POS_2_TRANSLATE_ATTR, AminoTranslationWorker::tr("Translate from "), AminoTranslationWorker::tr("What position would sequence translated from : first, second, third or from all positions."));
    a << new Attribute(pt, BaseTypes::STRING_TYPE(), false, QVariant("all"));

    Descriptor atd(AUTO_TRANSLATION_ATTR, AminoTranslationWorker::tr("Auto selected genetic code"), AminoTranslationWorker::tr("Set if genetic code will be selected automatically."));
    a << new Attribute(atd, BaseTypes::BOOL_TYPE(), true, QVariant(true));

    Descriptor desc(ACTOR_ID,
        AminoTranslationWorker::tr("Amino Translation"),
        AminoTranslationWorker::tr("Translate sequence into it's amino translation"
        " . Output a sequence."));

    Descriptor ttd(ID_ATTR, AminoTranslationWorker::tr("Genetic code"), AminoTranslationWorker::tr("Which genetic code should be used for translating the input nucleotide sequence."));
    Attribute* translAttribute = new Attribute(ttd, BaseTypes::STRING_TYPE(), false, QVariant(DNATranslationID(1)));

    translAttribute->addRelation(new VisibilityRelation(AUTO_TRANSLATION_ATTR, QVariant(false)));
    a << translAttribute;

    QMap<QString, PropertyDelegate*> delegates;

    QVariantMap idMap;
    QList<DNATranslation*> TTs = AppContext::getDNATranslationRegistry()->
        lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()),
        DNATranslationType_NUCL_2_AMINO);
    foreach(DNATranslation* tt, TTs) {
        idMap[tt->getTranslationName()] = tt->getTranslationId();
    }
    delegates[ID_ATTR] = new ComboBoxDelegate(idMap);
    delegates[AUTO_TRANSLATION_ATTR] = new ComboBoxWithBoolsDelegate();

    QVariantMap posTransMap;
    posTransMap["all"] = "all";
    posTransMap["first"] = "first";
    posTransMap["second"] = "second";
    posTransMap["third"] = "third";
    delegates[POS_2_TRANSLATE_ATTR] = new ComboBoxDelegate(posTransMap);

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    proto->setEditor(new DelegateEditor(delegates));

    proto->setIconPath(":core/images/show_trans.png");
    proto->setPrompter(new AminoTranslationPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new AminoTranslationWorkerFactory());
}

QString AminoTranslationPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString offset = getParameter(POS_2_TRANSLATE_ATTR).toString();
    bool autoTranslation = getParameter(AUTO_TRANSLATION_ATTR).toBool();
    QString translationId;
    QString usingGenetic;

    offset = getHyperlink(POS_2_TRANSLATE_ATTR, offset);

    if (!autoTranslation) {
        translationId = getParameter(ID_ATTR).toString();
        DNATranslation* aminoTT = AppContext::getDNATranslationRegistry()->
            lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()), DNATranslationType_NUCL_2_AMINO, translationId);
        QString ttName = aminoTT->getTranslationName();
        usingGenetic = ttName = getHyperlink(ID_ATTR, ttName);
    } else {
        usingGenetic = getHyperlink(AUTO_TRANSLATION_ATTR, "auto selected genetic code");
    }

    QString doc = QString("Translated sequence to amino from %1 position using %2.").
        arg(offset). // offset from sequence will be translated
        arg(usingGenetic); // genetic code
    return doc;
}

AminoTranslationWorker::AminoTranslationWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {}

void AminoTranslationWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_SEQ_PORT_ID());
}

static QVector<U2Region> getTranslatedRegions(const QString& attribute, qint64 seqLen) {
    QVector<U2Region> translatedRegions;

    if (attribute == "first") {
        translatedRegions.append(U2Region(0, seqLen));
    } else if (attribute == "second") {
        translatedRegions.append(U2Region(1, seqLen - 1));
    } else if (attribute == "third") {
        translatedRegions.append(U2Region(2, seqLen - 2));
    } else if (attribute == "all") {
        translatedRegions.append(U2Region(0, seqLen));
        translatedRegions.append(U2Region(1, seqLen - 1));
        translatedRegions.append(U2Region(2, seqLen - 2));
    } else {
        assert(false);
    }
    return translatedRegions;
}

Task* AminoTranslationWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);

        if (inputMessage.isEmpty()) {
            output->put(Message::getEmptyMapMessage());
        }

        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();

        QSharedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));


        if (NULL == seqObj.data()) {
            algoLog.trace("Sequence is not found");
            return NULL;
        }

        if (seqObj->getAlphabet() == NULL) {
            algoLog.trace("Alphabet is not found");
            return NULL;
        }


        if (!seqObj->getAlphabet()->isNucleic()) {
            algoLog.trace("Alphabet is not nucleic");
            return NULL;
            //return new FailTask(tr("Alphabet is not nucleic"));
        }

        QStringList offsets = actor->getParameter(POS_2_TRANSLATE_ATTR)->getAttributeValue<QString>(context).split(OFFSET_DELIMITER, QString::SkipEmptyParts);
        QString resultName = "Amino translation";

        QVector<U2Region> regionsDirect = getTranslatedRegions(actor->getParameter(POS_2_TRANSLATE_ATTR)->getAttributeValue<QString>(context), seqObj->getSequenceLength());

        bool autoTranslation = actor->getParameter(AUTO_TRANSLATION_ATTR)->getAttributeValue<bool>(context);
        DNATranslation* aminoTT = NULL;

        if (autoTranslation) {
            aminoTT = AppContext::getDNATranslationRegistry()->lookupTranslation(seqObj->getAlphabet(), DNATranslationType_NUCL_2_AMINO, seqObj->getStringAttribute(Translation_Table_Id_Attribute));
            if (aminoTT == NULL) {
                aminoTT = GObjectUtils::findAminoTT(seqObj.data(), false);
            }
        } else {
            QString translId = actor->getParameter(ID_ATTR)->getAttributeValue<QString>(context);
            aminoTT = AppContext::getDNATranslationRegistry()->lookupTranslation(seqObj->getAlphabet(), DNATranslationType_NUCL_2_AMINO, translId);
        }
        if (aminoTT == NULL) {
            return new FailTask("Selected genetic code is not supported ");
        }

        AminoTranslationSettings  config;

        config.regionsDirect = regionsDirect;
        config.resultName = resultName;
        config.seqObj = seqObj;
        config.aminoTT = aminoTT;

        DbiDataStorage *storage = context->getDataStorage();
        SAFE_POINT(NULL != storage, "Invalid workflow data storage!", NULL);
        TranslateSequence2AminoTask *transTask = new TranslateSequence2AminoTask(config,
            storage->getDbiRef());

        connect(transTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return transTask;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void AminoTranslationWorker::cleanup() {

}

void AminoTranslationWorker::sl_taskFinished() {
    TranslateSequence2AminoTask* translate2AminoTask = qobject_cast<TranslateSequence2AminoTask*>(sender());
    if (translate2AminoTask->getState() != Task::State_Finished ||
        translate2AminoTask->isCanceled() || translate2AminoTask->hasError()) {
        return;
    }

    if (output) {
        QVariantMap channelContext = output->getContext();
        int metadataId = output->getContextMetadataId();

        QList<U2SequenceObject*> seqObjs = translate2AminoTask->popResults();
        foreach(U2SequenceObject* seqObj, seqObjs) {
            QVariantMap msgData;
            U2OpStatusImpl os;
            DNASequence seqData = seqObj->getWholeSequence(os);
            CHECK_OP_EXT(os, reportError(tr("Error on getting %1 sequence: ").arg(seqObj->getGObjectName()) + os.getError()), );
            SharedDbiDataHandler seqId = context->getDataStorage()->putSequence(seqData);
            msgData[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(seqId);
            output->setContext(channelContext, metadataId);
            output->put(Message(BaseTypes::DNA_SEQUENCE_TYPE(), msgData));
        }
    }
}

}
}
