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

#include "GenericReadActor.h"
#include "CoreLib.h"
#include "DocActors.h"
#include "util/DatasetValidator.h"

#include <U2Lang/WorkflowEnv.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/URLAttribute.h>

#include <U2Core/DNAInfo.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/GObjectTypes.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>

#include <QFileInfo>
#include <limits.h>

/* TRANSLATOR U2::Workflow::SeqReadPrompter */
/* TRANSLATOR U2::Workflow::CoreLib */

namespace U2 {
namespace Workflow {

/************************************************************************/
/* GenericReadDocProto */
/************************************************************************/
GenericReadDocProto::GenericReadDocProto(const Descriptor &desc)
    : ReadDbObjActorPrototype(desc)
{
    {
        attrs << new URLAttribute(BaseAttributes::URL_IN_ATTRIBUTE(),
            BaseTypes::URL_DATASETS_TYPE(), true);
    }

    setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));

    if(AppContext::isGUIMode()) {
        setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
    }

    setValidator(new DatasetValidator());
}

const QString GenericSeqActorProto::MODE_ATTR("mode");
const QString GenericSeqActorProto::GAP_ATTR("merge-gap");
const QString GenericSeqActorProto::ACC_ATTR("accept-accession");
const QString GenericSeqActorProto::LIMIT_ATTR("sequence-count-limit");

const QString GenericSeqActorProto::TYPE("generic.seq");
const QString GenericMAActorProto::TYPE("generic.ma");

GenericSeqActorProto::GenericSeqActorProto() : GenericReadDocProto(CoreLibConstants::GENERIC_READ_SEQ_PROTO_ID)
{
    setCompatibleDbObjectTypes(QSet<GObjectType>() << GObjectTypes::SEQUENCE);

    setDisplayName(U2::Workflow::CoreLib::tr("Read Sequence"));
    desc = U2::Workflow::CoreLib::tr("Input one or several files with nucleotide or protein sequences."
                                     " A file may also contain annotations. Any format, supported by UGENE, is allowed (GenBank, FASTA, etc.)."
                                     " The element outputs message(s) with the sequence and annotations data.");
    QMap<Descriptor, DataTypePtr> m;
    m[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
    m[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
    m[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    m[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();

    DataTypePtr seqTypeset(new MapDataType(Descriptor(TYPE), m));
    bool treg = WorkflowEnv::getDataTypeRegistry()->registerEntry(seqTypeset);
    Q_UNUSED(treg);assert(treg);

    ports << new PortDescriptor(Descriptor(BasePorts::OUT_SEQ_PORT_ID(), U2::Workflow::CoreLib::tr("Sequence"),
                                            U2::Workflow::CoreLib::tr("A sequence of any type (nucleotide, protein).")),
                                seqTypeset, false, true);
    {
        Descriptor md(GenericSeqActorProto::MODE_ATTR, SeqReadPrompter::tr("Mode"),
            SeqReadPrompter::tr("If the file contains more than one sequence, <i>Split</i> mode sends them \"as is\" to the output, "
            "while <i>Merge</i> appends all the sequences and outputs the sole merged sequence."));
        Descriptor gd(GenericSeqActorProto::GAP_ATTR, SeqReadPrompter::tr("Merging gap"),
            SeqReadPrompter::tr("In <i>Merge</i> mode, the specified number of gaps are inserted between the original sequences. "
            "This is helpful, for example, to avoid finding false positives at the merge boundaries."));
        Descriptor acd(GenericSeqActorProto::ACC_ATTR, SeqReadPrompter::tr("Accession filter"),
            SeqReadPrompter::tr("Reports only sequences containing the specified regular expression."
                                "<p><i>Leave it empty to switch off this filter. Use <b>*</b> to mask many symbol and use <b>?</b> to mask one symbol.</i></p>"));
        Descriptor ld(LIMIT_ATTR, SeqReadPrompter::tr("Sequence count limit"),
            SeqReadPrompter::tr("<i>Split mode</i> only."
            "<p>Read only first N sequences from each file."
            "<br>Set 0 value for reading all sequences.</p>"));

        attrs << new Attribute(md, BaseTypes::NUM_TYPE(), true, SPLIT);
        attrs << new Attribute(gd, BaseTypes::NUM_TYPE(), false, 10);
        attrs << new Attribute(ld, BaseTypes::NUM_TYPE(), false, 0);
        attrs << new Attribute(acd, BaseTypes::STRING_TYPE(), false, QString());
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap modeMap;
        QString splitStr = SeqReadPrompter::tr("Split");
        QString mergeStr = SeqReadPrompter::tr("Merge");
        modeMap[splitStr] = SPLIT;
        modeMap[mergeStr] = MERGE;
        getEditor()->addDelegate(new ComboBoxDelegate(modeMap), MODE_ATTR);
    }
    {
        QVariantMap m; m["minimum"] = 0; m["maximum"] = INT_MAX;
        getEditor()->addDelegate(new SpinBoxDelegate(m), GAP_ATTR);
        getEditor()->addDelegate(new SpinBoxDelegate(m), LIMIT_ATTR);
    }
    setPrompter(new ReadDocPrompter(U2::Workflow::CoreLib::tr("Reads sequence(s) from <u>%1</u>.")));

    QString seqSlotId = BasePorts::OUT_SEQ_PORT_ID() + "." + BaseSlots::DNA_SEQUENCE_SLOT().getId();
    QString annsSlotId = BasePorts::OUT_SEQ_PORT_ID() + "." + BaseSlots::ANNOTATION_TABLE_SLOT().getId();
}

GenericMAActorProto::GenericMAActorProto() : GenericReadDocProto(CoreLibConstants::GENERIC_READ_MA_PROTO_ID)
{
    setCompatibleDbObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);

    setDisplayName(U2::Workflow::CoreLib::tr("Read Alignment"));
    desc = U2::Workflow::CoreLib::tr("Input one or several files in one of the multiple sequence alignment formats, supported by UGENE (ClustalW, FASTA, etc.)."
                " The element outputs message(s) with the alignment data.");

    QMap<Descriptor, DataTypePtr> m;
    m[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
    m[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    DataTypePtr blockTypeset(new MapDataType(Descriptor(TYPE), m));
    bool treg = WorkflowEnv::getDataTypeRegistry()->registerEntry(blockTypeset);
    Q_UNUSED(treg);assert(treg);

    ports << new PortDescriptor(Descriptor(BasePorts::OUT_MSA_PORT_ID(), U2::Workflow::CoreLib::tr("Multiple sequence alignment"), ""),
                                blockTypeset, false, true);

    setPrompter(new ReadDocPrompter(U2::Workflow::CoreLib::tr("Reads MSA(s) from <u>%1</u>.")));

    if( AppContext::isGUIMode() ) {
        setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
    }
}

bool GenericMAActorProto::isAcceptableDrop(const QMimeData* md, QVariantMap* params) const {
    QList<DocumentFormat*> fs;
    QString url = WorkflowUtils::getDropUrl(fs, md);
    foreach(DocumentFormat* f, fs) {
        if (f->getSupportedObjectTypes().contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT)) {
            if (params != NULL) {
                params->insert(BaseAttributes::URL_IN_ATTRIBUTE().getId(), url);
            }
            return true;
        }
    }
    /*DocumentFormatConstraints sc;
    sc.supportedObjectTypes.append(GObjectTypes::SEQUENCE);
    foreach(DocumentFormat* f, fs) {
        if (f->checkConstraints(sc)) {
            if (params) {
                params->insert(CoreLib::URL_ATTR_ID, url);
            }
            return true;
        }
    }*/
    if (QFileInfo(url).isDir()) {
        if (params) {
            params->insert(BaseAttributes::URL_IN_ATTRIBUTE().getId(), url + "/*");
        }
        return true;
    }

    return false;
}

bool GenericSeqActorProto::isAcceptableDrop(const QMimeData* md, QVariantMap* params) const {
    QList<DocumentFormat*> fs;
    const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
    if (gomd && params) {
        const U2SequenceObject* obj = qobject_cast<const U2SequenceObject*>(gomd->objPtr.data());
        if (obj) {
            params->insert(BaseAttributes::URL_IN_ATTRIBUTE().getId(), obj->getDocument()->getURLString());
            QString acc = obj->getStringAttribute(DNAInfo::ACCESSION);
            if (acc.isEmpty()) {
                acc = obj->getSequenceName();
            }
            params->insert(ACC_ATTR, acc);
        }
    }

    QString url = WorkflowUtils::getDropUrl(fs, md);
    foreach(DocumentFormat* f, fs) {
        if (f->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE)) {
            if (params) {
                params->insert(BaseAttributes::URL_IN_ATTRIBUTE().getId(), url);
            }
            return true;
        }
    }
    if (QFileInfo(url).isDir()) {
        if (params) {
            params->insert(BaseAttributes::URL_IN_ATTRIBUTE().getId(), url + "/*");
        }
        return true;
    }

    return false;
}

QString SeqReadPrompter::composeRichDoc() {
    //TODO finish
    //BusPort* input = qobject_cast<BusPort*>(target->getPort(CoreLib::DATA_PORT_ID));
    QString url = getURL(BaseAttributes::URL_IN_ATTRIBUTE().getId());
    return QString("%1").arg(url);
}

}//namespace Workflow
}//namespace U2
