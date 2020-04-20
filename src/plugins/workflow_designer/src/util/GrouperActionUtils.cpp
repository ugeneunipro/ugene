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

#include <QScopedPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/BaseTypes.h>

#include "GrouperActionUtils.h"

namespace U2 {
namespace Workflow {

/************************************************************************/
/* GrouperActionUtils */
/************************************************************************/
ActionPerformer *GrouperActionUtils::getActionPerformer(const GrouperOutSlot &slot, WorkflowContext *context, const PerformersMap &perfs) {
    GrouperSlotAction *actionPtr = slot.getAction();
    SAFE_POINT(NULL != actionPtr, "GrouperActionUtils::getActionPerformer - action is null", NULL);

    GrouperSlotAction action(*actionPtr);
    QString type = action.getType();
    if (ActionTypes::MERGE_SEQUENCE == type) {
        QString seqSlot = slot.getOutSlotId();
        MergeSequencePerformer *msp = new MergeSequencePerformer(seqSlot, action, context);
        foreach (ActionPerformer *p, perfs) {
            if (ActionTypes::MERGE_ANNS != p->getActionType()) {
                continue;
            }
            QVariantMap params = p->getParameters();
            if (params.contains(MergeAnnotationPerformer::PARENT_SEQUENCE_SLOT)) {
                QString parentSlot = params[MergeAnnotationPerformer::PARENT_SEQUENCE_SLOT].toString();
                if (parentSlot == seqSlot) {
                    p->setParentPerformer(msp);
                    break;
                }
            }
        }
        return msp;
    } else if (ActionTypes::MERGE_MSA == type) {
        return new MergerMSAPerformer(slot.getOutSlotId(), action, context);
    } else if (ActionTypes::SEQUENCE_TO_MSA == type) {
        return new Sequence2MSAPerformer(slot.getOutSlotId(), action, context);
    } else if (ActionTypes::MERGE_STRING == type) {
        return new MergerStringPerformer(slot.getOutSlotId(), action, context);
    } else if (ActionTypes::MERGE_ANNS == type) {
        QString seqSlot = slot.getAction()->getParameterValue(ActionParameters::SEQ_SLOT).toString();
        MergeAnnotationPerformer *map = new MergeAnnotationPerformer(slot.getOutSlotId(), action, context);
        if (!seqSlot.isEmpty()) {
            foreach (ActionPerformer *p, perfs) {
                if (ActionTypes::MERGE_SEQUENCE != p->getActionType()) {
                    continue;
                }
                if (p->getOutSlot() == seqSlot) {
                    map->setParentPerformer(p);
                    break;
                }
            }
        }
        return map;
    }
    assert(false);
    return NULL;
}

bool GrouperActionUtils::equalData(const QString &groupOp, const QVariant &data1, const QVariant &data2, DataTypePtr dataType, WorkflowContext *context) {
    if (BaseTypes::DNA_SEQUENCE_TYPE() == dataType) {
        SharedDbiDataHandler seqId1 = data1.value<SharedDbiDataHandler>();
        SharedDbiDataHandler seqId2 = data2.value<SharedDbiDataHandler>();

        if (GroupOperations::BY_ID() == groupOp) {
            return seqId1 == seqId2;
        }

        U2OpStatusImpl os;
        QScopedPointer<U2SequenceObject> seqObj1(StorageUtils::getSequenceObject(context->getDataStorage(), seqId1));
        QScopedPointer<U2SequenceObject> seqObj2(StorageUtils::getSequenceObject(context->getDataStorage(), seqId2));
        QString name1 = seqObj1->getSequenceName();
        QString name2 = seqObj2->getSequenceName();

        if (GroupOperations::BY_NAME() == groupOp) {
            return name1 == name2;
        } else if (GroupOperations::BY_VALUE() == groupOp) {
            if (name1 != name2) {
                return false;
            }
            U2OpStatusImpl os;
            QByteArray seq1 = seqObj1->getWholeSequenceData(os);
            CHECK_OP(os, false);
            QByteArray seq2 = seqObj2->getWholeSequenceData(os);
            CHECK_OP(os, false);

            return seq1 == seq2;
        }
    }
    else if (BaseTypes::MULTIPLE_ALIGNMENT_TYPE() == dataType) {
        SharedDbiDataHandler alId1 = data1.value<SharedDbiDataHandler>();
        SharedDbiDataHandler alId2 = data2.value<SharedDbiDataHandler>();

        QScopedPointer<MultipleSequenceAlignmentObject> alObj1(StorageUtils::getMsaObject(context->getDataStorage(), alId1));
        SAFE_POINT(NULL != alObj1.data(), "NULL MSA Object!", false);

        QScopedPointer<MultipleSequenceAlignmentObject> alObj2(StorageUtils::getMsaObject(context->getDataStorage(), alId2));
        SAFE_POINT(NULL != alObj2.data(), "NULL MSA Object!", false);


        const MultipleSequenceAlignment al1 = alObj1->getMultipleAlignment();
        const MultipleSequenceAlignment al2 = alObj2->getMultipleAlignment();

        if (GroupOperations::BY_NAME() == groupOp) {
            return al1->getName() == al2->getName();
        } else { // id or value
            if (al1->getMsaRows().size() != al2->getMsaRows().size()) {
                return false;
            }

            QList<MultipleSequenceAlignmentRow> rows1 = al1->getMsaRows();
            QList<MultipleSequenceAlignmentRow> rows2 = al2->getMsaRows();
            QList<MultipleSequenceAlignmentRow>::const_iterator it1 = rows1.constBegin();
            QList<MultipleSequenceAlignmentRow>::const_iterator it2 = rows2.constBegin();
            for (; it1 != rows1.constEnd(); ++it1, ++it2) {
                if (**it1 != **it2) {
                    return false;
                }
            }
            return true;
        }
    }
    else if (BaseTypes::STRING_TYPE() == dataType) {
        return data1.toString() == data2.toString();
    }

    return false;
}

void GrouperActionUtils::applyActions(WorkflowContext *context, QList<GrouperOutSlot> outSlots, const QVariantMap &mData, PerformersMap &perfs) {
    foreach (const GrouperOutSlot &slot, outSlots) {
        QString key = slot.getBusMapInSlotId();
        if (mData.keys().contains(key)) {
            if (!perfs.contains(slot.getOutSlotId())) {
                ActionPerformer *p = getActionPerformer(slot, context, perfs);
                SAFE_POINT(NULL != p, "GrouperActionUtils::applyActions - performer is NULL",);

                perfs[slot.getOutSlotId()] = p;
            }
        }
    }
    QList<ActionPerformer*> perfList = perfs.values();

    while (!outSlots.isEmpty()) {
        foreach (const GrouperOutSlot &slot, outSlots) {
            QString key = slot.getBusMapInSlotId();
            if (mData.keys().contains(key)) {
                ActionPerformer *p = perfs.value(slot.getOutSlotId(), NULL);
                ActionPerformer *parent = p->getParentPerformer();
                if (perfList.contains(parent)) {
                    continue;
                }

                QVariant newData = mData.value(key);
                if (NULL != parent) {
                    QVariantMap params = parent->getParameters();
                    p->setParameters(params);
                }
                p->applyAction(newData);
                perfList.removeOne(p);
            }
            outSlots.removeOne(slot);
        }
    }
}

/************************************************************************/
/* Action performers */
/************************************************************************/
ActionPerformer::ActionPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context)
: outSlot(outSlot), action(action), context(context), started(false), parent(NULL)
{

}

QString ActionPerformer::getOutSlot() const {
    return outSlot;
}

void ActionPerformer::setParameters(const QVariantMap &) {

}

QVariantMap ActionPerformer::getParameters() const {
    return QVariantMap();
}

QString ActionPerformer::getActionType() const {
    return action.getType();
}

MergeSequencePerformer::MergeSequencePerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context)
: ActionPerformer(outSlot, action, context), prevSeqLen(0)
{

}

bool MergeSequencePerformer::applyAction(const QVariant &newData) {
    U2OpStatusImpl os;
    SharedDbiDataHandler seqId = newData.value<SharedDbiDataHandler>();
    QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));

    if (!started) {
        QString name;
        if (action.hasParameter(ActionParameters::SEQ_NAME)) {
            name = action.getParameterValue(ActionParameters::SEQ_NAME).toString();
        } else {
            name = "Merged sequence";
        }
        importer.startSequence(os, context->getDataStorage()->getDbiRef(), U2ObjectDbi::ROOT_FOLDER, name, false);
        CHECK_OP(os, false);
        started = true;
    } else {
        int gap = action.getParameterValue(ActionParameters::GAP).toInt();
        if (gap > 0) {
            importer.addDefaultSymbolsBlock(gap, os);
        }
        prevSeqLen = importer.getCurrentLength();
    }

    QByteArray seqData = seqObj->getWholeSequenceData(os);
    CHECK_OP(os, false);
    importer.addBlock(seqData.constData(), seqObj->getSequenceLength(), os);
    CHECK_OP(os, false);

    return true;
}

QVariant MergeSequencePerformer::finishAction(U2OpStatus &os) {
    U2Sequence seq = importer.finalizeSequenceAndValidate(os);
    CHECK_OP(os, QVariant());
    U2EntityRef entRef(context->getDataStorage()->getDbiRef(), seq.id);
    SharedDbiDataHandler handler = context->getDataStorage()->getDataHandler(entRef);
    return qVariantFromValue<SharedDbiDataHandler>(handler);
}

QVariantMap MergeSequencePerformer::getParameters() const {
    QVariantMap res;
    res[PREV_SEQ_LENGTH] = prevSeqLen;
    return res;
}

QString MergeSequencePerformer::PREV_SEQ_LENGTH = QString("prev-seq-length");

Sequence2MSAPerformer::Sequence2MSAPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context)
    : ActionPerformer(outSlot, action, context)
{

}

bool Sequence2MSAPerformer::applyAction(const QVariant &newData) {
    SharedDbiDataHandler seqId = newData.value<SharedDbiDataHandler>();
    QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
    if (NULL == seqObj.data()) {
        return false;
    }

    U2OpStatus2Log os;
    QString rowName = seqObj->getGObjectName();
    QByteArray bytes = seqObj->getWholeSequenceData(os);
    CHECK_OP(os, false);

    if (!started) {
        QString name;
        if (action.hasParameter(ActionParameters::MSA_NAME)) {
            name = action.getParameterValue(ActionParameters::MSA_NAME).toString();
        } else {
            name = "Grouped alignment";
        }
        result->setName(name);
        result->setAlphabet(seqObj->getAlphabet());
        started = true;
    }

    bool unique = false;
    if (action.hasParameter(ActionParameters::UNIQUE)) {
        unique = action.getParameterValue(ActionParameters::UNIQUE).toBool();
    }

    if (unique) {
        foreach (const MultipleSequenceAlignmentRow &currRow, result->getMsaRows()) {
            if ((currRow->getName() == rowName) &&
                (currRow->getData() == bytes)) {
                    return true;
            }
        }
    }

    result->addRow(rowName, bytes);

    return true;
}

QVariant Sequence2MSAPerformer::finishAction(U2OpStatus &) {
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(result);
    return qVariantFromValue<SharedDbiDataHandler>(msaId);
}

MergerMSAPerformer::MergerMSAPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context)
    : ActionPerformer(outSlot, action, context)
{

}

bool MergerMSAPerformer::applyAction(const QVariant &newData) {
    SharedDbiDataHandler newAlId = newData.value<SharedDbiDataHandler>();
    QScopedPointer<MultipleSequenceAlignmentObject> newAlObj(StorageUtils::getMsaObject(context->getDataStorage(), newAlId));
    SAFE_POINT(NULL != newAlObj.data(), "NULL MSA Object!", false);
    const MultipleSequenceAlignment newAl = newAlObj->getMultipleAlignment();

    if (!started) {
        QString name;
        if (action.hasParameter(ActionParameters::MSA_NAME)) {
            name = action.getParameterValue(ActionParameters::MSA_NAME).toString();
        } else {
            name = "Grouped alignment";
        }
        result->setName(name);
        result->setAlphabet(newAl->getAlphabet());
        started = true;
    }

    bool unique = false;
    if (action.hasParameter(ActionParameters::UNIQUE)) {
        unique = action.getParameterValue(ActionParameters::UNIQUE).toBool();
    }

    U2OpStatus2Log os;
    const QList<MultipleSequenceAlignmentRow> rows = result->getMsaRows();
    foreach (const MultipleSequenceAlignmentRow &newRow, newAl->getMsaRows()) {
        if (unique) {
            if (!rows.contains(newRow)) {
                result->addRow(newRow->getRowDbInfo(), newRow->getSequence(), os);
            }
        } else {
            result->addRow(newRow->getRowDbInfo(), newRow->getSequence(), os);
        }
    }

    return true;
}

QVariant MergerMSAPerformer::finishAction(U2OpStatus &) {
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(result);
    return qVariantFromValue<SharedDbiDataHandler>(msaId);
}

MergerStringPerformer::MergerStringPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context)
: ActionPerformer(outSlot, action, context)
{

}

bool MergerStringPerformer::applyAction(const QVariant &newData) {
    if (!started) {
        started = true;
    } else {
        QString sep = action.getParameterValue(ActionParameters::SEPARATOR).toString();
        result += sep;
    }

    result += newData.toString();

    return true;
}

QVariant MergerStringPerformer::finishAction(U2OpStatus &) {
    return result;
}

MergeAnnotationPerformer::MergeAnnotationPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context)
: ActionPerformer(outSlot, action, context), offset(0)
{
    started = true;
}

static void shiftAnns(QList<SharedAnnotationData> &newAnns, qint64 offset) {
    QList<SharedAnnotationData> res;
    foreach (SharedAnnotationData d, newAnns) {
        U2Region::shift(offset, d->location->regions);
        res << d;
    }
    newAnns = res;
}

bool MergeAnnotationPerformer::applyAction(const QVariant &newData) {
    QList<SharedAnnotationData> newAnns = StorageUtils::getAnnotationTable(context->getDataStorage(), newData);

    bool unique = false;
    if (action.hasParameter(ActionParameters::UNIQUE)) {
        unique = action.getParameterValue(ActionParameters::UNIQUE).toBool();
    }

    if (offset > 0) {
        shiftAnns(newAnns, offset);
        offset = 0;
    }

    if (unique) {
        foreach (SharedAnnotationData newD, newAnns) {
            bool found = false;
            foreach (const SharedAnnotationData &d, result) {
                if (*newD == *d) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result << newD;
            }
        }
    } else {
        result << newAnns;
    }

    return true;
}

QVariant MergeAnnotationPerformer::finishAction(U2OpStatus &) {
    const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(result);
    return qVariantFromValue<SharedDbiDataHandler>(tableId);
}

QString MergeAnnotationPerformer::PARENT_SEQUENCE_SLOT = QString("parent-seq-slot");

QVariantMap MergeAnnotationPerformer::getParameters() const {
    QVariantMap res;
    res[PARENT_SEQUENCE_SLOT] = action.getParameterValue(ActionParameters::SEQ_SLOT);
    return res;
}

void MergeAnnotationPerformer::setParameters(const QVariantMap &map) {
    offset = map.value(MergeSequencePerformer::PREV_SEQ_LENGTH, 0).toLongLong();
}

} // Workflow
} // U2
