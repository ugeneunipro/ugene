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

#ifndef _GROUPER_ACTION_UTILS_
#define _GROUPER_ACTION_UTILS_

#include <U2Core/AnnotationData.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Lang/Datatype.h>
#include <U2Lang/GrouperOutSlot.h>
#include <U2Lang/WorkflowContext.h>

namespace U2 {
namespace Workflow {

/**
 * Base class for all action performers
 */
class ActionPerformer {
public:
    ActionPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context);
    virtual ~ActionPerformer() {}
    virtual bool applyAction(const QVariant &newData) = 0;
    virtual QVariant finishAction(U2OpStatus &os) = 0;
    virtual void setParameters(const QVariantMap &);
    virtual QVariantMap getParameters() const;
    virtual QString getActionType() const;

    QString getOutSlot() const;
    void setParentPerformer(ActionPerformer *parent) {this->parent = parent;}
    ActionPerformer *getParentPerformer() const {return parent;}

protected:
    QString outSlot;
    GrouperSlotAction action;
    WorkflowContext *context;
    bool started;
    qint64 lastDataNumber;
    ActionPerformer *parent;
};

typedef QMap<QString, ActionPerformer*> PerformersMap;

/**
 * Utility class for grouper actions
 */
class GrouperActionUtils {
public:
    static ActionPerformer *getActionPerformer(const GrouperOutSlot &slot, WorkflowContext *context, const PerformersMap &perfs);
    static bool equalData(const QString &groupOp, const QVariant &data1, const QVariant &data2, DataTypePtr dataType, WorkflowContext *context);
    static void applyActions(WorkflowContext *context, QList<GrouperOutSlot> outSlots, const QVariantMap &mData, PerformersMap &perfs);
};

/************************************************************************/
/* Implemented action performers */
/************************************************************************/
class MergeSequencePerformer : public ActionPerformer {
public:
    MergeSequencePerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context);
    virtual bool applyAction(const QVariant &newData);
    virtual QVariant finishAction(U2OpStatus &os);
    virtual QVariantMap getParameters() const;

    static QString PREV_SEQ_LENGTH;
private:
    U2SequenceImporter importer;
    qint64 prevSeqLen;
};

class Sequence2MSAPerformer : public ActionPerformer {
public:
    Sequence2MSAPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context);
    virtual bool applyAction(const QVariant &newData);
    virtual QVariant finishAction(U2OpStatus &os);

private:
    MultipleSequenceAlignment result;
};

class MergerMSAPerformer : public ActionPerformer {
public:
    MergerMSAPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context);
    virtual bool applyAction(const QVariant &newData);
    virtual QVariant finishAction(U2OpStatus &os);

private:
    MultipleSequenceAlignment result;
};

class MergerStringPerformer : public ActionPerformer {
public:
    MergerStringPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context);
    virtual bool applyAction(const QVariant &newData);
    virtual QVariant finishAction(U2OpStatus &os);

private:
    QString result;
};

class MergeAnnotationPerformer : public ActionPerformer {
public:
    MergeAnnotationPerformer(const QString &outSlot, const GrouperSlotAction &action, WorkflowContext *context);
    virtual bool applyAction(const QVariant &newData);
    virtual QVariant finishAction(U2OpStatus &os);
    virtual QVariantMap getParameters() const;
    virtual void setParameters(const QVariantMap &map);

    static QString PARENT_SEQUENCE_SLOT;
private:
    QList<SharedAnnotationData> result;
    qint64 offset;
};

} // Workflow
} // U2


#endif // _GROUPER_ACTION_UTILS_
