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

#include "TaskStatusBar.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineHelpProvider.h>
#include <U2Lang/WorkflowRunTask.h>

#include <QEvent>
#include <QString>

#include <math.h>
#include <stdio.h>

#define LOG_SETTINGS_ROOT QString("log_settings/")

#define sizeProgress 10
namespace U2 {

const QString TaskStatusBarCon::NO_TASK_STATUS_BAR_CMD_OPTION  = "log-no-task-progress";

bool TaskStatusBarCon::helpRegistered = false;

TaskStatusBarCon::TaskStatusBarCon() {
    taskToTrack = NULL;
#ifdef Q_OS_WIN32
    //TO DO: May be use following variant, but it don`t work always
    //COORD conSize=GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));
    //printf("X=%d, Y=%d\n",conSize.X,conSize.Y);
    emptyLine=QString((int)80-1, QChar(' '));//80 spaces
#endif

    if( !helpRegistered ) {
        setTSBCmdlineHelp();
    }

    setTSBSettings();

    Settings * settings = AppContext::getSettings();
    if( settings->getValue( TSB_SETTINGS_ROOT + "showTaskStatusBar", false ).toBool() ){
        connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));
        updateState();
    }
}

void TaskStatusBarCon::setTSBCmdlineHelp() {
    assert( !helpRegistered );
    helpRegistered = true;

    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();
    assert( NULL != cmdLineRegistry );

    CMDLineHelpProvider * noTSBSection = new CMDLineHelpProvider(
        NO_TASK_STATUS_BAR_CMD_OPTION,
        tr( "Specifies not to show the task progress." ),
        tr( "A task progress is shown by default when a task is running."
            " This option specifies not to show the progress." ));

    cmdLineRegistry->registerCMDLineHelpProvider( noTSBSection );
}

void TaskStatusBarCon::setTSBSettings() {
    if (AppContext::getCMDLineRegistry()->hasParameter( NO_TASK_STATUS_BAR_CMD_OPTION )) {
        AppContext::getSettings()->setValue( TSB_SETTINGS_ROOT + "showTaskStatusBar", false);
    } else {
        AppContext::getSettings()->setValue( TSB_SETTINGS_ROOT + "showTaskStatusBar", true);
    }
}

TaskStatusBarCon::~TaskStatusBarCon(){
    printf("                                                                               \r");//80 spaces
}
void TaskStatusBarCon::updateState() {
    if (taskToTrack ==  NULL) {
        return;
    }

    int nTasks = AppContext::getTaskScheduler()->getTopLevelTasks().size();//+1 is hard code
    int nSubTasks = taskToTrack->getSubtasks().size();
    int progress = taskToTrack->getProgress();
    if (progress==-1) {
        progress=0;
    }

    char progressLine[sizeProgress+3];
    progressLine[0]='[';
    progressLine[sizeProgress+1]=']';
    progressLine[sizeProgress+2]='\0';
    for(int i=0;i<sizeProgress;i++){
        if ((i)*(100/sizeProgress)<progress){
            progressLine[i+1]='#';
        }else{
            progressLine[i+1]=' ';
        }
    }

    QByteArray ba = taskToTrack->getTaskName().toLocal8Bit();
    char* buf = ba.data();
#ifdef Q_OS_WIN32
    // a bit of magic to workaround Windows console encoding issues
    CharToOemA(buf,buf);
#endif
    static int working;
    char ch = 0;
    if (working==0){
        ch='\\';
        working=1;
    }else if (working==1){
        ch='|';
        working=2;
    }else if (working==2){
        ch='/';
        working=3;
    }else if (working==3){
        ch='-';
        working=0;
    }else{
         working=0;
    }
#ifdef Q_OS_WIN32
    //TO DO: Need refactoring this place for linux
    printf("%s\r", emptyLine.toLatin1().constData());//80 spaces
    //printf("                                                                               \r");//80 spaces
#endif
    if(!AppContext::getSettings()->getValue(LOG_SETTINGS_ROOT + "colorOut", false).toBool()){
    if (nSubTasks<=1){
        printf("%c %s %d%% Tasks: %d, Info: %s \r",ch,progressLine,progress,nTasks+1,buf);
    }else{
        printf("%c %s %d%% Tasks: %d, SubTs: %d, Info: %s \r",ch,progressLine,progress,nTasks+1,nSubTasks,buf);
    }
    }else{
    #ifdef Q_OS_WIN32
        if (nSubTasks<=1){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED );
            printf("%c %s %d%% ",ch,progressLine,progress);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
            printf("Tasks: %d, Info: %s \r",nTasks+1,buf);
        }else{
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED );
            printf("%c %s %d%% ",ch,progressLine,progress);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
            printf("Tasks: %d, SubTs: %d, Info: %s \r",nTasks+1,nSubTasks,buf);
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0007 );//0x0007 is white color
    #else
        if (nSubTasks<=1){
            printf("\e[31m%c %s %d%% \e[32mTasks: %d, Info: %s \e[0m\r",ch,progressLine,progress,nTasks+1,buf);
        }else{
            printf("\e[31m%c %s %d%% \e[32mTasks: %d, SubTs: %d, Info: %s \e[0m\r",ch,progressLine,progress,nTasks+1,nSubTasks,buf);
        }

    #endif
    }

}

void TaskStatusBarCon::sl_taskStateChanged(Task* t) {
    assert(taskToTrack == NULL);
    if (t->isFinished()) {
        return;
    }
    WorkflowRunTask* workflowTask = qobject_cast<WorkflowRunTask*>(t);
    if (workflowTask == NULL) { // track progress only for workflow tasks
        return;
    }
    setTaskToTrack(t);
    AppContext::getTaskScheduler()->disconnect(this);
}

void TaskStatusBarCon::setTaskToTrack(Task* t) {
    assert(taskToTrack == NULL);
    taskToTrack = t;
    connect(taskToTrack, SIGNAL(si_stateChanged()), SLOT(sl_taskStateChanged()));
    connect(taskToTrack, SIGNAL(si_progressChanged()), SLOT(sl_update()));
    connect(taskToTrack, SIGNAL(si_descriptionChanged()), SLOT(sl_update()));
    updateState();
}

void TaskStatusBarCon::sl_taskStateChanged() {

    assert(taskToTrack == sender());
    if (!taskToTrack->isFinished()) {
        updateState();
        return;
    }
    taskToTrack->disconnect(this);
    taskToTrack = NULL;

    foreach(Task* newT, AppContext::getTaskScheduler()->getTopLevelTasks()) {
        if (!newT->isFinished()) {
            setTaskToTrack(newT);
            break;
        }
    }
    if (taskToTrack == NULL) {
        connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));
    }
    updateState();
}
void TaskStatusBarCon::sl_update() {
    updateState();
}

} //namespace
