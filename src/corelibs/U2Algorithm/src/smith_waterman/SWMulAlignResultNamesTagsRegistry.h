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

#ifndef _U2_SW_MUL_ALIGN_RESULT_NAMES_TAGS_REGISTRY_H_
#define _U2_SW_MUL_ALIGN_RESULT_NAMES_TAGS_REGISTRY_H_

#include <U2Core/global.h>
#include "SWMulAlignResultNamesTag.h"
#include "SWMulAlignExternalPropTag.h"
#include "SmithWatermanReportCallback.h"

#include <QBitArray>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QStringList>

const char OPEN_SQUARE_BRACKET = '[';
const char CLOSE_SQUARE_BRACKET = ']';
const char SHORTHAND_AND_LABEL_SEPARATOR = ' ';

extern const QString SEQ_NAME_PREFIX_TAG_SHORTHAND;
extern const QString PTRN_NAME_PREFIX_TAG_SHORTHAND;
extern const QString SUBSEQ_START_POS_TAG_SHORTHAND;
extern const QString SUBSEQ_END_POS_TAG_SHORTHAND;
extern const QString COUNTER_TAG_SHORTHAND;

namespace U2 {

class U2ALGORITHM_EXPORT SWMulAlignResultNamesTagsRegistry : public QObject {
    Q_OBJECT
public:
    SWMulAlignResultNamesTagsRegistry();
    inline ~SWMulAlignResultNamesTagsRegistry();

    // returned list isn't stored in the SWMulAlignResultNamesTagsRegistry instance, so the first one is considered
    inline QList<QPushButton *> * getTagsButtons() const; // to be deallocated outside

    // returned list isn't stored in the SWMulAlignResultNamesTagsRegistry instance, so the first one is considered
    inline QStringList * getDefaultTagsForMobjectsNames() const; // to be deallocated outside

    // returned list isn't stored in the SWMulAlignResultNamesTagsRegistry instance, so the first one is considered
    inline QStringList * getDefaultTagsForRefSubseqNames() const; // to be deallocated outside

    // returned list isn't stored in the SWMulAlignResultNamesTagsRegistry instance, so the first one is considered
    inline QStringList * getDefaultTagsForPtrnSubseqNames() const; // to be deallocated outside

    // returned list isn't stored in the SWMulAlignResultNamesTagsRegistry instance, so the first one is considered
    // to be deallocated outside; also if bit is set in returned QBitArray object then corresponding button
    // from result of getDefaultTagsForPtrnSubseqNames() method is considered to be used only with subsequence name fields
    inline QBitArray * getBitmapOfTagsApplicability() const;

    QString parseStringWithTags(const QString & str, const SmithWatermanReportCallbackMAImpl::TagExpansionPossibleData & expansionSet) const;

    // this method is supposed to be invoked before each string with counter tags parsing
    //if there's need to begin counting from 1
    inline void resetCounters();

private:
    bool registerTag(SWMulAlignResultNamesTag * tag);
    QString tagExpansion(const QString & shorthand, const QVariant & argument = QVariant()) const;
    QList<SWMulAlignResultNamesTag *> * getTagsWithCorrectOrder() const;

    QMutex mutex;
    QHash<const QString, SWMulAlignResultNamesTag *> tags;
};



inline QList<QPushButton *> * SWMulAlignResultNamesTagsRegistry::getTagsButtons() const
{
    QList<QPushButton *> * tagsButtons = new QList<QPushButton *>;
    QList<SWMulAlignResultNamesTag *> *arrangedTags = getTagsWithCorrectOrder();

    foreach(SWMulAlignResultNamesTag * tag, *arrangedTags) {
        QPushButton * button = new QPushButton(OPEN_SQUARE_BRACKET + tag->getShorthand() +
            CLOSE_SQUARE_BRACKET + SHORTHAND_AND_LABEL_SEPARATOR + tag->getLabel());
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        tagsButtons->append(button);
    }
    delete arrangedTags;

    return tagsButtons;
}

inline SWMulAlignResultNamesTagsRegistry::~SWMulAlignResultNamesTagsRegistry()
{
    foreach(SWMulAlignResultNamesTag * tag, tags.values()) {
        delete tag;
    }
}

inline QBitArray * SWMulAlignResultNamesTagsRegistry::getBitmapOfTagsApplicability() const
{
    QBitArray * bitmap = new QBitArray(tags.count());
    quint8 counter = 0;

    foreach(SWMulAlignResultNamesTag * tag, tags.values()) {
        if(tag->isAcceptableForSubseqNamesOnly()) {
            bitmap->setBit(counter);
        }
        ++counter;
    }

    return bitmap;
}

inline void SWMulAlignResultNamesTagsRegistry::resetCounters()
{
    foreach(SWMulAlignResultNamesTag * tag, tags.values()) {
        SWMulAlignExternalPropTag * externalPropertyTag = dynamic_cast<SWMulAlignExternalPropTag *>(tag);
        if(NULL != externalPropertyTag) {
            if(SWMulAlignExternalPropTag::COUNTER == externalPropertyTag->getType()) {
                externalPropertyTag->resetCounter();
            }
        }
    }
}

inline QStringList * SWMulAlignResultNamesTagsRegistry::getDefaultTagsForMobjectsNames() const
{
    QStringList * result = new QStringList();
    result->append(PTRN_NAME_PREFIX_TAG_SHORTHAND);
    result->append(SEQ_NAME_PREFIX_TAG_SHORTHAND);
    result->append(COUNTER_TAG_SHORTHAND);

    return result;
}

inline QStringList * SWMulAlignResultNamesTagsRegistry::getDefaultTagsForRefSubseqNames() const
{
    QStringList * result = new QStringList();
    result->append(SEQ_NAME_PREFIX_TAG_SHORTHAND);
    result->append(SUBSEQ_START_POS_TAG_SHORTHAND);
    result->append(SUBSEQ_END_POS_TAG_SHORTHAND);

    return result;
}

inline QStringList * SWMulAlignResultNamesTagsRegistry::getDefaultTagsForPtrnSubseqNames() const
{
    QStringList * result = new QStringList();
    result->append(PTRN_NAME_PREFIX_TAG_SHORTHAND);
    result->append(SUBSEQ_START_POS_TAG_SHORTHAND);
    result->append(SUBSEQ_END_POS_TAG_SHORTHAND);

    return result;
}

} // namespace

#endif // _U2_SW_MUL_ALIGN_RESULT_NAMES_TAGS_REGISTRY_H_
