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


#include <U2Core/IOAdapter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>

#include "SaveDocumentStreamingTask.h"

namespace U2 {

SaveDocumentStreamingTask::SaveDocumentStreamingTask( Document* d, IOAdapter* i )
: Task(tr("Save document"), TaskFlags(TaskFlag_None)), lock(NULL), doc(d), io(i) {
    if( NULL == doc ) {
        stateInfo.setError(L10N::badArgument("doc"));
        return;
    }
    if( NULL == io || !io->isOpen() ) {
        stateInfo.setError(L10N::badArgument("IO adapter"));
        return;
    }
    lock = new StateLock( getTaskName() );
    tpm = Progress_Manual;
}

SaveDocumentStreamingTask::~SaveDocumentStreamingTask() {
    assert( NULL == lock );
}

void SaveDocumentStreamingTask::prepare() {
    if( stateInfo.hasError() ) {
        return;
    }
    doc->lockState( lock );
}

void SaveDocumentStreamingTask::run() {
    if( stateInfo.hasError() ) {
        return;
    }
    DocumentFormat* df = doc->getDocumentFormat();
    df->storeDocument( doc, io, stateInfo);
}

Task::ReportResult SaveDocumentStreamingTask::report() {
    if( NULL != doc ) {
        doc->makeClean();
        doc->unlockState( lock );
    }
    if( NULL != lock ) {
        delete lock;
        lock = NULL;
    }
    return ReportResult_Finished;
}

Document* SaveDocumentStreamingTask::getDocument() const {
    return doc;
}

} // U2
