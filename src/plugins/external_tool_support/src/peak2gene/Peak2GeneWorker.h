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

#ifndef _U2_PEAK2GENE_SUPPORT_WORKER_
#define _U2_PEAK2GENE_SUPPORT_WORKER_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "Peak2GeneTask.h"
#include "Peak2GeneSettings.h"
#include "utils/CistromeDelegate.h"

namespace U2 {
namespace LocalWorkflow {

class Peak2GeneWorker : public BaseWorker {
    Q_OBJECT
public:
    Peak2GeneWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();

private:
    IntegralBus *inChannel;
    IntegralBus *output;

private:
    Peak2GeneSettings createPeak2GeneSettings(U2OpStatus &os);

private slots:
    void sl_taskFinished();
}; // Peak2GeneWorker

class Peak2GeneComboBoxWithUrlsDelegate : public CistromeComboBoxWithUrlsDelegate {
public:
    Peak2GeneComboBoxWithUrlsDelegate(const QVariantMap& items, bool _isPath = false, QObject *parent = 0) : CistromeComboBoxWithUrlsDelegate(items, _isPath, parent) {}

private:
    virtual void updateUgeneSettings();

    virtual QString getDataPathName();
    virtual QString getAttributeName();
    virtual QString getDefaultValue() { return "hg19"; }
}; // Peak2GeneComboBoxWithUrlsDelegate

class Peak2GeneWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    Peak2GeneWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);
}; // Peak2GeneWorkerFactory

class Peak2GenePrompter : public PrompterBase<Peak2GenePrompter> {
    Q_OBJECT
public:
    Peak2GenePrompter(Actor *p = NULL) : PrompterBase<Peak2GenePrompter>(p) {}

protected:
    QString composeRichDoc();

}; // Peak2GenePrompter

} // LocalWorkflow
} // U2

#endif // _U2_PEAK2GENE_SUPPORT_WORKER_
