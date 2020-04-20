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

#ifndef _U2_MOD_H_
#define _U2_MOD_H_

#include <U2Core/U2OpStatus.h>
#include <U2Core/DbiConnection.h>
#include <U2Core/U2Type.h>

#include <QSet>

namespace U2 {

class U2AbstractDbi;

/** Modification types */
class U2CORE_EXPORT U2ModType {
public:
    /** Object */
    static const qint64 objUpdatedName;

    /** Sequence modification types*/
    static const qint64 sequenceUpdatedData;

    /** MSA modification types */
    static const qint64 msaUpdatedAlphabet;
    static const qint64 msaAddedRows;
    static const qint64 msaAddedRow;
    static const qint64 msaRemovedRows;
    static const qint64 msaRemovedRow;
    static const qint64 msaUpdatedRowInfo;
    static const qint64 msaUpdatedGapModel;
    static const qint64 msaSetNewRowsOrder;
    static const qint64 msaLengthChanged;

    /** UDR modification types */
    static const qint64 udrUpdated;

    static bool isObjectModType(qint64 modType) { return modType > 0 && modType < 999; }
    static bool isSequenceModType(qint64 modType) { return modType >= 1000 && modType < 1100; }
    static bool isMsaModType(qint64 modType) { return modType >= 3000 && modType < 3100; }
    static bool isUdrModType(qint64 modType) { return modType >= 4000 && modType < 4100; }
};

/** Single modification of a dbi object */
class U2CORE_EXPORT U2SingleModStep {
public:
    /** ID of the modification in the database */
    qint64         id;

    /** ID of the dbi object */
    U2DataId       objectId;

    /** The object has been modified from 'version' to 'version + 1' */
    qint64         version;

    /** Type of the object modification */
    qint64         modType;

    /** Detailed description of the modification */
    QByteArray     details;

    /** ID of the multiple modifications step */
    qint64         multiStepId;
};

/**
 * Create an instance of this class when it is required to join
 * different modification into a one user action, i.e. all
 * these modifications will be undo/redo as a single action.
 * The user modifications step is finished when the object destructor is called.
 * Parameter "masterObjId" specifies ID of the object that initiated the changes.
 * Note that there might be other modified objects (e.g. child objects).
 *
 * WARNING!: you should limit the scope of the created instance to as small as possible,
 * as it "blocks" database!!
 */
class U2CORE_EXPORT U2UseCommonUserModStep {
public:
    U2UseCommonUserModStep(U2Dbi* _dbi, const U2DataId& _masterObjId, U2OpStatus& os);
    U2UseCommonUserModStep(const U2EntityRef &masterObjEntity, U2OpStatus &os);
    ~U2UseCommonUserModStep();

    U2Dbi * getDbi() const;
private:
    U2Dbi* dbi;
    bool valid;
    QScopedPointer<DbiConnection> con;
    const U2DataId masterObjId;

private:
    void init(U2OpStatus &os);
};

/** Helper class to track info about an object */
class U2CORE_EXPORT ModificationAction {
public:
    ModificationAction(U2AbstractDbi* dbi, const U2DataId& masterObjId);
    virtual ~ModificationAction();

    /**
        Verifies if modification tracking is enabled for the object.
        If it is, gets the object version.
        If there are tracking steps with greater or equal version (e.g. left from "undo"), removes these records.
        Returns the type of modifications  tracking for the object.
     */
    virtual U2TrackModType prepare(U2OpStatus& os) = 0;

    /**
        Adds the object ID to the object IDs set.
        If tracking is enabled, adds a new single step to the list.
     */
    virtual void addModification(const U2DataId& objId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) = 0;

    /**
        If tracking is enabled, creates modification steps in the database.
        Increments version of all objects in the set.
     */
    virtual void complete(U2OpStatus& os) = 0;

    /** Returns modification tracking type of the master object. */
    U2TrackModType getTrackModType() const { return trackMod; }

protected:
    U2AbstractDbi* dbi;

    U2DataId masterObjId;
    U2TrackModType trackMod;
    QSet<U2DataId> objIds;
    QList<U2SingleModStep> singleSteps;
};

} // namespace

#endif
