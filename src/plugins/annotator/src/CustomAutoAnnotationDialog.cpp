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

#include <QPushButton>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/AutoAnnotationUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Gui/HelpButton.h>

#include "CustomPatternAnnotationTask.h"
#include "CustomAutoAnnotationDialog.h"

namespace U2 {

CustomAutoAnnotationDialog::CustomAutoAnnotationDialog(ADVSequenceObjectContext* ctx)
 : QDialog(ctx->getAnnotatedDNAView()->getWidget()), seqCtx(ctx)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749017");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Annotate"));

    loadSettings();
}

void CustomAutoAnnotationDialog::loadSettings() {

    QStringList filteredFeatures = AppContext::getSettings()->getValue(FILTERED_FEATURE_LIST, QStringList() ).toStringList();

    featureBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::FEATURE));
    genesBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::GENE));
    originBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::ORIGIN));
    primerBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::PRIMER));
    promotersBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::PROMOTER));
    regulatoryBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::REGULATORY));
    terminatorBox->setChecked( !filteredFeatures.contains(PlasmidFeatureTypes::TERMINATOR));
}

void CustomAutoAnnotationDialog::saveSettings() {

    QStringList filteredFeatures;

    if (!featureBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::FEATURE);
    }
    if (!genesBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::GENE);
    }
    if (!originBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::ORIGIN);
    }
    if (!primerBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::PRIMER);
    }
    if (!promotersBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::PROMOTER);
    }
    if (!regulatoryBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::REGULATORY);
    }
    if (!terminatorBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::TERMINATOR);
    }

    AppContext::getSettings()->setValue(FILTERED_FEATURE_LIST, filteredFeatures );

}


void CustomAutoAnnotationDialog::accept() {

    saveSettings();
    AutoAnnotationUtils::triggerAutoAnnotationsUpdate(seqCtx, PLASMID_FEATURES_GROUP_NAME);

    QDialog::accept();
}

}//namespace
