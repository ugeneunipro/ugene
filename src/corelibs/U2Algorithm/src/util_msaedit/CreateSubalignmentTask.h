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

#ifndef _U2_CREATE_SUBALIGNMENT_TASK_H_
#define _U2_CREATE_SUBALIGNMENT_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/GUrl.h>
#include <U2Core/U2Region.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentProviderTask.h>

namespace U2{

class U2ALGORITHM_EXPORT CreateSubalignmentSettings {
public:
    CreateSubalignmentSettings(){}

    CreateSubalignmentSettings(const U2Region& w, const QStringList& sNames, const GUrl& path, bool save, bool add, DocumentFormatId formatId)
        : window(w), sequenceNames(sNames), url(path), saveImmediately(save), addToProject(add), formatIdToSave(formatId) {}

    CreateSubalignmentSettings(const U2Region& w, const QList<qint64>& rIds, const GUrl& path, bool save, bool add, DocumentFormatId formatId)
        : window(w), rowIds(rIds), url(path), saveImmediately(save), addToProject(add), formatIdToSave(formatId) {}

    U2Region         window;

    // Row ids to export
    QList<qint64>    rowIds;

    // Sequence names to export. Ignored if rowIds is not empty!
    // This field may be removed after all code is migrated to row ids.
    QStringList      sequenceNames;

    GUrl             url;
    bool             saveImmediately;
    bool             addToProject;
    DocumentFormatId formatIdToSave;
};


class U2ALGORITHM_EXPORT CreateSubalignmentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    CreateSubalignmentTask(MultipleSequenceAlignmentObject* _maObj, const CreateSubalignmentSettings& settings);

    virtual void prepare();
    const CreateSubalignmentSettings& getSettings() { return cfg; }

private:
    Document*                   origDoc;
    MultipleSequenceAlignmentObject*           origMAObj;
    MultipleSequenceAlignmentObject*           resultMAObj;

    CreateSubalignmentSettings  cfg;
    bool                        createCopy;
};

}

#endif
