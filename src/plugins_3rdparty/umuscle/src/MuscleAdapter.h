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

#ifndef _U2_UMUSCLE_ADAPTER_H_
#define _U2_UMUSCLE_ADAPTER_H_

#include <QObject>

#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class TaskStateInfo;

class MuscleAdapter : public QObject {
Q_OBJECT
public:
    static void align(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti, bool mhack = true);

    static void refine(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti);

    static void align2Profiles(const MultipleSequenceAlignment& ma1, const MultipleSequenceAlignment& ma2, MultipleSequenceAlignment& res, TaskStateInfo& ti);
    
    static void addUnalignedSequencesToProfile( const MultipleSequenceAlignment& ma, const MultipleSequenceAlignment& unalignedSeqs, MultipleSequenceAlignment& res, TaskStateInfo& ti);
    static QString getBadAllocError();

private:
    static void alignUnsafe(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti, bool mhack);
    
    static void refineUnsafe(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti);

    static void align2ProfilesUnsafe(const MultipleSequenceAlignment& ma1, const MultipleSequenceAlignment& ma2, MultipleSequenceAlignment& res, TaskStateInfo& ti);

    static void addUnalignedSequencesToProfileUnsafe(const MultipleSequenceAlignment& ma, const MultipleSequenceAlignment& unalignedSeqs, MultipleSequenceAlignment& res, TaskStateInfo& ti);
};

}//namespace

#endif
