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

#ifndef _U2_CEAS_SUPPORT_WORKER_
#define _U2_CEAS_SUPPORT_WORKER_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "CEASSupportTask.h"
#include "CEASSettings.h"
#include "utils/CistromeDelegate.h"

namespace U2 {
namespace LocalWorkflow {

class CEASReportWorker : public BaseWorker {
    Q_OBJECT
public:
    CEASReportWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();

private:
    CommunicationChannel *inChannel;

private:
    CEASSettings createCEASSettings(U2OpStatus &os);
    CEASTaskSettings createTaskSettings(U2OpStatus &os);

private slots:
    void sl_taskFinished();
}; // CEASReportWorker

class CeasComboBoxWithUrlsDelegate : public CistromeComboBoxWithUrlsDelegate {
public:
    CeasComboBoxWithUrlsDelegate(const QVariantMap& items, bool _isPath = false, QObject *parent = 0) : CistromeComboBoxWithUrlsDelegate(items, _isPath, parent) {}

private:
    virtual void updateUgeneSettings();

    virtual QString getDataPathName();
    virtual QString getAttributeName();
    virtual QString getDefaultValue() { return "hg19"; }
}; // CeasComboBoxWithUrlsDelegate

class CEASReportWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    CEASReportWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);
}; // CEASReportWorkerFactory

class CEASReportPrompter : public PrompterBase<CEASReportPrompter> {
    Q_OBJECT
public:
    CEASReportPrompter(Actor *p = NULL) : PrompterBase<CEASReportPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // CEASReportPrompter

} // LocalWorkflow
} // U2

#endif // _U2_CEAS_SUPPORT_WORKER_
