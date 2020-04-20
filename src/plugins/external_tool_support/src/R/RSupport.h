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

#ifndef _U2_R_SUPPORT_H_
#define _U2_R_SUPPORT_H_

#include <U2Core/ExternalToolRegistry.h>

#include "RunnerTool.h"
#include "utils/ExternalToolSupportAction.h"

namespace U2 {

class RSupport : public RunnerTool {
    Q_OBJECT
public:
    RSupport(const QString& id, const QString& name, const QString& path = "");

    static const QString ET_R;
    static const QString ET_R_ID;
};

class RModuleSupport : public ExternalToolModule {
    Q_OBJECT
public:
    RModuleSupport(const QString& id, const QString& name);
protected:
    QString getScript() const;
};

class RModuleGostatsSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleGostatsSupport(const QString& id, const QString& name);

    static const QString ET_R_GOSTATS;
    static const QString ET_R_GOSTATS_ID;
};

class RModuleGodbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleGodbSupport(const QString& id, const QString& name);

    static const QString ET_R_GO_DB;
    static const QString ET_R_GO_DB_ID;
};

class RModuleHgu133adbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleHgu133adbSupport(const QString& id, const QString& name);

    static const QString ET_R_HGU133A_DB;
    static const QString ET_R_HGU133A_DB_ID;
};

class RModuleHgu133bdbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleHgu133bdbSupport(const QString& id, const QString& name);

    static const QString ET_R_HGU133B_DB;
    static const QString ET_R_HGU133B_DB_ID;
};

class RModuleHgu133plus2dbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleHgu133plus2dbSupport(const QString& id, const QString& name);

    static const QString ET_R_HGU1333PLUS2_DB;
    static const QString ET_R_HGU1333PLUS2_DB_ID;
};

class RModuleHgu95av2dbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleHgu95av2dbSupport(const QString& id, const QString& name);

    static const QString ET_R_HGU95AV2_DB;
    static const QString ET_R_HGU95AV2_DB_ID;
};

class RModuleMouse430a2dbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleMouse430a2dbSupport(const QString& id, const QString& name);

    static const QString ET_R_MOUSE430A2_DB;
    static const QString ET_R_MOUSE430A2_DB_ID;
};

class RModuleCelegansdbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleCelegansdbSupport(const QString& id, const QString& name);

    static const QString ET_R_CELEGANS_DB;
    static const QString ET_R_CELEGANS_DB_ID;
};

class RModuleDrosophila2dbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleDrosophila2dbSupport(const QString& id, const QString& name);

    static const QString ET_R_DROSOPHILA2_DB;
    static const QString ET_R_DROSOPHILA2_DB_ID;
};

class RModuleOrghsegdbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleOrghsegdbSupport(const QString& id, const QString& name);

    static const QString ET_R_ORG_HS_EG_DB;
    static const QString ET_R_ORG_HS_EG_DB_ID;
};

class RModuleOrgmmegdbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleOrgmmegdbSupport(const QString& id, const QString& name);

    static const QString ET_R_ORG_MM_EG_DB;
    static const QString ET_R_ORG_MM_EG_DB_ID;
};

class RModuleOrgceegdbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleOrgceegdbSupport(const QString& id, const QString& name);

    static const QString ET_R_ORG_CE_EG_DB;
    static const QString ET_R_ORG_CE_EG_DB_ID;
};

class RModuleOrgdmegdbSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleOrgdmegdbSupport(const QString& id, const QString& name);

    static const QString ET_R_ORG_DM_EG_DB;
    static const QString ET_R_ORG_DM_EG_DB_ID;
};

class RModuleSeqlogoSupport : public RModuleSupport {
    Q_OBJECT
public:
    RModuleSeqlogoSupport(const QString& id, const QString& name);

    static const QString ET_R_SEQLOGO;
    static const QString ET_R_SEQLOGO_ID;
};

}   // namespace

#endif // _U2_R_SUPPORT_H_
