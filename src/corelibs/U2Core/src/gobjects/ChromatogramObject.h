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

#include <QMutex>

#include <U2Core/Chromatogram.h>
#include <U2Core/GObject.h>
#include <U2Core/U2RawData.h>

namespace U2 {

class U2CORE_EXPORT U2Chromatogram : public U2RawData {
public:
    U2Chromatogram();
    U2Chromatogram(const U2DbiRef& dbiRef);
    U2Chromatogram(const U2RawData& rawData);

    U2DataType getType() const override;
};

class U2CORE_EXPORT ChromatogramObject : public GObject {
    Q_OBJECT
public:
    static ChromatogramObject* createInstance(const Chromatogram& chroma,
                                              const QString& objectName,
                                              const U2DbiRef& dbiRef,
                                              U2OpStatus& os,
                                              const QVariantMap& hints = {});

    ChromatogramObject(const QString& objectName,
                       const U2EntityRef& chromaRef,
                       const QVariantMap& hints = {});

    const Chromatogram& getChromatogram() const;
    void setChromatogram(U2OpStatus& os, const Chromatogram& chromatogram);

    GObject* clone(const U2DbiRef& dstRef, U2OpStatus& os, const QVariantMap& hints = {}) const override;

protected:
    void loadDataCore(U2OpStatus& os) override;

private:
    mutable QMutex mutex;
    mutable bool cached;
    mutable Chromatogram cache;
};

}  // namespace U2
