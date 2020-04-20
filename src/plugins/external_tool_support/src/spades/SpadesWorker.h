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

#ifndef _U2_SPADES_WORKER_
#define _U2_SPADES_WORKER_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/DatasetFetcher.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

class GenomeAssemblyTaskSettings;

namespace U2 {
namespace LocalWorkflow {

class SpadesWorker : public BaseWorker {
    Q_OBJECT
public:
    SpadesWorker(Actor *p);

    static const QString DATASET_TYPE_STANDARD_ISOLATE;
    static const QString DATASET_TYPE_MDA_SINGLE_CELL;

    static const QString RUNNING_MODE_ERROR_CORRECTION_AND_ASSEMBLY;
    static const QString RUNNING_MODE_ASSEMBLY_ONLY;
    static const QString RUNNING_MODE_ERROR_CORRECTION_ONLY;

    static const QString K_MER_AUTO;

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();
    virtual bool isReady() const;

private:
    bool processInputMessagesAndCheckReady();
    void trySetDone(U2OpStatus &os);

    QList<DatasetFetcher> readsFetchers;
    QList<IntegralBus*> inChannels;
    IntegralBus *output;

private:
    GenomeAssemblyTaskSettings getSettings(U2OpStatus &os);

private slots:
    void sl_taskFinished();
}; // SpadesWorker

class SpadesWorkerFactory : public DomainFactory {
    Q_DECLARE_TR_FUNCTIONS(SpadesWorkerFactory)
public:
    SpadesWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

    static int getReadsUrlSlotIdIndex(const QString& portId, bool& isPaired);

    static const QString ACTOR_ID;

    static const QStringList READS_URL_SLOT_ID_LIST;
    static const QStringList READS_PAIRED_URL_SLOT_ID_LIST;

    static const QStringList IN_TYPE_ID_LIST;

    static const QString OUT_TYPE_ID;

    static const QString SCAFFOLD_OUT_SLOT_ID;
    static const QString CONTIGS_URL_OUT_SLOT_ID;

    static const QString SEQUENCING_PLATFORM_ID;

    static const QString IN_PORT_ID_SINGLE_UNPAIRED;
    static const QString IN_PORT_ID_SINGLE_CSS;
    static const QString IN_PORT_ID_SINGLE_CLR;
    static const QString IN_PORT_ID_SINGLE_NANOPORE;
    static const QString IN_PORT_ID_SINGLE_SANGER;
    static const QString IN_PORT_ID_SINGLE_TRUSTED;
    static const QString IN_PORT_ID_SINGLE_UNTRUSTED;
    static const QString IN_PORT_ID_PAIR_DEFAULT;
    static const QString IN_PORT_ID_PAIR_MATE;
    static const QString IN_PORT_ID_PAIR_HQ_MATE;

    static const QStringList IN_PORT_ID_LIST;
    static const QStringList IN_PORT_PAIRED_ID_LIST;

    static const QString MAP_TYPE_ID;

    static const QString OUT_PORT_DESCR;

    static const QString OUTPUT_DIR;

    static const QString BASE_SPADES_SUBDIR;

    static const QString getPortNameById(const QString& portId);

    static const StrStrMap PORT_ID_2_YAML_LIBRARY_NAME;
    static StrStrMap getPortId2YamlLibraryName();

}; // SpadesWorkerFactory

class SpadesPrompter : public PrompterBase<SpadesPrompter> {
    Q_OBJECT
public:
    SpadesPrompter(Actor *p = NULL) : PrompterBase<SpadesPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // SpadesPrompter

} // LocalWorkflow
} // U2

#endif // _U2_SPADES_WORKER_
