/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "EnzymesQuery.h"

#include <QCoreApplication>
#include <QInputDialog>

#include <U2Core/TaskSignalMapper.h>

#include <U2Gui/HelpButton.h>

#include <U2Lang/BaseTypes.h>

#include "enzymes_dialog/FindEnzymesDialogQueryDesigner.h"
#include "EnzymesIO.h"
#include "FindEnzymesTask.h"

namespace U2 {

/************************************************************************/
/* QDEnzymesActor                                                       */
/************************************************************************/

static const QString ENZYMES_ATTR = "enzymes";
static const QString CIRC_ATTR = "circular";
static const QString MIN_ATTR = "min";
static const QString MAX_ATTR = "max";

QDEnzymesActor::QDEnzymesActor(QDActorPrototype const* proto)
    : QDActor(proto) {
    selectorFactory = nullptr;
    cfg->setAnnotationKey("<rsite>");
    units["enzyme"] = new QDSchemeUnit(this);
}

QString QDEnzymesActor::getText() const {
    return tr("Find enzymes");
}

Task* QDEnzymesActor::getAlgorithmTask(const QVector<U2Region>& location) {
    Task* t = nullptr;

    bool circular = cfg->getParameter(CIRC_ATTR)->getAttributePureValue().toBool();

    assert(!location.isEmpty());
    t = new Task(tr("Enzymes query"), TaskFlag_NoRun);

    QList<SEnzymeData> enzymes;
    QString s = cfg->getParameter(ENZYMES_ATTR)->getAttributePureValue().toString();
    ids = s.split(QRegExp("\\s*,\\s*"));
    const QList<SEnzymeData>& loadedEnzymes = EnzymesSelectorWidget::getLoadedEnzymes();
    foreach (const SEnzymeData& d, loadedEnzymes) {
        if (ids.contains(d->id)) {
            enzymes.append(d);
        }
    }

    foreach (const U2Region& r, location) {
        auto st = new FindEnzymesTask(scheme->getEntityRef(), r, {}, enzymes, INT_MAX, circular);
        t->addSubTask(st);
        enzymesTasks.append(st);
    }
    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onAlgorithmTaskFinished()));
    return t;
}

void QDEnzymesActor::sl_onAlgorithmTaskFinished() {
    for (FindEnzymesTask* st : qAsConst(enzymesTasks)) {
        foreach (const QString& id, ids) {
            QList<SharedAnnotationData> resultAnnotations = st->getResultsAsAnnotations(id);
            for (const SharedAnnotationData& ad : qAsConst(resultAnnotations)) {
                QDResultUnit ru(new QDResultUnitData);
                ru->strand = ad->getStrand();
                ru->quals = ad->qualifiers;
                ru->quals.append(U2Qualifier("id", id));
                ru->region = ad->location->regions[0];
                ru->owner = units.value("enzyme");
                auto g = new QDResultGroup(QDStrand_Both);
                g->add(ru);
                results.append(g);
            }
        }
    }
    enzymesTasks.clear();
    ids.clear();
}

QDEnzymesActorPrototype::QDEnzymesActorPrototype() {
    descriptor.setId("rsite");
    descriptor.setDisplayName(QDEnzymesActor::tr("Restriction Sites"));
    descriptor.setDocumentation(QDEnzymesActor::tr("Finds restriction cut sites in supplied DNA sequence."));

    Descriptor ed(ENZYMES_ATTR, QDEnzymesActor::tr("Enzymes"), QDEnzymesActor::tr("Restriction enzymes used to recognize the restriction sites."));
    Descriptor cd(CIRC_ATTR, QDEnzymesActor::tr("Circular"), QDEnzymesActor::tr("If <i>True</i> considers the sequence circular. That allows one to search for restriction sites between the end and the beginning of the sequence."));

    attributes << new Attribute(ed, BaseTypes::STRING_TYPE(), false);
    attributes << new Attribute(cd, BaseTypes::BOOL_TYPE(), false, false);

    QMap<QString, PropertyDelegate*> delegates;

    auto f = new EnzymesSelectorDialogHandler;
    delegates[ENZYMES_ATTR] = new StringSelectorDelegate("", f);

    editor = new DelegateEditor(delegates);
}

/************************************************************************/
/* EnzymesSelectorDialogHandler                                         */
/************************************************************************/

QDialog* EnzymesSelectorDialogHandler::createSelectorDialog(const QString&) {
    return new FindEnzymesDialogQueryDesigner();
}

QString EnzymesSelectorDialogHandler::getSelectedString(QDialog* dlg) {
    auto enzDlg = qobject_cast<FindEnzymesDialogQueryDesigner*>(dlg);
    SAFE_POINT_NN(enzDlg, QString());

    return enzDlg->getSelectedString();
}

}  // namespace U2
