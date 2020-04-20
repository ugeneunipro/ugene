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

#ifndef _U2_OPSTATUS_H_
#define _U2_OPSTATUS_H_

#include <U2Core/global.h>

namespace U2 {

/**
    Operation status.
    If operation failed, the reason is written in 'error' field
    If operation hangs, it can be canceled with 'cancelFlag'
*/
class U2CORE_EXPORT U2OpStatus {
public:
    virtual ~U2OpStatus(){}

    virtual void setError(const QString & err) = 0;
    virtual QString getError() const  = 0;
    virtual bool hasError() const = 0;

    virtual bool isCanceled() const = 0;
    virtual void setCanceled(bool v)  = 0;

    virtual bool isCoR() const  {return isCanceled() || hasError();}

    /** Returns progress value in percents. Special value '-1' is used to indicate 'No info' progress state */
    virtual int getProgress() const = 0;
    virtual void setProgress(int v)  = 0;

    virtual QString getDescription() const = 0;
    virtual void setDescription(const QString& desc)  = 0;

    virtual bool hasWarnings() const = 0;
    virtual QStringList getWarnings() const = 0;
    virtual void addWarning(const QString& w) = 0;
    virtual void addWarnings(const QStringList& wList) = 0;

};

} //namespace

#endif
