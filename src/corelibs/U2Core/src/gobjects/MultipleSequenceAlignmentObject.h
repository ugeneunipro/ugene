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

#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/MultipleAlignment.h>

namespace U2 {

class U2CORE_EXPORT MultipleSequenceAlignmentObject : public MultipleAlignmentObject {
    Q_OBJECT
public:
    MultipleSequenceAlignmentObject(const QString& name,
                                    const U2EntityRef& msaRef,
                                    const QVariantMap& hintsMap = {},
                                    const MultipleAlignment& msaData = {});

    /** GObject methods */
    // Actually this method doesn't exactly clone MSA database rows, row ID will be generated for each copied row again
    MultipleSequenceAlignmentObject* clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints = {}) const override;

    void deleteColumnsWithGaps(U2OpStatus& os, int requiredGapsCount = -1);

};

}  // namespace U2
