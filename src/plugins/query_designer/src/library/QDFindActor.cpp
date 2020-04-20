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

#include "QDFindActor.h"

#include <U2Core/Log.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/FailTask.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/U2AlphabetUtils.h>

#include <U2Lang/BaseTypes.h>


namespace U2 {

static const QString PATTERN_ATTR("pattern");

QDFindActor::QDFindActor(QDActorPrototype const* proto) : QDActor(proto) {
    units["find"] = new QDSchemeUnit(this);
}

int QDFindActor::getMinResultLen() const {
    return cfg->getParameter(PATTERN_ATTR)->getAttributeValueWithoutScript<QString>().length();
}

int QDFindActor::getMaxResultLen() const {
    return getMinResultLen();
}

QString QDFindActor::getText() const {
    QString hypPtrn = cfg->getParameter(PATTERN_ATTR)->getAttributeValueWithoutScript<QString>().toUpper();

    if (hypPtrn.isEmpty()) {
        hypPtrn = QString("&lt;<a href=%1>pattern</a>&gt;").arg(PATTERN_ATTR);
    }
    else {
        hypPtrn = QString("<a href=%1>%2</a>")
            .arg(PATTERN_ATTR)
            .arg(hypPtrn);
    }

    return hypPtrn;
}

Task* QDFindActor::getAlgorithmTask(const QVector<U2Region>& location) {
    Task* t = new Task(tr("Find"), TaskFlag_NoRun);
    settings.sequence = scheme->getSequence().seq;
    settings.pattern = cfg->getParameter(PATTERN_ATTR)->getAttributeValueWithoutScript<QString>().toLatin1().toUpper();
    settings.maxResult2Find = FindAlgorithmSettings::MAX_RESULT_TO_FIND_UNLIMITED;

    switch(getStrandToRun()) {
        case QDStrand_Both:
            settings.strand = FindAlgorithmStrand_Both;
            break;
        case QDStrand_DirectOnly:
            settings.strand = FindAlgorithmStrand_Direct;
            break;
        case QDStrand_ComplementOnly:
            settings.strand = FindAlgorithmStrand_Complement;
            break;
    }

    if (settings.strand != FindAlgorithmStrand_Direct) {
        DNATranslation* compTT = NULL;
        if (scheme->getSequence().alphabet->isNucleic()) {
            compTT = AppContext::getDNATranslationRegistry()->
                lookupComplementTranslation(scheme->getSequence().alphabet);
        }
        if (compTT  != NULL) {
            settings.complementTT = compTT ;
        } else {
            QString err = tr("%1: can not find complement translation.").arg(getParameters()->getLabel());
            return new FailTask(err);
        }
    }

    if (settings.pattern.isEmpty()) {
        QString err = tr("%1: pattern is empty.").arg(getParameters()->getLabel());
        return new FailTask(err);
    }

    const DNAAlphabet* ptrnAl = U2AlphabetUtils::findBestAlphabet(settings.pattern);
    if (ptrnAl->getType()!=DNAAlphabet_NUCL) {
        QString err = tr("%1: pattern has to be nucleic").arg(getParameters()->getLabel());
        return new FailTask(err);
    }

    foreach(const U2Region& r, location) {
        FindAlgorithmTaskSettings s(settings);
        s.searchRegion = r;
        FindAlgorithmTask* findTask = new FindAlgorithmTask(s);
        connect(new TaskSignalMapper(findTask), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onFindTaskFinished(Task*)));
        t->addSubTask(findTask);
    }
    return t;
}

void QDFindActor::sl_onFindTaskFinished(Task* t) {
    FindAlgorithmTask* findTask = qobject_cast<FindAlgorithmTask*>(t);
    QList<FindAlgorithmResult> res = findTask->popResults();
    foreach(const FindAlgorithmResult& r, res) {
        QDResultUnit ru(new QDResultUnitData);
        ru->strand = r.strand;
        ru->region = r.region;
        ru->owner = units.value("find");
        QDResultGroup* g = new QDResultGroup(QDStrand_DirectOnly);
        g->add(ru);
        results.append(g);
    }
}

QDFindActorPrototype::QDFindActorPrototype() {
    descriptor.setId("search");
    descriptor.setDisplayName(QDFindActor::tr("Pattern"));
    descriptor.setDocumentation(QDFindActor::tr("Finds pattern."));

    Descriptor pd(PATTERN_ATTR, QDFindActor::tr("Pattern"), QDFindActor::tr("A subsequence pattern to look for."));
    attributes << new Attribute(pd, BaseTypes::STRING_TYPE(), true);
}

}//namespace
