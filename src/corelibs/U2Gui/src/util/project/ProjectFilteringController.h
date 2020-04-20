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

#ifndef _U2_PROJECT_FILTERING_CONTROLLER_H_
#define _U2_PROJECT_FILTERING_CONTROLLER_H_

#include <QTimer>

#include <U2Core/AbstractProjectFilterTask.h>
#include <U2Core/GObject.h>

namespace U2 {

class AbstractProjectFilterTask;
class ProjectTreeControllerModeSettings;

class ProjectFilteringController : public QObject {
    Q_OBJECT
public:
    ProjectFilteringController(QObject *p = NULL);

    void startFiltering(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs);
    void stopFiltering();

signals:
    void si_objectsFiltered(const QString &groupName, const QList<QPointer<GObject> > &objs);
    void si_filteringStarted();
    void si_filteringFinished();

private slots:
    void sl_objectsFiltered(const QString &groupName, const SafeObjList &objs);
    void sl_filteringFinished();
    void sl_startFiltering();

private:
    void addNewActiveTask(AbstractProjectFilterTask *task);
    void connectNewTask(AbstractProjectFilterTask *task);

    QSet<AbstractProjectFilterTask *> activeFilteringTasks;
    QTimer filterStarter;
    ProjectTreeControllerModeSettings lastSettings;
    QList<QPointer<Document> > lastDocs;

    static const int FILTER_START_INTERVAL;
};

} // namespace U2

#endif // _U2_PROJECT_FILTERING_CONTROLLER_H_
