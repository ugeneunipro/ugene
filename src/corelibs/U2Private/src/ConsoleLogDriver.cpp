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

#include "ConsoleLogDriver.h"

#include <U2Core/Timer.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineHelpProvider.h>
#include <U2Core/CMDLineCoreOptions.h>

#ifdef Q_OS_WIN32
#include "windows.h"
#endif
#include <stdio.h>
//#include <conio.h>
#define LOG_SETTINGS_ROOT QString("log_settings/")
#define TSB_SETTINGS_ROOT QString("task_status_bar_settings/")

namespace U2 {

const QString ConsoleLogDriver::LOG_SHOW_DATE_CMD_OPTION           = "log-show-date";
const QString ConsoleLogDriver::LOG_SHOW_LEVEL_CMD_OPTION          = "log-show-level";
const QString ConsoleLogDriver::LOG_SHOW_CATEGORY_CMD_OPTION       = "log-show-category";
const QString ConsoleLogDriver::LOG_LEVEL_NONE_CMD_OPTION          = "log-level-none";
const QString ConsoleLogDriver::LOG_LEVEL_ERROR_CMD_OPTION         = "log-level-error";
const QString ConsoleLogDriver::LOG_LEVEL_INFO_CMD_OPTION          = "log-level-info";
const QString ConsoleLogDriver::LOG_LEVEL_DETAILS_CMD_OPTION       = "log-level-details";
const QString ConsoleLogDriver::LOG_LEVEL_TRACE_CMD_OPTION         = "log-level-trace";
const QString ConsoleLogDriver::COLOR_OUTPUT_CMD_OPTION            = "log-color-output";
const QString ConsoleLogDriver::LOG_SETTINGS_ACTIVE_FLAG           = "activeFlagLevel";
//const QString ConsoleLogDriver::LOG_FORMAT                         = "log-format";
//const QString ConsoleLogDriver::LOG_LEVEL                          = "log-level";

bool ConsoleLogDriver::helpRegistered = false;

ConsoleLogDriver::ConsoleLogDriver() : printToConsole (true) {
    LogServer::getInstance()->addListener(this);

    if( !helpRegistered ) {
        setLogCmdlineHelp();
    }

    setLogSettings();
    setCmdLineSettings();
}

ConsoleLogDriver::~ConsoleLogDriver() {
    LogServer::getInstance()->removeListener(this);
}

void ConsoleLogDriver::setLogCmdlineHelp() {
    assert( !helpRegistered );
    helpRegistered = true;

    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();
    assert( NULL != cmdLineRegistry );

    CMDLineHelpProvider * logFormat = new CMDLineHelpProvider(
        CMDLineCoreOptions::LOG_FORMAT,
        tr( "Specifies the format of a log line." ),
        tr( "Specifies the format of a log line."
            "\n\nUse the following notations:"
            " L - level, C - category, YYYY or YY - year, MM - month, dd - day,"
            " hh - hour, mm - minutes, ss - seconds, zzz - milliseconds."
            " \n\nBy default, logformat=\"[L][hh:mm]\"." ),
        tr( "\"<format_string>\"" ));

    CMDLineHelpProvider * logLevel = new CMDLineHelpProvider(
        CMDLineCoreOptions::LOG_LEVEL,
        tr( "Sets the log level." ),
        tr( "Sets the log level per category. If a category is not specified,"
            " the log level is applied to all categories."
            "\n\nThe following categories are available: \n\"Algorithms\""
            ", \"Console\", \"Core Services\", \"Input/Output\", \"Performance\""
            ", \"Remote Service\", \"Scripts\", \"Tasks\"."
            "\n\nThe following log levels are available: TRACE, DETAILS, INFO, ERROR or NONE."
            "\n\nBy default, loglevel=\"ERROR\"." ),
        tr( "\"<category1>=<level1> [<category2>=<level2> ...]\" | <level>" ));

    CMDLineHelpProvider *coloredOutput = new CMDLineHelpProvider(
        ConsoleLogDriver::COLOR_OUTPUT_CMD_OPTION, tr("Enables colored output."));

    cmdLineRegistry->registerCMDLineHelpProvider( logFormat );
    cmdLineRegistry->registerCMDLineHelpProvider( logLevel );
    cmdLineRegistry->registerCMDLineHelpProvider(coloredOutput);
}

QString ConsoleLogDriver::getLevelName(int i) const{
    switch(i) {
        case 0:
            return "TRACE";
        case 1:
            return "DETAILS";
        case 2:
            return "INFO";
        case 3:
            return "ERROR";
        default:
            return "";
    }
}

void ConsoleLogDriver::setLogSettings() {
    CMDLineRegistry *cmd = AppContext::getCMDLineRegistry();
    if(cmd->hasParameter(CMDLineCoreOptions::LOG_FORMAT)) {
       QString logFormat = cmd->getParameterValue(CMDLineCoreOptions::LOG_FORMAT);
       settings.showLevel = logFormat.contains("L", Qt::CaseSensitive);
       settings.showCategory = logFormat.contains("C", Qt::CaseSensitive);
       settings.showDate = logFormat.contains(QRegExp("[M{2}Y{2,4}d{2}H{2}m{2}s{2}z{3}]"));
       settings.logPattern = logFormat;
    } else if(cmd->hasParameter( LOG_SHOW_DATE_CMD_OPTION ) || //old options
        cmd->hasParameter( LOG_SHOW_LEVEL_CMD_OPTION ) ||
        cmd->hasParameter( LOG_SHOW_CATEGORY_CMD_OPTION )){

        settings.logPattern = "";
        if (cmd->hasParameter( LOG_SHOW_DATE_CMD_OPTION )) {
            settings.showDate = true;
            settings.logPattern += "[hh:mm]";
        }
        if (cmd->hasParameter( LOG_SHOW_LEVEL_CMD_OPTION )) {
            settings.showLevel = true;
            settings.logPattern += "[L]";
        }
        if (cmd->hasParameter( LOG_SHOW_CATEGORY_CMD_OPTION )) {
            settings.showCategory = true;
            settings.logPattern += "[C]";
        }
    } else{
        settings.logPattern = "[hh:mm][L]";
    }

    QString logLevel;
    if(cmd->hasParameter(CMDLineCoreOptions::LOG_LEVEL)){
        logLevel = cmd->getParameterValue(CMDLineCoreOptions::LOG_LEVEL);
    } else if( cmd->hasParameter( LOG_LEVEL_NONE_CMD_OPTION ) ){
        logLevel = "NONE";
    } else if(cmd->hasParameter( LOG_LEVEL_ERROR_CMD_OPTION )) {
        logLevel = "ERROR";
    }else if( cmd->hasParameter( LOG_LEVEL_INFO_CMD_OPTION ) ) {
        logLevel = "INFO";
    } else if( cmd->hasParameter( LOG_LEVEL_DETAILS_CMD_OPTION ) ) {
        logLevel = "DETAILS";
    } else if( cmd->hasParameter( LOG_LEVEL_TRACE_CMD_OPTION ) ) {
        logLevel = "TRACE";
    }else {
        logLevel = "ERROR";
    }

    LogServer* ls = LogServer::getInstance();
    const QStringList& categoryList = ls->getCategories();
    logLevel = logLevel.remove(" ");
    QStringList cats = logLevel.split(QRegExp("[,=]"));

    LogCategories::init();
    if(cats.size() == 1) {
        int minLevel = 10;
        for (int i=0; i<LogLevel_NumLevels; i++) {
            if(getLevelName((LogLevel)i) == logLevel) {
                minLevel = i;
            }
        }
        for (int i=0; i<LogLevel_NumLevels; i++) {
            settings.activeLevelGlobalFlag[i] = (i >= minLevel);
        }
        foreach(const QString &str, categoryList) {
            LoggerSettings cs;
            cs.categoryName = str;
            for (int i=0; i<LogLevel_NumLevels; i++) {
                cs.activeLevelFlag[i] = (i >= minLevel);
            }
            settings.categories[str] = cs;
        }
    } else {

        foreach(const QString &str, categoryList) {
            LoggerSettings cs;
            cs.categoryName = str;
            QString catWithoutSpaces = str;
            catWithoutSpaces = catWithoutSpaces.remove(" ");
            if(logLevel.contains(catWithoutSpaces)) {
                int ind = cats.indexOf(catWithoutSpaces);
                QString level = cats[ind + 1];
                int minLevel = 10;
                for (int i=0; i<LogLevel_NumLevels; i++) {
                    if(getLevelName((LogLevel)i) == level) {
                        cs.activeLevelFlag[i] = true;
                        minLevel = i;
                    } else {
                        cs.activeLevelFlag[i] = (i > minLevel);
                    }
                }

            } else {
                for (int i=0; i<LogLevel_NumLevels; i++) {
                    cs.activeLevelFlag[i] = false;
                }
            }
            settings.categories[str] = cs;
        }
    }
}

void ConsoleLogDriver::setCmdLineSettings() {
    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();
    assert( NULL != cmdLineRegistry );
    Settings * settings = AppContext::getSettings();
    assert( NULL != settings );

    if (cmdLineRegistry->hasParameter( COLOR_OUTPUT_CMD_OPTION )) {
        colored = true;
    } else {
        colored = false;
    }
    if (cmdLineRegistry->hasParameter( CMDLineCoreOptions::TEAMCITY_OUTPUT )) {
        settings->setValue( LOG_SETTINGS_ROOT + "teamcityOut", true );
    }
}

QString ConsoleLogDriver::prepareText(const LogMessage& msg) const {
    QString prefix = settings.logPattern;
    prefix.replace("C", getEffectiveCategory(msg));
    prefix.replace("L", LogCategories::getLocalizedLevelName(msg.level));
    QStringList date =  GTimer::createDateTime(msg.time).toString("yyyy:yy:MM:dd:hh:mm:ss:zzz").split(":");
    prefix.replace("YYYY", date[0]);
    prefix.replace("YY", date[1]);
    prefix.replace("MM", date[2]);
    prefix.replace("dd", date[3]);
    prefix.replace("hh", date[4]);
    prefix.replace("mm", date[5]);
    prefix.replace("ss", date[6]);
    prefix.replace("zzz", date[7]);
    QString spacing = prefix.isEmpty() ? QString() : QString(" ");
    QString text = prefix + spacing + msg.text;

    return text;
}


void ConsoleLogDriver::onMessage(const LogMessage& msg) {
    if (!printToConsole || !settings.activeLevelGlobalFlag[msg.level]) {
        return;
    }
    QString effCategory = getEffectiveCategory(msg);
    if (effCategory.isEmpty()) {
        return;
    }

    //this is Hard Code
    if (msg.text.contains(QString("##teamcity"))&&(!AppContext::getSettings()->getValue("log_settings/teamcityOut",true).toBool())) {
        return;
    }
    if (effCategory == ULOG_CAT_USER_INTERFACE) {
        return; //do not print UI related messages
    }
    QByteArray ba = prepareText(msg).toLocal8Bit();
    char* buf = ba.data();
#ifdef Q_OS_WIN32
    // a bit of magic to workaround Windows console encoding issues
    CharToOemA(buf,buf);
#endif
    if(AppContext::getSettings()->getValue(TSB_SETTINGS_ROOT + "showTaskStatusBar", true).toBool()){
        printf("                                                                               \r");//80 spaces for remove TaskStatusBar
    }
    if(!colored/*AppContext::getSettings()->getValue(LOG_SETTINGS_ROOT + "colorOut", false).toBool()*/){
        printf("%s \n", buf);
    }else{
#ifdef Q_OS_WIN32
        if (msg.level==LogLevel_ERROR){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED );
        } else if (msg.level==LogLevel_DETAILS){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN );
        } else if (msg.level==LogLevel_TRACE){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE );
        }
        printf("%s \n", buf);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
#else
        //TO DO: for MacOs release coloring function
        if (msg.level==LogLevel_ERROR){
            printf("\e[31m%s \e[0m\n", buf);
        }else if (msg.level==LogLevel_DETAILS){
            printf("\e[32m%s \e[0m\n", buf);
        }else if (msg.level==LogLevel_TRACE){
            printf("\e[34m%s \e[0m\n", buf);
        }
#endif
    }
    fflush(stdout);
}

QString ConsoleLogDriver::getEffectiveCategory(const LogMessage& msg) const {
    QString result;
    foreach (const QString& category, msg.categories) {
        const LoggerSettings& cs = settings.getLoggerSettings(category);
        if (cs.activeLevelFlag[msg.level]) {
            result = category;
            break;
        }
    }

    return result;
}

}//namespace

