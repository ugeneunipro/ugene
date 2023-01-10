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

#ifndef _U2_MAKE_BLAST_DB_ALIGNER_SUBTASK_H_
#define _U2_MAKE_BLAST_DB_ALIGNER_SUBTASK_H_

#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>

namespace U2 {
namespace Workflow {

class MakeBlastDbAlignerSubtask : public Task {
    Q_OBJECT
public:
    MakeBlastDbAlignerSubtask(const QString& referenceUrl,
                              const SharedDbiDataHandler& referenceDbHandler,
                              DbiDataStorage* storage);
    void prepare() override;

    const QString& getResultPath() const;

private:
    QString getAcceptableTempDir() const;

    const QString referenceUrl;
    const SharedDbiDataHandler referenceDbHandler;

    DbiDataStorage* storage = nullptr;

    QString databaseNameAndPath;
};

}  // namespace Workflow
}  // namespace U2

#endif  // _U2_MAKE_BLAST_DB_ALIGNER_SUBTASK_H_
