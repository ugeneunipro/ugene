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

#include <U2Core/BaseDocumentFormats.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "StringtieGeneAbundanceReportPrompter.h"
#include "StringtieGeneAbundanceReportWorker.h"
#include "StringtieGeneAbundanceReportWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

const QString StringtieGeneAbundanceReportWorkerFactory::ACTOR_ID = "stringtie-gene-abundance-report";
const QString StringtieGeneAbundanceReportWorkerFactory::INPUT_PORT_ID = "in";
const QString StringtieGeneAbundanceReportWorkerFactory::OUTPUT_FILE_ATTR_ID = "output-url";

StringtieGeneAbundanceReportWorkerFactory::StringtieGeneAbundanceReportWorkerFactory()
    : DomainFactory(ACTOR_ID)
{
}

Worker *StringtieGeneAbundanceReportWorkerFactory::createWorker(Actor *actor) {
    return new StringtieGeneAbundanceReportWorker(actor);
}

void StringtieGeneAbundanceReportWorkerFactory::init() {
    const QString portId = StringtieGeneAbundanceReportWorkerFactory::INPUT_PORT_ID;

    QList<PortDescriptor *> ports;
    {
        const Descriptor inSlotDesc(BaseSlots::URL_SLOT().getId(),
                                    tr("Input URL url"),
                                    tr("Input URL."));

        QMap<Descriptor, DataTypePtr> inType;
        inType[inSlotDesc] = BaseTypes::STRING_TYPE();

        const Descriptor inPortDesc(portId,
                                    tr("Input StringTie gene abundance file(s) url"),
                                    tr("URL(s) to sorted gene abundance file(s), produced by StringTie."));
        ports << new PortDescriptor(inPortDesc,
                                    DataTypePtr(new MapDataType(ACTOR_ID + "-in", inType)),
                                    true,
                                    false,
                                    Attribute::CanBeEmpty);
    }

    QList<Attribute *> attributes;
    {
        const Descriptor outputFileDesc(OUTPUT_FILE_ATTR_ID,
                                        tr("Output file"),
                                        tr("Specify the name of the output tab-delimited text file."));
        attributes << new Attribute(outputFileDesc,
                                    BaseTypes::STRING_TYPE(),
                                    Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);
    }

    QMap<QString, PropertyDelegate *> delegates;
    {
        DelegateTags outputFileTags;
        outputFileTags.set(DelegateTags::PLACEHOLDER_TEXT, tr("Auto"));
        outputFileTags.set(DelegateTags::FILTER,
                           DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT,
                                                                   true,
                                                                   QStringList()));
        outputFileTags.set(DelegateTags::FORMAT, BaseDocumentFormats::PLAIN_TEXT);
        delegates[OUTPUT_FILE_ATTR_ID] = new URLDelegate(outputFileTags, "stringtie/gene_abudance_report");
    }

    const Descriptor desc(ACTOR_ID,
                          tr("StringTie Gene Abundance Report"),
                          tr("The element summarizes gene abundance output of StringTie and saves the result "
                             "into a common tab-delimited text file. The first two columns of the file are "
                             "\"Gene ID\" and \"Gene name\". Each other column contains \"FPKM\" values for "
                             "the genes from an input gene abundance file."
                             "<br><br>Provide URL(s) to the StringTie gene abundance file(s) to the input "
                             "port of the element."));

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new StringtieGeneAbundanceReportPrompter(NULL));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_RNA_SEQ(), proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new StringtieGeneAbundanceReportWorkerFactory());
}

void StringtieGeneAbundanceReportWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

}   // namespace LocalWorkflow
}   // namespace U2
