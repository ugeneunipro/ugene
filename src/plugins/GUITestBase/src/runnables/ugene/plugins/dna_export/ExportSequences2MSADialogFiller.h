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

#ifndef _U2_GT_RUNNABLES_EXPORT_SEQUENCES2MSA_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_EXPORT_SEQUENCES2MSA_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

    class ExportSequenceAsAlignmentFiller : public Filler {
    public:
        enum FormatToUse {Clustalw, Fasta, Msf, Mega, Nexus, Sam, Stockholm};

        ExportSequenceAsAlignmentFiller(HI::GUITestOpStatus &_os, const QString &_path, const QString &_name, ExportSequenceAsAlignmentFiller::FormatToUse _format,
            bool addDocumentToProject = false, GTGlobals::UseMethod method = GTGlobals::UseMouse);
        ExportSequenceAsAlignmentFiller(HI::GUITestOpStatus &_os, CustomScenario *scenario);

        void commonScenario();

    private:
        QString path, name;
        GTGlobals::UseMethod useMethod;
        FormatToUse format;
        bool addToProject;
        QMap<FormatToUse, QString> comboBoxItems;
    };
}

#endif
