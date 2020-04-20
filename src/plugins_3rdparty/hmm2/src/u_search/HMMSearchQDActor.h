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

#ifndef _U2_HMM2_QDACTOR_H_
#define _U2_HMM2_QDACTOR_H_

#include "U2Lang/QDScheme.h"
#include <U2Lang/QueryDesignerRegistry.h>


namespace U2 {

class HMMSearchTask;

class HMM2QDActor : public QDActor {
    Q_OBJECT
public:
    HMM2QDActor(QDActorPrototype const* proto);
    int getMinResultLen() const;
    int getMaxResultLen() const;
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const { return QColor(0x66,0xa3,0xd2); }
    virtual bool hasStrand() const { return false; }
    virtual void updateEditor();
private slots:
    void sl_onTaskFinished(Task*);
    void sl_evChanged(int);
private:
    QMap<HMMSearchTask*, qint64> offsets;
};

class HMM2QDActorPrototype : public QDActorPrototype {
public:
    HMM2QDActorPrototype();
    QIcon getIcon() const { return QIcon(":hmm2/images/hmmer_16.png"); }
    QDActor* createInstance() const { return new HMM2QDActor(this); }
};

} //namespace

#endif //_U2_HMM2_QDACTOR_H_
