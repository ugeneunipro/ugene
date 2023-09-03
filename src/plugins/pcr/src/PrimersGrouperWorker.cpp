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

#include "PrimersGrouperWorker.h"

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

namespace U2 {
namespace LocalWorkflow {

/************************************************************************/
/* PrimersGrouperPromter */
/************************************************************************/
QString PrimersGrouperPromter::composeRichDoc() {
    QString res;

    auto readsProducer = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()))->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());

    QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    QString readsUrl = readsProducer ? readsProducer->getLabel() : unsetStr;

    QString resultFile = getParameter(PrimersGrouperWorkerFactory::OUT_FILE).toString();
    resultFile = resultFile.isEmpty() ? unsetStr : resultFile;

    return tr("Read primers from <u>%1</u> by pairs. Select groups of primer pairs, which can be simultaneously used in one reaction tube."
              " Save result to <u>%2</u>.")
        .arg(readsUrl)
        .arg(resultFile);
}

/************************************************************************/
/* PrimersGrouperWorker */
/************************************************************************/
void PrimersGrouperWorker::init() {
    inPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
}

void PrimersGrouperWorker::cleanup() {
}

Task* PrimersGrouperWorker::tick() {
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
        QString reportFileUrl = getValue<QString>(PrimersGrouperWorkerFactory::OUT_FILE);
        Task* t = new PrimerGrouperTask(reportFileUrl, data);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onTaskFinished(Task*)));
        return t;
    }
    return nullptr;
}

void PrimersGrouperWorker::sl_onTaskFinished(Task* t) {
    QString reportFileUrl = getValue<QString>(PrimersGrouperWorkerFactory::OUT_FILE);
    auto grouperTask = qobject_cast<PrimerGrouperTask*>(t);

    if (!grouperTask->hasError() && !grouperTask->isCanceled()) {
        if (!grouperTask->getReport().isEmpty()) {
            context->getMonitor()->addOutputFile(reportFileUrl, getActor()->getId(), true);
        } else {
            context->getMonitor()->addError(tr("No valid groups found"), getActor()->getId(), WorkflowNotification::U2_WARNING);
        }
    }

    setDone();
}

/************************************************************************/
/* PrimersGrouperWorkerFactory */
/************************************************************************/
const QString PrimersGrouperWorkerFactory::ACTOR_ID("primers-grouper");
const QString PrimersGrouperWorkerFactory::OUT_FILE("output-file");

void PrimersGrouperWorkerFactory::init() {
    QList<PortDescriptor*> p;
    QList<Attribute*> a;
    {
        Descriptor id(BasePorts::IN_SEQ_PORT_ID(),
                      PrimersGrouperWorker::tr("Primer pairs"),
                      PrimersGrouperWorker::tr("Pairs of primers, which must be grouped."));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("groupPrimers.seq", inM)), true, false, IntegralBusPort::BLIND_INPUT);
    }

    Descriptor desc(PrimersGrouperWorkerFactory::ACTOR_ID,
                    PrimersGrouperWorker::tr("Group Primer Pairs"),
                    PrimersGrouperWorker::tr("Select groups of primer pairs, which can be simultaneously used in one reaction tube."
                                             "<p>The primers must be supplied in the following order: pair1_direct_primer, pair1_reverse_primer, "
                                             "pair2_direct_primer, pair2_reverse_primer, etc."));

    Descriptor reportFileDesc(PrimersGrouperWorkerFactory::OUT_FILE,
                              PrimersGrouperWorker::tr("Output report file"),
                              PrimersGrouperWorker::tr("Path to the report output file."));

    QList<Attribute*> attrs;
    attrs << new Attribute(reportFileDesc, BaseTypes::STRING_TYPE(), true);

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, attrs);

    QMap<QString, PropertyDelegate*> delegates;
    QString filter = FileFilters::createFileFilter(PrimersGrouperWorker::tr("Report file"), {"html"}, false);
    DelegateTags tags;
    tags.set("filter", filter);
    tags.set("extensions", {"html"});

    delegates[OUT_FILE] = new URLDelegate(tags, "");

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new PrimersGrouperPromter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new PrimersGrouperWorkerFactory());
}

}  // namespace LocalWorkflow

/************************************************************************/
/* PrimerGrouperTask */
/************************************************************************/
PrimerGrouperTask::PrimerGrouperTask(const QString& outputFileUrl, const QList<DNASequence>& sequences)
    : Task(tr("Primer grouper task"), TaskFlag_None), outputUrl(outputFileUrl) {
    for (int i = 0; i < sequences.size() - 1; i += 2) {
        primerPairs.append(PrimersPair(sequences.at(i), sequences.at(i + 1)));
    }
}

static bool groupsCompareFunction(const QList<int>& firstGroup, const QList<int>& secondGroup) {
    if (firstGroup.size() == secondGroup.size()) {
        for (int i = 0; i < firstGroup.size(); i++) {
            if (firstGroup[i] == secondGroup[i]) {
                continue;
            }
            return firstGroup[i] > secondGroup[i];
        }
    }
    return firstGroup.size() > secondGroup.size();
}

void PrimerGrouperTask::run() {
    CHECK(!primerPairs.empty(), );
    findCompatibleGroups();  // primersCompatibilityMatrix);
}

bool PrimerGrouperTask::isCompatiblePairs(int firstPairIndex, int secondPairIndex) {
    SAFE_POINT(firstPairIndex >= 0 && firstPairIndex < primerPairs.length(), "Bad first pair index: " + QString::number(firstPairIndex) + ", length: " + QString::number(primerPairs.length()), false);
    SAFE_POINT(secondPairIndex >= 0 && secondPairIndex < primerPairs.length(), "Bad second pair index: " + QString::number(secondPairIndex) + ", length: " + QString::number(primerPairs.length()), false);

    PrimersPair firstPair = primerPairs.at(firstPairIndex);
    PrimersPair secondPair = primerPairs.at(secondPairIndex);

    HeteroDimersFinder forwardCalc(firstPair.first.constData(), secondPair.first.constData());
    HeteroDimersFinder reverseCalc(firstPair.second.constData(), secondPair.second.constData());

    HeteroDimersFinder crossCalc1(firstPair.first.constData(), secondPair.second.constData());
    HeteroDimersFinder crossCalc2(firstPair.second.constData(), secondPair.first.constData());

    return !forwardCalc.getResult().canBeFormed && !reverseCalc.getResult().canBeFormed && !crossCalc1.getResult().canBeFormed && !crossCalc2.getResult().canBeFormed;
}

void PrimerGrouperTask::findCompatibleGroups() {
    algoLog.details("PrimerGrouperTask got pairs: " + QString::number(primerPairs.length()));

    // Start with a sorted set of primer pairs, so the result will be repeatable.
    std::stable_sort(primerPairs.begin(), primerPairs.end(), [](const PrimersPair& p1, const PrimersPair& p2) {
        return (p1.first.getName() + p1.second.getName()) < (p2.first.getName() + p2.second.getName());
    });

    // Add each primer pair (by index) into a compatible group.
    QList<QList<int>> groups;
    for (int primerPairIndex = 0; primerPairIndex < primerPairs.size(); primerPairIndex++) {
        if (isCanceled()) {
            return;
        }
        stateInfo.setProgress(primerPairIndex * 100 / primerPairs.size());

        // Check all already created groups for compatibility.
        bool isPrimerPairAddedToGroup = false;
        for (int groupIndex = 0; groupIndex < groups.size() && !isPrimerPairAddedToGroup; groupIndex++) {
            QList<int>& group = groups[groupIndex];
            for (int i = 0; i < group.length(); i++) {
                int primerPairIndexFromGroup = group[i];
                if (isCompatiblePairs(primerPairIndexFromGroup, primerPairIndex)) {
                    isPrimerPairAddedToGroup = true;
                    group.append(primerPairIndexFromGroup);
                    break;
                }
            }
        }
        if (!isPrimerPairAddedToGroup) {
            groups.append({primerPairIndex});
        }
    }

    algoLog.details("PrimerGrouperTask: made groups: " + QString::number(groups.size()));
    if (!groups.isEmpty()) {
        std::stable_sort(groups.begin(), groups.end(), groupsCompareFunction);
        createReport(groups);
        writeReportToFile();
    }
}
void PrimerGrouperTask::createReport(const QList<QList<int>>& correctPrimersGroups) {
    CHECK(!correctPrimersGroups.empty(), );

    report += "<!DOCTYPE html>\n";
    report += "<html>\n";
    report += "<body>\n";

    report += "<table bordercolor=\"gray\" border=\"1\">";

    report += "<tr>";
    report += createColumn(LocalWorkflow::PrimersGrouperWorker::tr("Group name"));
    report += createColumn(LocalWorkflow::PrimersGrouperWorker::tr("Forward primer name"));
    report += createColumn(LocalWorkflow::PrimersGrouperWorker::tr("Forward primer sequence"));
    report += createColumn(LocalWorkflow::PrimersGrouperWorker::tr("Reverse primer name"));
    report += createColumn(LocalWorkflow::PrimersGrouperWorker::tr("Reverse primer sequence"));
    report += "</tr>";

    fillReportTable(correctPrimersGroups);

    report += "</table>";

    report += "</body>\n";
    report += "</html>\n";
}

void PrimerGrouperTask::fillReportTable(const QList<QList<int>>& correctPrimersGroups) {
    int index = 1;
    foreach (const QList<int>& curGroup, correctPrimersGroups) {
        QString column1, column2, column3, column4;
        for (int curIndex : qAsConst(curGroup)) {
            const PrimersPair& currentPair = primerPairs.at(curIndex);
            column1 += currentPair.first.getName();
            column1 += "<br>";
            column2 += currentPair.first.constData();
            column2 += "<br>";
            column3 += currentPair.second.getName();
            column3 += "<br>";
            column4 += currentPair.second.constData();
            column4 += "<br>";
        }
        report += createRow(tr("Group %1").arg(index), column1, column2, column3, column4);
        index++;
    }
}

void PrimerGrouperTask::writeReportToFile() {
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

QString PrimerGrouperTask::createRow(const QString& groupName, const QString& forwardName, const QString& reverseName, const QString& forwardTm, const QString& reverseTm) {
    QString newRow;
    newRow += "<tr>";
    newRow += QString("<td valign=\"middle\">%1</td>").arg(groupName);
    newRow += createCell(forwardName);
    newRow += createCell(reverseName);
    newRow += createCell(forwardTm);
    newRow += createCell(reverseTm);
    newRow += "</tr>";
    return newRow;
}

QString PrimerGrouperTask::createCell(const QString& value, bool center, const QString& attrs) {
    const QString align = center ? "center" : "left";
    const QString attrsString = attrs.isEmpty() ? "" : " " + attrs;
    return QString("<td align=\"%1\"%2>%3</td>").arg(align).arg(attrsString).arg(value);
}

QString PrimerGrouperTask::createColumn(const QString& name, const QString& attrs) {
    const QString attrsString = attrs.isEmpty() ? "" : " " + attrs;
    return QString("<th%1/><p align=\"left\"><strong>%2</strong></p></th>").arg(attrsString).arg(name);
}

}  // namespace U2
