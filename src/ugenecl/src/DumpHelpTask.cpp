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

#include "DumpHelpTask.h"

#include <cstdio>
#ifdef Q_OS_WIN32
#include "windows.h"
#endif

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/Version.h>
#include <U2Core/Log.h>

#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/WorkflowIOTasks.h>

#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineHelpProvider.h>
#include <U2Core/CMDLineCoreOptions.h>

void printStringToConsole(const char* format, const QString& str){
    QByteArray ba = str.toLocal8Bit();
    char* buf = ba.data();
#ifdef Q_OS_WIN32
    // a bit of magic to workaround Windows console encoding issues
    CharToOemA(buf,buf);
#endif
    printf(format, buf);
}

namespace U2 {

const QString DumpHelpTask::VERSION_INFO = QString("\nConsole version of UGENE %1\n").arg(Version::appVersion().text);

static void dumpProgramNameAndUsage() {
    printStringToConsole( "%s" , DumpHelpTask::VERSION_INFO);
    printStringToConsole("%s", "Usage: ugene [[--task=]task_name] [--task_parameter=value] [-task_parameter value] "
        "[--option[=value]] [-option [value]]\n\n");
}

static void dumpSectionName( const QString & name ) {
    printStringToConsole( "   --%-20s", name);
}

static void dumpSectionContent( const QString & content ) {
    printStringToConsole( "\t%s", content);
}

static void dumpSectionIndent() {
    printStringToConsole( "%28s", " " );
}

static void dumpTaskName(const QString & taskName) {
    printStringToConsole( "     %-20s\n", taskName);
}

static void dumpOptionHelpSyntax(const QString & option, const QString & argsDescription) {
    QString optionHelp;
    if (argsDescription.isEmpty()) {
        optionHelp = "ugene --" + option;
    } else {
        optionHelp = "ugene --" + option + "=" + argsDescription;
    }
    printStringToConsole( "%s\n", optionHelp);
}

static void dumpOptionHelpDescription(const QString & description) {
    printStringToConsole( "%s\n", description);
}

void DumpHelpTask::dumpHelp() {
    dumpProgramNameAndUsage();

    printStringToConsole("%s", "\nOptions: \n" );
    QString prevSectionName;
    QList<CMDLineHelpProvider* > helpProviders = AppContext::getCMDLineRegistry()->listCMDLineHelpProviders();
    foreach (CMDLineHelpProvider* hProvider, helpProviders) {
        assert(hProvider != NULL);
        const QString& sectionName = hProvider->getHelpSectionNames();
        if(sectionName != prevSectionName) {
            dumpSectionName( sectionName );
            prevSectionName = sectionName;
        } else {
            dumpSectionIndent();
        }
        dumpSectionContent( hProvider->getHelpSectionShortDescription() );
        printStringToConsole("%s", "\n");
    }
    printStringToConsole("%s", "\n" );

    printStringToConsole("%s", "\nAvailable tasks:\n");
    QStringList dataDirs = QDir::searchPaths(PATH_PREFIX_DATA);
    foreach(const QString & url, dataDirs ) {
        QString dirUrl = url + "/cmdline/";
        QDir dir(dirUrl);
        if(dir.exists()) {
            QStringList entries = dir.entryList(QDir::Files | QDir::Readable);
            foreach( const QString & file, entries ) {
                foreach(const QString & ext, WorkflowUtils::WD_FILE_EXTENSIONS) {
                    if( file.endsWith(ext) ) {
                        dumpTaskName(file.mid(0, file.size() - ext.size() - 1)); // 1 comes from "."
                        break;
                    }
                }
            }
        }
    }
    printStringToConsole("%s", "Use ugene --help=<task-name> to get full help\n");
}

void DumpHelpTask::prepare() {
    CMDLineRegistry * cmdlineRegistry = AppContext::getCMDLineRegistry();
    assert(cmdlineRegistry != NULL);
    if (cmdlineRegistry->hasParameter(CMDLineCoreOptions::USAGE)) {
        dumpProgramNameAndUsage();
        return;
    }

    QString paramName = cmdlineRegistry->getParameterValue( CMDLineCoreOptions::HELP );
    paramName = paramName.isEmpty() ? cmdlineRegistry->getParameterValue(CMDLineCoreOptions::HELP_SHORT) : paramName;
    if( paramName.isEmpty() ) {
        dumpHelp();
        return;
    }
    QList<CMDLineHelpProvider* > helpProviders = AppContext::getCMDLineRegistry()->listCMDLineHelpProviders();
    int ind = 0;
    int sz = helpProviders.size();
    for( ind = 0; ind < sz; ++ind ) {
        CMDLineHelpProvider * cur = helpProviders.at(ind);
        if( cur->getHelpSectionFullName() == paramName || cur->getHelpSectionShortName() == paramName) {
            break;
        }
    }
    if(ind == sz) {
        // try to find help of workflow designer schema with such name
        QString pathToSchema = WorkflowUtils::findPathToSchemaFile(paramName);
        if( pathToSchema.isEmpty() ) {
            coreLog.error(tr("Can't find help for '%1'").arg(paramName));
            return;
        }

        // will be deleted in sl_loadSchemaStateChanged
        Schema * schema = new Schema();
        Metadata * meta = new Metadata();

        schema->setDeepCopyFlag(true);
        addSubTask(new LoadWorkflowTask( schema, meta, pathToSchema ));
        return;
    }

    // Dumping help of the selected section in the registered help pages
    //
    printStringToConsole("%s", "\n" );

    assert(0 != helpProviders.at(ind)->getHelpSectionFullName());
    dumpOptionHelpSyntax(helpProviders.at(ind)->getHelpSectionFullName(),
        helpProviders.at(ind)->getHelpSectionArgsDescription());

    printStringToConsole("%s", "\n" );

    QString description;
    if (!helpProviders.at(ind)->getHelpSectionFullDescription().isEmpty())
    {
        description = helpProviders.at(ind)->getHelpSectionFullDescription();
    }
    else
    {
        assert(!helpProviders.at(ind)->getHelpSectionShortDescription().isEmpty());
        description = helpProviders.at(ind)->getHelpSectionShortDescription();

        // If the section has several short descriptions, append the next short descriptions
        for (int i = ind + 1; i < sz; ++i) {
            CMDLineHelpProvider * provider = helpProviders.at(i);
            if( provider->getHelpSectionFullName() != paramName && provider->getHelpSectionShortName() != paramName) {
                break;
            }
            description += "\n";
            description += helpProviders.at(i)->getHelpSectionShortDescription();
        }
    }
    dumpOptionHelpDescription(description);
}

static void dumpSchemaMetadata(Metadata * meta) {
    assert(meta != NULL);
    printStringToConsole( "\n%s\n", meta->comment);
}

static void dumpSchemaCmdlineParameters( Schema * schema ) {
    assert(schema != NULL);
    printStringToConsole("%s", "Parameters:\n");
    foreach( Actor * actor, schema->getProcesses() ) {
        assert(actor != NULL);
        QMap<QString, QString>::const_iterator it = actor->getParamAliases().constBegin();
        while( it != actor->getParamAliases().constEnd() ) {
            QString alias = it.value();
            dumpSectionName(alias);
            Attribute * attr = actor->getParameter(it.key());
            assert(attr != NULL);
            if(actor->getAliasHelp().contains(alias)) {
                dumpSectionContent( QString("%1 [%2]").arg(actor->getAliasHelp().value(alias)).arg(attr->getAttributeType()->getDisplayName()));
            } else {
                dumpSectionContent(DumpHelpTask::tr("No help available for this parameter"));
            }
            printStringToConsole("%s", "\n" );
            ++it;
        }
    }
    printStringToConsole("%s", "\n" );
}

QList<Task*> DumpHelpTask::onSubTaskFinished(Task* subTask) {
    LoadWorkflowTask * loadTask = qobject_cast<LoadWorkflowTask*>(subTask);
    assert(loadTask != NULL);

    Schema * schema = loadTask->getSchema();
    Metadata * meta = loadTask->getMetadata();

    dumpSchemaMetadata(meta);
    dumpSchemaCmdlineParameters(schema);

    delete schema;
    delete meta;

    return QList<Task*>();
}

void DumpHelpTask::dumpParameters() {
    QList<StrStrPair> params = AppContext::getCMDLineRegistry()->getParameters();
    QList<StrStrPair>::const_iterator it = params.constBegin();
    while( it != params.constEnd() ) {
        printStringToConsole( "key: \"%s\"", it->first);
        printStringToConsole( " and value: \"%s\"\n", it->second);
        ++it;
    }
}

DumpHelpTask::DumpHelpTask(): Task(tr("Dump help task"), TaskFlags_NR_FOSCOE) {
}

} //namespace
