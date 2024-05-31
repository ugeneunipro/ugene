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
              addResultToProject(true) {
        }
        QString referenceUrl;
        QStringList readUrls;
        int minIdentity;
        int qualityThreshold;
        QString outAlignment;
        bool addResultToProject;
    };

    AlignToReferenceBlastDialogFiller(const Settings& settings);
    AlignToReferenceBlastDialogFiller(CustomScenario* c);

    void commonScenario() override;

    static void setReference(const QString& referenceUrl, QWidget* dialog);
    static void setReads(const QStringList& readUrls, QWidget* dialog);
    static void setDestination(const QString& destinationUrl, QWidget* dialog);

private:
    Settings settings;
};

}  // namespace U2
