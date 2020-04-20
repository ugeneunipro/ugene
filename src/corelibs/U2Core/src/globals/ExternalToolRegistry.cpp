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

#include <QRegularExpression>

#include "ExternalToolRegistry.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/Log.h>

namespace U2 {

////////////////////////////////////////
//ExternalToolValidation
const QString ExternalToolValidation::DEFAULT_DESCR_KEY = "DEFAULT_DESCR";

////////////////////////////////////////
//ExternalTool
ExternalTool::ExternalTool(QString _id, QString _name, QString _path)
    : id(_id),
      name(_name),
      path(_path),
      isValidTool(false),
      toolKitName(_name),
      muted(false),
      isModuleTool(false),
      isCustomTool(false),
    isRunnerTool(false)
{
    if (NULL != AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
}

const QString &ExternalTool::getId() const {
    return id;
}

const QString &ExternalTool::getName() const {
    return name;
}

const QString &ExternalTool::getPath() const {
    return path;
}

const QIcon &ExternalTool::getIcon() const {
    return icon;
}

const QIcon &ExternalTool::getGrayIcon() const {
    return grayIcon;
}

const QIcon &ExternalTool::getWarnIcon() const {
    return warnIcon;
}

const QString &ExternalTool::getDescription() const {
    return description;
}

const QString &ExternalTool::getToolRunnerProgramId() const {
    return toolRunnerProgram;
}

QStringList ExternalTool::getToolRunnerAdditionalOptions() const {
    return QStringList();
}

const QString &ExternalTool::getExecutableFileName() const {
    return executableFileName;
}

const QStringList &ExternalTool::getValidationArguments() const {
    return validationArguments;
}

const QString &ExternalTool::getValidMessage() const {
    return validMessage;
}

const QString &ExternalTool::getVersion() const {
    return version;
}

const QString &ExternalTool::getPredefinedVersion() const {
    return predefinedVersion;
}

const QRegExp &ExternalTool::getVersionRegExp() const {
    return versionRegExp;
}

const QString &ExternalTool::getToolKitName() const {
    return toolKitName;
}

const StrStrMap &ExternalTool::getErrorDescriptions() const {
    return errorDescriptions;
}

const StrStrMap &ExternalTool::getAdditionalInfo() const {
    return additionalInfo;
}

QStringList ExternalTool::getAdditionalPaths() const {
    return QStringList();
}

QStringList ExternalTool::getRunParameters() const {
    return QStringList();
}

void ExternalTool::extractAdditionalParameters(const QString & /*output*/) {
    // do nothing
}

void ExternalTool::performAdditionalChecks(const QString& /*toolPath*/) {
    // do nothing
}

ExternalToolValidation ExternalTool::getToolValidation() {
    ExternalToolValidation result(toolRunnerProgram, executableFileName, validationArguments, validMessage, errorDescriptions);
    return result;
}

const QList<ExternalToolValidation> &ExternalTool::getToolAdditionalValidations() const {
    return additionalValidators;
}

const QStringList &ExternalTool::getDependencies() const {
    return dependencies;
}

const QString& ExternalTool::getAdditionalErrorMessage() const {
    return additionalErrorMesage;
}

void ExternalTool::setAdditionalErrorMessage(const QString& message) {
    additionalErrorMesage = message;
}

bool ExternalTool::hasAdditionalErrorMessage() const {
    return !additionalErrorMesage.isEmpty();
}

void ExternalTool::setPath(const QString& _path) {
    if (path != _path) {
        path = _path;
        emit si_pathChanged();
    }
}

void ExternalTool::setValid(bool _isValid) {
    isValidTool = _isValid;
    emit si_toolValidationStatusChanged(isValidTool);
}

void ExternalTool::setVersion(const QString& _version) {
    version = _version;
}

void ExternalTool::setAdditionalInfo(const StrStrMap &newAdditionalInfo) {
    additionalInfo = newAdditionalInfo;
}

bool ExternalTool::isValid() const {
    return isValidTool;
}

bool ExternalTool::isMuted() const {
#ifdef UGENE_NGS
    // Tool cannot be muted in the NGS pack
    return false;
#else
    return muted;
#endif
}

bool ExternalTool::isModule() const {
    return isModuleTool;
}

bool ExternalTool::isCustom() const {
    return isCustomTool;
}

bool ExternalTool::isRunner() const {
    return isRunnerTool;
}

////////////////////////////////////////
//ExternalToolValidationListener
ExternalToolValidationListener::ExternalToolValidationListener(const QString& toolId) {
    toolIds << toolId;
}

ExternalToolValidationListener::ExternalToolValidationListener(const QStringList& _toolIds) {
    toolIds = _toolIds;
}

void ExternalToolValidationListener::sl_validationTaskStateChanged() {
    Task* validationTask = qobject_cast<Task*>(sender());
    SAFE_POINT(NULL != validationTask, "Unexpected message sender", );
    if (validationTask->isFinished()) {
        emit si_validationComplete();
    }
}

////////////////////////////////////////
//ExternalToolRegistry
ExternalToolRegistry::ExternalToolRegistry() :
    manager(NULL) {
}

ExternalToolRegistry::~ExternalToolRegistry() {
    registryOrder.clear();
    qDeleteAll(registry.values());
}

ExternalTool* ExternalToolRegistry::getByName(const QString& name) const {
    ExternalTool* result = nullptr;
    foreach(ExternalTool* tool, registry.values()) {
        CHECK_CONTINUE(tool->getName() == name);

        result = tool;
        break;
    }

    return result;
}

ExternalTool* ExternalToolRegistry::getById(const QString& id) const {
    return registry.value(id, NULL);
}

QString ExternalToolRegistry::getToolNameById(const QString& id) const {
    ExternalTool* et = getById(id);
    CHECK(nullptr != et, QString());

    return et->getName();
}

namespace {
bool containsCaseInsensitive(const QList<QString>& values, const QString& value) {
    bool result = false;
    foreach(const QString& v, values) {
        CHECK_CONTINUE(QString::compare(v, value, Qt::CaseInsensitive) == 0);
        result = true;
        break;
    };
    return result;
}
}

bool ExternalToolRegistry::registerEntry(ExternalTool *t) {
    if (containsCaseInsensitive(registry.keys(), t->getId())) {
        return false;
    } else {
        registryOrder.append(t);
        registry.insert(t->getId(), t);
        emit si_toolAdded(t->getId());
        return true;
    }
}

void ExternalToolRegistry::unregisterEntry(const QString &id){
    CHECK(registry.contains(id), );
    emit si_toolIsAboutToBeRemoved(id);
    ExternalTool *et = registry.take(id);
    if (nullptr != et) {
        int idx = registryOrder.indexOf(et);
        if (-1 != idx){
            registryOrder.removeAt(idx);
        }

        delete et;
    }
}

QList<ExternalTool*> ExternalToolRegistry::getAllEntries() const
{
    return registryOrder;
}
QList< QList<ExternalTool*> > ExternalToolRegistry::getAllEntriesSortedByToolKits() const
{
    QList< QList<ExternalTool*> > res;
    QList<ExternalTool*> list= registryOrder;
    while(!list.isEmpty()){
        QString name=list.first()->getToolKitName();
        QList<ExternalTool*> toolKitList;
        for(int i=0;i<list.length();i++){
            if(name == list.at(i)->getToolKitName()){
                toolKitList.append(list.takeAt(i));
                i--;
            }
        }
        res.append(toolKitList);
    }
    return res;
}

void ExternalToolRegistry::setManager(ExternalToolManager* _manager) {
    manager = _manager;
}

ExternalToolManager* ExternalToolRegistry::getManager() const {
    return manager;
}

ExternalToolValidation DefaultExternalToolValidations::pythonValidation(){
    QString pythonExecutable = "python";
    QStringList pythonArgs;
    pythonArgs << "--version";
    QString pmsg = "Python";
    StrStrMap perrMsgs;
    perrMsgs.insert(ExternalToolValidation::DEFAULT_DESCR_KEY, "Python 2 required for this tool. Please install Python or set your PATH variable if you have it installed.");

    ExternalToolValidation pythonValidation("", pythonExecutable, pythonArgs, pmsg, perrMsgs);
    return pythonValidation;
}

ExternalToolValidation DefaultExternalToolValidations::rValidation(){
    QString rExecutable = "Rscript";
    QStringList rArgs;
    rArgs << "--version";
    QString rmsg = "R";
    StrStrMap rerrMsgs;
    rerrMsgs.insert(ExternalToolValidation::DEFAULT_DESCR_KEY, "R Script required for this tool. Please install R Script or set your PATH variable if you have it installed.");

    ExternalToolValidation rValidation("", rExecutable, rArgs, rmsg, rerrMsgs);
    return rValidation;
}

}//namespace
