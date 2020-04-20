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

#include "DesignerUtils.h"

#include <U2Lang/WorkflowUtils.h>
#include <U2Gui/DialogUtils.h>

//TODO FIX translator
namespace U2 {

QString DesignerUtils::getSchemaFileFilter(bool any, bool addOldExt) {
    QStringList exts(WorkflowUtils::WD_FILE_EXTENSIONS);
    if(addOldExt) {
        exts << WorkflowUtils::WD_XML_FORMAT_EXTENSION;
    }
    return DialogUtils::prepareFileFilter(WorkflowUtils::tr("UGENE workflow documents"), exts, any);
}

} //ns
