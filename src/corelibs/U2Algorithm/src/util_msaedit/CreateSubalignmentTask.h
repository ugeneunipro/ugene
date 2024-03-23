/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/MsaObject.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class U2ALGORITHM_EXPORT CreateSubalignmentSettings {
public:
    CreateSubalignmentSettings(const QList<qint64>& rowIds,
                               const U2Region& columnRange,
                               const GUrl& url,
                               bool saveImmediately,
                               bool addToProject,
                               const DocumentFormatId& formatIdToSave);

    /** Row ids to export. */
    QList<qint64> rowIds;

    /** Column range to export. */
    U2Region columnRange;

    GUrl url;
    bool saveImmediately;
    bool addToProject;
    DocumentFormatId formatIdToSave;
};

class U2ALGORITHM_EXPORT CreateSubalignmentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    CreateSubalignmentTask(MsaObject* _maObj, const CreateSubalignmentSettings& settings);

    void prepare() override;
    const CreateSubalignmentSettings& getSettings() {
        return cfg;
    }

private:
    Document* origDoc;
    MsaObject* origMAObj;
    MsaObject* resultMAObj;

    CreateSubalignmentSettings cfg;
    bool createCopy;
};

}  // namespace U2
