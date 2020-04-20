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

#ifndef _U2_BLAST_QUERY_H_
#define _U2_BLAST_QUERY_H_

#include "RemoteBLASTTask.h"
#include "RemoteBLASTConsts.h"

#include <U2Lang/QDScheme.h>
#include <U2Lang/QueryDesignerRegistry.h>


namespace U2 {

class QDCDDActor : public QDActor {
    Q_OBJECT
public:
    QDCDDActor(QDActorPrototype const* a);
    int getMinResultLen() const;
    int getMaxResultLen() const;
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const { return QColor(0x6A, 0x94, 0xd4); }
    virtual bool hasStrand() const { return false; }
private slots:
    void sl_onAlgorithmTaskFinished();
private:
    RemoteBLASTTaskSettings settings;
    QMap<RemoteBLASTTask*, int> offsetMap;
};

class QDCDDActorPrototype : public QDActorPrototype {
public:
    QDCDDActorPrototype();
    virtual QDActor* createInstance() const { return new QDCDDActor(this); }
    virtual QIcon getIcon() const { return QIcon(":remote_blast/images/remote_db_request.png"); }
};

}//namespace

#endif
