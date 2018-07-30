/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_EXTERNAL_TOOL_REGISTRY_H
#define _U2_EXTERNAL_TOOL_REGISTRY_H

#include <QIcon>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <U2Core/IdRegistry.h>
#include <U2Core/global.h>
#include <U2Core/StrPackUtils.h>

namespace U2 {

//additional tool validations. Even with other executables
class U2CORE_EXPORT ExternalToolValidation {
public:

    ExternalToolValidation(const QString& _toolRunnerProgram, const QString& _executableFile, const QStringList& _arguments, const QString& _expectedMsg, const StrStrMap& _possibleErrorsDescr = StrStrMap())
        :toolRunnerProgram(_toolRunnerProgram)
        ,executableFile(_executableFile)
        ,arguments(_arguments)
        ,expectedMsg(_expectedMsg)
        ,possibleErrorsDescr(_possibleErrorsDescr)
    {}

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
    ExternalTool(QString name, QString path = "");
    ~ExternalTool();

    const QString&      getName()  const { return name; }
    const QString&      getPath()  const { return path; }
    const QIcon&        getIcon()  const { return icon; }
    const QIcon&        getGrayIcon()  const { return grayIcon; }
    const QIcon&        getWarnIcon()  const { return warnIcon; }
    const QString&      getDescription()  const { return description; }
    const QString&      getToolRunnerProgram()  const { return toolRunnerProgramm; }
    const virtual QStringList  getToolRunnerAdditionalOptions() { return QStringList();}
    const QString&      getExecutableFileName()  const { return executableFileName; }
    const QStringList&  getValidationArguments()  const { return validationArguments; }
    const QString&      getValidMessage()  const { return validMessage; }
    const QString&      getVersion()  const { return version; }
    const QRegExp&      getVersionRegExp()  const { return versionRegExp; }
    const QString&      getToolKitName()  const { return toolKitName; }
    const StrStrMap&    getErrorDescriptions()  const { return errorDescriptions; }
    const StrStrMap&    getAdditionalInfo() const;
    virtual QStringList getAdditionalPaths() const;

    virtual void        getAdditionalParameters(const QString& output) { Q_UNUSED(output) }

    ExternalToolValidation getToolValidation();
    const QList<ExternalToolValidation>& getToolAdditionalValidations() const { return additionalValidators; }
    const QStringList& getDependencies() const { return dependencies; }

    void setPath(const QString& _path);
    void setValid(bool _isValid);
    void setVersion(const QString& _version);
    void setAdditionalInfo(const StrStrMap &additionalInfo);

    bool isValid() const { return isValidTool; }
    bool isMuted() const;
    bool isModule() const { return isModuleTool; }

signals:
    void si_pathChanged();
    void si_toolValidationStatusChanged(bool isValid);

protected:
    QString     name;                   // tool name
    QString     path;                   // tool path
    QIcon       icon;                   // valid tool icon
    QIcon       grayIcon;               // not set tool icon
    QIcon       warnIcon;               // invalid tool icon
    QString     description;            // tool description
    QString     toolRunnerProgramm;     // starter program (e.g. python for scripts)
    QString     executableFileName;     // executable file name (without path)
    QStringList validationArguments;    // arguments to validation run (such as --version)
    QString     validMessage;           // expected message in the validation run output
    QString     version;                // tool version
    QRegExp     versionRegExp;          // RegExp to get the version from the validation run output
    bool        isValidTool;            // tool state
    QString     toolKitName;            // toolkit which includes the tool
    StrStrMap   errorDescriptions;      // error messages for the tool's standard errors
    StrStrMap   additionalInfo;         // any additional info, it is specific for the extenal tool
    QList<ExternalToolValidation> additionalValidators;     // validators for the environment state (e.g. some external program should be installed)
    QStringList dependencies;           // a list of dependencies for the tool of another external tools (e.g. python for python scripts).
    bool        muted;                  // a muted tool doesn't write its validation error to the log
    bool        isModuleTool;           // a module tool is a part of another external tool

}; // ExternalTool

class U2CORE_EXPORT ExternalToolModule : public ExternalTool {
    Q_OBJECT
public:
    ExternalToolModule(const QString& name) :
        ExternalTool(name, "") { isModuleTool = true; }
};

class U2CORE_EXPORT ExternalToolValidationListener : public QObject {
    Q_OBJECT
public:
    ExternalToolValidationListener(const QString& toolName = QString());
    ExternalToolValidationListener(const QStringList& toolNames);

    const QStringList& getToolNames() const { return toolNames; }

    void validationFinished() { emit si_validationComplete(); }

    void setToolState(const QString& toolName, bool isValid) { toolStates.insert(toolName, isValid); }
    bool getToolState(const QString& toolName) const { return toolStates.value(toolName, false); }

signals:
    void si_validationComplete();

public slots:
    void sl_validationTaskStateChanged();

private:
    QStringList toolNames;
    QMap<QString, bool> toolStates;
};

class U2CORE_EXPORT ExternalToolManager : public QObject {
    Q_OBJECT
public:
    enum ExternalToolState {
        NotDefined,
        NotValid,
        Valid,
        ValidationIsInProcess,
        SearchingIsInProcess,
        NotValidByDependency,
        NotValidByCyclicDependency
    };

    ExternalToolManager() {}
    virtual ~ExternalToolManager() {}

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void check(const QString& toolName, const QString& toolPath, ExternalToolValidationListener* listener) = 0;
    virtual void check(const QStringList& toolNames, const StrStrMap& toolPaths, ExternalToolValidationListener* listener) = 0;

    virtual void validate(const QString& toolName, ExternalToolValidationListener* listener = NULL) = 0;
    virtual void validate(const QString& toolName, const QString& path, ExternalToolValidationListener* listener = NULL) = 0;
    virtual void validate(const QStringList& toolNames, ExternalToolValidationListener* listener = NULL) = 0;
    virtual void validate(const QStringList& toolNames, const StrStrMap& toolPaths, ExternalToolValidationListener* listener = NULL) = 0;

    virtual bool isValid(const QString& toolName) const = 0;
    virtual ExternalToolState getToolState(const QString& toolName) const = 0;

signals:
    void si_startupChecksFinish();
};

//this register keeps order of items added
//entries are given in the same order as they are added
class U2CORE_EXPORT ExternalToolRegistry : public QObject {
    Q_OBJECT
public:
    ExternalToolRegistry();
    ~ExternalToolRegistry();

    ExternalTool* getByName(const QString& id);

    bool registerEntry(ExternalTool* t);
    void unregisterEntry(const QString& id);

    void setToolkitDescription(const QString& toolkit, const QString& desc) {toolkits[toolkit] = desc;}
    QString getToolkitDescription(const QString& toolkit) const {return toolkits[toolkit];}

    QList<ExternalTool*> getAllEntries() const;
    QList< QList<ExternalTool*> > getAllEntriesSortedByToolKits() const;

    void setManager(ExternalToolManager* manager);
    ExternalToolManager* getManager() const;

protected:
    QList<ExternalTool*>            registryOrder;
    QMap<QString, ExternalTool*>    registry;
    QMap<QString, QString>          toolkits;
    QString                         temporaryDirectory;
    ExternalToolManager*            manager;

}; // ExternalToolRegistry

class U2CORE_EXPORT DefaultExternalToolValidations{
public:
    static ExternalToolValidation pythonValidation();
    static ExternalToolValidation rValidation();
};

} //namespace
#endif // U2_EXTERNAL_TOOL_REGISTRY_H
