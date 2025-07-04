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

#include <QList>
#include <QMap>

#include <U2Core/AnnotationData.h>

#include <U2Designer/PrompterBase.h>

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class GeneByGeneReportWorker : public BaseWorker {
    Q_OBJECT
public:
    GeneByGeneReportWorker(Actor* p);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

    // for files in the report
    virtual QStringList getOutputFiles();

private:
    IntegralBus* inChannel;
    QStringList outFiles;
    QMap<QString, QPair<DNASequence, QList<SharedAnnotationData>>> geneData;

private slots:
    void sl_taskFinished();
};  // GeneByGeneReportWorker

class GeneByGeneReportWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    GeneByGeneReportWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    static void init();
    virtual Worker* createWorker(Actor* a);
};  // GeneByGeneReportWorkerFactory

class GeneByGeneReportPrompter : public PrompterBase<GeneByGeneReportPrompter> {
    Q_OBJECT
public:
    GeneByGeneReportPrompter(Actor* p = nullptr)
        : PrompterBase<GeneByGeneReportPrompter>(p) {
    }

protected:
    QString composeRichDoc();

};  // GeneByGeneReportPrompter

}  // namespace LocalWorkflow
}  // namespace U2
