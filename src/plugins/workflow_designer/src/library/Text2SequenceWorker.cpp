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

#include <U2Core/AppContext.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/FailTask.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Gui/SeqPasterWidgetController.h>
#include "Text2SequenceWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString Text2SequenceWorkerFactory::ACTOR_ID("convert-text-to-sequence");

static const Descriptor TEXT_2_SEQUENCE_IN_TYPE_ID("text-2-sequence-in-type");
static const Descriptor TEXT_2_SEQUENCE_OUT_TYPE_ID("text-2-sequence-out-type");

static const QString SEQ_NAME_ATTR_ID("sequence-name");
static const QString ALPHABET_ATTR_ID("alphabet");
static const QString SKIP_SYM_ATTR_ID("skip-unknown");
static const QString REPLACE_SYM_ATTR_ID("replace-unknown-with");

static const QString SEQ_NAME_ATTR_DEF_VAL("Sequence");
static const QString ALPHABET_ATTR_ID_DEF_VAL("Auto");

/*******************************
 * Text2SequenceWorker
 *******************************/
QMap<QString, QString> Text2SequenceWorker::cuteAlIdNames = Text2SequenceWorker::initCuteAlNames();

QMap<QString, QString> Text2SequenceWorker::initCuteAlNames() {
    QMap<QString, QString> res;
    res[BaseDNAAlphabetIds::RAW()] = "All symbols";
    res[BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()] = "Standard DNA";
    res[BaseDNAAlphabetIds::NUCL_RNA_DEFAULT()] = "Standard RNA";
    res[BaseDNAAlphabetIds::NUCL_DNA_EXTENDED()] = "Extended DNA";
    res[BaseDNAAlphabetIds::NUCL_RNA_EXTENDED()] = "Extended RNA";
    res[BaseDNAAlphabetIds::AMINO_DEFAULT()] = "Standard amino";
    return res;
}

void Text2SequenceWorker::init() {
    txtPort = ports.value(BasePorts::IN_TEXT_PORT_ID());
    outSeqPort = ports.value(BasePorts::OUT_SEQ_PORT_ID());
}

Task * Text2SequenceWorker::tick() {
    while (txtPort->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(txtPort);
        if (inputMessage.isEmpty()) {
            outSeqPort->transit();
            continue;
        }
        QString seqName = actor->getParameter(SEQ_NAME_ATTR_ID)->getAttributeValue<QString>(context);
        if(seqName.isEmpty()) {
            return new FailTask(tr("Sequence name not set"));
        }
        if(tickedNum++ > 0) {
            seqName += QString::number(tickedNum);
        }
        QString alId = actor->getParameter(ALPHABET_ATTR_ID)->getAttributeValue<QString>(context);
        if(alId.isEmpty()) {
            alId = ALPHABET_ATTR_ID_DEF_VAL;
        } else {
            alId = cuteAlIdNames.key(alId, alId);
        }
        bool skipUnknown = actor->getParameter(SKIP_SYM_ATTR_ID)->getAttributeValue<bool>(context);
        QChar replaceChar;
        if(!skipUnknown) {
            QString replaceStr = actor->getParameter(REPLACE_SYM_ATTR_ID)->getAttributeValue<QString>(context);
            assert(replaceStr.size() <= 1);
            if(replaceStr.isEmpty()) {
                return new FailTask(tr("skip flag should be set or replace character defined"));
            }
            replaceChar = replaceStr.at(0);
        }
        QByteArray txt = inputMessage.getData().toMap().value(BaseSlots::TEXT_SLOT().getId()).value<QString>().toUtf8();

        const DNAAlphabet * alphabet = (alId == ALPHABET_ATTR_ID_DEF_VAL) ? U2AlphabetUtils::findBestAlphabet(txt) : U2AlphabetUtils::getById(alId);
        if (alphabet == NULL) {
            QString msg;
            if(alId == ALPHABET_ATTR_ID_DEF_VAL) {
                msg = tr("Alphabet cannot be automatically detected");
            } else {
                msg = tr("Alphabet '%1' cannot be found");
            }
            return new FailTask(msg);
        }

        QByteArray normSequence = SeqPasterWidgetController::getNormSequence(alphabet, txt, !skipUnknown, replaceChar);
        DNASequence result(seqName, normSequence, alphabet);
        QVariantMap msgData;
        {
            SharedDbiDataHandler seqId = context->getDataStorage()->putSequence(result);
            msgData[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(seqId);
        }
        if(outSeqPort) {
            outSeqPort->put(Message(BaseTypes::DNA_SEQUENCE_TYPE(), msgData));
        }
    }
    if(txtPort->isEnded()) {
        setDone();
        outSeqPort->setEnded();
    }
    return NULL;
}

void Text2SequenceWorker::cleanup() {
}

/*******************************
 * Text2SequenceWorkerFactory
 *******************************/
void Text2SequenceWorkerFactory::init() {
    // ports description
    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::TEXT_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr inSet(new MapDataType(TEXT_2_SEQUENCE_IN_TYPE_ID, inM));
        Descriptor inPortDesc(BasePorts::IN_TEXT_PORT_ID(), Text2SequenceWorker::tr("Input text"),
            Text2SequenceWorker::tr("A text which will be converted to sequence"));
        portDescs << new PortDescriptor(inPortDesc, inSet, true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        DataTypePtr outSet(new MapDataType(TEXT_2_SEQUENCE_OUT_TYPE_ID, outM));
        Descriptor outPortDesc(BasePorts::OUT_SEQ_PORT_ID(), Text2SequenceWorker::tr("Output sequence"),
            Text2SequenceWorker::tr("Converted sequence"));
        portDescs << new PortDescriptor(outPortDesc, outSet, false);
    }
    // attributes description
    QList<Attribute*> attrs;
    {
        Descriptor seqNameDesc(SEQ_NAME_ATTR_ID, Text2SequenceWorker::tr("Sequence name"), Text2SequenceWorker::tr("Result sequence name."));
        Descriptor alphabetDesc(ALPHABET_ATTR_ID, Text2SequenceWorker::tr("Sequence alphabet"),
            Text2SequenceWorker::tr("Select one of the listed alphabets or choose auto to auto-detect."));
        Descriptor skipSymbolsDesc(SKIP_SYM_ATTR_ID, Text2SequenceWorker::tr("Skip unknown symbols"),
            Text2SequenceWorker::tr("Do not include symbols that are not contained in alphabet."));
        Descriptor replaceSymbolsDesc(REPLACE_SYM_ATTR_ID, Text2SequenceWorker::tr("Replace unknown symbols with"),
            Text2SequenceWorker::tr("Replace unknown symbols with given character."));

        attrs << new Attribute(seqNameDesc, BaseTypes::STRING_TYPE(), /* required */ true, QVariant(SEQ_NAME_ATTR_DEF_VAL));
        attrs << new Attribute(alphabetDesc, BaseTypes::STRING_TYPE(), false, QVariant(ALPHABET_ATTR_ID_DEF_VAL));
        attrs << new Attribute(skipSymbolsDesc, BaseTypes::BOOL_TYPE(), false, QVariant(true));
        attrs << new Attribute(replaceSymbolsDesc, BaseTypes::STRING_TYPE(), false);
    }

    Descriptor protoDesc(Text2SequenceWorkerFactory::ACTOR_ID,
        Text2SequenceWorker::tr("Convert Text to Sequence"),
        Text2SequenceWorker::tr("Converts input text to sequence."));
    ActorPrototype * proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);

    // proto delegates
    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap alMap;
        QList<const DNAAlphabet*> alps = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets();
        foreach(const DNAAlphabet *a, alps){
            alMap[a->getName()] = Text2SequenceWorker::cuteAlIdNames[a->getId()];
        }
        alMap[ALPHABET_ATTR_ID_DEF_VAL] = ALPHABET_ATTR_ID_DEF_VAL;
        delegates[ALPHABET_ATTR_ID] = new ComboBoxDelegate(alMap);

        delegates[REPLACE_SYM_ATTR_ID] = new CharacterDelegate();
    }
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new Text2SequencePrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CONVERTERS(), proto);
    WorkflowEnv::getDomainRegistry()->getById( LocalDomainFactory::ID )->registerEntry( new Text2SequenceWorkerFactory() );
}

Worker * Text2SequenceWorkerFactory::createWorker(Actor* a) {
    return new Text2SequenceWorker(a);
}

/*******************************
 * Text2SequencePrompter
 *******************************/
QString Text2SequencePrompter::composeRichDoc() {
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    IntegralBusPort * input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_TEXT_PORT_ID()));
    Actor * txtProducer = input->getProducer(BaseSlots::TEXT_SLOT().getId());
    QString txtProducetStr = tr(" from <u>%1</u>").arg(txtProducer ? txtProducer->getLabel() : unsetStr);

    QString seqName = getRequiredParam(SEQ_NAME_ATTR_ID);
    QString seqNameStr = tr("sequence with name <u>%1</u>").arg(getHyperlink(SEQ_NAME_ATTR_ID, seqName));

    QString alId = getParameter(ALPHABET_ATTR_ID).value<QString>();
    QString seqAlStr;
    if(alId == ALPHABET_ATTR_ID_DEF_VAL) {
        seqAlStr = getHyperlink(ALPHABET_ATTR_ID, tr("Automatically detect sequence alphabet"));
    } else {
        alId = Text2SequenceWorker::cuteAlIdNames.key(alId, "");
        const DNAAlphabet * alphabet = AppContext::getDNAAlphabetRegistry()->findById(alId);
        QString alphStr = getHyperlink(ALPHABET_ATTR_ID, alphabet ? alphabet->getName() : unsetStr);
        seqAlStr = tr("Set sequence alphabet to %1").arg(alphStr);
    }

    bool skipUnknown = getParameter(SKIP_SYM_ATTR_ID).value<bool>();
    QString replaceStr = getRequiredParam(REPLACE_SYM_ATTR_ID);
    QString unknownSymbolsStr;
    if (skipUnknown) {
        unknownSymbolsStr = getHyperlink(SKIP_SYM_ATTR_ID, tr("skipped"));
    } else {
        unknownSymbolsStr = QString("%1 %2")
            .arg(getHyperlink(SKIP_SYM_ATTR_ID, tr("replaced with symbol")))
            .arg(getHyperlink(REPLACE_SYM_ATTR_ID, replaceStr));
    }

    QString doc = tr("Convert input text%1 to %2. %3. Unknown symbols are %4.")
        .arg(txtProducetStr)
        .arg(seqNameStr)
        .arg(seqAlStr)
        .arg(unknownSymbolsStr);
    return doc;
}

} // LocalWorkflow
} // U2
