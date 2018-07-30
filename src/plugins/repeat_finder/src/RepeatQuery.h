/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_REPEAT_QUERY_H_
#define _U2_REPEAT_QUERY_H_

#include <QAction>

#include "FindRepeatsTask.h"

#include "U2Lang/QDScheme.h"
#include <U2Lang/QueryDesignerRegistry.h>


namespace U2 {

class QDRepeatActor : public QDActor {
    Q_OBJECT
public:
    int getMinResultLen() const;
    int getMaxResultLen() const;
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QList< QPair<QString,QString> > saveConfiguration() const;
    void loadConfiguration(const QList< QPair<QString,QString> >& strMap);
    QColor defaultColor() const { return QColor(0x66,0xa3,0xd2); }
    virtual bool hasStrand() const { return false; }
protected:
    QDRepeatActor(QDActorPrototype const* proto);
    friend class QDRepeatActorPrototype;
private slots:
    void sl_onAlgorithmTaskFinished();
private:
    //void addResults( const SharedAnnotationData& ad, bool complement );

    FindRepeatsTaskSettings settings;
    QList<FindRepeatsToAnnotationsTask*> repTasks;
};

class QDRepeatActorPrototype : public QDActorPrototype {
public:
    QDRepeatActorPrototype();
    QIcon getIcon() const { return QIcon(":repeat_finder/images/repeats.png"); }
    QDActor* createInstance() const { return new QDRepeatActor(this); }
};

}//namespace

#endif
