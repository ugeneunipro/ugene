/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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


#ifndef _HI_GUI_TEST_OP_STATUS_H_
#define _HI_GUI_TEST_OP_STATUS_H_

#include "core/global.h"

#include <QString>

namespace HI {

class HI_EXPORT GUITestOpStatus
{
public:
    GUITestOpStatus(){}

    virtual void setError(const QString & err){error = err; throw  this;}
    virtual QString getError() const {return error;}
    virtual bool hasError() const {return !error.isEmpty();}

    //TODO:Remove this methods
    virtual bool isCanceled() const {return false;}
    virtual bool isCoR() const  {return isCanceled() || hasError();}

private:
    QString error;
};
}
#endif // GUITESTOPSTATUS_H
