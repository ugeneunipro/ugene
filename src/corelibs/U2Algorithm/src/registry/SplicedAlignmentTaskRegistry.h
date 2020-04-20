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

#ifndef _U2_SPLICED_ALIGNMENT_TASK_REGISTRY_H_
#define _U2_SPLICED_ALIGNMENT_TASK_REGISTRY_H_

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>

#include <U2Core/global.h>

namespace U2 {

class SplicedAlignmentTaskFactory;
class SplicedAlignmentTask;

class U2ALGORITHM_EXPORT SplicedAlignmentTaskRegistry : public QObject {
    Q_OBJECT
public:
    explicit SplicedAlignmentTaskRegistry(QObject *parent = 0);
    ~SplicedAlignmentTaskRegistry();
    bool registerTaskFactory(SplicedAlignmentTaskFactory* alg, const QString& algId);
    void unregisterTaskFactory(const QString& algId);
    bool hadRegistered(const QString& algId);
    SplicedAlignmentTaskFactory* getAlgorithm(const QString& algId);
    QStringList getAlgNameList();
private:
    QMutex mutex;
    QMap<QString, SplicedAlignmentTaskFactory*> algMap;
    Q_DISABLE_COPY(SplicedAlignmentTaskRegistry)
};

} // namespace

#endif // _U2_SPLICED_ALIGNMENT_TASK_REGISTRY_H_
