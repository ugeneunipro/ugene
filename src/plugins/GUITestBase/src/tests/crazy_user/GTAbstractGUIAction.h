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

#ifndef _U2_GT_ABSTRACT_GUI_ACTION_H_
#define _U2_GT_ABSTRACT_GUI_ACTION_H_

#include <U2Core/U2OpStatusUtils.h>
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_crazy_user {
using namespace HI;

class GTAbstractGUIAction : public Runnable {
public:
    enum Priority {Priority_Normal, Priority_High};

    GTAbstractGUIAction(Priority priority) : priority(priority), obj(NULL){}
    virtual ~GTAbstractGUIAction(){}

    GTAbstractGUIAction(const GTAbstractGUIAction& a) : Runnable(a), priority(a.priority), obj(a.obj), os() {}

    virtual void init(QObject* obj) {
        this->obj = obj;
    }
    virtual GTAbstractGUIAction* clone() const = 0;

    const QString objectClassName() const {
        return NULL == obj ? "" : obj->metaObject()->className();
    }
    static bool lessThan(const GTAbstractGUIAction* lv, const GTAbstractGUIAction* rv) {
        return lv->priority > rv->priority;
    }
    Priority getPriority() const {
        return priority;
    }
protected:
    Priority priority;
    QObject *obj;
    HI::GUITestOpStatus os;
private:
    GTAbstractGUIAction& operator=(GTAbstractGUIAction&);
};

typedef QMap<QString, const GTAbstractGUIAction*> GTAbstractGUIActionMap;

}

} //namespace

#endif
