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

#include <QScopedPointer>

#include <U2Core/AssemblyObject.h>
#include <U2Core/CloneObjectTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include "CloneAssemblyWithReferenceToDbiTask.h"

namespace U2 {

CloneAssemblyWithReferenceToDbiTask::CloneAssemblyWithReferenceToDbiTask(const U2Assembly &assembly,
                                                                         const U2Sequence &reference,
                                                                         const U2DbiRef &srcDbiRef,
                                                                         const U2DbiRef &dstDbiRef,
                                                                         const QVariantMap &hints) :
    Task(tr("Clone assembly object to the destination database"), TaskFlags_FOSE_COSC),
    assembly(assembly),
    reference(reference),
    srcDbiRef(srcDbiRef),
    dstDbiRef(dstDbiRef),
    dstFolder(hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString()),
    cloneAssemblyTask(NULL),
    cloneReferenceTask(NULL)
{
    SAFE_POINT_EXT(assembly.hasValidId(), setError(tr("Invalid assembly ID")), );
    SAFE_POINT_EXT(reference.hasValidId(), setError(tr("Invalid assembly ID")), );
    SAFE_POINT_EXT(srcDbiRef.isValid(), setError(tr("Invalid source database reference")), );
    SAFE_POINT_EXT(dstDbiRef.isValid(), setError(tr("Invalid destination database reference")), );
}

void CloneAssemblyWithReferenceToDbiTask::prepare() {
    AssemblyObject *assemblyObject = new AssemblyObject(assembly.visualName, U2EntityRef(srcDbiRef, assembly.id));
    cloneAssemblyTask = new CloneObjectTask(assemblyObject, dstDbiRef, dstFolder);
    addSubTask(cloneAssemblyTask);

    U2SequenceObject *sequenceObject = new U2SequenceObject(reference.visualName, U2EntityRef(srcDbiRef, reference.id));
    cloneReferenceTask = new CloneObjectTask(sequenceObject, dstDbiRef, dstFolder);
    addSubTask(cloneReferenceTask);
}

void CloneAssemblyWithReferenceToDbiTask::run() {
    delete cloneAssemblyTask->getSourceObject();
    delete cloneReferenceTask->getSourceObject();

    QScopedPointer<GObject> clonedObject(cloneAssemblyTask->takeResult());
    SAFE_POINT_EXT(NULL != clonedObject, setError(tr("Can't get the cloned object")), );
    QScopedPointer<AssemblyObject> clonedAssemblyObject(qobject_cast<AssemblyObject *>(clonedObject.data()));
    SAFE_POINT_EXT(NULL != clonedAssemblyObject, setError(tr("Unexpected result object: expect AssemblyObject, got %1 object").arg(clonedObject->getGObjectType())), );
    clonedObject.take();

    clonedObject.reset(cloneReferenceTask->takeResult());
    SAFE_POINT_EXT(NULL != clonedObject, setError(tr("Can't get the cloned object")), );
    QScopedPointer<U2SequenceObject> clonedSequenceObject(qobject_cast<U2SequenceObject *>(clonedObject.data()));
    SAFE_POINT_EXT(NULL != clonedSequenceObject, setError(tr("Unexpected result object: expect U2SequenceObject, got %1 object").arg(clonedObject->getGObjectType())), );
    clonedObject.take();

    DbiConnection con(dstDbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    U2Assembly clonedAssembly = con.dbi->getAssemblyDbi()->getAssemblyObject(clonedAssemblyObject->getEntityRef().entityId, stateInfo);
    CHECK_OP(stateInfo, );
    U2Sequence clonedSequence = con.dbi->getSequenceDbi()->getSequenceObject(clonedSequenceObject->getEntityRef().entityId, stateInfo);
    CHECK_OP(stateInfo, );

    clonedAssembly.referenceId = clonedSequence.id;
    con.dbi->getAssemblyDbi()->updateAssemblyObject(clonedAssembly, stateInfo);
}

}   // namespace U2
