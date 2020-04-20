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

#ifndef _U2_DATATYPE_SERIALIZE_UTILS_H_
#define _U2_DATATYPE_SERIALIZE_UTILS_H_

#include <U2Core/BioStruct3D.h>
#include <U2Core/DNAChromatogram.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/PFMatrix.h>
#include <U2Core/PWMatrix.h>
#include <U2Core/PhyTree.h>

namespace U2 {

/**
 * The class of utilities for serializing/deserializing chromatograms
 * into/from the binary representation.
 */
class U2CORE_EXPORT DNAChromatogramSerializer {
public:
    /**
     * The serializer identifier.
     */
    static const QString ID;

    /**
     * Returns the binary representation of @chroma.
     */
    static QByteArray serialize(const DNAChromatogram &chroma);

    /**
     * Returns DNAChromatogram extracted from @binary.
     */
    static DNAChromatogram deserialize(const QByteArray &binary, U2OpStatus &os);
};

/**
 * The class of utilities for serializing/deserializing phylogenetic
 * trees into/from the binary representation of Newick format.
 */
class U2CORE_EXPORT NewickPhyTreeSerializer {
public:
    /**
     * The serializer identifier.
     */
    static const QString ID;

    /**
     * Returns the binary representation of @tree.
     */
    static QByteArray serialize(const PhyTree &tree);

    /**
     * Returns PhyTree extracted from @binary.
     */
    static PhyTree deserialize(const QByteArray &binary, U2OpStatus &os);

    /**
     * Returns the list of trees parsed from @io
     */
    static QList<PhyTree> parseTrees(IOAdapter *io, U2OpStatus &os);
};

/**
 * The class of utilities for serializing/deserializing 3D structures
 * into/from the binary representation.
 */
class U2CORE_EXPORT BioStruct3DSerializer {
public:
    /**
     * The serializer identifier.
     */
    static const QString ID;

    /**
     * Returns the binary representation of @bioStruct.
     */
    static QByteArray serialize(const BioStruct3D &bioStruct);

    /**
     * Returns BioStruct3D extracted from @binary.
     */
    static BioStruct3D deserialize(const QByteArray &binary, U2OpStatus &os);
};

/**
 * The class of utilities for serializing/deserializing weight matrices
 * into/from the binary representation.
 */
class U2CORE_EXPORT WMatrixSerializer {
public:
    /**
     * The serializer identifier.
     */
    static const QString ID;

    /**
     * Returns the binary representation of @matrix.
     */
    static QByteArray serialize(const PWMatrix &matrix);

    /**
     * Returns PWMatrix extracted from @binary.
     */
    static PWMatrix deserialize(const QByteArray &binary, U2OpStatus &os);
};

/**
 * The class of utilities for serializing/deserializing frequency matrices
 * into/from the binary representation.
 */
class U2CORE_EXPORT FMatrixSerializer {
public:
    /**
     * The serializer identifier.
     */
    static const QString ID;

    /**
     * Returns the binary representation of @matrix.
     */
    static QByteArray serialize(const PFMatrix &matrix);

    /**
     * Returns PFMatrix extracted from @binary.
     */
    static PFMatrix deserialize(const QByteArray &binary, U2OpStatus &os);
};


class DatatypeSerializers : public QObject {
    Q_OBJECT
};

} // U2

#endif // _U2_DATATYPE_SERIALIZE_UTILS_H_
