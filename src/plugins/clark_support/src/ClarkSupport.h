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

#ifndef _U2_CLARK_SUPPORT_H_
#define _U2_CLARK_SUPPORT_H_

#include <U2Core/ExternalToolRegistry.h>

namespace U2 {

class ClarkSupport : public ExternalTool {
    Q_OBJECT
public:
    ClarkSupport(const QString& id, const QString& name, const QString& path = "");

    static void registerTools(ExternalToolRegistry *etRegistry);
    static void unregisterTools(ExternalToolRegistry *etRegistry);

    static const QString CLARK_GROUP;
    static const QString ET_CLARK;
    static const QString ET_CLARK_ID;
    static const QString ET_CLARK_L;
    static const QString ET_CLARK_L_ID;
    static const QString ET_CLARK_BUILD_SCRIPT;
    static const QString ET_CLARK_BUILD_SCRIPT_ID;
    static const QString ET_CLARK_GET_ACCSSN_TAX_ID;
    static const QString ET_CLARK_GET_ACCSSN_TAX_ID_ID;
    static const QString ET_CLARK_GET_TARGETS_DEF;
    static const QString ET_CLARK_GET_TARGETS_DEF_ID;
    static const QString ET_CLARK_GET_FILES_TO_TAX_NODES;
    static const QString ET_CLARK_GET_FILES_TO_TAX_NODES_ID;

};

}//namespace
#endif // _U2_CLARK_SUPPORT_H_
