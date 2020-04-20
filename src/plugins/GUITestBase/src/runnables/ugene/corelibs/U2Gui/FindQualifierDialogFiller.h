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

#ifndef _U2_GT_RUNNABLES_FIND_QUALIFIER_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_FIND_QUALIFIER_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;
class FindQualifierFiller : public Filler {
public:
    struct FindQualifierFillerSettings {
        FindQualifierFillerSettings(const QString& name = QString(),
                                    const QString& value = QString(),
                                    bool exactMatch = true,
                                    bool nextQualifier = true,
                                    unsigned int nextCount = 1,
                                    bool expectTheEndOfTree = false,
                                    bool expectNoResults = false,
                                    bool selectAll = true)
            : name(name),
              value(value),
              exactMatch(exactMatch),
              nextQualifier(nextQualifier),
              nextCount(nextCount),
              expectTheEndOfTree(expectTheEndOfTree),
              expectNoResults(expectNoResults),
              selectAll(selectAll) {}

        QString name;
        QString value;
        bool exactMatch;
        bool nextQualifier;
        unsigned int nextCount;
        bool expectTheEndOfTree;
        bool expectNoResults;
        bool selectAll;
    };

    FindQualifierFiller(HI::GUITestOpStatus &os, const FindQualifierFillerSettings& settings = FindQualifierFillerSettings());
    FindQualifierFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);

    void commonScenario();

private:
    FindQualifierFillerSettings settings;
};

}   // namespace U2

#endif // _U2_GT_RUNNABLES_FIND_QUALIFIER_DIALOG_FILLER_H_
