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

#ifndef _U2_CONSERVATION_PLOT_SUPPORT_WORKER_
#define _U2_CONSERVATION_PLOT_SUPPORT_WORKER_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "ConservationPlotTask.h"
#include "ConservationPlotSettings.h"
#include "utils/CistromeDelegate.h"

namespace U2 {
namespace LocalWorkflow {

class ConservationPlotWorker : public BaseWorker {
    Q_OBJECT
public:
    ConservationPlotWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();

private:
    IntegralBus *inChannel;
    QList<SharedDbiDataHandler> plotData;

private:
    ConservationPlotSettings createConservationPlotSettings(U2OpStatus &os);

private slots:
    void sl_taskFinished();
}; // ConservationPlotWorker

class ConservationPlotComboBoxWithUrlsDelegate : public CistromeComboBoxWithUrlsDelegate {
public:
    ConservationPlotComboBoxWithUrlsDelegate(const QVariantMap& items, bool _isPath = false, QObject *parent = 0) : CistromeComboBoxWithUrlsDelegate(items, _isPath, parent) {}

private:
    virtual void updateUgeneSettings();

    virtual QString getDataPathName();
    virtual QString getAttributeName();
    virtual QString getDefaultValue() { return "hg19"; }
}; // ConservationPlotComboBoxWithUrlsDelegate

class ConservationPlotWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    ConservationPlotWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);
}; // ConservationPlotWorkerFactory

class ConservationPlotPrompter : public PrompterBase<ConservationPlotPrompter> {
    Q_OBJECT
public:
    ConservationPlotPrompter(Actor *p = NULL) : PrompterBase<ConservationPlotPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // ConservationPlotPrompter

} // LocalWorkflow
} // U2

#endif // _U2_CONSERVATION_PLOT_SUPPORT_WORKER_
