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

#ifndef _U2_ALIGN_TO_REFERENCE_BLAST_DIALOG_FILLER_
#define _U2_ALIGN_TO_REFERENCE_BLAST_DIALOG_FILLER_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

/**
 * @brief The AlignToReferenceBlastDialogFiller class
 */
class AlignToReferenceBlastDialogFiller : public Filler {
public:
    struct Settings {
        Settings()
            : minIdentity(80),
              qualityThreshold(30),
              addResultToProject(true)
        {}
        QString referenceUrl;
        QStringList readUrls;
        int minIdentity;
        int qualityThreshold;
        QString outAlignment;
        bool addResultToProject;
    };

    AlignToReferenceBlastDialogFiller(const Settings &settings, HI::GUITestOpStatus &os);
    AlignToReferenceBlastDialogFiller(HI::GUITestOpStatus &os, CustomScenario* c);

    void commonScenario();

    static void setReference(HI::GUITestOpStatus &os, const QString &referenceUrl, QWidget *dialog);
    static void setReads(HI::GUITestOpStatus &os, const QStringList &readUrls, QWidget *dialog);
    static void setDestination(HI::GUITestOpStatus &os, const QString &destinationUrl, QWidget *dialog);

private:
    Settings settings;
};

}

#endif // _U2_ALIGN_TO_REFERENCE_BLAST_DIALOG_FILLER_

