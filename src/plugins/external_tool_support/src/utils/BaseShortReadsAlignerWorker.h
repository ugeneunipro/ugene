/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Algorithm/DnaAssemblyTask.h>

#include <U2Core/GUrl.h>

#include <U2Designer/PrompterBase.h>

#include <U2Lang/DatasetFetcher.h>
#include <U2Lang/LocalDomain.h>

namespace U2 {

class DnaAssemblyToRefTaskSettings;
class DnaAssemblyToReferenceTask;

namespace LocalWorkflow {

const QString IN_PORT_DESCR("in-data");
const QString REFERENCE_GENOME("reference");
const QString REFERENCE_INPUT_TYPE = "reference-input-type";
const QString INDEX_DIR("index-dir");
const QString INDEX_BASENAME("index-basename");

class BaseShortReadsAlignerWorker : public BaseWorker {
    Q_OBJECT
public:
    BaseShortReadsAlignerWorker(Actor* a, const QString& algName);
    void init() override;
    Task* tick() override;
    void cleanup() override;
    bool isReady() const override;

protected:
    virtual QVariantMap getCustomParameters() const {
        return QVariantMap();
    }
    DnaAssemblyToRefTaskSettings getSettings(U2OpStatus& os);
    virtual void setGenomeIndex(DnaAssemblyToRefTaskSettings& settings) = 0;
    virtual QString getDefaultFileName() const = 0;
    virtual QString getBaseSubdir() const = 0;
    QList<ShortReadSet> toUrls(const QList<Message>& messages, const QString& urlSlotId, ShortReadSet::LibraryType libType, ShortReadSet::MateOrder order) const;
    bool isReadyToRun() const;
    bool dataFinished() const;
    QString checkPairedReads() const;
    virtual QString getAlignerSubdir() const;

protected:
    QString algName;
    IntegralBus* inChannel;
    IntegralBus* inPairedChannel;
    IntegralBus* output;
    bool pairedReadsInput;
    bool filterUnpaired;

    DatasetFetcher readsFetcher;
    DatasetFetcher pairedReadsFetcher;

public slots:
    void sl_taskFinished();
};

class ShortReadsAlignerSlotsValidator : public PortValidator {
public:
    bool validate(const IntegralBusPort* port, NotificationsList& notificationList) const override;
};

class BaseShortReadsAlignerWorkerFactory : public DomainFactory {
protected:
    BaseShortReadsAlignerWorkerFactory(const QString& actorId)
        : DomainFactory(actorId) {
    }

    static QList<PortDescriptor*> getPortDescriptors();

    static void addCommonAttributes(QList<Attribute*>& attrs, QMap<QString, PropertyDelegate*>& delegates, const QString& descrIndexFolder, const QString& descrIndexBasename);

    static int getThreadsCount();
};

class ShortReadsAlignerPrompter : public PrompterBase<ShortReadsAlignerPrompter> {
    Q_OBJECT
public:
    ShortReadsAlignerPrompter(Actor* p = nullptr)
        : PrompterBase<ShortReadsAlignerPrompter>(p) {
    }

protected:
    QString composeRichDoc() override;
};

}  // namespace LocalWorkflow
}  // namespace U2
