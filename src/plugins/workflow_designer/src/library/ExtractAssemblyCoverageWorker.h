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

#ifndef _U2_EXTRACT_ASSEMBLY_COVERAGE_WORKER_H_
#define _U2_EXTRACT_ASSEMBLY_COVERAGE_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

class ExportCoverageSettings;
class ExportCoverageTask;

namespace LocalWorkflow {

class ExtractAssemblyCoverageWorker : public BaseWorker {
    Q_OBJECT
public:
    ExtractAssemblyCoverageWorker(Actor *actor);

    void init();
    Task * tick();
    void cleanup();

private slots:
    void sl_taskFinished();

private:
    bool hasAssembly() const;
    U2EntityRef takeAssembly(U2OpStatus &os);
    ExportCoverageSettings getSettings() const;
    Task *createTask(const U2EntityRef &assembly);
    void finish();
};

class ExtractAssemblyCoverageWorkerFactory : public DomainFactory {
public:
    ExtractAssemblyCoverageWorkerFactory();

    Worker *createWorker(Actor *actor);

    static void init();

    static const QString ACTOR_ID;
    static const QString EXPORT_COVERAGE;
    static const QString EXPORT_BASES_QUANTITY;
};

class ExtractAssemblyCoverageWorkerPrompter : public PrompterBase<ExtractAssemblyCoverageWorkerPrompter>{
    Q_OBJECT
public:
    ExtractAssemblyCoverageWorkerPrompter(Actor *actor = NULL);

protected:
    QString composeRichDoc();
};

/**
 * Since output formats are not registered in any registry, FileExtensionRelation can't be used
*/
class ExtractAssemblyCoverageFileExtensionRelation : public AttributeRelation {
public:
    ExtractAssemblyCoverageFileExtensionRelation(const QString &relatedAttrId);

    QVariant getAffectResult(const QVariant &influencingValue,
                             const QVariant &dependentValue,
                             DelegateTags *infTags,
                             DelegateTags *depTags) const;
    void updateDelegateTags(const QVariant &influencingValue, DelegateTags *dependentTags) const;
    RelationType getType() const;
    ExtractAssemblyCoverageFileExtensionRelation *clone() const;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_EXTRACT_ASSEMBLY_COVERAGE_WORKER_H_
