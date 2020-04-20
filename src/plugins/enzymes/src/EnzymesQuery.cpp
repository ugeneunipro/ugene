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

#include "EnzymesQuery.h"
#include "EnzymesIO.h"
#include "FindEnzymesTask.h"

#include <U2Core/TaskSignalMapper.h>
#include <U2Core/Log.h>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <QCoreApplication>
#include <QDir>

#include <U2Lang/BaseTypes.h>

#include <QInputDialog>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>

namespace U2 {

/************************************************************************/
/* QDEnzymesActor                                                       */
/************************************************************************/

static const QString ENZYMES_ATTR = "enzymes";
static const QString CIRC_ATTR = "circular";
static const QString MIN_ATTR = "min";
static const QString MAX_ATTR = "max";

QDEnzymesActor::QDEnzymesActor(QDActorPrototype const* proto) : QDActor(proto) {
    selectorFactory = NULL;
    cfg->setAnnotationKey("<rsite>");
    units["enzyme"] = new QDSchemeUnit(this);
}

QString QDEnzymesActor::getText() const {
    return tr("Find enzymes");
}

Task* QDEnzymesActor::getAlgorithmTask(const QVector<U2Region>& location) {
    Task* t = NULL;

    bool circular = cfg->getParameter(CIRC_ATTR)->getAttributePureValue().toBool();

    assert(!location.isEmpty());
    t = new Task(tr("Enzymes query"), TaskFlag_NoRun);

    QList<SEnzymeData> enzymes;
    QString s = cfg->getParameter(ENZYMES_ATTR)->getAttributePureValue().toString();
    ids = s.split(QRegExp("\\s*,\\s*"));
    const QList<SEnzymeData>& loadedEnzymes = EnzymesSelectorWidget::getLoadedEnzymes();
    foreach(const SEnzymeData& d, loadedEnzymes) {
        if (ids.contains(d->id)) {
            enzymes.append(d);
        }
    }

    foreach(const U2Region& r, location) {
        FindEnzymesTask* st = new FindEnzymesTask(scheme->getEntityRef(), r, enzymes, INT_MAX, circular);
        t->addSubTask(st);
        enzymesTasks.append(st);
    }
    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onAlgorithmTaskFinished()));
    return t;
}

void QDEnzymesActor::sl_onAlgorithmTaskFinished() {
    foreach (FindEnzymesTask *st, enzymesTasks) {
        foreach (const QString &id, ids) {
            QList<SharedAnnotationData> dataz = st->getResultsAsAnnotations(id);
            foreach (const SharedAnnotationData &ad, dataz) {
                QDResultUnit ru(new QDResultUnitData);
                ru->strand = ad->getStrand();
                ru->quals = ad->qualifiers;
                ru->quals.append(U2Qualifier("id", id));
                ru->region = ad->location->regions[0];
                ru->owner = units.value("enzyme");
                QDResultGroup *g = new QDResultGroup(QDStrand_Both);
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

    EnzymesSelectorDialogHandler* f = new EnzymesSelectorDialogHandler;
    delegates[ENZYMES_ATTR] = new StringSelectorDelegate("", f);

    editor = new DelegateEditor(delegates);
}

/************************************************************************/
/* EnzymesSelectorDialogHandler                                         */
/************************************************************************/
QString EnzymesSelectorDialogHandler::getSelectedString(QDialog* dlg) {
    EnzymesSelectorDialog* enzDlg = qobject_cast<EnzymesSelectorDialog*>(dlg);
    assert(enzDlg);
    return enzDlg->getSelectedString();
}

/************************************************************************/
/* EnzymesSelectorDialog                                                */
/************************************************************************/

EnzymesSelectorDialog::EnzymesSelectorDialog(EnzymesSelectorDialogHandler* parent)
: factory(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748951");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QVBoxLayout* vl = new QVBoxLayout();
    enzSel = new EnzymesSelectorWidget();
    vl->setMargin(0);
    vl->addWidget(enzSel);
    enzymesSelectorWidget->setLayout(vl);
    enzymesSelectorWidget->setMinimumSize(enzSel->size());

}

QString EnzymesSelectorDialog::getSelectedString() const {
    QString res;
    const QList<SEnzymeData>& enzymes = enzSel->getSelectedEnzymes();
    foreach(const SEnzymeData& enzyme, enzymes) {
        res+=enzyme->id + ',';
    }
    res.remove(res.length()-1, 1);
    return res;
}

}//namespace
