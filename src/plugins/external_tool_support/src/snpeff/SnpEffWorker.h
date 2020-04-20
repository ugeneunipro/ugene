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

#ifndef _U2_SNPEFF_WORKER_H_
#define _U2_SNPEFF_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

class SnpEffPrompter;
typedef PrompterBase<SnpEffPrompter> SnpEffBase;

class SnpEffPrompter : public SnpEffBase {
    Q_OBJECT
public:
    SnpEffPrompter(Actor* p = 0) : SnpEffBase(p) {}
protected:
    QString composeRichDoc();
}; //SnpEffPrompter


class SnpEffWorker: public BaseWorker {
    Q_OBJECT
public:
    SnpEffWorker(Actor *a);
    void init();
    Task * tick();
    void cleanup();

    static const QString BASE_SNPEFF_SUBDIR;

    static const QString INPUT_PORT;
    static const QString OUTPUT_PORT;
    static const QString OUT_MODE_ID;
    static const QString CUSTOM_DIR_ID;

    static const QString INPUT_FORMAT;
    static const QString OUTPUT_FORMAT;
    static const QString GENOME;
    static const QString TEST_DB;
    static const QString UPDOWN_LENGTH;

    static const QString CANON;
    static const QString HGVS;
    static const QString LOF;
    static const QString MOTIF;

private:
    IntegralBus *inputUrlPort;
    IntegralBus *outputUrlPort;

public slots:
    void sl_taskFinished( Task *task );

private:
    QString takeUrl();
    void sendResult(const QString &url);
}; //SnpEffWorker

class SnpEffFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    SnpEffFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new SnpEffWorker(a); }
}; //RmdupBamWorkerFactory


class SnpEffLogProcessor : public ExternalToolLogProcessor {
public:
    SnpEffLogProcessor(WorkflowMonitor *monitor, const QString &actor);

    void processLogMessage(const QString &message);

private:
    void addNotification(const QString &key, int count);
    static StrStrMap initWellKnownMessages();
    static QMap<QString, QRegExp> initWellKnownCatchers();

    WorkflowMonitor *monitor;
    const QString actor;

    static const StrStrMap wellKnownMessages;
    static const QMap<QString, QRegExp> messageCatchers;
};

} //LocalWorkflow
} //U2

#endif //_U2_SNPEFF_WORKER_H_
