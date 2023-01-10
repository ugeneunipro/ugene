/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "AnnotatedDNAView.h"

namespace U2 {

ADVGlobalAction::ADVGlobalAction(AnnotatedDNAView* v, const QIcon& icon, const QString& text, int ps, ADVGlobalActionFlags fl)
    : GObjectViewAction(v, v, text), pos(ps), flags(fl) {
    setIcon(icon);
    connect(v, SIGNAL(si_activeSequenceWidgetChanged(ADVSequenceWidget*, ADVSequenceWidget*)), SLOT(sl_activeSequenceChanged()));
    updateState();
    v->addADVAction(this);
}

void ADVGlobalAction::sl_activeSequenceChanged() {
    updateState();
}

void ADVGlobalAction::updateState() {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(getObjectView());
    ADVSequenceWidget* w = av->getActiveSequenceWidget();
    bool enabled = w != nullptr;
    if (enabled && flags.testFlag(ADVGlobalActionFlag_SingleSequenceOnly) && qobject_cast<ADVSingleSequenceWidget*>(w) == nullptr) {
        enabled = false;
    }
    if (enabled && !alphabetFilter.isEmpty()) {
        DNAAlphabetType t = w->getActiveSequenceContext()->getAlphabet()->getType();
        enabled = alphabetFilter.contains(t);
    }
    setEnabled(enabled);
}

}  // namespace U2
