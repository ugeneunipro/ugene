/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "CustomAutoAnnotationDialog.h"

#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/AutoAnnotationUtils.h>

#include "CustomPatternAnnotationTask.h"

namespace U2 {

CustomAutoAnnotationDialog::CustomAutoAnnotationDialog(ADVSequenceObjectContext* ctx)
    : QDialog(ctx->getAnnotatedDNAView()->getWidget()), seqCtx(ctx) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930930");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Annotate"));
    connect(pbAll, &QAbstractButton::clicked, this, [this]() {
        setCheckboxesStates(CheckType::All);
    });
    connect(pbNone, &QAbstractButton::clicked, this, [this]() {
        setCheckboxesStates(CheckType::None);
    });
    connect(pbInvert, &QAbstractButton::clicked, this, [this]() {
        setCheckboxesStates(CheckType::Invert);
    });

    loadSettings();
}

void CustomAutoAnnotationDialog::setCheckboxesStates(CheckType checkType) {
    auto allChildren = groupBox->children();
    for (auto child : qAsConst(allChildren)) {
        auto checkBox = qobject_cast<QCheckBox*>(child);
        CHECK_CONTINUE(checkBox != nullptr);

        switch (checkType) {
        case CheckType::All:
            checkBox->setChecked(true);
            break;
        case CheckType::None:
            checkBox->setChecked(false);
            break;
        case CheckType::Invert:
            checkBox->setChecked(!checkBox->isChecked());
            break;
        }
    }
}

void CustomAutoAnnotationDialog::loadSettings() {
    QStringList filteredFeatures = AppContext::getSettings()->getValue(FILTERED_FEATURE_LIST, QStringList()).toStringList();

    promotersBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::PROMOTER));
    terminatorBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::TERMINATOR));
    regulatoryBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::REGULATORY_SEQUENCE));
    replicationBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::REPLICATION_ORIGIN));
    selectableBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::SELECTABLE_MARKER));
    reporterGeneBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::REPORTER_GENE));
    twoHybridGeneBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::TWO_HYBRID_GENE));
    localizationBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::LOCALIZATION_SEQUENCE));
    affinityBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::AFFINITY_TAG));
    geneBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::GENE));
    primerBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::PRIMER));
    miscellaneousBox->setChecked(!filteredFeatures.contains(PlasmidFeatureTypes::MISCELLANEOUS));
}

void CustomAutoAnnotationDialog::saveSettings() {
    QStringList filteredFeatures;

    if (!promotersBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::PROMOTER);
    }
    if (!terminatorBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::TERMINATOR);
    }
    if (!regulatoryBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::REGULATORY_SEQUENCE);
    }
    if (!replicationBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::REPLICATION_ORIGIN);
    }
    if (!selectableBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::SELECTABLE_MARKER);
    }
    if (!reporterGeneBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::REPORTER_GENE);
    }
    if (!twoHybridGeneBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::TWO_HYBRID_GENE);
    }
    if (!localizationBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::LOCALIZATION_SEQUENCE);
    }
    if (!affinityBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::AFFINITY_TAG);
    }
    if (!geneBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::GENE);
    }
    if (!primerBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::PRIMER);
    }
    if (!miscellaneousBox->isChecked()) {
        filteredFeatures.append(PlasmidFeatureTypes::MISCELLANEOUS);
    }

    AppContext::getSettings()->setValue(FILTERED_FEATURE_LIST, filteredFeatures);
}

void CustomAutoAnnotationDialog::accept() {
    saveSettings();
    AutoAnnotationUtils::triggerAutoAnnotationsUpdate(seqCtx, PLASMID_FEATURES_GROUP_NAME);

    QDialog::accept();
}

}  // namespace U2
