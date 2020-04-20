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

#ifndef _U2_JAVASUPPORT_H_
#define _U2_JAVASUPPORT_H_

#include <U2Core/ExternalToolRegistry.h>

#include "RunnerTool.h"

namespace U2 {

class JavaSupport : public RunnerTool {
    Q_OBJECT
public:
    enum Architecture {
        x32,
        x64
    };
    JavaSupport(const QString& id, const QString &name, const QString &path = "");
    void extractAdditionalParameters(const QString& output);
    Architecture getArchitecture() const;

    static const QString ET_JAVA;
    static const QString ET_JAVA_ID;

private:
    static QString architecture2string(Architecture architecture);
    static Architecture string2architecture(const QString &string);

    static const QString ARCHITECTURE;
    static const QString ARCHITECTURE_X32;
    static const QString ARCHITECTURE_X64;
    static const QStringList RUN_PARAMETERS;
};

} // U2

#endif // _U2_JAVASUPPORT_H_
