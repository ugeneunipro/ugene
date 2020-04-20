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

#ifndef _U2_WORKFLOW_SEQ_ACTOR_H_
#define _U2_WORKFLOW_SEQ_ACTOR_H_

#include <U2Lang/Attribute.h>
#include <U2Lang/ReadDbObjActorPrototype.h>

#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace Workflow {

class GenericReadDocProto : public ReadDbObjActorPrototype {
public:
    GenericReadDocProto(const Descriptor &desc);
};

class GenericMAActorProto : public GenericReadDocProto {
public:
    static const QString TYPE;
    GenericMAActorProto();
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class GenericSeqActorProto : public GenericReadDocProto {
public:
    enum Mode{
        SPLIT,
        MERGE,
    };

    static const QString MODE_ATTR;
    static const QString GAP_ATTR;
    static const QString ACC_ATTR;
    static const QString LIMIT_ATTR;
    static const QString TYPE;

    GenericSeqActorProto();
    virtual bool isAcceptableDrop(const QMimeData*, QVariantMap*) const;
};

class SeqReadPrompter;
typedef PrompterBase<SeqReadPrompter> SeqReadPrompterBase;

class SeqReadPrompter : public SeqReadPrompterBase {
    Q_OBJECT
public:
    SeqReadPrompter(Actor* p = 0) : SeqReadPrompterBase(p) {}
protected:
    QString composeRichDoc();
};

}//namespace Workflow
}//namespace U2
#endif
