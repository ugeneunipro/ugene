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

#include "PerfMonitorView.h"

#include <QVBoxLayout>

#include <U2Core/Counter.h>
#include <U2Core/Timer.h>

#ifdef Q_OS_LINUX
#    include <sys/sysinfo.h>
#endif

#ifdef Q_OS_WIN32
#    include <Psapi.h>
#endif

namespace U2 {

#ifdef Q_OS_LINUX
static GCounter virtMemoryCounter("PerfMonitor::VIRTmemoryUsage", "mbytes", 0, 1024 * 1024);
#endif
#ifdef Q_OS_WIN32
static GCounter memoryCounter("PerfMonitor::memoryUsage", "mbytes", 0, 1024 * 1024);
#endif

PerfMonitorView::PerfMonitorView()
    : MWMDIWindow(tr("Application counters")) {
    tree = new QTreeWidget();
    tree->setColumnCount(4);
    tree->setSortingEnabled(true);

    QTreeWidgetItem* treeHeader = tree->headerItem();
    treeHeader->setText(0, tr("Name"));
    treeHeader->setText(1, tr("Value"));
    treeHeader->setText(2, tr("Scale"));
    treeHeader->setText(3, tr("Reportable"));

    QVBoxLayout* viewLayout = new QVBoxLayout();
    viewLayout->setMargin(0);
    viewLayout->addWidget(tree);
    setLayout(viewLayout);

#ifdef Q_OS_LINUX
    struct sysinfo usage;
    sysinfo(&usage);
    virtMemoryCounter.value = usage.totalram;
#endif
#ifdef Q_OS_WIN32
    PROCESS_MEMORY_COUNTERS memCounter;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
    memoryCounter.value = memCounter.WorkingSetSize;
#endif

    updateCounters();

    startTimer(1000);
}

void PerfMonitorView::timerEvent(QTimerEvent*) {
#ifdef Q_OS_LINUX
    struct sysinfo usage;
    sysinfo(&usage);
    virtMemoryCounter.value = usage.totalram;
#endif
#ifdef Q_OS_WIN32
    PROCESS_MEMORY_COUNTERS memCounter;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
    memoryCounter.value = memCounter.WorkingSetSize;
#endif
    updateCounters();
}

void PerfMonitorView::updateCounters() {
    GTIMER(c1, t1, "PerfMonitor::updateCounters");
    bool hasNewCounters = false;
    const QList<GCounter*> counterList = GCounter::getAllCounters();
    for (const GCounter* counters : qAsConst(counterList)) {
        PerfTreeItem* ci = findCounterItem(counters);
        if (ci == nullptr) {
            ci = new PerfTreeItem(counters);
            tree->addTopLevelItem(ci);
            hasNewCounters = true;
        }
        ci->update();
    }

    if (hasNewCounters) {
        for (int i = 0; i < tree->columnCount(); i++) {
            tree->resizeColumnToContents(i);
        }
    }
}

PerfTreeItem* PerfMonitorView::findCounterItem(const GCounter* c) const {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto counterTreeItem = static_cast<PerfTreeItem*>(tree->topLevelItem(i));
        if (counterTreeItem->counter == c) {
            return counterTreeItem;
        }
    }
    return nullptr;
}

PerfTreeItem::PerfTreeItem(const GCounter* counter)
    : counter(counter) {
    update();
}

void PerfTreeItem::update() {
    setText(0, counter->name);
    setText(1, QString::number(counter->getScaledValue(), 'f'));
    setText(2, counter->suffix);
    setText(3, counter->isReportable ? "Yes" : "No");
}
}  // namespace U2
