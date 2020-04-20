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

#ifndef _U2_WELCOME_PAGE_ACTION_H_
#define _U2_WELCOME_PAGE_ACTION_H_

#include <U2Core/global.h>

namespace U2 {

class U2GUI_EXPORT WelcomePageAction {
public:
    WelcomePageAction(const QString &id);

    virtual ~WelcomePageAction();
    virtual void perform() = 0;

    const QString & getId() const;

private:
    QString id;
};

class U2GUI_EXPORT BaseWelcomePageActions {
public:
    static const QString LOAD_DATA;
    static const QString CREATE_SEQUENCE;
    static const QString CREATE_WORKFLOW;
    static const QString QUICK_START;
};

} // U2

#endif // _U2_WELCOME_PAGE_ACTION_H_
