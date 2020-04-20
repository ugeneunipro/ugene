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

#ifndef _U2_ADV_SPLIT_WIDGET_H_
#define _U2_ADV_SPLIT_WIDGET_H_

#include <U2Core/global.h>

#include <QSplitter>


namespace U2 {


class AnnotatedDNAView;
class GObject;

class U2VIEW_EXPORT ADVSplitWidget : public QWidget {
    Q_OBJECT
public:
    ADVSplitWidget(AnnotatedDNAView* view) : dnaView(view) { }
    AnnotatedDNAView* getAnnotatedDNAView() const {return dnaView;}
    virtual bool acceptsGObject(GObject* objects)  = 0;
    virtual void updateState(const QVariantMap& m) = 0;
    virtual void saveState(QVariantMap& m) = 0;

    virtual bool onCloseEvent() { return true; }
protected:
    AnnotatedDNAView* dnaView;
};


}//namespace

#endif //_U2_ADV_SPLIT_WIDGET_H_
