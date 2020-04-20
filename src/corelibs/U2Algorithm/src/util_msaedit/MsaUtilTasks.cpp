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

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GHints.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "MsaUtilTasks.h"


namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// TranslateMsa2AminoTask

TranslateMsa2AminoTask::TranslateMsa2AminoTask( MultipleSequenceAlignmentObject* obj )
: Task(tr("Translate nucleic alignment to amino"), TaskFlags_FOSE_COSC), maObj(obj)
{
    SAFE_POINT_EXT(NULL != maObj, setError(tr("Invalid MSA object detected")),);
    SAFE_POINT_EXT(maObj->getAlphabet()->isNucleic(), setError(tr("Multiple alignment already has amino-acid alphabet")),);

    QList<DNATranslation*> translations =
        AppContext::getDNATranslationRegistry()->lookupTranslation(maObj->getAlphabet(), DNATranslationType_NUCL_2_AMINO);
    CHECK_EXT(translations.isEmpty(), setError(tr("Unable to find suitable translation for %1").arg(maObj->getGObjectName())), );

    translation = AppContext::getDNATranslationRegistry()->getStandardGeneticCodeTranslation(maObj->getAlphabet());
}

TranslateMsa2AminoTask::TranslateMsa2AminoTask( MultipleSequenceAlignmentObject* obj, const QString& translationId )
: Task(tr("Translate nucleic alignment to amino"), TaskFlags_FOSE_COSC), maObj(obj)
{
    SAFE_POINT_EXT(NULL != maObj, setError(tr("Invalid MSA object detected")),);
    SAFE_POINT_EXT(maObj->getAlphabet()->isNucleic(), setError(tr("Multiple alignment already has amino-acid alphabet")),);

    translation = AppContext::getDNATranslationRegistry()->lookupTranslation(translationId);
}


void TranslateMsa2AminoTask::run() {
    SAFE_POINT_EXT(NULL != translation, setError(tr("Invalid translation object")),);

    QList<DNASequence> lst = MSAUtils::ma2seq(maObj->getMultipleAlignment(), true);
    resultMA = MultipleSequenceAlignment(maObj->getMultipleAlignment()->getName(), translation->getDstAlphabet());

    foreach (const DNASequence& dna, lst) {
        int buflen = dna.length() / 3;
        QByteArray buf(buflen,'\0');
        translation->translate(dna.seq.constData(), dna.length(), buf.data(), buflen);
        buf.replace("*","X");
        resultMA->addRow(dna.getName(), buf);
    }
}


Task::ReportResult TranslateMsa2AminoTask::report() {
    if (!resultMA->isEmpty()) {
        maObj->setMultipleAlignment(resultMA);
    }

    return ReportResult_Finished;
}


//////////////////////////////////////////////////////////////////////////
/// AlignInAminoFormTask

AlignInAminoFormTask::AlignInAminoFormTask( MultipleSequenceAlignmentObject* obj, AlignGObjectTask* t, const QString& trId )
: Task (tr("Align in amino form"), TaskFlags_FOSE_COSC), alignTask(t), maObj(obj), clonedObj(NULL), traslId(trId), tmpDoc(NULL)
{
    setMaxParallelSubtasks(1);
}

AlignInAminoFormTask::~AlignInAminoFormTask() {
    delete tmpDoc;
}

void AlignInAminoFormTask::prepare() {
    SAFE_POINT_EXT(NULL != maObj, setError(tr("Invalid MSA object detected")),);
    CHECK_EXT(maObj->getAlphabet()->isNucleic(), setError(tr("AlignInAminoFormTask: Input alphabet is not nucleic!")), );
    CHECK_EXT(!maObj->getMultipleAlignment()->isEmpty(), setError(tr("AlignInAminoFormTask: Input alignment is empty!")), );

    MultipleSequenceAlignment msa = maObj->getMsaCopy();
    const U2DbiRef& dbiRef = maObj->getEntityRef().dbiRef;

    //Create temporal document for the workflow run task
    const AppSettings* appSettings = AppContext::getAppSettings();
    SAFE_POINT_EXT(NULL != appSettings, setError(tr("Invalid applications settings detected")), );

    UserAppsSettings* usersSettings = appSettings->getUserAppsSettings();
    SAFE_POINT_EXT(NULL != usersSettings, setError(tr("Invalid users applications settings detected")), );
    const QString tmpDirPath = usersSettings->getCurrentProcessTemporaryDirPath();
    U2OpStatus2Log os;
    const QString fileName = GUrlUtils::prepareTmpFileLocation(tmpDirPath, "tmpAlignment", "fasta", os);

    IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(fileName));
    const Document* maDocument = maObj->getDocument();
    SAFE_POINT_EXT(NULL != maDocument, setError(tr("Invalid MSA document detected")), );
    DocumentFormat* docFormat = maDocument->getDocumentFormat();
    tmpDoc = docFormat->createNewLoadedDocument(iof, fileName, os);
    CHECK_OP(os, );

    //Create copy of multiple alignment object
    clonedObj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, msa, stateInfo);
    CHECK_OP(stateInfo, );
    clonedObj->setGHints(new GHintsDefaultImpl(maObj->getGHintsMap()));

    tmpDoc->addObject(clonedObj);

    alignTask->setMAObject(clonedObj);
    addSubTask(new TranslateMsa2AminoTask(clonedObj,traslId));
    addSubTask(alignTask);
}

void AlignInAminoFormTask::run() {
    CHECK_OP(stateInfo, );

    SAFE_POINT_EXT(NULL != clonedObj, setError(tr("NULL clonedObj in AlignInAminoFormTask::prepare!")),);

    const MultipleSequenceAlignment newMsa = clonedObj->getMsa();
    const QList<MultipleSequenceAlignmentRow> rows = newMsa->getMsaRows();

    //Create gap map from amino-acid alignment
    foreach (const MultipleSequenceAlignmentRow &row, rows) {
        const int rowIdx = MSAUtils::getRowIndexByName(maObj->getMsa(), row->getName());
        const MultipleSequenceAlignmentRow curRow = maObj->getMsa()->getMsaRow(row->getName());
        SAFE_POINT_EXT(rowIdx >= 0, setError(tr("Can not find row %1 in original alignment.").arg(row->getName())),);

        QList<U2MsaGap> gapsList;
        foreach(const U2MsaGap& gap, row->getGapModel()) {
            gapsList << U2MsaGap(gap.offset * 3, gap.gap * 3);
        }
        rowsGapModel[curRow->getRowId()] = gapsList;
    }
}

Task::ReportResult AlignInAminoFormTask::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), stateInfo);
    CHECK_OP(stateInfo, Task::ReportResult_Finished);
    maObj->updateGapModel(stateInfo, emptyGapModel);
    CHECK_OP(stateInfo, Task::ReportResult_Finished);
    maObj->updateGapModel(stateInfo, rowsGapModel);
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

    return ReportResult_Finished;
}


} // U2

