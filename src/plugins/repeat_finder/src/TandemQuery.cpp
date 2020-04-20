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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Lang/BaseTypes.h>
#include <U2Designer/DelegateEditors.h>

#include "TandemQuery.h"

namespace U2 {

namespace {
    const QString MIN_PERIOD_ATTRIBUTE = "min-period";
    const QString MAX_PERIOD_ATTRIBUTE = "max-period";
    const QString ALGORITHM_ATTRIBUTE = "algorithm";
    const QString MIN_TANDEM_SIZE_ATTRIBUTE = "min-tandem-size";
    const QString MIN_REPEAT_COUNT_ATTRIBUTE = "min-repeat-count";
    const QString SHOW_OVERLAPPED_TANDEMS_ATTRIBUTE = "show-overlapped-tandems";
    const QString N_THREADS_ATTRIBUTE = "n-threads";

    const QString ALGORITHM_SUFFIX = "suffix";
    const QString ALGORITHM_SUFFIX_BINARY = "suffix-binary";
}

// QDTandemActor

int QDTandemActor::getMinResultLen() const {
    return cfg->getParameter(MIN_TANDEM_SIZE_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
}

int QDTandemActor::getMaxResultLen() const {
    return DEFAULT_MAX_RESULT_LENGTH;
}

QString QDTandemActor::getText() const {

    int minPeriod = cfg->getParameter(MIN_PERIOD_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    QString minPeriodString = QString("<a href=\"%1\">%2</a>").arg(MIN_PERIOD_ATTRIBUTE).arg(minPeriod);

    int maxPeriod = cfg->getParameter(MAX_PERIOD_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    QString maxPeriodString = QString("<a href=\"%1\">%2</a>").arg(MAX_PERIOD_ATTRIBUTE).arg(maxPeriod);

    QString text = QDTandemActor::tr("Searches for tandem repeats with period size between %1 and %2.").arg(minPeriodString).arg(maxPeriodString);

    return text;
}

bool QDTandemActor::hasStrand() const {
    return false;
}

Task *QDTandemActor::getAlgorithmTask(const QVector<U2Region> &location) {
    settings.minPeriod = cfg->getParameter(MIN_PERIOD_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    settings.maxPeriod = cfg->getParameter(MAX_PERIOD_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    settings.algo = (TSConstants::TSAlgo)cfg->getParameter(ALGORITHM_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    settings.minTandemSize = cfg->getParameter(MIN_TANDEM_SIZE_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    settings.minRepeatCount = cfg->getParameter(MIN_REPEAT_COUNT_ATTRIBUTE)->getAttributeValueWithoutScript<int>();
    settings.showOverlappedTandems = cfg->getParameter(SHOW_OVERLAPPED_TANDEMS_ATTRIBUTE)->getAttributeValueWithoutScript<bool>();
    settings.nThreads = cfg->getParameter(N_THREADS_ATTRIBUTE)->getAttributeValueWithoutScript<int>();

    const DNASequence& dnaSeq = scheme->getSequence();
    Task *task = new Task(tr("TandemQDTask"), TaskFlag_NoRun);
    foreach(const U2Region &r, location) {
        FindTandemsTaskSettings localSettings(settings);
        localSettings.seqRegion = r;
        TandemFinder *subTask = new TandemFinder(localSettings, dnaSeq);
        task->addSubTask(subTask);
        subTasks.append(subTask);
    }
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_onAlgorithmTaskFinished()));
    return task;
}

void QDTandemActor::sl_onAlgorithmTaskFinished() {
    QList<SharedAnnotationData> annotations;
    {
        const DNASequence& dnaSeq = scheme->getSequence();
        FindTandemsToAnnotationsTask helperTask(settings, dnaSeq, "repeat unit", QString(), "", GObjectReference());
        foreach(TandemFinder *task, subTasks) {
            annotations.append(helperTask.importTandemAnnotations(task->getResults(), task->getSettings().seqRegion.startPos, task->getSettings().showOverlappedTandems));
        }
    }
    subTasks.clear();
    foreach(const SharedAnnotationData &annotation, annotations) {
        QDResultGroup *group = new QDResultGroup(QDStrand_Both);
        foreach(U2Region region, annotation->location->regions) {
            QDResultUnit resultUnit(new QDResultUnitData);
            resultUnit->quals = annotation->qualifiers;
            resultUnit->region = region;
            resultUnit->owner = units.value("tandem");
            resultUnit->strand = annotation->getStrand();
            group->add(resultUnit);
        }
        results.append(group);
    }
}

QList<QPair<QString, QString> > QDTandemActor::saveConfiguration() const {
    QList<QPair<QString, QString> > result = QDActor::saveConfiguration();
    Attribute *algorithmAttribute = cfg->getParameter(ALGORITHM_ATTRIBUTE);
    for (int i = 0;i < result.size();i++) {
        QPair<QString, QString> &attribute = result[i];
        if (algorithmAttribute->getId() == attribute.first) {
            TSConstants::TSAlgo algorithm = (TSConstants::TSAlgo)algorithmAttribute->getAttributeValueWithoutScript<int>();
            switch(algorithm) {
            case TSConstants::AlgoSuffix:
                attribute.second = ALGORITHM_SUFFIX;
                break;
            case TSConstants::AlgoSuffixBinary:
                attribute.second = ALGORITHM_SUFFIX_BINARY;
                break;
            default:
                break;
            }
        }
    }
    return result;
}

void QDTandemActor::loadConfiguration(const QList<QPair<QString, QString> > &strMap) {
    QDActor::loadConfiguration(strMap);
    foreach(const StringAttribute &attribute, strMap) {
        if (ALGORITHM_ATTRIBUTE == attribute.first) {
            int algorithm = TSConstants::AlgoSuffix;
            if(ALGORITHM_SUFFIX == attribute.second) {
                algorithm = TSConstants::AlgoSuffix;
            } else if(ALGORITHM_SUFFIX_BINARY == attribute.second) {
                algorithm = TSConstants::AlgoSuffixBinary;
            }
            cfg->setParameter(ALGORITHM_ATTRIBUTE, qVariantFromValue(algorithm));
        }
    }
}

QColor QDTandemActor::defaultColor() const {
    return QColor(0x66, 0xa3, 0xd2);
}

QDTandemActor::QDTandemActor(QDActorPrototype const *prototype):
    QDActor(prototype)
{
    cfg->setAnnotationKey("repeat_unit");
    units["tandem"] = new QDSchemeUnit(this);
}

// QDTandemActorPrototype

QDTandemActorPrototype::QDTandemActorPrototype() {
    descriptor.setId("tandems");
    descriptor.setDisplayName(QDTandemActor::tr("Tandem Repeats"));
    descriptor.setDocumentation(QDTandemActor::tr("Finds tandem repeats in a supplied sequence, stores found regions as annotations."));

    {
        Descriptor minPeriodDescriptor(MIN_PERIOD_ATTRIBUTE, QDTandemActor::tr("Min period"), QDTandemActor::tr("Minimum acceptable repeat length measured in base symbols."));
        Descriptor maxPeriodDescriptor(MAX_PERIOD_ATTRIBUTE, QDTandemActor::tr("Max period"), QDTandemActor::tr("Maximum acceptable repeat length measured in base symbols."));
        Descriptor algorithmDescriptor(ALGORITHM_ATTRIBUTE, QDTandemActor::tr("Algorithm"), QDTandemActor::tr("The algorithm parameter allows one to select the search algorithm. The default and a fast one is the optimized suffix array algorithm."));
        Descriptor minTandemSizeDescriptor(MIN_TANDEM_SIZE_ATTRIBUTE, QDTandemActor::tr("Min tandem size"), QDTandemActor::tr("The minimum tandem size sets the limit on minimum acceptable length of the tandem, i.e. the minimum total repeats length of the searched tandem."));
        Descriptor minRepeatCountDescriptor(MIN_REPEAT_COUNT_ATTRIBUTE, QDTandemActor::tr("Min repeat count"), QDTandemActor::tr("The minimum number of repeats of a searched tandem."));
        Descriptor showOverlappedTandemsDescriptor(SHOW_OVERLAPPED_TANDEMS_ATTRIBUTE, QDTandemActor::tr("Search for overlapped tandems"), QDTandemActor::tr("If this parameter is set to <i>True</i> then overlapped tandems should be included into the result."));
        Descriptor nThreadsDescriptor(N_THREADS_ATTRIBUTE, QDTandemActor::tr("Parallel threads"), QDTandemActor::tr("Number of parallel threads used for the task."));

        FindTandemsTaskSettings defaultSettings;
        defaultSettings.minRepeatCount = 3; // the default constructor initializes it to an invalid value
        defaultSettings.maxPeriod = 1000000; // the default constructor initializes it to an invalid value

        attributes.append(new Attribute(minPeriodDescriptor, BaseTypes::NUM_TYPE(), true, defaultSettings.minPeriod));
        attributes.append(new Attribute(maxPeriodDescriptor, BaseTypes::NUM_TYPE(), true, defaultSettings.maxPeriod));
        attributes.append(new Attribute(algorithmDescriptor, BaseTypes::NUM_TYPE(), false, defaultSettings.algo));
        attributes.append(new Attribute(minTandemSizeDescriptor, BaseTypes::NUM_TYPE(), false, defaultSettings.minTandemSize));
        attributes.append(new Attribute(minRepeatCountDescriptor, BaseTypes::NUM_TYPE(), false, defaultSettings.minRepeatCount));
        attributes.append(new Attribute(showOverlappedTandemsDescriptor, BaseTypes::BOOL_TYPE(), false, defaultSettings.showOverlappedTandems));
        attributes.append(new Attribute(nThreadsDescriptor, BaseTypes::NUM_TYPE(), false, defaultSettings.nThreads));
    }
    {
        QMap<QString, PropertyDelegate *> delegates;
        {
            QVariantMap properties;
            properties["minimum"] = 1;
            properties["maximum"] = 1000000;
            properties["suffix"] = QDTandemActor::tr(" bp");
            delegates[MIN_PERIOD_ATTRIBUTE] = new SpinBoxDelegate(properties);
        }
        {
            QVariantMap properties;
            properties["minimum"] = 1;
            properties["maximum"] = 1000000;
            properties["suffix"] = QDTandemActor::tr(" bp");
            delegates[MAX_PERIOD_ATTRIBUTE] = new SpinBoxDelegate(properties);
        }
        {
            QVariantMap items;
            items["Suffix index"] = TSConstants::AlgoSuffix;
            items["Suffix index (optimized)"] = TSConstants::AlgoSuffixBinary;
            delegates[ALGORITHM_ATTRIBUTE] = new ComboBoxDelegate(items);
        }
        {
            QVariantMap properties;
            properties["minimum"] = 3;
            properties["maximum"] = 1000000000;
            delegates[MIN_TANDEM_SIZE_ATTRIBUTE] = new SpinBoxDelegate(properties);
        }
        {
            QVariantMap properties;
            properties["minimum"] = 3;
            properties["maximum"] = 1000000000;
            properties["prefix"] = QDTandemActor::tr("x");
            delegates[MIN_REPEAT_COUNT_ATTRIBUTE] = new SpinBoxDelegate(properties);
        }
        {
            QVariantMap properties;
            properties["specialValueText"] = "Auto";
            delegates[N_THREADS_ATTRIBUTE] = new SpinBoxDelegate(properties);
        }
        editor = new DelegateEditor(delegates);
    }
}

QIcon QDTandemActorPrototype::getIcon() const {
    return QIcon(":repeat_finder/images/repeats_tandem.png");
}

QDActor *QDTandemActorPrototype::createInstance() const {
    return new QDTandemActor(this);
}

} // namespace U2
