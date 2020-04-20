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

#include "SWMulAlignResultNamesTagsRegistry.h"
#include "SWMulAlignSeqPrefixTag.h"
#include "SWMulAlignSubseqPropTag.h"
#include "SWMulAlignExternalPropTag.h"

#include <QMutexLocker>

const quint32 SEQ_NAME_PREFIX_LENGTH = 10;
const QString SEQ_NAME_PREFIX_TAG_SHORTHAND = "SN";
const char * SEQ_NAME_PREFIX_TAG_LABEL = "Reference sequence name prefix";
const QString PTRN_NAME_PREFIX_TAG_SHORTHAND = "PN";
const char * PTRN_NAME_PREFIX_TAG_LABEL ="Pattern sequence name prefix";

const QString SUBSEQ_START_POS_TAG_SHORTHAND = "S";
const char * SUBSEQ_START_POS_TAG_LABEL = "Subsequence start position";
const QString SUBSEQ_END_POS_TAG_SHORTHAND = "E";
const char * SUBSEQ_END_POS_TAG_LABEL = "Subsequence end position";
const QString SUBSEQ_LENGTH_TAG_SHORTHAND = "L";
const char * SUBSEQ_LENGTH_TAG_LABEL = "Subsequence length";

const QString DATE_TAG_SHORTHAND = "MDY";
const char * DATE_TAG_LABEL = "Date";
const QString TIME_TAG_SHORTHAND = "hms";
const char * TIME_TAG_LABEL = "Time";
const QString COUNTER_TAG_SHORTHAND = "C";
const char * COUNTER_TAG_LABEL = "Counter";

const qint32 NOT_FOUND_SUBSTR_INDEX = -1;

namespace U2 {

SWMulAlignResultNamesTagsRegistry::SWMulAlignResultNamesTagsRegistry()
{
    registerTag(new SWMulAlignSeqPrefixTag(SEQ_NAME_PREFIX_TAG_SHORTHAND, tr(SEQ_NAME_PREFIX_TAG_LABEL), SEQ_NAME_PREFIX_LENGTH));
    registerTag(new SWMulAlignSeqPrefixTag(PTRN_NAME_PREFIX_TAG_SHORTHAND, tr(PTRN_NAME_PREFIX_TAG_LABEL), SEQ_NAME_PREFIX_LENGTH));

    registerTag(new SWMulAlignSubseqPropTag(SUBSEQ_START_POS_TAG_SHORTHAND, tr(SUBSEQ_START_POS_TAG_LABEL), SWMulAlignSubseqPropTag::START));
    registerTag(new SWMulAlignSubseqPropTag(SUBSEQ_END_POS_TAG_SHORTHAND, tr(SUBSEQ_END_POS_TAG_LABEL), SWMulAlignSubseqPropTag::END));
    registerTag(new SWMulAlignSubseqPropTag(SUBSEQ_LENGTH_TAG_SHORTHAND, tr(SUBSEQ_LENGTH_TAG_LABEL), SWMulAlignSubseqPropTag::LENGTH));

    registerTag(new SWMulAlignExternalPropTag(DATE_TAG_SHORTHAND, DATE_TAG_LABEL, SWMulAlignExternalPropTag::DATE));
    registerTag(new SWMulAlignExternalPropTag(TIME_TAG_SHORTHAND, TIME_TAG_LABEL, SWMulAlignExternalPropTag::TIME));
    registerTag(new SWMulAlignExternalPropTag(COUNTER_TAG_SHORTHAND, COUNTER_TAG_LABEL, SWMulAlignExternalPropTag::COUNTER));
}

QString SWMulAlignResultNamesTagsRegistry::tagExpansion(const QString & shorthand, const QVariant & argument) const
{
    assert(tags.contains(shorthand));

    return tags[shorthand]->expandTag(argument);
}

QString SWMulAlignResultNamesTagsRegistry::parseStringWithTags(const QString & str, const SmithWatermanReportCallbackMAImpl::TagExpansionPossibleData & expansionSet) const
{
    QString resultStr = str;
    foreach(SWMulAlignResultNamesTag * tag, tags) {
        qint32 tagIndex = 0;
        const QString tagShorthand = tag->getShorthand();
        const QString tagMnemonis = OPEN_SQUARE_BRACKET + tagShorthand + CLOSE_SQUARE_BRACKET;

        Q_FOREVER {
            tagIndex = resultStr.indexOf(tagMnemonis, tagIndex);
            if(NOT_FOUND_SUBSTR_INDEX == tagIndex) {
                break;
            }

            QVariant tagExpansionData;
            if(PTRN_NAME_PREFIX_TAG_SHORTHAND == tagShorthand) {
                tagExpansionData.setValue(expansionSet.patternName);
            } else if(SEQ_NAME_PREFIX_TAG_SHORTHAND == tagShorthand) {
                tagExpansionData.setValue(expansionSet.refSequenceName);
            } else if(SUBSEQ_START_POS_TAG_SHORTHAND == tagShorthand || SUBSEQ_END_POS_TAG_SHORTHAND == tagShorthand ||
            SUBSEQ_LENGTH_TAG_SHORTHAND == tagShorthand) {
                assert(NULL != expansionSet.curProcessingSubseq);
                tagExpansionData.setValue(*expansionSet.curProcessingSubseq);
            }

            resultStr.replace(tagIndex, tagMnemonis.length(), tag->expandTag(tagExpansionData));
        }
    }

    return resultStr;
}

bool SWMulAlignResultNamesTagsRegistry::registerTag(SWMulAlignResultNamesTag * tag)
{
    QMutexLocker locker(&mutex);
    QString key = tag->getShorthand();
    if (tags.contains(key)) {
        return false;
    }
    tags[key] = tag;
    return true;
}

QList<SWMulAlignResultNamesTag *> * SWMulAlignResultNamesTagsRegistry::getTagsWithCorrectOrder() const
{
    QList<SWMulAlignResultNamesTag *> *result = new QList<SWMulAlignResultNamesTag *>;
    qint16 tagIndex = 0;
    QString tagShorthand;
    foreach(SWMulAlignResultNamesTag * tag, tags.values()) {
        tagShorthand = tag->getShorthand();

        if(SEQ_NAME_PREFIX_TAG_SHORTHAND == tagShorthand)
            tagIndex = 0;
        else if(PTRN_NAME_PREFIX_TAG_SHORTHAND == tagShorthand)
            tagIndex = 1;
        else if(SUBSEQ_START_POS_TAG_SHORTHAND == tagShorthand)
            tagIndex = 2;
        else if(SUBSEQ_END_POS_TAG_SHORTHAND == tagShorthand)
            tagIndex = 3;
        else if(SUBSEQ_LENGTH_TAG_SHORTHAND == tagShorthand)
            tagIndex = 4;
        else if(COUNTER_TAG_SHORTHAND == tagShorthand)
            tagIndex = 5;
        else if(DATE_TAG_SHORTHAND == tagShorthand)
            tagIndex = 6;
        else if(TIME_TAG_SHORTHAND == tagShorthand)
            tagIndex = 7;
        else
            assert(0);

        result->insert(tagIndex, tag);
    }

    return result;
}

} // namespace
