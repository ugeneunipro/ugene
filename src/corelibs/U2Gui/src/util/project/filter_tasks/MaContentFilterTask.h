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

#ifndef _U2_MA_CONTENT_FILTER_TASK_H_
#define _U2_MA_CONTENT_FILTER_TASK_H_

#include <U2Core/AbstractProjectFilterTask.h>

namespace U2 {

class MultipleAlignmentObject;
class U2SequenceObject;

//////////////////////////////////////////////////////////////////////////
/// MaContentFilterTask
//////////////////////////////////////////////////////////////////////////

class MsaContentFilterTask : public AbstractProjectFilterTask {
public:    
    MsaContentFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs);
protected:
    bool filterAcceptsObject(GObject *obj);
    
};

class McaReadContentFilterTask : public AbstractProjectFilterTask {
public:    
    McaReadContentFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs);
protected:
    bool filterAcceptsObject(GObject *obj);
};

class McaReferenceContentFilterTask : public AbstractProjectFilterTask {
public:    
    McaReferenceContentFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs);
protected:
    bool filterAcceptsObject(GObject *obj);
};

//////////////////////////////////////////////////////////////////////////
/// MaContentFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

class U2GUI_EXPORT MsaContentFilterTaskFactory : public ProjectFilterTaskFactory {
protected:
    AbstractProjectFilterTask * createNewTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs) const;
};

class U2GUI_EXPORT McaReadContentFilterTaskFactory : public ProjectFilterTaskFactory {
protected:
    AbstractProjectFilterTask * createNewTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs) const;
};

class U2GUI_EXPORT McaReferenceContentFilterTaskFactory : public ProjectFilterTaskFactory {
protected:
    AbstractProjectFilterTask * createNewTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs) const;
};

} // namespace U2

#endif // _U2_OBJ_NAME_FILTER_TASK_H_
