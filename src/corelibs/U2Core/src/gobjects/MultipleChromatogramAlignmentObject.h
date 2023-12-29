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

#include <U2Core/GObject.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/MultipleAlignment.h>

namespace U2 {

class MultipleAlignment;
class U2SequenceObject;

class U2CORE_EXPORT MultipleChromatogramAlignmentObject : public MultipleAlignmentObject {
    Q_OBJECT
public:
    static const QString MCAOBJECT_REFERENCE;

    MultipleChromatogramAlignmentObject(const QString& name,
                                        const U2EntityRef& mcaRef,
                                        const QVariantMap& hintsMap = QVariantMap(),
                                        const MultipleAlignment& mca = {});

    ~MultipleChromatogramAlignmentObject() override;

    GObject* clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints = {}) const override;

    U2SequenceObject* getReferenceObj() const;

    void deleteColumnsWithGaps(U2OpStatus& os);

    int getReferenceLengthWithGaps() const;

protected:
    QList<U2Region> getColumnsWithGaps() const;
    QVector<U2MsaGap> getReferenceGapModel() const;

    mutable U2SequenceObject* referenceObj;
};

}  // namespace U2
