﻿/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QIcon>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <U2Core/StrPackUtils.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

// additional tool validations. Even with other executables
class U2CORE_EXPORT ExternalToolValidation {
public:
    ExternalToolValidation(const QString& _toolRunnerProgram, const QString& _executableFile, const QStringList& _arguments, const QString& _expectedMsg, const StrStrMap& _possibleErrorsDescr = StrStrMap())
        : toolRunnerProgram(_toolRunnerProgram), executableFile(_executableFile), arguments(_arguments), expectedMsg(_expectedMsg), possibleErrorsDescr(_possibleErrorsDescr) {
    }

public:
    QString toolRunnerProgram;
    QString executableFile;
    QStringList arguments;
    QString expectedMsg;
    StrStrMap possibleErrorsDescr;

    static const QString DEFAULT_DESCR_KEY;
};

class U2CORE_EXPORT ExternalTool : public QObject {
    Q_OBJECT
public:
    ExternalTool(const QString& id, const QString& dirName, const QString& name, const QString& path = "");

    const QString& getId() const;

    const QString& getDirName() const {
        return dirName;
    }

    const QString& getName() const;
    const QString& getPath() const;
    const QIcon& getIcon() const;
    const QIcon& getGrayIcon() const;
    const QIcon& getWarnIcon() const;
    const QString& getDescription() const;
    const QString& getToolRunnerProgramId() const;
    virtual QStringList getToolRunnerAdditionalOptions() const;
    const QString& getExecutableFileName() const;
    const QString& getVersion() const;
    const QString& getPredefinedVersion() const;
    const QRegExp& getVersionRegExp() const;
    const QString& getToolKitName() const;
    const StrStrMap& getErrorDescriptions() const;
    const StrStrMap& getAdditionalInfo() const;
    virtual QStringList getAdditionalPaths() const;
    virtual QStringList getRunParameters() const;

    virtual void extractAdditionalParameters(const QString& output);
    virtual void performAdditionalChecks(const QString& toolPath);
    /* Check external tool for paths compatibility. */
    void checkArgsAndPaths(const QStringList& arguments, U2OpStatus &os) const;
    /* Check paths which passed as arguments */
    void checkArgs(const QStringList& arguments, U2OpStatus &os) const;
    /* Check "static" paths e.g. path to tool, temp directory */
    void checkPaths(U2OpStatus &os) const;

    ExternalToolValidation getToolValidation();
    const QList<ExternalToolValidation>& getToolAdditionalValidations() const;
    const QStringList& getDependencies() const;
    const QString& getAdditionalErrorMessage() const;
    void setAdditionalErrorMessage(const QString& message);
    bool hasAdditionalErrorMessage() const;

    void setPath(const QString& _path);
    void setValid(bool isValid);
    void setChecked(bool isChecked);
    void setVersion(const QString& _version);
    void setAdditionalInfo(const StrStrMap& additionalInfo);

    bool isValid() const;
    bool isChecked() const;
    bool isMuted() const;
    bool isModule() const;
    bool isCustom() const;
    bool isRunner() const;

signals:
    void si_pathChanged();
    void si_toolValidationStatusChanged(bool isValid);

protected:
    /** Unique external tool id. */
    QString id;
    /** Tool folder name or sub-path in 'tools' directory. */
    QString dirName;
    /** Visual name of the tool. */
    QString name;
    QString path;  // tool path
    QIcon icon;  // valid tool icon
    QIcon grayIcon;  // not set tool icon
    QIcon warnIcon;  // invalid tool icon
    QString description;  // tool description
    QString toolRunnerProgram;  // starter program (e.g. python for scripts)
    QString executableFileName;  // executable file name (without path)
    QStringList validationArguments;  // arguments to validation run (such as --version)
    QString validMessage;  // expected message in the validation run output
    QString version;  // tool version
    QString predefinedVersion;  // tool's predefined version, this value is used if tool is not validated and there is no possibility to get actual version
    QRegExp versionRegExp;  // RegExp to get the version from the validation run output
    bool isValidTool;  // tool state

    /** If true the tool was already checked/validated by UGENE. */
    bool isCheckedTool;

    QString toolKitName;  // toolkit which includes the tool
    StrStrMap errorDescriptions;  // error messages for the tool's standard errors
    StrStrMap additionalInfo;  // any additional info, it is specific for the extenal tool
    QList<ExternalToolValidation> additionalValidators;  // validators for the environment state (e.g. some external program should be installed)
    QStringList dependencies;  // a list of dependencies for the tool of another external tools (e.g. python for python scripts).
    QString additionalErrorMesage;  // a string, which contains an error message, specific for each tool
    bool muted;  // a muted tool doesn't write its validation error to the log
    bool isModuleTool;  // a module tool is a part of another external tool
    bool isCustomTool;  // the tool is described in a user-written config file and imported to UGENE
    bool isRunnerTool;  // the tool could be used as script-runner

    enum class PathChecks {
        NonLatinArguments, /* Tool start command line: externalTool -human_T1.da -p C:/User/student/目錄/output.fa  */
        NonLatinTemporaryDirPath, /* Environment variable: TMP=C:/User/temp/目錄/ */
        NonLatinToolPath, /* Tool location: C:/tools/tool/目錄/ */
        NonLatinIndexPath, /* Setting->Preferences->Directories->Build inexes: C:/indexes/目錄/ */
        SpacesArguments, /* Tool start command line: externalTool -human_T1.da -p C:/User/student/S P A C E S/output.fa  */
        SpacesTemporaryDirPath, /* Environment variable: TMP=C:/User/temp/S P A C E S/ */
        SpacesToolPath /* Tool location: C:/tools/tool/S P A C E S/ */
    };

    QList<PathChecks> pathChecks;

};  // ExternalTool

class U2CORE_EXPORT ExternalToolModule : public ExternalTool {
    Q_OBJECT
public:
    ExternalToolModule(const QString& id, const QString& dirName, const QString& name)
        : ExternalTool(id, dirName, name) {
        isModuleTool = true;
    }
};

class U2CORE_EXPORT ExternalToolValidationListener : public QObject {
    Q_OBJECT
public:
    ExternalToolValidationListener(const QString& toolId = QString());
    ExternalToolValidationListener(const QStringList& toolIds);

    const QStringList& getToolIds() const {
        return toolIds;
    }

    void validationFinished() {
        emit si_validationComplete();
    }

    void setToolState(const QString& toolId, bool isValid) {
        toolStates.insert(toolId, isValid);
    }

    bool getToolState(const QString& toolId) const {
        return toolStates.value(toolId, false);
    }

signals:
    void si_validationComplete();

private:
    QStringList toolIds;
    QMap<QString, bool> toolStates;
};

class U2CORE_EXPORT ExternalToolManager : public QObject {
    Q_OBJECT
public:
    enum ExternalToolState {
        /** The initial tool state on UGENE startup or a new tool addition: not checked/processed by UGENE. */
        Unprocessed,

        /** Tool validation task or tool search task is in progress. */
        ValidationIsInProcess,

        /** The tool was checked and is valid and is ready to be used. */
        Valid,

        /** The tool was checked and is invalid. */
        NotValid,

        /** The tool is not valid because of the missing/not-valid dependency. */
        NotValidByDependency,
    };

    ExternalToolManager() {
    }
    virtual ~ExternalToolManager() {
    }

    virtual void validate(const QStringList& toolNames, const StrStrMap& toolPaths, ExternalToolValidationListener* listener = nullptr) = 0;

    virtual bool isValid(const QString& toolName) const = 0;

    /** Returns true if all startup checks are finished. */
    virtual bool isInStartupValidationMode() const = 0;

    virtual ExternalToolState getToolState(const QString& toolName) const = 0;

signals:
    /** Emitted when startup validation of external tools is finished. */
    void si_startupValidationFinished();
};

// this register keeps order of items added
// entries are given in the same order as they are added
class U2CORE_EXPORT ExternalToolRegistry : public QObject {
    Q_OBJECT
public:
    ExternalToolRegistry();
    ~ExternalToolRegistry();

    ExternalTool* getByName(const QString& name) const;
    ExternalTool* getById(const QString& id) const;
    QString getToolNameById(const QString& id) const;

    bool registerEntry(ExternalTool* tool);
    void unregisterEntry(const QString& id);

    void setToolkitDescription(const QString& toolkit, const QString& desc) {
        toolkits[toolkit] = desc;
    }
    QString getToolkitDescription(const QString& toolkit) const {
        return toolkits[toolkit];
    }

    QList<ExternalTool*> getAllEntries() const;
    QList<QList<ExternalTool*>> getAllEntriesSortedByToolKits() const;

    void setManager(ExternalToolManager* manager);
    ExternalToolManager* getManager() const;

signals:
    void si_toolAdded(const QString& id);
    void si_toolIsAboutToBeRemoved(const QString& id);

protected:
    QMap<QString, ExternalTool*> toolByLowerCaseIdMap;
    QMap<QString, QString> toolkits;
    ExternalToolManager* manager;

};  // ExternalToolRegistry

}  // namespace U2
