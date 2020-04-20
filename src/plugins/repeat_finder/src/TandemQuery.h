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

#ifndef _U2_TANDEM_QUERY_H_
#define _U2_TANDEM_QUERY_H_

#include "RF_SArray_TandemFinder.h"

#include "U2Lang/QDScheme.h"
#include <U2Lang/QueryDesignerRegistry.h>

namespace U2 {

class QDTandemActor : public QDActor {
    Q_OBJECT
public:
    virtual int getMinResultLen() const;
    virtual int getMaxResultLen() const;
    virtual QString getText() const;
    virtual Task *getAlgorithmTask(const QVector<U2Region> &location);
    virtual bool hasStrand() const;
    virtual QList<QPair<QString, QString> > saveConfiguration() const;
    virtual void loadConfiguration(const QList<QPair<QString, QString> > &strMap);
    virtual QColor defaultColor() const;
protected:
    QDTandemActor(QDActorPrototype const *prototype);
    friend class QDTandemActorPrototype;
private slots:
    void sl_onAlgorithmTaskFinished();
private:
    FindTandemsTaskSettings settings;
    QList<TandemFinder *> subTasks;
};

class QDTandemActorPrototype : public QDActorPrototype {
public:
    QDTandemActorPrototype();
    QIcon getIcon() const;
    virtual QDActor *createInstance() const;
};

} // namespace U2

#endif // _U2_TANDEM_QUERY_H_
