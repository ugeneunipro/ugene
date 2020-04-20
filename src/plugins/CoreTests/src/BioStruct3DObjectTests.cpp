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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/BaseDocumentFormats.h>

#include "BioStruct3DObjectTests.h"

namespace U2 {

#define VALUE_ATTR      "value"
#define OBJ_ATTR        "obj"
#define ATOM_ID_ATTR    "atom-id"
#define MODEL_ID_ATTR   "model-id"
#define X_COORD_ATTR    "x"
#define Y_COORD_ATTR    "y"
#define Z_COORD_ATTR    "z"



void GTest_BioStruct3DNumberOfAtoms::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

    bool ok = false;
    numAtoms = v.toInt(&ok);
    if (!ok) {
        failMissingValue(VALUE_ATTR);
    }

}

Task::ReportResult GTest_BioStruct3DNumberOfAtoms::report()
{

    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    BioStruct3DObject * biostructObj = qobject_cast<BioStruct3DObject*>(obj);
    if(biostructObj==NULL){
        stateInfo.setError(QString("can't cast to biostruct3d object from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    int tmpNumAtoms = biostructObj->getBioStruct3D().getNumberOfAtoms();

    if(tmpNumAtoms != numAtoms){
        stateInfo.setError(QString("number of atoms does not match: %1, expected %2 ").arg(tmpNumAtoms).arg(numAtoms));
    }

    return ReportResult_Finished;
}

///////////////////////////////////////////////////////////////////////////////////////////

void GTest_BioStruct3DNumberOfChains::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    QString v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }

    bool ok = false;
    numChains = v.toInt(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, int required").arg(VALUE_ATTR));
    }

}

Task::ReportResult GTest_BioStruct3DNumberOfChains::report()
{

    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    BioStruct3DObject * biostructObj = qobject_cast<BioStruct3DObject*>(obj);
    if(biostructObj==NULL){
        stateInfo.setError(QString("can't cast to biostruct3d object from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    int tmpNumChains = biostructObj->getBioStruct3D().moleculeMap.size();

    if(tmpNumChains != numChains) {
        stateInfo.setError(QString("number of polymer chains does not match: %1, expected %2 ").arg(tmpNumChains).arg(numChains));
    }

    return ReportResult_Finished;
}

///////////////////////////////////////////////////////////////////////////////////////////

void GTest_BioStruct3DAtomCoordinates::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    modelId = -1;

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    // atom id
    QString v = el.attribute(ATOM_ID_ATTR);
    if (v.isEmpty()) {
        failMissingValue(ATOM_ID_ATTR);
        return;
    }
    bool ok = false;
    atomId = v.toInt(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, int required").arg(ATOM_ID_ATTR));
    }

    // model id (if given)
    v = el.attribute(MODEL_ID_ATTR);
    if (!v.isEmpty()) {
        bool ok = false;
        modelId = v.toInt(&ok);
        if (!ok) {
            stateInfo.setError(QString("invalid value type %1, int required").arg(MODEL_ID_ATTR));
        }
    }

    // x coordinate
    v = el.attribute(X_COORD_ATTR);
    if (v.isEmpty()) {
        failMissingValue(X_COORD_ATTR);
        return;
    }
    ok = false;
    x = v.toDouble(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, double required").arg(X_COORD_ATTR));
    }

    // y coordinate
    v = el.attribute(Y_COORD_ATTR);
    if (v.isEmpty()) {
        failMissingValue(Y_COORD_ATTR);
        return;
    }
    ok = false;
    y = v.toDouble(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, double required").arg(Y_COORD_ATTR));
    }

    // z coordinate
    v = el.attribute(Z_COORD_ATTR);
    if (v.isEmpty()) {
        failMissingValue(Z_COORD_ATTR);
        return;
    }
    ok = false;
    z = v.toDouble(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, double required").arg(Z_COORD_ATTR));
    }

}

Task::ReportResult GTest_BioStruct3DAtomCoordinates::report()
{


    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    BioStruct3DObject * biostructObj = qobject_cast<BioStruct3DObject*>(obj);
    if(biostructObj==NULL){
        stateInfo.setError(QString("can't cast to biostruct3d object from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    if (modelId == -1) {
        modelId = biostructObj->getBioStruct3D().modelMap.keys().first();
    }


    const SharedAtom atom = biostructObj->getBioStruct3D().getAtomById(atomId, modelId);

    if (atom == NULL) {
        stateInfo.setError(QString("atom with index = %1 not found").arg(atomId));
        return ReportResult_Finished;
    }

    Vector3D coords(x,y,z);
    Vector3D tmpCoords = atom->coord3d;
    Vector3D diff = coords - tmpCoords;
    static const float EPSILON = 0.01f;
    if ((qAbs(diff.x) > EPSILON) || (qAbs(diff.y) > EPSILON) || (qAbs(diff.y) > EPSILON)) {
        stateInfo.setError(QString("atom coords not match: (%1,%2,%3)").arg(tmpCoords.x).arg(tmpCoords.y).arg(tmpCoords.z) +
        QString(", expected (%1,%2,%3) ").arg(x).arg(y).arg(z));
    }
    return ReportResult_Finished;

}
///////////////////////////////////////////////////////////////////////////////////////////

void GTest_BioStruct3DAtomChainIndex::init(XMLTestFormat *tf, const QDomElement& el)
{
    Q_UNUSED(tf);
    modelId = -1;

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    // atom id
    QString v = el.attribute(ATOM_ID_ATTR);
    if (v.isEmpty()) {
        failMissingValue(ATOM_ID_ATTR);
        return;
    }
    bool ok = false;
    atomId = v.toInt(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, int required").arg(ATOM_ID_ATTR));
    }

    // model id (if given)
    v = el.attribute(MODEL_ID_ATTR);
    if (!v.isEmpty()) {
        bool ok = false;
        modelId = v.toInt(&ok);
        if (!ok) {
            stateInfo.setError(QString("invalid value type %1, int required").arg(MODEL_ID_ATTR));
        }
    }


    //chain index
    v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    ok = false;
    chainId = v.toInt(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, int required").arg(VALUE_ATTR));
    }

}

Task::ReportResult GTest_BioStruct3DAtomChainIndex::report()
{
    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    BioStruct3DObject * biostructObj = qobject_cast<BioStruct3DObject*>(obj);
    if(biostructObj==NULL){
        stateInfo.setError(QString("can't cast to biostruct3d object from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    if (modelId == -1) {
        modelId = biostructObj->getBioStruct3D().modelMap.keys().first();
    }

    const SharedAtom atom = biostructObj->getBioStruct3D().getAtomById(atomId, modelId);
    if (atom == NULL) {
        stateInfo.setError(QString("atom with index = %1 not found").arg(atomId));
        return ReportResult_Finished;
    }

    int tmpId = atom->chainIndex;

    if (chainId != tmpId) {
        stateInfo.setError(QString("atom with id=%1 chainId does not match: %2, expected %3").arg(atomId).arg(tmpId).arg(chainId));
    }

    return ReportResult_Finished;


}
///////////////////////////////////////////////////////////////////////////////////////////

void GTest_BioStruct3DAtomResidueName::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    // default model id
    modelId = -1;

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    // atom id
    QString v = el.attribute(ATOM_ID_ATTR);
    if (v.isEmpty()) {
        failMissingValue(ATOM_ID_ATTR);
        return;
    }
    bool ok = false;
    atomId = v.toInt(&ok);
    if (!ok) {
        stateInfo.setError(QString("invalid value type %1, int required").arg(ATOM_ID_ATTR));
    }

    // model id (if given)
    v = el.attribute(MODEL_ID_ATTR);
    if (!v.isEmpty()) {
        bool ok = false;
        modelId = v.toInt(&ok);
        if (!ok) {
            stateInfo.setError(QString("invalid value type %1, int required").arg(MODEL_ID_ATTR));
        }
    }

    //residue name
    v = el.attribute(VALUE_ATTR);
    if (v.isEmpty()) {
        failMissingValue(VALUE_ATTR);
        return;
    }
    residueName = v;

}


Task::ReportResult GTest_BioStruct3DAtomResidueName::report()
{

    GObject *obj = getContext<GObject>(this, objContextName);
    if(obj==NULL){
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_ATTR));
        return ReportResult_Finished;
    }

    BioStruct3DObject * biostructObj = qobject_cast<BioStruct3DObject*>(obj);
    if(biostructObj==NULL){
        stateInfo.setError(QString("can't cast to biostruct3d object from: %1").arg(obj->getGObjectName()));
        return ReportResult_Finished;
    }

    const BioStruct3D& bioStruct = biostructObj->getBioStruct3D();

    if (modelId == -1) {
        modelId = bioStruct.modelMap.keys().first();
    }

    const SharedAtom atom = bioStruct.getAtomById(atomId, modelId);
    if (atom == NULL) {
        stateInfo.setError(QString("atom with index = %1 not found").arg(atomId));
        return ReportResult_Finished;
    }

    int chainId = atom->chainIndex;
    int residueId = atom->residueIndex.toInt();
    QString tmpName =  bioStruct.getResidueById(chainId, ResidueIndex(residueId,' '))->name;

    if (residueName != tmpName) {
        stateInfo.setError(QString("atom with id=%1 sequenceId does not match: %2, expected %3").arg(atomId).arg(tmpName).arg(residueName));
    }

    return ReportResult_Finished;


}

///////////////////////////////////////////////////////////////////////////////////////////

#define PDB_DIR_NAME_ENV "DIR_WITH_PDB_FILES"

void GTest_PDBFormatStressTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    Q_UNUSED(el);

    QString dirName = getEnv()->getVar(PDB_DIR_NAME_ENV);

    QDir dir(dirName);
    if (!dir.exists()) {
        stateInfo.setError(QString("Cannot_find_the_directory %1").arg(dirName));
        return;
    }

    dir.setFilter(QDir::Files);
    QFileInfoList fileList  = dir.entryInfoList();

    if (fileList.empty()) {
        stateInfo.setError(QString("Folder %1 is_empty").arg(dirName));
        return;
    }

    IOAdapterId         ioId(BaseIOAdapters::LOCAL_FILE);
    IOAdapterFactory*   iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    DocumentFormatId    format = BaseDocumentFormats::PLAIN_PDB;

    for (int i = 0; i < fileList.size(); ++i) {
        QFileInfo fileInfo = fileList.at(i);
        LoadDocumentTask* task = new LoadDocumentTask(format, fileInfo.absoluteFilePath(), iof);
        addSubTask(task);
        fileNames.insert(task, fileInfo.fileName());
    }

}


Task::ReportResult GTest_PDBFormatStressTest::report()
{

    foreach (const QPointer<Task> &task, getSubtasks()) {
        if (task->hasError()) {
            stateInfo.setError(stateInfo.getError()+fileNames.value(task) + "(" + task->getError() + ");   ");
        }
    }

    return ReportResult_Finished;

}

QList<Task*> GTest_PDBFormatStressTest::onSubTaskFinished( Task* subTask )
{
    subTask->cleanup();
    QList<Task*> lst;
    return lst;
}

///////////////////////////////////////////////////////////////////////////////////////////

#define ASN_DIR_NAME_ENV "DIR_WITH_ASN_FILES"

void GTest_ASNFormatStressTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    Q_UNUSED(el);

    QString dirName = getEnv()->getVar(ASN_DIR_NAME_ENV);

    if (dirName.isEmpty()) {
        failMissingValue(ASN_DIR_NAME_ENV);
        return;
    }


    QDir dir(dirName);
    if (!dir.exists()) {
        stateInfo.setError(QString("Cannot_find_the_directory %1").arg(dirName));
        return;
    }

    dir.setFilter(QDir::Files);
    QFileInfoList fileList  = dir.entryInfoList();

    if (fileList.empty()) {
        stateInfo.setError(QString("Folder %1 is_empty").arg(dirName));
        return;
    }

    IOAdapterId         ioId(BaseIOAdapters::LOCAL_FILE);
    IOAdapterFactory*   iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    DocumentFormatId    format = BaseDocumentFormats::PLAIN_ASN;

    for (int i = 0; i < fileList.size(); ++i) {
        QFileInfo fileInfo = fileList.at(i);
        LoadDocumentTask* task = new LoadDocumentTask(format, fileInfo.absoluteFilePath(), iof);
        addSubTask(task);
        fileNames.insert(task, fileInfo.fileName());
    }

}


Task::ReportResult GTest_ASNFormatStressTest::report()
{

    foreach (const QPointer<Task> &task, getSubtasks()) {
        if (task->hasError()) {
            stateInfo.setError(stateInfo.getError()+fileNames.value(task) + "(" + task->getError() + ");   ");
        }
    }

    return ReportResult_Finished;

}

QList<Task*> GTest_ASNFormatStressTest::onSubTaskFinished( Task* subTask )
{
    subTask->cleanup();
    QList<Task*> lst;
    return lst;
}
///////////////////////////////////////////////////////////////////////////////////////////

QList<XMLTestFactory*> BioStruct3DObjectTests::createTestFactories()
{
    QList<XMLTestFactory*> res;
    res.append(GTest_BioStruct3DNumberOfAtoms::createFactory());
    res.append(GTest_BioStruct3DNumberOfChains::createFactory());
    res.append(GTest_BioStruct3DAtomCoordinates::createFactory());
    res.append(GTest_BioStruct3DAtomResidueName::createFactory());
    res.append(GTest_BioStruct3DAtomChainIndex::createFactory());
    res.append(GTest_PDBFormatStressTest::createFactory());
    res.append(GTest_ASNFormatStressTest::createFactory());

    return res;

}


} //namespace
