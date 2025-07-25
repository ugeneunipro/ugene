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

#include <U2Core/GUrl.h>

#include <U2Designer/PrompterBase.h>

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class BedGraphToBigWigPrompter;
typedef PrompterBase<BedGraphToBigWigPrompter> BedGraphToBigWigBase;

class BedGraphToBigWigPrompter : public BedGraphToBigWigBase {
    Q_OBJECT
public:
    BedGraphToBigWigPrompter(Actor* p = 0)
        : BedGraphToBigWigBase(p) {
    }

protected:
    QString composeRichDoc() override;
};  // BedGraphToBigWigPrompter

class BedGraphToBigWigWorker : public BaseWorker {
    Q_OBJECT
public:
    BedGraphToBigWigWorker(Actor* a);
    void init() override;
    Task* tick() override;
    void cleanup() override;

    static const QString INPUT_PORT;
    static const QString OUTPUT_PORT;
    static const QString OUT_MODE_ID;
    static const QString CUSTOM_DIR_ID;
    static const QString OUT_NAME_ID;
    static const QString DEFAULT_NAME;
    static const QString BLOCK_SIZE;
    static const QString ITEMS_PER_SLOT;
    static const QString UNCOMPRESSED;
    static const QString GENOME;

protected:
    QString getDefaultFileName() const {
        return QString(".bigWig");
    }

private:
    IntegralBus* inputUrlPort;
    IntegralBus* outputUrlPort;
    QStringList outUrls;

public slots:
    void sl_taskFinished(Task* task);

private:
    QString takeUrl();
    QString getTargetName(const QString& fileUrl, const QString& outDir);
    void sendResult(const QString& url);
};  // BedGraphToBigWigWorker

class BedGraphToBigWigFactory : public DomainFactory {
    static const QString ACTOR_ID;

public:
    static void init();
    BedGraphToBigWigFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) override {
        return new BedGraphToBigWigWorker(a);
    }
};  // RmdupBamWorkerFactory

}  // namespace LocalWorkflow
}  // namespace U2
