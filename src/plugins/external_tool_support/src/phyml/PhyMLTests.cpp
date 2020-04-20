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

#include "PhyMLTests.h"
#include "PhyMLSupport.h"
#include <U2Core/SaveDocumentTask.h>

#include <U2Core/DocumentModel.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>

#include <U2Core/GObjectTypes.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/PhyTreeObject.h>

#include <QDir>
#include <U2Core/AppContext.h>
#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
namespace U2 {
QList<XMLTestFactory*> PhyMLToolTests::createTestFactories(){
    QList<XMLTestFactory* > res;
    res.append(GTest_PhyML::createFactory());
    return res;
}

void GTest_PhyML::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    treeObjFromDoc = NULL;
    task = NULL;
    input = NULL;
    maDoc = NULL;
    treeDoc = NULL;

    inputDocCtxName = el.attribute("in");
    if (inputDocCtxName.isEmpty()) {
        failMissingValue("in");
        return;
    }
    resultCtxName = el.attribute("sample");
    negative = el.attribute("negative");

    QString dataType = el.attribute("datatype");
    if(!dataType.isEmpty()) {
        settings.extToolArguments << "-d";
        settings.extToolArguments << dataType;
    }

    QString bootstrapString = el.attribute("bootstrap");
    if(!bootstrapString.isEmpty()){
        settings.extToolArguments << "-b";
        settings.extToolArguments << bootstrapString;
    }

    QString subtitutionalModel = el.attribute("model");
    if(!subtitutionalModel.isEmpty()) {
        settings.extToolArguments << "-m";
        settings.extToolArguments << subtitutionalModel;
    }

    QString ttRatioString = el.attribute("tt_ratio");
    if(!ttRatioString.isEmpty()) {
        settings.extToolArguments << "-t";
        settings.extToolArguments << ttRatioString;
    }

    QString subRatesString = el.attribute("substitution_rates");
    if(!subRatesString.isEmpty()) {
        settings.extToolArguments << "-t";
        settings.extToolArguments << subRatesString;
    }

    QString invSitesString = el.attribute("inv_sites");
    if(!invSitesString.isEmpty()) {
        settings.extToolArguments << "-v";
        settings.extToolArguments << invSitesString;
    }

    QString gammaFactorString = el.attribute("gamma");
    if(!gammaFactorString.isEmpty()) {
        settings.extToolArguments << "-a";
        settings.extToolArguments << gammaFactorString;
    }
}

void GTest_PhyML::prepare() {
    maDoc = getContext<Document>(this, inputDocCtxName);
    if (maDoc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(inputDocCtxName) );
        return;
    }

    QList<GObject*> list = maDoc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }

    GObject *obj = list.first();
    if(obj == NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    assert(obj != NULL);
    MultipleSequenceAlignmentObject* ma = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(ma == NULL){
        stateInfo.setError(  QString("error can't cast to multiple alignment from GObject") );
        return;
    }

    input = ma;

    treeDoc = getContext<Document>(this, resultCtxName);
    if (treeDoc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(resultCtxName) );
        return;
    }

    QList<GObject*> list2 = treeDoc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE);
    if (list2.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }

    GObject *obj2 = list2.first();
    if(obj2 == NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::PHYLOGENETIC_TREE) );
        return;
    }

    treeObjFromDoc = qobject_cast<PhyTreeObject*>(obj2);

    if(treeObjFromDoc == NULL){
        stateInfo.setError(  QString("error can't cast to phylogenetic tree from GObject") );
        return;
    }
    assert( obj != NULL);


    settings.algorithm = PhyMLSupport::PHYML;

    task = new PhyTreeGeneratorLauncherTask(input->getMultipleAlignment(), settings);
    addSubTask(task);
}

Task::ReportResult GTest_PhyML::report() {
    if(NULL == task) {
        if(!stateInfo.hasError()) {
            stateInfo.setError("PhyTreeGeneratorLauncherTask is not created");
        }
        return ReportResult_Finished;
    }
    if (!task->hasError()) {
        const PhyTree computedTree = task->getResult();
        const PhyTree& treeFromDoc = treeObjFromDoc->getTree();
        bool same = PhyTreeObject::treesAreAlike(computedTree, treeFromDoc);
        if(!same){
            if(negative.isEmpty()) {
                stateInfo.setError("Trees are not equal");
            }
            else {
                if(negative != "Trees are not equal") {
                    stateInfo.setError(QString("Negative test failed: error string is empty, expected error \"%1\", but current error is \"Trees are not equal\"").arg(negative));
                }
            }
        }
    }
    else if(!negative.isEmpty()) {
        if(negative != task->getError()) {
            stateInfo.setError(QString("Negative test failed: error string is empty, expected error \"%1\", but current error is \"%2\"").arg(negative).arg(task->getError()));
        }
        else {
            stateInfo.setError("");
        }
    }

    return ReportResult_Finished;
}


}//namespace
