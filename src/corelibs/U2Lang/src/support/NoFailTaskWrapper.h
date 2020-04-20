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

#ifndef _U2_NOFAILTASKWRAPPER_H_
#define _U2_NOFAILTASKWRAPPER_H_

#include <U2Core/Task.h>

namespace U2 {

/**
 * The class is needed to wrap a task that can be failed
 * but the workflow process would not be stopped.
 */
class U2LANG_EXPORT NoFailTaskWrapper : public Task {
    Q_OBJECT
public:
    NoFailTaskWrapper(Task *task);
    virtual void prepare();

    Task * originalTask() const;

    virtual bool hasWarning() const;
    virtual QStringList getWarnings() const;

private:
    Task *subTask;
};

} // U2

#endif // _U2_NOFAILTASKWRAPPER_H_
