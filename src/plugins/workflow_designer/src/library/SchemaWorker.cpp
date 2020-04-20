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

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/WorkflowEnv.h>

#include "SchemaWorker.h"

namespace U2 {
namespace LocalWorkflow {

SchemaWorker::SchemaWorker(Actor *a)
: BaseWorker(a)
{

}

bool SchemaWorker::isReady() const {
    return true;
}

bool SchemaWorker::isDone() const {
    return true;
}

Task *SchemaWorker::tick() {
    return NULL;
}

void SchemaWorker::init() {

}

void SchemaWorker::cleanup() {

}

QString SchemaWorkerPrompter::composeRichDoc() {
    return SchemaWorker::tr("Some workflow is in this element.");
}

bool SchemaWorkerFactory::init(Schema *schema, const QString &name, const QString &actorFilePath) {
    ActorPrototype *proto = IncludedProtoFactory::getSchemaActorProto(schema, name, actorFilePath);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_SNP_ANNOTATION(), proto);
    return true;
}

} // LocalWorkflow
} // U2
