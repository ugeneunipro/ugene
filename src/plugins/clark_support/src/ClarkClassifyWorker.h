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

#ifndef _U2_CLARK_WORKER_H_
#define _U2_CLARK_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrl.h>

#include "../../ngs_reads_classification/src/TaxonomySupport.h"

namespace U2 {
namespace LocalWorkflow {

class ClarkClassifySettings {
public:
    static const QString TOOL_DEFAULT;
    static const QString TOOL_LIGHT;

    enum Rank {
        // NB: values follow Clark definitions!
        Species=0, Genus, Family, Order, Class, Phylum
    };

    enum Mode {
        // NB: values follow Clark definitions!
        Full=0, Default, Express, Spectrum
    };

    ClarkClassifySettings();

    QString databaseUrl;
    QString tool;
    int gap;
    int factor;
    int minFreqTarget;
    int kmerSize;
    int numberOfThreads;
    bool extOut;
    bool preloadDatabase;
    Mode mode;
};

//////////////////////////////////////////////////
//ClarkClassify
class ClarkClassifyPrompter;
typedef PrompterBase<ClarkClassifyPrompter> ClarkClassifyBase;

class ClarkClassifyPrompter : public ClarkClassifyBase {
    Q_OBJECT
public:
    ClarkClassifyPrompter(Actor* p = 0) : ClarkClassifyBase(p) {}
protected:
    QString composeRichDoc();
};

class ClarkClassifyWorker: public BaseWorker {
    Q_OBJECT
public:
    ClarkClassifyWorker(Actor *a);
protected:
    void init();
    Task * tick();
    void cleanup();

private slots:
    void sl_taskFinished(Task *task);

protected:
    IntegralBus *input;
    IntegralBus *output;
    ClarkClassifySettings cfg;
    bool paired;
};

class ClarkClassifyValidator : public ActorValidator {
    Q_DECLARE_TR_FUNCTIONS(ClarkClassifyValidator)
public:
    bool validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString>& options) const;

private:
    bool validateDatabase(const Actor *actor, NotificationsList &notificationList) const;
    bool validateRefseqAvailability(const Actor *actor, NotificationsList &notificationList) const;

    bool checkRefseqAvailability(const Actor *actor, NotificationsList &notificationList, const QString &dataPathId) const;
    bool isDatabaseAlreadyBuilt(const Actor *actor) const;
};

class ClarkClassifyWorkerFactory : public DomainFactory {
public:
    static void init();
    static void cleanup();
    ClarkClassifyWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new ClarkClassifyWorker(a); }

    static const QString ACTOR_ID;

    static const QString INPUT_PORT;
    static const QString PAIRED_INPUT_PORT;

    static const QString INPUT_SLOT;
    static const QString PAIRED_INPUT_SLOT;

    static const QString OUTPUT_PORT;

    static const QString TOOL_VARIANT;
    static const QString DB_URL;
    static const QString OUTPUT_URL;
    static const QString TAXONOMY;
    static const QString TAXONOMY_RANK;
    static const QString K_LENGTH;
    static const QString K_MIN_FREQ;
    static const QString MODE;
    static const QString FACTOR;
    static const QString GAP;
    static const QString EXTEND_OUT;
    static const QString DB_TO_RAM;
    static const QString NUM_THREADS;
    static const QString SEQUENCING_READS;

    static const QString SINGLE_END;
    static const QString PAIRED_END;

    static const QString WORKFLOW_CLASSIFY_TOOL_CLARK;
};

class ClarkLogParser : public ExternalToolLogParser {
    Q_OBJECT
public:
    ClarkLogParser();

private:
    bool isError(const QString &line) const override;
    void setLastError(const QString &errorKey) override;
    static QMap<QString, QString> initWellKnownErrors();

    static const QMap<QString, QString> wellKnownErrors;
};

class ClarkClassifyTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    ClarkClassifyTask(const ClarkClassifySettings &cfg, const QString &readsUrl, const QString &pairedReadsUrl, const QString &reportUrl);

    const QString &getReportUrl() const {return reportUrl;}
    const TaxonomyClassificationResult &getParsedReport() const;
private:
    void prepare() override;
    void run() override;
    QStringList getArguments();

    const ClarkClassifySettings cfg;
    const QString readsUrl;
    const QString pairedReadsUrl;
    QString reportUrl;
    TaxonomyClassificationResult parsedReport;
};

} //LocalWorkflow
} //U2

Q_DECLARE_METATYPE(U2::LocalWorkflow::ClarkClassifySettings::Mode)

#endif //_U2_CLARK_WORKER_H_
