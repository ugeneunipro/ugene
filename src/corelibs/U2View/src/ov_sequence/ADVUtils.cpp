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

#include "ADVUtils.h"
#include "AnnotatedDNAView.h"
#include "ADVSingleSequenceWidget.h"
#include "ADVSequenceObjectContext.h"


namespace U2 {

ADVGlobalAction::ADVGlobalAction(AnnotatedDNAView* v, const QIcon& icon, const QString& text, int ps, ADVGlobalActionFlags fl)
: GObjectViewAction(v, v, text), pos(ps), flags(fl)
{
    setIcon(icon);
    connect(v, SIGNAL(si_focusChanged(ADVSequenceWidget*, ADVSequenceWidget*)), SLOT(sl_focusChanged(ADVSequenceWidget*, ADVSequenceWidget*)));
    updateState();
    v->addADVAction(this);
}

void ADVGlobalAction::sl_focusChanged(ADVSequenceWidget* fromW, ADVSequenceWidget* toW) {
    Q_UNUSED(fromW); Q_UNUSED(toW);
    updateState();
}

void ADVGlobalAction::updateState() {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(getObjectView());
    ADVSequenceWidget* w = av->getSequenceWidgetInFocus();
    bool enabled = w!=NULL;
    if (enabled && flags.testFlag(ADVGlobalActionFlag_SingleSequenceOnly) && qobject_cast<ADVSingleSequenceWidget*>(w) == NULL) {
        enabled = false;
    }
    if (enabled && !alphabetFilter.isEmpty()) {
        DNAAlphabetType t = w->getActiveSequenceContext()->getAlphabet()->getType();
        enabled = alphabetFilter.contains(t);
    }
    setEnabled(enabled);
}


QString ADVSelectionUtils::getSequenceIdsFromSelection(const QList<Annotation*> &selection, bool localBase) {
    QStringList genbankID ;
    foreach (const Annotation* ann, selection) {
        QString tmp = ann->findFirstQualifierValue("id");
        if (!tmp.isEmpty()) {
            if (!localBase) {
                int off = tmp.indexOf("|");
                int off1 = tmp.indexOf("|", off + 1);
                tmp = tmp.mid(off + 1, off1 - off - 1);
            }
            if (!genbankID.contains(tmp)) {
                genbankID.append(tmp);
            }
        }
    }
    return genbankID.join(",");
}

} //namespace
