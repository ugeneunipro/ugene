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
ExternalTool::ExternalTool(QString _name, QString _path)
    : name(_name),
      path(_path),
      isValidTool(false),
      toolKitName(_name),
      muted(false),
      isModuleTool(false)
{
    if (NULL != AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
}

ExternalTool::~ExternalTool() {
}

const StrStrMap &ExternalTool::getAdditionalInfo() const {
    return additionalInfo;
}

QStringList ExternalTool::getAdditionalPaths() const {
    return QStringList();
}

void ExternalTool::setPath(const QString& _path) {
    if (path!=_path) {
        path=_path;
        emit si_pathChanged();
    }
}
void ExternalTool::setValid(bool _isValid){
    isValidTool=_isValid;
    emit si_toolValidationStatusChanged(isValidTool);
}
void ExternalTool::setVersion(const QString& _version) {
    version=_version;
}

void ExternalTool::setAdditionalInfo(const StrStrMap &newAdditionalInfo) {
    additionalInfo = newAdditionalInfo;
}

ExternalToolValidation ExternalTool::getToolValidation() {
    ExternalToolValidation result(toolRunnerProgramm, executableFileName, validationArguments, validMessage, errorDescriptions);
    return result;
}

bool ExternalTool::isMuted() const {
#ifdef UGENE_NGS
    // Tool cannot be muted in the NGS pack
    return false;
#else
    return muted;
#endif
}

////////////////////////////////////////
//ExternalToolValidationListener
ExternalToolValidationListener::ExternalToolValidationListener(const QString& toolName) {
    toolNames << toolName;
}

ExternalToolValidationListener::ExternalToolValidationListener(const QStringList& _toolNames) {
    toolNames = _toolNames;
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

ExternalTool* ExternalToolRegistry::getByName(const QString& id)
{
    return registry.value(id, NULL);
}

bool ExternalToolRegistry::registerEntry(ExternalTool *t){
    if (registry.contains(t->getName())) {
        return false;
    } else {
        registryOrder.append(t);
        registry.insert(t->getName(), t);
        return true;
    }
}

void ExternalToolRegistry::unregisterEntry(const QString &id){
    ExternalTool* et = registry.take(id);
    if(et!=NULL){
        int idx = registryOrder.indexOf(et);
        if (idx!=-1){
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
