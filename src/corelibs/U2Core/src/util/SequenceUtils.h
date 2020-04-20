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

#ifndef _U1_SEQUENCE_UTILS_H_
#define _U1_SEQUENCE_UTILS_H_

#include <U2Core/U2Region.h>

namespace U2 {

class DNATranslation;
class Document;
class GObject;
class U2DbiRef;
class U2OpStatus;

class U2CORE_EXPORT U1SequenceUtils {
public:
    /** Extract sequence parts marked by the regions
        Note: the order of complemented regions is also reversed
        */
    static QList<QByteArray> extractRegions(const QByteArray& seq, const QVector<U2Region>& regions,
        const DNATranslation* complTT, const DNATranslation* aminoTT = NULL, bool circular = false, bool join = false);

    /** Joins sequence parts into a single sequence, with given gap */
    static QByteArray joinRegions(const QList<QByteArray>& parts, int gapSize = 0);

    /** Translates sequence parts, if  join == true -> joins parts before translation is started
    */
    static QList<QByteArray> translateRegions(const QList<QByteArray>& origParts, const DNATranslation* aminoTT, bool join);

    /** Returns regions locations as if they were joined */
    static QVector<U2Region> getJoinedMapping(const QList<QByteArray>& seqParts);

    static QList<GObject *> mergeSequences(const QList<Document*> doc, const U2DbiRef& ref, const QString& newStringUrl, QVariantMap& hints, U2OpStatus& os);
    static QList<GObject *> mergeSequences(Document* doc, const U2DbiRef& ref, QVariantMap& hints, U2OpStatus& os);
};


}//namespace

#endif
