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

#ifndef _U2_EXTERNAL_TOOL_REGISTRY_H
#define _U2_EXTERNAL_TOOL_REGISTRY_H

#include <QIcon>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <U2Core/StrPackUtils.h>

namespace U2 {

//additional tool validations. Even with other executables
class U2CORE_EXPORT ExternalToolValidation {
public:

    ExternalToolValidation(const QString& _toolRunnerProgram, const QString& _executableFile, const QStringList& _arguments, const QString& _expectedMsg, const StrStrMap& _possibleErrorsDescr = StrStrMap())
        :toolRunnerProgram(_toolRunnerProgram)
        , executableFile(_executableFile)
        , arguments(_arguments)
        , expectedMsg(_expectedMsg)
        , possibleErrorsDescr(_possibleErrorsDescr) {}

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
    ExternalTool(QString id, QString name, QString path);

    const QString&      getId() const;
    const QString&      getName() const;
    const QString&      getPath() const;
    const QIcon&        getIcon() const;
    const QIcon&        getGrayIcon() const;
    const QIcon&        getWarnIcon() const;
    const QString&      getDescription() const;
    const QString&      getToolRunnerProgramId() const;
    virtual QStringList getToolRunnerAdditionalOptions() const;
    const QString&      getExecutableFileName() const;
    const QStringList&  getValidationArguments() const;
    const QString&      getValidMessage() const;
    const QString&      getVersion() const;
    const QString&      getPredefinedVersion() const;
    const QRegExp&      getVersionRegExp() const;
    const QString&      getToolKitName() const;
    const StrStrMap&    getErrorDescriptions() const;
    const StrStrMap&    getAdditionalInfo() const;
    virtual QStringList getAdditionalPaths() const;
    virtual QStringList getRunParameters() const;

    virtual void        extractAdditionalParameters(const QString& output);
    virtual void        performAdditionalChecks(const QString& toolPath);

    ExternalToolValidation getToolValidation();
    const QList<ExternalToolValidation>& getToolAdditionalValidations() const;
    const QStringList& getDependencies() const;
    const QString& getAdditionalErrorMessage() const;
    void setAdditionalErrorMessage(const QString& message);
    bool hasAdditionalErrorMessage() const;

    void setPath(const QString& _path);
    void setValid(bool _isValid);
    void setVersion(const QString& _version);
    void setAdditionalInfo(const StrStrMap &additionalInfo);

    bool isValid() const;
    bool isMuted() const;
    bool isModule() const;
    bool isCustom() const;
    bool isRunner() const;

signals:
    void si_pathChanged();
    void si_toolValidationStatusChanged(bool isValid);

protected:
    QString     id;                     // tool id
    QString     name;                   // tool name
    QString     path;                   // tool path
    QIcon       icon;                   // valid tool icon
    QIcon       grayIcon;               // not set tool icon
    QIcon       warnIcon;               // invalid tool icon
    QString     description;            // tool description
    QString     toolRunnerProgram;      // starter program (e.g. python for scripts)
    QString     executableFileName;     // executable file name (without path)
    QStringList validationArguments;    // arguments to validation run (such as --version)
    QString     validMessage;           // expected message in the validation run output
    QString     version;                // tool version
    QString     predefinedVersion;      // tool's predefined version, this value is used if tool is not validated and there is no possibility to get actual version
    QRegExp     versionRegExp;          // RegExp to get the version from the validation run output
    bool        isValidTool;            // tool state
    QString     toolKitName;            // toolkit which includes the tool
    StrStrMap   errorDescriptions;      // error messages for the tool's standard errors
    StrStrMap   additionalInfo;         // any additional info, it is specific for the extenal tool
    QList<ExternalToolValidation> additionalValidators;     // validators for the environment state (e.g. some external program should be installed)
    QStringList dependencies;           // a list of dependencies for the tool of another external tools (e.g. python for python scripts).
    QString     additionalErrorMesage;  // a string, which contains an error message, specific for each tool
    bool        muted;                  // a muted tool doesn't write its validation error to the log
    bool        isModuleTool;           // a module tool is a part of another external tool
    bool        isCustomTool;           // the tool is described in a user-written config file and imported to UGENE
    bool        isRunnerTool;           // the tool could be used as script-runner

}; // ExternalTool

class U2CORE_EXPORT ExternalToolModule : public ExternalTool {
    Q_OBJECT
public:
    ExternalToolModule(const QString& id, const QString& name) :
        ExternalTool(id, name, "") {
        isModuleTool = true;
    }
};

class U2CORE_EXPORT ExternalToolValidationListener : public QObject {
    Q_OBJECT
public:
    ExternalToolValidationListener(const QString& toolId = QString());
    ExternalToolValidationListener(const QStringList& toolIds);

    const QStringList& getToolIds() const { return toolIds; }

    void validationFinished() { emit si_validationComplete(); }

    void setToolState(const QString& toolId, bool isValid) { toolStates.insert(toolId, isValid); }
    bool getToolState(const QString& toolId) const { return toolStates.value(toolId, false); }

signals:
    void si_validationComplete();

public slots:
    void sl_validationTaskStateChanged();

private:
    QStringList toolIds;
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

    virtual void validate(const QString& toolName, ExternalToolValidationListener* listener = nullptr) = 0;
    virtual void validate(const QString& toolName, const QString& path, ExternalToolValidationListener* listener = nullptr) = 0;
    virtual void validate(const QStringList& toolNames, ExternalToolValidationListener* listener = nullptr) = 0;
    virtual void validate(const QStringList& toolNames, const StrStrMap& toolPaths, ExternalToolValidationListener* listener = nullptr) = 0;

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

    ExternalTool* getByName(const QString& name) const;
    ExternalTool* getById(const QString& id) const;
    QString getToolNameById(const QString& id) const;

    bool registerEntry(ExternalTool* t);
    void unregisterEntry(const QString& id);

    void setToolkitDescription(const QString& toolkit, const QString& desc) { toolkits[toolkit] = desc; }
    QString getToolkitDescription(const QString& toolkit) const { return toolkits[toolkit]; }

    QList<ExternalTool*> getAllEntries() const;
    QList< QList<ExternalTool*> > getAllEntriesSortedByToolKits() const;

    void setManager(ExternalToolManager* manager);
    ExternalToolManager* getManager() const;

signals:
    void si_toolAdded(const QString &id);
    void si_toolIsAboutToBeRemoved(const QString &id);

protected:
    QList<ExternalTool*>            registryOrder;
    QMap<QString, ExternalTool*>    registry;
    QMap<QString, QString>          toolkits;
    QString                         temporaryDirectory;
    ExternalToolManager*            manager;

}; // ExternalToolRegistry

class U2CORE_EXPORT DefaultExternalToolValidations {
public:
    static ExternalToolValidation pythonValidation();
    static ExternalToolValidation rValidation();
};

} //namespace
#endif // U2_EXTERNAL_TOOL_REGISTRY_H
