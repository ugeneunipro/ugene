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

#include <U2Core/GUrl.h>
#include <U2Core/Task.h>

namespace U2 {

class IOAdapterFactory;

class U2CORE_EXPORT CopyDataTask : public Task {
    Q_OBJECT
public:
    enum ReplaceLineEndings {
        KEEP_AS_IS,
        LF
        //implement other line endings of necessity
    };
    CopyDataTask(IOAdapterFactory* ioFrom, const GUrl& urlFrom, IOAdapterFactory* ioTo, 
        const GUrl& urlTo, ReplaceLineEndings newLineEndings = ReplaceLineEndings::KEEP_AS_IS);

    void run() override;

private:
    static const int BUFFSIZE;
    static const char CHAR_CR;

    IOAdapterFactory* ioFrom;
    IOAdapterFactory* ioTo;
    GUrl urlFrom;
    GUrl urlTo;
    ReplaceLineEndings newLineEndings = ReplaceLineEndings::KEEP_AS_IS;
};

}  // namespace U2
