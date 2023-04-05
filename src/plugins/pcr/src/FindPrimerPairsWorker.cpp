/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "FindPrimerPairsWorker.h"

#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileFilters.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "TmCalculatorDelegate.h"

namespace U2 {
namespace LocalWorkflow {

/************************************************************************/
/* FindPrimerPairsPrompter */
/************************************************************************/
QString FindPrimerPairsPrompter::composeRichDoc() {
    QString res;

    auto readsProducer = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()))->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());

    QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    QString readsUrl = readsProducer ? readsProducer->getLabel() : unsetStr;

    return tr("Find correct pairs over primers from \"%1\".").arg(readsUrl);
}

/************************************************************************/
/* FindPrimerPairsWorker */
/************************************************************************/
void FindPrimerPairsWorker::init() {
    inPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
}

void FindPrimerPairsWorker::cleanup() {
}

Task* FindPrimerPairsWorker::tick() {
    if (inPort->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(inPort);
        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler seqId = qm.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return nullptr;
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));
        data.append(seq);
    }
    if (!inPort->hasMessage() && inPort->isEnded()) {
        QString reportFileUrl = getValue<QString>(FindPrimerPairsWorkerFactory::OUT_FILE);
        auto tempSettings = getValue<QVariantMap>(FindPrimerPairsWorkerFactory::TEMPERATURE_SETTINGS_ID);
        auto TmCalculator = AppContext::getTmCalculatorRegistry()->createTmCalculator(getValue<QVariantMap>(FindPrimerPairsWorkerFactory::TEMPERATURE_SETTINGS_ID));
        Task* t = new FindPrimersTask(reportFileUrl, data, TmCalculator);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onTaskFinished(Task*)));
        return t;
    }
    return nullptr;
}

void FindPrimerPairsWorker::sl_onTaskFinished(Task* t) {
    QString reportFileUrl = getValue<QString>(FindPrimerPairsWorkerFactory::OUT_FILE);
    auto findTask = qobject_cast<FindPrimersTask*>(t);

    if (!findTask->hasError() && !findTask->isCanceled()) {
        if (!findTask->getReport().isEmpty()) {
            context->getMonitor()->addOutputFile(reportFileUrl, getActor()->getId(), true);
        } else {
            context->getMonitor()->addError(tr("No correct primers pairs found"), getActor()->getId(), WorkflowNotification::U2_WARNING);
        }
    }

    setDone();
}

/************************************************************************/
/* FindPrimerPairsWorkerFactory */
/************************************************************************/
const QString FindPrimerPairsWorkerFactory::ACTOR_ID("find-primers");
const QString FindPrimerPairsWorkerFactory::OUT_FILE("output-file");
const QString FindPrimerPairsWorkerFactory::TEMPERATURE_SETTINGS_ID("temperature-settings");

void FindPrimerPairsWorkerFactory::init() {
    QList<PortDescriptor*> p;
    QList<Attribute*> a;
    {
        Descriptor id(BasePorts::IN_SEQ_PORT_ID(),
                      FindPrimerPairsWorker::tr("Input sequences"),
                      FindPrimerPairsWorker::tr("Set of primers, which must be tested."));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("findPrimers.seq", inM)), true, false, IntegralBusPort::BLIND_INPUT);
    }

    Descriptor desc(FindPrimerPairsWorkerFactory::ACTOR_ID,
                    FindPrimerPairsWorker::tr("Find correct primer pairs"),
                    FindPrimerPairsWorker::tr("Find correct primer pairs, which consist of valid primers without dimers."));

    Descriptor reportFileDesc(FindPrimerPairsWorkerFactory::OUT_FILE,
                              FindPrimerPairsWorker::tr("Output report file"),
                              FindPrimerPairsWorker::tr("Path to the report output file."));

    Descriptor temperatureDesc(FindPrimerPairsWorkerFactory::TEMPERATURE_SETTINGS_ID,
                               FindPrimerPairsWorker::tr("Temperature settings"),
                               FindPrimerPairsWorker::tr("Set up temperature calculation method."));

    QList<Attribute*> attrs;
    attrs << new Attribute(reportFileDesc, BaseTypes::STRING_TYPE(), true);
    attrs << new Attribute(temperatureDesc, BaseTypes::MAP_TYPE(), false);

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, attrs);

    QMap<QString, PropertyDelegate*> delegates;
    QString filter = FileFilters::createFileFilter(FindPrimerPairsWorker::tr("Report file"), {"html"});
    DelegateTags tags;
    tags.set("filter", filter);
    tags.set("extensions", {"html"});

    delegates[OUT_FILE] = new URLDelegate(tags, "");
    delegates[TEMPERATURE_SETTINGS_ID] = new TmCalculatorDelegate;

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new FindPrimerPairsPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new FindPrimerPairsWorkerFactory());
}

}  // namespace LocalWorkflow

/************************************************************************/
/* FindPrimersTask */
/************************************************************************/
FindPrimersTask::FindPrimersTask(const QString& outputFileUrl, const QList<DNASequence>& sequences, const QSharedPointer<TmCalculator>& _temperatureCalculator)
    : Task(tr("FindPrimersTask"), TaskFlag_None),
      sequences(sequences),
      temperatureCalculator(_temperatureCalculator),
      outputUrl(outputFileUrl) {
    SAFE_POINT(temperatureCalculator != nullptr, "FindPrimersTask: temperatureCalculator is null", )
}

void FindPrimersTask::run() {
    CHECK(!sequences.isEmpty(), );

    QList<DNASequence> correctPrimers;
    for (int i = 0; i < sequences.size(); i++) {
        if (isCanceled()) {
            return;
        }
        PrimerStatisticsCalculator calc(sequences.at(i).constData(), temperatureCalculator);
        if (calc.getFirstError().isEmpty()) {
            correctPrimers.append(sequences.at(i));
        }
        stateInfo.setProgress(i * 10 / sequences.size());
    }

    for (int i = 0; i < correctPrimers.size(); i++) {
        if (isCanceled()) {
            return;
        }
        stateInfo.setProgress(10 + i * 90 / correctPrimers.size());
        for (int j = i + 1; j < correctPrimers.size(); j++) {
            QByteArray forwardPrimer = correctPrimers.at(i).constData();
            QByteArray reversePrimer = correctPrimers.at(j).constData();

            PrimersPairStatistics calc(forwardPrimer, reversePrimer, temperatureCalculator);

            if (calc.getFirstError().isEmpty()) {
                QString newRow = createRow(correctPrimers.at(i).getName(), correctPrimers.at(j).getName(), calc.getForwardCalculator().getTm(), calc.getReverseCalculator().getTm());
                rows.append(newRow);
            }
        }
    }
    if (!rows.isEmpty()) {
        createReport();
        writeReportToFile();
    }
}

void FindPrimersTask::createReport() {
    CHECK(!rows.isEmpty(), );

    report += "<!DOCTYPE html>\n";
    report += "<html>\n";
    report += "<body>\n";
    report += "<h2>" + tr("Correct primers pairs") + "</h2><br>\n";

    report += "<table>";
    report += "<tr>";
    report += createColumn(LocalWorkflow::FindPrimerPairsWorker::tr("Forward primer name"));
    report += createColumn(LocalWorkflow::FindPrimerPairsWorker::tr("Reverse primer name"));
    report += createColumn(LocalWorkflow::FindPrimerPairsWorker::tr("Forward Tm"));
    report += createColumn(LocalWorkflow::FindPrimerPairsWorker::tr("Reverse Tm"));
    report += "</tr>";

    foreach (const QString& curRow, rows) {
        report += curRow;
    }

    report += "</table>";
    report += "</body>\n";
    report += "</html>\n";
}

void FindPrimersTask::writeReportToFile() {
    IOAdapterId ioAdapterId = IOAdapterUtils::url2io(outputUrl);
    IOAdapterFactory* ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioAdapterId);
    CHECK_EXT(ioAdapterFactory != nullptr, setError(tr("No IO adapter found for URL: %1").arg(outputUrl)), );

    QScopedPointer<IOAdapter> ioAdapter(ioAdapterFactory->createIOAdapter());

    if (!ioAdapter->open(outputUrl, IOAdapterMode_Write)) {
        setError(L10N::errorOpeningFileWrite(outputUrl));
        return;
    }

    if (0 == ioAdapter->writeBlock(report.toLocal8Bit())) {
        setError(L10N::errorWritingFile(outputUrl));
    }

    ioAdapter->close();
}

QString FindPrimersTask::createRow(const QString& forwardName, const QString& reverseName, double forwardTm, double reverseTm) {
    QString newRow;
    newRow += "<tr>";
    newRow += createCell(forwardName);
    newRow += createCell(reverseName);
    newRow += createCell(QString::number(forwardTm));
    newRow += createCell(QString::number(reverseTm));
    newRow += "</tr>";
    return newRow;
}

QString FindPrimersTask::createCell(const QString& value) {
    return QString("<td align=\"center\">%1</td>").arg(value);
}

QString FindPrimersTask::createColumn(const QString& name) {
    return QString(R"(<th width="30%"/><p align="center"><strong>%2</strong></p></th>)").arg(name);
}

}  // namespace U2
