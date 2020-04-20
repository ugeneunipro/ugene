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

#ifndef _U2_SERVICE_TYPE_H_
#define _U2_SERVICE_TYPE_H_

#include "ServiceModel.h"

namespace U2 {

class AppResourcePool;

const ServiceType Service_PluginViewer          = 101;
const ServiceType Service_Project               = 102;
const ServiceType Service_ProjectView           = 103;

const ServiceType Service_DNAGraphPack          = 104;
const ServiceType Service_DNAExport             = 105;
const ServiceType Service_TestRunner            = 106;
const ServiceType Service_ScriptRegistry        = 107;
const ServiceType Service_ExternalToolSupport   = 108;
const ServiceType Service_GUITesting            = 109;
const ServiceType Service_WorkflowDesigner      = 110;
const ServiceType Service_QueryDesigner         = 111;

const ServiceType Service_MinCoreServiceId = 500;
const ServiceType Service_MaxCoreServiceId = 1000;

}//namespace

#endif
