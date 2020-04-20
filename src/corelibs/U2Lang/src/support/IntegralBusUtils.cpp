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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBus.h>
#include <U2Lang/IntegralBusModel.h>

#include "IntegralBusUtils.h"

namespace U2 {
namespace Workflow {

IntegralBusUtils::SplitResult IntegralBusUtils::splitCandidates(const QList<Descriptor> &candidates, const Descriptor &toDesc, DataTypePtr toDatatype) {
    CandidatesSplitter *splitter = CandidatesSplitterRegistry::instance()->findSplitter(toDesc, toDatatype);
    SAFE_POINT(NULL != splitter, "NULL splitter", SplitResult());
    return splitter->splitCandidates(candidates);
}

void IntegralBusUtils::remapBus(StrStrMap &busMap, const ActorId &oldId, const ActorId &newId, const PortMapping &mapping) {
    foreach (QString key, busMap.uniqueKeys()) {
        U2OpStatus2Log os;
        QList<IntegralBusSlot> slotList = IntegralBusSlot::listFromString(busMap[key], os);
        QList<IntegralBusSlot> newSlots;
        foreach (const IntegralBusSlot &slot, slotList) {
            IntegralBusSlot newSlot = slot;
            if (slot.actorId() == oldId) {
                U2OpStatusImpl os;
                QString newSlotId = mapping.getDstSlotId(slot.getId(), os);
                if (!os.hasError()) {
                    newSlot = IntegralBusSlot(newSlotId, "", newId);
                }
            }
            newSlots << newSlot;
        }
        busMap[key] = IntegralBusSlot::listToString(newSlots);
    }
}

void IntegralBusUtils::remapPathedSlotString(QString &pathedSlotStr, const ActorId &oldId, const ActorId &newId, const PortMapping &mapping) {
    if (pathedSlotStr.isEmpty()) {
        return;
    }
    QString slotStr;
    QStringList path;
    BusMap::parseSource(pathedSlotStr, slotStr, path);

    U2OpStatus2Log logOs;
    IntegralBusSlot slot = IntegralBusSlot::fromString(slotStr, logOs);
    if (slot.actorId() == oldId) {
        U2OpStatusImpl os;
        QString newSlot = mapping.getDstSlotId(slot.getId(), os);
        if (!os.hasError()) {
            slot = IntegralBusSlot(newSlot, "", newId);
        }
    }
    pathedSlotStr = slot.toString();

    if (!path.isEmpty()) {
        for (QStringList::iterator i=path.begin(); i!=path.end(); i++) {
            if (*i == oldId) {
                *i = newId;
            }
        }
        pathedSlotStr += ">" + path.join(",");
    }
}

namespace {
    enum StringSlotType {
        TEXT,
        URL,
        DATASET
    };

    bool isUrlSlot(const Descriptor &slot) {
        const QString id = slot.getId();
        return id.contains(BaseSlots::URL_SLOT().getId());
    }
    bool isDatasetSlot(const Descriptor &slot) {
        return (BaseSlots::DATASET_SLOT() == slot);
    }

    StringSlotType getSlotType(const Descriptor &slot) {
        if (isUrlSlot(slot)) {
            return URL;
        }
        if (isDatasetSlot(slot)) {
            return DATASET;
        }
        return TEXT;
    }
}

QList<Descriptor> IntegralBusUtils::getSlotsByType(const QMap<Descriptor, DataTypePtr> &busMap, const Descriptor &slot, const DataTypePtr &type) {
    QList<Descriptor> result = busMap.keys(type);
    CHECK(BaseTypes::STRING_TYPE() == type, result);
    CHECK(result.size() > 1, result);

    const StringSlotType targetType = getSlotType(slot);
    foreach (const Descriptor &desc, result) {
        if (targetType != getSlotType(desc)) {
            result.removeOne(desc);
        }
    }
    return result;
}

/************************************************************************/
/* Splitters */
/************************************************************************/
class DefaultSplitter : public CandidatesSplitter {
public:
    DefaultSplitter() : CandidatesSplitter(ID) {}

    bool canSplit(const Descriptor & /*toDesc*/, DataTypePtr /*toDatatype*/) {
        return true;
    }

    static const QString ID;

protected:
    bool isMain(const QString & /*candidateSlotId*/) {
        return true;
    }
};

const QString DefaultSplitter::ID = "default";

class TextSplitter : public CandidatesSplitter {
public:
    TextSplitter() : CandidatesSplitter(ID) {}

    bool canSplit(const Descriptor & /*toDesc*/, DataTypePtr toDatatype) {
        return (BaseTypes::STRING_TYPE() == toDatatype);
    }

    static const QString ID;

protected:
    bool isMain(const QString &candidateSlotId) {
        return (BaseSlots::URL_SLOT().getId() != candidateSlotId
            && BaseSlots::DATASET_SLOT().getId() != candidateSlotId);
    }
};
const QString TextSplitter::ID = "text";

class DatasetsSplitter : public CandidatesSplitter {
public:
    DatasetsSplitter() : CandidatesSplitter(ID) {}

    bool canSplit(const Descriptor &toDesc, DataTypePtr toDatatype) {
        return ((BaseTypes::STRING_TYPE() == toDatatype) && isDatasetSlot(toDesc));
    }

    static const QString ID;

protected:
    bool isMain(const QString &candidateSlotId) {
        return (BaseSlots::DATASET_SLOT().getId() == candidateSlotId);
    }
};

const QString DatasetsSplitter::ID = "datasets";

class UrlSplitter : public CandidatesSplitter {
public:
    UrlSplitter() : CandidatesSplitter(ID) {}

    bool canSplit(const Descriptor &toDesc, DataTypePtr toDatatype) {
        return ((BaseTypes::STRING_TYPE() == toDatatype) && isUrlSlot(toDesc));
    }

    static const QString ID;

protected:
    bool isMain(const QString &candidateSlotId) {
        return (BaseSlots::URL_SLOT().getId() == candidateSlotId);
    }
};

const QString UrlSplitter::ID = "url";

/************************************************************************/
/* CandidatesSplitter */
/************************************************************************/
CandidatesSplitter::CandidatesSplitter(const QString &_id)
    : id(_id)
{

}

CandidatesSplitter::~CandidatesSplitter() {

}

IntegralBusUtils::SplitResult CandidatesSplitter::splitCandidates(const QList<Descriptor> &candidates) {
    IntegralBusUtils::SplitResult r;
    foreach (const Descriptor &c, candidates) {
        U2OpStatus2Log os;
        IntegralBusSlot slot = IntegralBusSlot::fromString(c.getId(), os);
        if (slot.getId().isEmpty() || isMain(slot.getId())) {
            r.mainDescs << c;
        } else {
            r.otherDescs << c;
        }
    }
    return r;
}

const QString &CandidatesSplitter::getId() const {
    return id;
}

/************************************************************************/
/* CandidatesSplitterRegistry */
/************************************************************************/
CandidatesSplitterRegistry *CandidatesSplitterRegistry::_instance = NULL;
CandidatesSplitterRegistry * CandidatesSplitterRegistry::instance() {
    if (NULL == _instance) {
        _instance = new CandidatesSplitterRegistry();
    }
    return _instance;
}

CandidatesSplitter * CandidatesSplitterRegistry::findSplitter(const Descriptor &toDesc, DataTypePtr toDatatype) {
    foreach (CandidatesSplitter *splitter, splitters) {
        if (splitter->canSplit(toDesc, toDatatype)) {
            return splitter;
        }
    }
    return NULL;
}

CandidatesSplitter *CandidatesSplitterRegistry::findSplitter(const QString &id) {
    foreach (CandidatesSplitter *splitter, splitters) {
        if (id == splitter->getId()) {
            return splitter;
        }
    }
    return NULL;
}

void CandidatesSplitterRegistry::registerSplitter(CandidatesSplitter *splitter) {
    if (!splitters.contains(splitter)) {
        splitters.prepend(splitter);
    }
}

void CandidatesSplitterRegistry::unregisterSplitter(const QString &id) {
    CandidatesSplitter *splitter = findSplitter(id);
    CHECK(NULL != splitter, );
    splitters.removeAll(splitter);
    delete splitter;
}

CandidatesSplitterRegistry::CandidatesSplitterRegistry() {
    // The order is needed
    splitters << new UrlSplitter();
    splitters << new DatasetsSplitter();
    splitters << new TextSplitter();
    splitters << new DefaultSplitter();
}

CandidatesSplitterRegistry::~CandidatesSplitterRegistry() {
    qDeleteAll(splitters);
    splitters.clear();
}

} // Workflow
} // U2
