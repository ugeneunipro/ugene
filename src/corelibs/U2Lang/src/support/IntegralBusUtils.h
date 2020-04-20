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

#ifndef _U2_INTEGRAL_BUS_UTILS_H_
#define _U2_INTEGRAL_BUS_UTILS_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/Datatype.h>

namespace U2 {
namespace Workflow {

class IntegralBusUtils {
public:
    class SplitResult {
    public:
        QList<Descriptor> mainDescs;
        QList<Descriptor> otherDescs;
    };

    static SplitResult splitCandidates(const QList<Descriptor> &candidates, const Descriptor &toDesc, DataTypePtr toDatatype);
    static void remapBus(StrStrMap &busMap, const ActorId &oldId, const ActorId &newId, const PortMapping &mapping);
    static void remapPathedSlotString(QString &pathedSlotStr, const ActorId &oldId, const ActorId &newId, const PortMapping &mapping);
    static QList<Descriptor> getSlotsByType(const QMap<Descriptor, DataTypePtr> &busMap, const Descriptor &slot, const DataTypePtr &type);
};

class U2LANG_EXPORT CandidatesSplitter {
public:
    CandidatesSplitter(const QString &id);
    virtual ~CandidatesSplitter();

    virtual bool canSplit(const Descriptor &toDesc, DataTypePtr toDatatype) = 0;
    virtual IntegralBusUtils::SplitResult splitCandidates(const QList<Descriptor> &candidates);

    const QString &getId() const;

protected:
    virtual bool isMain(const QString &candidateSlotId) = 0;

private:
    const QString id;
};

class U2LANG_EXPORT CandidatesSplitterRegistry {
public:
    ~CandidatesSplitterRegistry();
    static CandidatesSplitterRegistry * instance();

    CandidatesSplitter * findSplitter(const Descriptor &toDesc, DataTypePtr toDatatype);
    CandidatesSplitter * findSplitter(const QString &id);

    void registerSplitter(CandidatesSplitter *splitter);
    void unregisterSplitter(const QString &id);

private:
    static CandidatesSplitterRegistry *_instance;
    QList<CandidatesSplitter*> splitters;

private:
    CandidatesSplitterRegistry();
};

} // Workflow
} // U2

#endif // _U2_INTEGRAL_BUS_UTILS_H_
