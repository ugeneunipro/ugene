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

#include <QString>

#include "core/global.h"

namespace HI {

class HI_EXPORT GUITestOpStatus final {
public:
    void setError(const QString& err) {
        if (!error.isEmpty()) {
            qWarning("Can't override error! Current error: %s, new error: %s", error.toLocal8Bit().constData(), err.toLocal8Bit().constData());
        } else {
            error = err;
        }
        throw this;
    }

    QString getError() const {
        return error;
    }

    bool hasError() const {
        return !error.isEmpty();
    }

    bool isCoR() const {
        return hasError();
    }

private:
    QString error;
};
}  // namespace HI
