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

#ifndef _U2_PYTHON_SUPPORT_H_
#define _U2_PYTHON_SUPPORT_H_

#include <U2Core/ExternalToolRegistry.h>

#include "RunnerTool.h"
#include "utils/ExternalToolSupportAction.h"

namespace U2 {

class PythonSupport : public RunnerTool {
    Q_OBJECT
public:
    PythonSupport(const QString& id, const QString& name, const QString& path = "");

    static const QString ET_PYTHON;
    static const QString ET_PYTHON_ID;
};

class PythonModuleSupport : public ExternalToolModule {
    Q_OBJECT
public:
    PythonModuleSupport(const QString& id, const QString& name);
};

class PythonModuleDjangoSupport : public PythonModuleSupport {
    Q_OBJECT
public:
    PythonModuleDjangoSupport(const QString& id, const QString& name);

    static const QString ET_PYTHON_DJANGO;
    static const QString ET_PYTHON_DJANGO_ID;
};

class PythonModuleNumpySupport : public PythonModuleSupport {
    Q_OBJECT
public:
    PythonModuleNumpySupport(const QString& id, const QString& name);

    static const QString ET_PYTHON_NUMPY;
    static const QString ET_PYTHON_NUMPY_ID;
};

class PythonModuleBioSupport : public PythonModuleSupport {
    Q_OBJECT
public:
    PythonModuleBioSupport(const QString& id, const QString& name);

    static const QString ET_PYTHON_BIO;
    static const QString ET_PYTHON_BIO_ID;
};


}//namespace
#endif // _U2_PYTHON_SUPPORT_H_
