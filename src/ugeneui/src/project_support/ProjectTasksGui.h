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

#ifndef _U2_PROJECT_TASKS_GUI_H_
#define _U2_PROJECT_TASKS_GUI_H_

#include <QDir>

#include <U2Core/AppResources.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/Task.h>

#include <U2Test/XMLTestUtils.h>

class QDomDocument;

namespace U2 {

class LoadDocumentTask;
class LoadUnloadedDocumentTask;
class Project;
class StateLock;

/// CloseProjectTask schema:
//	->CloseProjectTask (serial subtasks):
//		->SaveProjectTask
//		->UnregisterProjectTask	
class CloseProjectTask : public Task {
    Q_OBJECT

public:
	CloseProjectTask();
	virtual void prepare();
};


/// OpenProjectTask schema:
//	->OpenProjectTask (serial subtasks):
//		->CloseProjectTask
//		->LoadProjectTask
//		->RegisterProjectTask (added after LoadProjectTask is successfully finished)

class LoadProjectTask;
class OpenProjectTask : public Task {
    Q_OBJECT

public:
	OpenProjectTask(const QString& url, const QString& name = QString::null);
    
	virtual void prepare();
	
protected:
	virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
	QString url;
	QString name;
	LoadProjectTask* loadProjectTask;
};

/// SaveProjectTask schema:
//	->SaveProjectTask
//		->SaveDocumentsTask
class SaveProjectTask : public Task {
    Q_OBJECT

public:
    SaveProjectTask(SaveProjectTaskKind k, Project* p = NULL, const QString& url = QString::null, bool silentSave_ = false);
    ~SaveProjectTask();

    virtual void prepare();

private:
    SaveProjectTaskKind k;
    Project* proj;
    QString url;
    bool silentSave;
};

class SaveOnlyProjectTask : public Task {
    Q_OBJECT
public:
    SaveOnlyProjectTask(Project* p = NULL, const QString& url = QString::null);
    ~SaveOnlyProjectTask();

    virtual void prepare();
    ReportResult report();
    
private:
    QList<Task*> onSubTaskFinished(Task* subTask);
    void _run();

    Task *sub;
    Project* proj;
    QString url;
    StateLock* lock;
    QList<Document*> phantomDocs;
};


/// LoadProjectTask
class LoadProjectTask : public Task {
    Q_OBJECT

public:
	LoadProjectTask(const QString& url);
	~LoadProjectTask();

	virtual void run();
	ReportResult report();

	Project* getProject() const {return proj;}
	Project* detachProject() {Project* tmp = proj; proj = NULL; return tmp;}

private:
	QString         version;
	Project*        proj;
	QString         url;
	QDomDocument*   xmlDoc;
};


/// register project task
//TODO: remove service+plugin binding -> add 'singleton' property to service. After its done this task will not be needed

class RegisterProjectServiceTask : public Task {
    Q_OBJECT
public:
    RegisterProjectServiceTask(Project* proj);
    void prepare();
private:
    Project* proj;
};



class ExportProjectTask : public Task{
	Q_OBJECT
public:
	ExportProjectTask(const QString& _destinationDir, const QString& projectFile, bool _compress = false);

	void prepare();
    ReportResult report();

private:
	void _run(TaskStateInfo& ts, Project* p, QString url, QStringList paths);

	bool        compress;
	QString     destinationDir;
    QString     projectFile;
};

//cppcheck-suppress noConstructor
class GTest_LoadProject : public XmlTest {
	Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_LoadProject, "load-project", TaskFlags_NR_FOSCOE);

	void prepare();

	ReportResult report();

	virtual void cleanup();

	QList<Task*> onSubTaskFinished(Task* subTask);
private:
	QStringList			docContextList;
 	QString             projContextName;
	LoadProjectTask*    loadTask;
	MultiTask			*mt;
 	bool                contextAdded;
 	QString             url;	
};

//cppcheck-suppress noConstructor
class GTest_ExportProject : public XmlTest {
	Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_ExportProject, "export-project", TaskFlags_NR_FOSCOE);

	void prepare();

	ReportResult report();

	virtual void cleanup();
private:
	bool removeDir( const QDir &dir);
	QString             url;	
	QString				projContextname;
	ExportProjectTask   *exportTask;
};

//cppcheck-suppress noConstructor
class GTest_UnloadProject : public XmlTest {
	Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_UnloadProject, "unload-project", TaskFlags_NR_FOSCOE);

	void prepare();
private:
	QStringList unloadDocList;
};

//cppcheck-suppress noConstructor
class GTest_LoadDocumentFromProject : public XmlTest {
	Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_LoadDocumentFromProject, "load-document-from-project", TaskFlags_NR_FOSCOE);

	void prepare();
	void cleanup();
	ReportResult report();
private:
	LoadUnloadedDocumentTask *loadTask;
	Document* loadedDoc;
	QString documentFileName;
	bool contextAdded;
};

class ProjectTests {
public:
	static QList<XMLTestFactory*> createTestFactories();
};

}//namespace

#endif
