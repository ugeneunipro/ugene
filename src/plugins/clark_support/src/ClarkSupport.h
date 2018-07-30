/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#define CLARK_GROUP "CLARK"
#define ET_CLARK "CLARK"
#define ET_CLARK_L "CLARK-l"
#define ET_CLARK_getAccssnTaxID "getAccssnTaxID"
#define ET_CLARK_getfilesToTaxNodes "getfilesToTaxNodes"
#define ET_CLARK_getTargetsDef "getTargetsDef"
#define ET_CLARK_buildScript "builddb.sh"

namespace U2 {

class ClarkSupport : public ExternalTool {
    Q_OBJECT
public:
    ClarkSupport(const QString& name, const QString& path = "");

    static void registerTools(ExternalToolRegistry *etRegistry);
    static void unregisterTools(ExternalToolRegistry *etRegistry);

private slots:
    void sl_toolValidationStatusChanged(bool isValid);
};

}//namespace
#endif // _U2_CLARK_SUPPORT_H_
