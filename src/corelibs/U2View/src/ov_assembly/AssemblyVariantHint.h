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

#ifndef _ASSEMBLY_VARIANT_HINT_
#define _ASSEMBLY_VARIANT_HINT_

#include <U2Core/U2Variant.h>

#include "AssemblyReadsAreaHint.h"

namespace U2 {

class AssemblyVariantHint : public AssemblyReadsAreaHint {
    Q_OBJECT
public:
    AssemblyVariantHint(QWidget *parent);
    void setData(const QList<U2Variant> &varList);

protected:
    virtual void leaveEvent(QEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
};

} // U2

#endif // _ASSEMBLY_VARIANT_HINT_
