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

#include "WMQuery.h"
#include "WeightMatrixIO.h"

#include <U2Designer/DelegateEditors.h>
#include <U2Lang/BaseTypes.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/FailTask.h>
#include <U2Core/DNAAlphabet.h>

#include <U2Gui/DialogUtils.h>

#include <QApplication>

namespace U2 {

/************************************************************************/
/* WMQDTask                                                             */
/************************************************************************/

WMQDTask::WMQDTask(const QString& url, const WeightMatrixSearchCfg& cfg,
                   const DNASequence&  sqnc, const QString& resName, const QVector<U2Region>& location)
: Task(tr("Weight matrix query"), TaskFlag_NoRun), settings(cfg), dnaSeq(sqnc),
resultName(resName), location(location)
{
    readTask = new PWMatrixReadTask(url);
    addSubTask(readTask);
}

QList<Task*> WMQDTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subtasks;
    if (subTask==readTask) {
        PWMatrix model = readTask->getResult();
        foreach(const U2Region& r, location) {
            QByteArray seqBlock = dnaSeq.seq.mid(r.startPos, r.length);
            subtasks << new WeightMatrixSingleSearchTask(model,
                seqBlock,
                settings,
                r.startPos);
        }
    }
    else {
        WeightMatrixSingleSearchTask* t = qobject_cast<WeightMatrixSingleSearchTask*>(subTask);
        assert(t);
        res << t->takeResults();
    }
    return subtasks;
}

QList<WeightMatrixSearchResult> WMQDTask::takeResults() {
    QList<WeightMatrixSearchResult> results = res;
    res.clear();
    return results;
}

/************************************************************************/
/* QDWMActor                                                            */
/************************************************************************/

static const QString SCORE_ATTR = "min-score";
static const QString PROFILE_URL_ATTR = "matrix";

QDWMActor::QDWMActor(QDActorPrototype const* proto) : QDActor(proto) {
    units["wm"] = new QDSchemeUnit(this);
}

int QDWMActor::getMinResultLen() const {
    return 1;
}

int QDWMActor::getMaxResultLen() const {
    if (!scheme->getSequence().isNull()) {
        return scheme->getSequence().length();
    }
    return 30;//FIX ME: supply reasonable value
}

QString QDWMActor::getText() const {
    QMap<QString, Attribute*> params = cfg->getParameters();

    QString strandName;
    switch (strand) {
    case QDStrand_Both: strandName = tr("both strands"); break;
    case QDStrand_DirectOnly: strandName = tr("direct strand"); break;
    case QDStrand_ComplementOnly: strandName = tr("complement strand"); break;
    }

    QString profileUrl = params.value(PROFILE_URL_ATTR)->getAttributeValueWithoutScript<QString>();
    if (profileUrl.isEmpty()) {
        profileUrl = "unset";
    }
    profileUrl = QString("<a href=%1>%2</a>").arg(PROFILE_URL_ATTR).arg(profileUrl);
    int score = params.value(SCORE_ATTR)->getAttributeValueWithoutScript<int>();
    QString scoreStr = QString("<a href=%1>%2%</a>").arg(SCORE_ATTR).arg(score);

    QString doc = tr("Searches TFBS with all profiles from <u>%1</u> "
        "<br> Recognizes sites with <u>similarity %2</u>, processes <u>%3</u>.")
        .arg(profileUrl)
        .arg(scoreStr)
        .arg(strandName);

    return doc;
}

Task* QDWMActor::getAlgorithmTask(const QVector<U2Region>& location) {
    Task* t = NULL;
    const DNASequence& dnaSeq = scheme->getSequence();
    QMap<QString, Attribute*> params = cfg->getParameters();

    WeightMatrixSearchCfg config;
    config.minPSUM = params.value(SCORE_ATTR)->getAttributeValueWithoutScript<int>();
    const QString& modelUrl = params.value(PROFILE_URL_ATTR)->getAttributeValueWithoutScript<QString>();

    if (dnaSeq.alphabet->getType() == DNAAlphabet_NUCL) {
        config.complOnly = strand == QDStrand_ComplementOnly;
        if (strand == QDStrand_Both || strand == QDStrand_ComplementOnly) {
            DNATranslation* compTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dnaSeq.alphabet);
            if (compTT  != NULL) {
                config.complTT = compTT;
            }
        }
        t = new WMQDTask(modelUrl, config, dnaSeq, "", location);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onAlgorithmTaskFinished(Task*)));
    } else {
        QString err = tr("%1: sequence should be nucleic.").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    return t;
}

void QDWMActor::sl_onAlgorithmTaskFinished(Task* t) {
    WMQDTask* wmqdt = qobject_cast<WMQDTask*>(t);
    assert(wmqdt);
    QList<WeightMatrixSearchResult> res = wmqdt->takeResults();
    foreach(const WeightMatrixSearchResult& r, res) {
        QDResultUnit ru(new QDResultUnitData);
        ru->strand = r.strand;
        QMapIterator<QString,QString> it(r.qual);
        while (it.hasNext()) {
            it.next();
            ru->quals.append(U2Qualifier(it.key(), it.value()));
        }
        ru->region = r.region;
        ru->owner = units.value("wm");
        QDResultGroup::buildGroupFromSingleResult(ru, results);
    }
}

/************************************************************************/
/* Factory                                                              */
/************************************************************************/

QDWMActorPrototype::QDWMActorPrototype() {
    descriptor.setId("wsearch");
    descriptor.setDisplayName(QObject::tr("Weight Matrix"));
    descriptor.setDocumentation(QObject::tr("Searches the sequence for transcription factor binding sites significantly similar to the specified weight matrix."));

    Descriptor scd(SCORE_ATTR, QObject::tr("Min score"), QApplication::translate("PWMSearchDialog", "Minimum score to detect transcription factor binding site", 0));
    Descriptor mx(PROFILE_URL_ATTR, QObject::tr("Matrix"), QObject::tr("Path to profile"));

    attributes << new Attribute(scd, BaseTypes::NUM_TYPE(), false, 85);
    attributes << new Attribute(mx, BaseTypes::STRING_TYPE(), true);

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m; m["minimum"] = 1; m["maximum"] = 100; m["suffix"] = "%";
        delegates[SCORE_ATTR] = new SpinBoxDelegate(m);
    }

    delegates[PROFILE_URL_ATTR] = new URLDelegate(WeightMatrixIO::getPWMFileFilter(), WeightMatrixIO::WEIGHT_MATRIX_ID, true);

    editor = new DelegateEditor(delegates);
}

}//namespace
