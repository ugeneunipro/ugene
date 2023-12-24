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

#include <U2Core/MultipleAlignment.h>
#include <U2Core/StateLockableDataModel.h>
#include <U2Core/Task.h>

namespace U2 {

class MultipleChromatogramAlignmentObject;

class ConvertMca2MsaTask : public Task {
    Q_OBJECT
public:
    ConvertMca2MsaTask(MultipleChromatogramAlignmentObject* mcaObject, bool includeReference);

    MultipleAlignment getMsa() const;

private:
    void prepare() override;
    void run() override;
    ReportResult report() override;

    MultipleChromatogramAlignmentObject* mcaObject;
    bool includeReference;

    MultipleAlignment msa = {MultipleAlignmentDataType::MSA};
    QScopedPointer<StateLocker> locker;
};

}  // namespace U2
