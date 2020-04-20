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

#ifndef _U2_IMPORT_OPTIONS_WIDGET_FILLER_H_
#define _U2_IMPORT_OPTIONS_WIDGET_FILLER_H_

#include <QVariantMap>
#include <GTGlobals.h>

namespace U2 {
using namespace HI;
class ImportOptionsWidget;

class ImportOptionsWidgetFiller {
public:
    static void fill(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);

private:
    static void setDestinationFolder(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setKeepFoldersStructure(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setProcessFoldersRecusively(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setCreateSubfolderForTopLevelFolder(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setCreateSubfolderForEachFile(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setImportUnknownAsUdr(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setMultiSequencePolicy(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setAceFormatPolicy(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setCreateSubfolderForDocument(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setMergeMultiSequencePolicySeparatorSize(HI::GUITestOpStatus& os, ImportOptionsWidget* optionsWidget, const QVariantMap& data);
};

}   // namespace U2

#endif // _U2_IMPORT_OPTIONS_WIDGET_FILLER_H_
