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

#include "CopyDataTask.h"

#include <QString>

#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const int CopyDataTask::BUFFSIZE = 32 * 1024;
const char CopyDataTask::CHAR_CR = '\r';

CopyDataTask::CopyDataTask(IOAdapterFactory* _ioFrom, const GUrl& _urlFrom, IOAdapterFactory* _ioTo, const GUrl& _urlTo, ReplaceLineEndings newLineEndings_)
    : Task(tr("Copy Data Task"), TaskFlag_None), ioFrom(_ioFrom), ioTo(_ioTo),
      urlFrom(_urlFrom), urlTo(_urlTo), newLineEndings(newLineEndings_) {
    SAFE_POINT_NN(ioFrom, );
    SAFE_POINT_NN(ioTo, );
    tpm = Progress_Manual;
}

void CopyDataTask::run() {
    QScopedPointer<IOAdapter> from(ioFrom->createIOAdapter());
    QScopedPointer<IOAdapter> where(ioTo->createIOAdapter());
    from->open(urlFrom, IOAdapterMode_Read);
    if (!from->isOpen()) {
        stateInfo.setError(L10N::errorOpeningFileRead(urlFrom));
        return;
    }

    int count = 0;
    int count_w = 0;
    bool replacementOcurred = false;
    QByteArray buff(BUFFSIZE, 0);
    QBitArray CRCharMap = TextUtils::createBitMap(CHAR_CR);

    count = from->readBlock(buff.data(), BUFFSIZE);
    if (newLineEndings == ReplaceLineEndings::LF) {
        int newLen = TextUtils::remove(buff.data(), count, CRCharMap);
        if (count != newLen) {
            count = newLen;
            replacementOcurred = true;
        }
    }
    if (count == 0 || count == -1) {
        stateInfo.setError(tr("Cannot get data from: '%1'").arg(urlFrom.getURLString()));
        return;
    }

    if (!where->open(urlTo, IOAdapterMode_Write)) {
        stateInfo.setError(L10N::errorOpeningFileWrite(urlTo));
        return;
    }

    while (count > 0) {
        count_w = where->writeBlock(buff.data(), count);
        if (stateInfo.cancelFlag) {
            break;
        }
        stateInfo.progress = from->getProgress();
        count = from->readBlock(buff.data(), BUFFSIZE);
        if (newLineEndings == ReplaceLineEndings::LF && count > 0) {
            int newLen = TextUtils::remove(buff.data(), count, CRCharMap);
            if (count != newLen) {
                count = newLen;
                replacementOcurred = true;
            }
        }
    }
    if (count < 0 || count_w < 0) {
        if (!stateInfo.hasError()) {
            stateInfo.setError(tr("IO adapter error. %1").arg(from->errorString()));
        }
    }
    if (replacementOcurred) {
        stateInfo.addWarning(tr("Line endings were changed in target file %1 during copy process.").arg(where->getURL().getURLString()));
    }
}

}  // namespace U2
