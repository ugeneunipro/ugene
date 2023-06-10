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

#include <GTGlobals.h>

#include <QVariantMap>

namespace U2 {
using namespace HI;
class ImportOptionsWidget;

class ImportOptionsWidgetFiller {
public:
    static void fill(ImportOptionsWidget* optionsWidget, const QVariantMap& data);

private:
    static void setDestinationFolder(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setKeepFoldersStructure(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setProcessFoldersRecursively(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setCreateSubfolderForTopLevelFolder(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setCreateSubfolderForEachFile(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setImportUnknownAsUdr(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setMultiSequencePolicy(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setAceFormatPolicy(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setCreateSubfolderForDocument(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
    static void setMergeMultiSequencePolicySeparatorSize(ImportOptionsWidget* optionsWidget, const QVariantMap& data);
};

}  // namespace U2
