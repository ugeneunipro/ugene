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

#include <QFileInfo>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>
#include <U2Lang/WorkflowScriptEngine.h>

#include "ScriptEngineUtils.h"
#include "ScriptLibrary.h"
#include "SequencePrototype.h"

namespace U2 {

WorkflowScriptRegistry::~WorkflowScriptRegistry() {
    qDeleteAll(list);
}

void WorkflowScriptRegistry::registerScriptFactory(WorkflowScriptFactory* f) {
    assert(!list.contains(f));
    list.append(f);
}

void WorkflowScriptRegistry::unregisterScriptFactory(WorkflowScriptFactory* f) {
    list.removeAll(f);
}

const QList<WorkflowScriptFactory*> &WorkflowScriptRegistry::getFactories() const {
    return list;
}

void WorkflowScriptLibrary::initEngine(WorkflowScriptEngine *engine) {
    DbiClassPrototype::registerScriptClass<SequenceScriptClass>(engine);
    QScriptValue foo = engine->globalObject();

    foo.setProperty("print", engine->newFunction(print));

    // unrefactored obsolete deprecated functions
    foo.setProperty("subsequence",engine->newFunction(getSubsequence));
    foo.setProperty("complement", engine->newFunction(complement));
    foo.setProperty("size", engine->newFunction(sequenceSize));
    foo.setProperty("translate", engine->newFunction(translate));
    foo.setProperty("charAt", engine->newFunction(charAt));
    foo.setProperty("alphabetType", engine->newFunction(alphabetType));
    foo.setProperty("getName", engine->newFunction(sequenceName));
    foo.setProperty("printToLog",engine->newFunction(debugOut));
    foo.setProperty("isAmino", engine->newFunction(isAmino));
    foo.setProperty("getMinimumQuality", engine->newFunction(getMinimumQuality));
    foo.setProperty("getTrimmedByQuality", engine->newFunction(getTrimmedByQuality));
    foo.setProperty("hasQuality",engine->newFunction(hasQuality));
    foo.setProperty("sequenceFromText", engine->newFunction(sequenceFromText));

    foo.setProperty("createAlignment", engine->newFunction(createAlignment));
    foo.setProperty("sequenceFromAlignment",engine->newFunction(getSequenceFromAlignment));
    foo.setProperty("addToAlignment",engine->newFunction(addToAlignment));
    foo.setProperty("findInAlignment",engine->newFunction(findInAlignment));
    foo.setProperty("removeFromAlignment",engine->newFunction(removeFromAlignment));
    foo.setProperty("rowNum",engine->newFunction(rowNum));
    foo.setProperty("columnNum",engine->newFunction(columnNum));
    foo.setProperty("alignmentAlphabetType",engine->newFunction(alignmentAlphabetType));

    foo.setProperty("annotatedRegions", engine->newFunction(getAnnotationRegion));
    foo.setProperty("addQualifier", engine->newFunction(addQualifier));
    foo.setProperty("getLocation", engine->newFunction(getLocation));
    foo.setProperty("filterByQualifier", engine->newFunction(filterByQualifier));
    foo.setProperty("hasAnnotationName", engine->newFunction(hasAnnotationName));

    foo.setProperty("writeFile", engine->newFunction(writeFile));
    foo.setProperty("appendFile", engine->newFunction(appendFile));
    foo.setProperty("readFile", engine->newFunction(readFile));
    foo.setProperty("readSequences", engine->newFunction(readSequences));

    if(AppContext::getWorkflowScriptRegistry() != NULL) {
        foreach(WorkflowScriptFactory* f, AppContext::getWorkflowScriptRegistry()->getFactories()) {
            f->createScript(engine);
        }
    }
}

QScriptValue WorkflowScriptLibrary::print(QScriptContext *ctx, QScriptEngine *) {
    scriptLog.info(ctx->argument(0).toString());
    return 0;
}

static DNASequence getSequence(QScriptContext *ctx, QScriptEngine *engine, int argNum) {
    WorkflowScriptEngine *wse = ScriptEngineUtils::workflowEngine(engine);
    CHECK(NULL != wse, DNASequence());

    SharedDbiDataHandler seqId = ScriptEngineUtils::getDbiId(engine, ctx->argument(argNum),
        SequenceScriptClass::CLASS_NAME);
    QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(wse->getWorkflowContext()->getDataStorage(), seqId));
    CHECK(!seqObj.isNull(), DNASequence());
    U2OpStatusImpl os;
    DNASequence seq = seqObj->getWholeSequence(os);
    CHECK_OP(os, DNASequence());
    return seq;
}

static QList<SharedAnnotationData> getAnnotationTable(QScriptContext *ctx, QScriptEngine *engine, int argNum) {
    WorkflowScriptEngine *wse = ScriptEngineUtils::workflowEngine(engine);
    QList<SharedAnnotationData> result = StorageUtils::getAnnotationTable(wse->getWorkflowContext()->getDataStorage(), ctx->argument(argNum).toVariant());
    return result;
}

static MultipleSequenceAlignment getAlignment(QScriptContext *ctx, QScriptEngine *engine, int argNum) {
    WorkflowScriptEngine *wse = ScriptEngineUtils::workflowEngine(engine);
    CHECK(NULL != wse, MultipleSequenceAlignment());

    SharedDbiDataHandler msaId = ScriptEngineUtils::getDbiId(engine, ctx->argument(argNum));
    QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(wse->getWorkflowContext()->getDataStorage(), msaId));
    CHECK(!msaObj.isNull(), MultipleSequenceAlignment());
    return msaObj->getMsaCopy();
}

static QScriptValue putSequence(QScriptEngine *engine, const DNASequence &seq) {
    WorkflowScriptEngine *wse = ScriptEngineUtils::workflowEngine(engine);
    CHECK(NULL != wse, QScriptValue::NullValue);
    WorkflowContext *ctx = wse->getWorkflowContext();
    SharedDbiDataHandler id = ctx->getDataStorage()->putSequence(seq);

    CHECK(NULL != ScriptEngineUtils::getSequenceClass(engine), QScriptValue());
    return ScriptEngineUtils::getSequenceClass(engine)->newInstance(id);
}

static QScriptValue putAnnotationTable(QScriptEngine *engine, const QList<SharedAnnotationData> &anns) {
    WorkflowScriptEngine *wse = ScriptEngineUtils::workflowEngine(engine);
    CHECK(NULL != wse, QScriptValue::NullValue);
    WorkflowContext *ctx = wse->getWorkflowContext();
    SharedDbiDataHandler id = ctx->getDataStorage()->putAnnotationTable(anns);
    return engine->newVariant(qVariantFromValue(id));
}

static QScriptValue putAlignment(QScriptEngine *engine, const MultipleSequenceAlignment &msa) {
    WorkflowScriptEngine *wse = ScriptEngineUtils::workflowEngine(engine);
    CHECK(NULL != wse, QScriptValue::NullValue);
    WorkflowContext *ctx = wse->getWorkflowContext();
    SharedDbiDataHandler id = ctx->getDataStorage()->putAlignment(msa);
    return engine->newVariant(qVariantFromValue(id));
}

// unrefactored obsolete deprecated functions
QScriptValue WorkflowScriptLibrary::getSubsequence(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }else {
        int beg, end;
        QScriptValue calee = ctx->callee();
        DNASequence dna = getSequence(ctx, engine, 0);

        QVariant var = ctx->argument(1).toVariant();
        bool ok;
        beg = var.toInt(&ok);
        if(!ok) {
            return ctx->throwError(QObject::tr("Second argument must be a number"));
        }

        var = ctx->argument(2).toVariant();
        end = var.toInt(&ok);
        if(!ok) {
            return ctx->throwError(QObject::tr("Third argument must be a number"));
        }

        if(dna.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        if(beg < 0 || beg >= dna.seq.length()){
            return ctx->throwError(QObject::tr("Subsequence's start offset is out of range"));
        }
        if(end < 1 || end > dna.seq.length()){
            return ctx->throwError(QObject::tr("Subsequence's end offset is out of range"));
        }
        if(end-beg < 1){
            return ctx->throwError(QObject::tr("Invalid subsequence region"));
        }
        QString newName(dna.getName() + "_" + QByteArray::number(beg) + "_" + QByteArray::number(end));
        DNASequence subsequence(newName, dna.seq.mid(beg, end - beg + 1),dna.alphabet);
        calee.setProperty("res", putSequence(engine, subsequence));
        return calee.property("res");
    }
}

QScriptValue WorkflowScriptLibrary::concatSequence(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }
    QByteArray result;
    const DNAAlphabet *alph = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    for(int i = 0; i < ctx->argumentCount();i++) {
        DNASequence dna = getSequence(ctx, engine, i);
        if(dna.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        if(dna.alphabet->isAmino()) {
            alph = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::AMINO_DEFAULT());
        }
        result.append(dna.seq);
    }
    DNASequence concatenation("joined sequence", result, alph);
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putSequence(engine, concatenation));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::complement(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence dna = getSequence(ctx, engine, 0);
    if(dna.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    if(!dna.alphabet->isNucleic()) {
        return ctx->throwError(QObject::tr("Alphabet must be nucleotide"));
    }

    DNATranslation *complTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dna.alphabet);
    complTT->translate(dna.seq.data(),dna.seq.size(), dna.seq.data(), dna.seq.size());

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putSequence(engine, dna));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::sequenceSize(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    int size = seq.length();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(size));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::translate(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 1 || ctx->argumentCount() > 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    bool aminoSeq = seq.alphabet->isAmino();
    if(aminoSeq) {
        return ctx->throwError(QObject::tr("Alphabet must be nucleotide"));
    }
    int offset = 0;
    if(ctx->argumentCount() == 2) {
        bool ok;
        QVariant var = ctx->argument(1).toInt32();
        offset = var.toInt(&ok); //no need to check OK because, if var not integer its changed to default value
        if(offset < 0 || offset > 2) {
            return ctx->throwError(QObject::tr("Offset must be from interval [0,2]"));
        }
    }

    DNATranslationType dnaTranslType = (seq.alphabet->getType() == DNAAlphabet_NUCL) ? DNATranslationType_NUCL_2_AMINO : DNATranslationType_RAW_2_AMINO;
    QList<DNATranslation*> aminoTTs = AppContext::getDNATranslationRegistry()->lookupTranslation(seq.alphabet, dnaTranslType);
    if(aminoTTs.isEmpty()) {
        return ctx->throwError(QObject::tr("Translation table is empty"));
    }
    DNATranslation *aminoT;
    aminoT = AppContext::getDNATranslationRegistry()->getStandardGeneticCodeTranslation(seq.alphabet);

    aminoT->translate(seq.seq.data() + offset, seq.length() - offset, seq.seq.data(), seq.length());
    seq.seq.resize(seq.length()/3);

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putSequence(engine, seq));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::charAt(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    QVariant var = ctx->argument(1).toVariant();
    bool ok;
    int position = var.toInt(&ok);
    if(!ok) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }

    if(position >= seq.length() || position < 0) {
        return ctx->throwError(QObject::tr("Position is out of range"));
    }

    QString c(seq.seq.at(position));
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(c));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::sequenceName(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    QString name = seq.getName();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", name);
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::alphabetType(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    QString alph = seq.alphabet->getName();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(alph));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::sequenceFromText(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString text = ctx->argument(0).toString();
    //QString name = ctx->argument(1).toString();
    DNASequence seq("sequence", text.toLatin1());
    seq.alphabet = U2AlphabetUtils::findBestAlphabet(seq.seq);
    if(seq.alphabet->getId() == BaseDNAAlphabetIds::RAW()) {
        seq.seq = QByteArray();
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putSequence(engine, seq));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::isAmino(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    bool isAmino = seq.alphabet->isAmino();
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", isAmino);
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::getMinimumQuality(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    QByteArray codes = seq.quality.qualCodes;

    int minQual = 100;
    for(int i = 0; i < codes.size();i++) {
        if(minQual > codes[i]) {
            minQual = codes[i];
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", minQual);
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::getTrimmedByQuality(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }else {
        int minQuality = 100;
        int minLength = 0;
        QScriptValue calee = ctx->callee();

        DNASequence dna = getSequence(ctx, engine, 0);

        QVariant var = ctx->argument(1).toVariant();
        bool ok;
        minQuality = var.toInt(&ok);
        if(!ok) {
            return ctx->throwError(QObject::tr("Second argument must be a number"));
        }

        var = ctx->argument(2).toVariant();
        minLength = var.toInt(&ok);
        if(!ok) {
            return ctx->throwError(QObject::tr("Third argument must be a number"));
        }

        if(dna.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }

        int seqLen = dna.length();
        if(seqLen > dna.quality.qualCodes.length()){
            //quality length is shorter than the length of the sequence. return the an empty sequence
            DNASequence emptySequence ("empty", QByteArray(), NULL);
            calee.setProperty("res", putSequence(engine, emptySequence));
            return calee.property("res");
        }else{
            int endPosition = seqLen-1;
            for (; endPosition>=0; endPosition--){
                if(dna.quality.getValue(endPosition) >= minQuality){
                    break;
                }
            }
            if(endPosition>=0 && endPosition+1 >= minLength){
                DNASequence trimmed(dna.getName(), dna.seq.left(endPosition+1), dna.alphabet);
                trimmed.quality = dna.quality;
                trimmed.quality.qualCodes = trimmed.quality.qualCodes.left(endPosition+1);
                calee.setProperty("res", putSequence(engine, trimmed));
                return calee.property("res");
            }else{
                DNASequence emptySequence("empty", QByteArray(), NULL);
                calee.setProperty("res", putSequence(engine, emptySequence));
                return calee.property("res");
            }
        }
    }
}

QScriptValue WorkflowScriptLibrary::hasQuality(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    QByteArray codes = seq.quality.qualCodes;
    bool isHasQuality = !codes.isEmpty();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", isHasQuality);
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::getSequenceFromAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2 && ctx->argumentCount() != 4) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment align = getAlignment(ctx, engine, 0);
    if(align->isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }

    bool ok;
    QVariant var = ctx->argument(1).toVariant();
    int row = var.toInt(&ok);
    if(!ok) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }
    if(row < 0 || row >= align->getNumRows()) {
        return ctx->throwError(QObject::tr("Row is out of range"));
    }

    MultipleSequenceAlignmentRow aRow = align->getMsaRow(row)->getExplicitCopy();
    aRow->simplify();
    U2OpStatus2Log os;
    QByteArray arr = aRow->toByteArray(os, aRow->getCoreLength());
    if(ctx->argumentCount() == 4) {
        var = ctx->argument(2).toVariant();
        int beg = var.toInt(&ok);
        if(!ok) {
            return ctx->throwError(QObject::tr("Third argument must be a number"));
        }

        var = ctx->argument(3).toVariant();
        int len = var.toInt(&ok);
        if(!ok) {
            return ctx->throwError(QObject::tr("Fourth argument must be a number"));
        }

        if(beg <= 0 || beg > arr.length()) {
            return ctx->throwError(QObject::tr("Offset is out of range"));
        }
        if(len <= 0 || (beg + len) > arr.length()) {
            return ctx->throwError(QObject::tr("Length is out of range"));
        }
        arr = arr.mid(beg,len);
    }
    DNASequence seq(aRow->getName(),arr,align->getAlphabet());

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putSequence(engine, seq));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::findInAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString name;
    DNASequence seq;
    MultipleSequenceAlignment aln = getAlignment(ctx, engine, 0);
    if(aln->isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    int row = 0;
    QScriptValue val = ctx->argument(1);
    name = val.toString();
    if(!name.isEmpty()) {
        row = aln->getRowNames().indexOf(name);
    }else{
        seq = val.toVariant().value<DNASequence>();
        if(seq.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        foreach(const DNASequence& alnSeq, MSAUtils::ma2seq(aln, true)) {
            if(alnSeq.seq == seq.seq) {
                break;
            }
            row++;
        }
        if(row == aln->getNumRows()) {
            row = -1;
        }
    }
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(row));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::createAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment align;
    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }
    align->setAlphabet(seq.alphabet);
    align->addRow(seq.getName(), seq.seq);

    for(int i = 1; i < ctx->argumentCount(); i++) {
        DNASequence seq = getSequence(ctx, engine, i);
        if(seq.seq.isEmpty()) {
            return ctx->throwError(QObject::tr("Empty or invalid sequence"));
        }
        if(seq.alphabet != align->getAlphabet()) {
            return ctx->throwError(QObject::tr("Alphabets of each sequence must be the same"));
        }
        align->addRow(seq.getName(), seq.seq);
    }

    return putAlignment(engine, align);
}

QScriptValue WorkflowScriptLibrary::addToAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 2 && ctx->argumentCount() >3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment align = getAlignment(ctx, engine, 0);
    DNASequence seq = getSequence(ctx, engine, 1);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    if(align->isEmpty()) {
        align->setAlphabet(seq.alphabet);
    }

    if(seq.alphabet != align->getAlphabet()) {
        return ctx->throwError(QObject::tr("Alphabets don't match"));
    }

    int row = -1;
    if(ctx->argumentCount() == 3) {
        if(!ctx->argument(2).isNumber()) {
            return ctx->throwError(QObject::tr("Third argument must be a number"));
        }
        row = ctx->argument(2).toInt32();
        if(row > align->getLength()) {
            row = -1;
        }
    }
    align->addRow(seq.getName(), seq.seq, row);

    return putAlignment(engine, align);
}

QScriptValue WorkflowScriptLibrary::removeFromAlignment(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment aln = getAlignment(ctx, engine, 0);
    if(aln->isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }

    QVariant var = ctx->argument(1).toVariant();
    bool ok;
    int row = var.toInt(&ok);
    if(!ok) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }

    if(row < 0 || row >= aln->getLength()) {
        return ctx->throwError(QObject::tr("Row is out of range"));
    }

    U2OpStatus2Log os;
    aln->removeRow(row, os);
    return putAlignment(engine, aln);
}

QScriptValue WorkflowScriptLibrary::rowNum(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment aln = getAlignment(ctx, engine, 0);
    if(aln->isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    int num = aln->getNumRows();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(num));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::columnNum(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment aln = getAlignment(ctx, engine, 0);
    if(aln->isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    int num = aln->getLength();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(num));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::alignmentAlphabetType(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    MultipleSequenceAlignment aln = getAlignment(ctx, engine, 0);
    if(aln->isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid alignment"));
    }
    QString alph = aln->getAlphabet()->getName();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(alph));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::getAnnotationRegion(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    DNASequence seq = getSequence(ctx, engine, 0);
    if(seq.seq.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty or invalid sequence"));
    }

    const QList<SharedAnnotationData> anns = getAnnotationTable(ctx, engine, 1);
    if (anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString name = ctx->argument(2).toString();
    if (name.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty name"));
    }
    QList<QScriptValue> result;

    foreach (const SharedAnnotationData &ann, anns) {
        if (ann->name == name) {
            DNASequence resultedSeq;
            const QByteArray & sequence = seq.seq;
            QVector<U2Region> location = ann->getRegions();
            QByteArray & res = resultedSeq.seq;
            QVector<U2Region> extendedRegions;

            //extend regions
            U2Region sequenceRange(0, sequence.size());
            foreach(const U2Region& reg, location) {
                U2Region ir = reg.intersect(sequenceRange);
                extendedRegions << ir;
            }

            for(int i = 0, end = extendedRegions.size(); i < end; ++i) {
                U2Region reg = extendedRegions.at(i);
                QByteArray partSeq(sequence.constData() + reg.startPos, reg.length);
                res.append(partSeq);
            }
            resultedSeq.alphabet = seq.alphabet;
            resultedSeq.setName(seq.getName() + "_" + name);

            result << putSequence(engine, resultedSeq);
        }
    }

    QScriptValue array = engine->newArray(result.size());
    for (int i=0; i<result.size(); i++) {
        array.setProperty(i, result.at(i));
    }
    QScriptValue calee = ctx->callee();
    calee.setProperty("res", array);
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::filterByQualifier(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount()!= 3) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    const QList<SharedAnnotationData> anns = getAnnotationTable(ctx, engine, 0);
    if(anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString qual = ctx->argument(1).toString();
    if(qual.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier name"));
    }
    QString val = ctx->argument(2).toString();
    if(val.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier value"));
    }

    QList<SharedAnnotationData> res;
    foreach (const SharedAnnotationData &ann, anns) {
        if (ann->qualifiers.contains(U2Qualifier(qual, val))) {
            res << ann;
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putAnnotationTable(engine, res));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::addQualifier(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 3 && ctx->argumentCount() > 4) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QList<SharedAnnotationData> anns = getAnnotationTable(ctx, engine, 0);
    if(anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString qual = ctx->argument(1).toString();
    if(qual.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier name"));
    }
    QString val = ctx->argument(2).toString();
    if(val.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty qualifier value"));
    }

    if (ctx->argumentCount() == 4) {
        QString name = ctx->argument(3).toString();
        if(name.isEmpty()) {
            return ctx->throwError(QObject::tr("forth argument must be a string"));
        }
        for (int i = 0; i < anns.size(); i++) {
            if (anns[i]->name == name) {
                anns[i]->qualifiers.append(U2Qualifier(qual,val));
            }
        }
    } else {
        for (int i = 0; i < anns.size(); i++) {
            anns[i]->qualifiers.append(U2Qualifier(qual, val));
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", putAnnotationTable(engine, anns));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::getLocation(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() != 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    const QList<SharedAnnotationData> anns = getAnnotationTable(ctx, engine, 0);
    if (anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QVariant var = ctx->argument(1).toVariant();
    bool ok;
    int num = var.toInt(&ok);
    if(!ok) {
        return ctx->throwError(QObject::tr("Second argument must be a number"));
    }

    if(num < 0 || num > anns.size()) {
        return ctx->throwError(QObject::tr("Index is out of range"));
    }
    QVector<U2Region> loc = anns[num]->getRegions();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", engine->newVariant(QVariant::fromValue<QVector<U2Region> >(loc)));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::hasAnnotationName(QScriptContext *ctx, QScriptEngine *engine){
    if(ctx->argumentCount()!= 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    const QList<SharedAnnotationData> anns = getAnnotationTable(ctx, engine, 0);
    if (anns.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid annotations"));
    }
    QString annName = ctx->argument(1).toString();
    if (annName.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty annotation name"));
    }

    bool hasAnnotation = false;
    foreach (const SharedAnnotationData &ann, anns) {
        if (ann->name == annName) {
            hasAnnotation = true;
            break;
        }
    }

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", hasAnnotation);
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::writeFile(QScriptContext *ctx, QScriptEngine * /*engine*/){
    if(ctx->argumentCount()!= 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString filePath = ctx->argument(0).toString();
    if(filePath.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid file path"));
    }

    QString data = ctx->argument(1).toString();
    if(data.isEmpty()) {
        return ctx->throwError(QObject::tr("No data to write"));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return ctx->throwError(QObject::tr("Cannot open the file by given path"));
    }

    file.write(data.toLatin1());

    file.close();

    return 0;
}

QScriptValue WorkflowScriptLibrary::appendFile(QScriptContext *ctx, QScriptEngine * /*engine*/){
    if(ctx->argumentCount()!= 2) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString filePath = ctx->argument(0).toString();
    if(filePath.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid file path"));
    }

    QString data = ctx->argument(1).toString();
    if(data.isEmpty()) {
        return ctx->throwError(QObject::tr("No data to write"));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        return ctx->throwError(QObject::tr("Cannot open the file by given path"));
    }

    file.write(data.toLatin1());

    file.close();

    return 0;
}

QScriptValue WorkflowScriptLibrary::readFile(QScriptContext *ctx, QScriptEngine * /*engine*/){
    if(ctx->argumentCount()!= 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }

    QString filePath = ctx->argument(0).toString();
    if(filePath.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid file path"));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly |  QIODevice::Text)){
        return ctx->throwError(QObject::tr("Cannot open the file by given path"));
    }

    QByteArray result = file.readAll();

    file.close();

    QScriptValue calee = ctx->callee();
    calee.setProperty("res", QString(result));
    return calee.property("res");
}

QScriptValue WorkflowScriptLibrary::readSequences(QScriptContext *ctx, QScriptEngine *engine) {
    DbiDataStorage *storage = ScriptEngineUtils::dataStorage(engine);
    CHECK(NULL != storage, QScriptValue());

    if (ctx->argumentCount() != 1) {
        return ctx->throwError(QObject::tr("Incorrect number of arguments"));
    }
    QString filePath = ctx->argument(0).toString();
    QString fileName = QFileInfo(filePath).fileName();
    if (filePath.isEmpty()) {
        return ctx->throwError(QObject::tr("Empty file path"));
    }

    QList<DocumentFormat*> detection = DocumentUtils::toFormats(DocumentUtils::detectFormat(filePath));
    if (detection.isEmpty()) {
        return ctx->throwError(QObject::tr("Can't detect the sequence file format: ") + fileName);
    }
    DocumentFormat *format = detection.first();
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(storage->getDbiRef());

    U2OpStatusImpl os;
    QScopedPointer<Document> doc(format->loadDocument(iof, filePath, hints, os));
    if (os.hasError()) {
        return ctx->throwError(os.getError());
    }
    doc->setDocumentOwnsDbiResources(false);

    QList<GObject*> objects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (objects.isEmpty()) {
        return ctx->throwError(QObject::tr("There are no sequences in the file: ") + fileName);
    }

    QScriptValue array = engine->newArray(objects.size());
    for (int i=0; i<objects.size(); i++) {
        SharedDbiDataHandler id = storage->getDataHandler(objects[i]->getEntityRef());
        QScriptValue value = ScriptEngineUtils::getSequenceClass(engine)->newInstance(id);
        array.setProperty(i, value);
    }

    ctx->callee().setProperty("res", array);
    return ctx->callee().property("res");
}

QScriptValue WorkflowScriptLibrary::debugOut(QScriptContext *ctx, QScriptEngine *) {
    QString msg = "";
    if(ctx->argument(0).isNumber()) {
        msg = QString::number(ctx->argument(0).toInt32());
    } else if(ctx->argument(0).isString()) {
        msg = ctx->argument(0).toString();
    } else if(ctx->argument(0).isBool()) {
        if(ctx->argument(0).toBool()) {
            msg = "true";
        } else {
            msg = "false";
        }
    }
    scriptLog.details(msg);
    return 0;
}

WorkflowScriptFactory::~WorkflowScriptFactory() {

}

} // U2
