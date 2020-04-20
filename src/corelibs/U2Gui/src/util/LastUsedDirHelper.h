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

#ifndef _U2_LAST_USED_DIR_HELPER_H_
#define _U2_LAST_USED_DIR_HELPER_H_

#include <U2Core/global.h>

namespace U2 {

class U2GUI_EXPORT LastUsedDirHelper {
public:
    LastUsedDirHelper(const QString& domain = QString(), const QString& defaultVal = QString()); //reads dir

    virtual ~LastUsedDirHelper(); //if url is not empty -> derives dir from url and stores it
    

    static QString getLastUsedDir(const QString& toolType = QString(), const QString& defaultVal = QString());

    static void setLastUsedDir(const QString& ld, const QString& toolType = QString());

    operator const QString&() const {return dir;} 

    void saveLastUsedDir();

    QString domain;

    //last used dir
    QString dir;

    // value returned by file dialog
    QString url;
};

} //namespace

#endif
