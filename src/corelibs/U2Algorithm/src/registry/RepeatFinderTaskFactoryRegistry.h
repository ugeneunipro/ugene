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

#ifndef _U2_REPEAT_FINDER_ALG_REGISTRY_H_
#define _U2_REPEAT_FINDER_ALG_REGISTRY_H_

#include <QHash>
#include <QString>
#include <QMutex>
#include <QObject>

#include <U2Core/global.h>
#include <U2Algorithm/RepeatFinderTaskFactory.h>


namespace U2 {

enum RepeatFinderTaskFactoryId {};

class U2ALGORITHM_EXPORT RepeatFinderTaskFactoryRegistry: public QObject {
    Q_OBJECT
public:
    RepeatFinderTaskFactoryRegistry(QObject* pOwn = 0);
    ~RepeatFinderTaskFactoryRegistry();

    bool registerFactory(RepeatFinderTaskFactory* factory, const QString& factoryId);
    bool hadRegistered(const QString& factoryId);
    RepeatFinderTaskFactory* getFactory(const QString& factoryId);
    QStringList getListFactoryNames();

private:
    QMutex mutex;
    QHash<QString, RepeatFinderTaskFactory*> factories;
};

} // namespace

#endif
