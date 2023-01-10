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

#ifndef _U2_BASE_ALIGNMENT_ALGORITHM_IDS_H_
#define _U2_BASE_ALIGNMENT_ALGORITHM_IDS_H_

#include <U2Core/global.h>

namespace U2 {

class U2ALGORITHM_EXPORT BaseAlignmentAlgorithmsIds {
public:
    // align sequences to alignment algorithms
    static const QString ALIGN_SEQUENCES_TO_ALIGNMENT_BY_UGENE;
    static const QString ALIGN_SEQUENCES_TO_ALIGNMENT_BY_MAFFT;

    /**
     * Built-in algorithm ids for "Align selected sequences" to alignment.
     * TODO: these are the same algorithm as 'ALIGN_SEQUENCES_TO_ALIGNMENT' but with a separate visual (action) names.
     *  This should be changed and action names assigned in UI. The algorithm must hold only generic name: MAFFT, UGENE...
     *  TO be fixed as a part of UGENE-7499.
     */
    static const QString ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT_BY_MAFFT;
};

}  // namespace U2

#endif  // _U2_BASE_ALIGNMENT_ALGORITHM_IDS_H_
