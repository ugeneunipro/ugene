/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "FindEnzymesDialog.h"

#include <QDialogButtonBox>
#include <QMessageBox>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Gui/HelpButton.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AutoAnnotationUtils.h>

#include "EnzymesSelectorWidget.h"
#include "FindEnzymesTask.h"
#include "ResultsCountFilter.h"
#include "RegionSelectorWithExclude.h"

namespace U2 {

FindEnzymesDialog::FindEnzymesDialog(QWidget* parent, const QPointer<ADVSequenceObjectContext>& _advSequenceContext)
    : QDialog(parent), advSequenceContext(_advSequenceContext) {

    initTitleAndLayout();
    initEnzymesSelectorWidget();
    initResultsCountFilter();
    initRegionSelectorWithExclude();
    initDialogButtonBox();
}

void FindEnzymesDialog::accept() {
    if (advSequenceContext.isNull()) {
        QMessageBox::critical(this, tr("Error!"), tr("Sequence has been alredy closed."));
        return;
    }

    QList<SEnzymeData> selectedEnzymes = enzSel->getSelectedEnzymes();
    bool ok = false;
    U2Location searchLocation = regionSelector->getRegionSelectorLocation(&ok);
    U2Location excludeLocation;
    if (regionSelector->isExcludeCheckboxChecked()) {
        bool prevOk = ok;
        excludeLocation = regionSelector->getExcludeRegionSelectorLocation(&ok);
        ok = prevOk && ok;
    }
    if (!ok) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("Invalid 'Search in' or 'Uncut' region/location!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(tr("Given region or genbank location is invalid, please correct it."));
        msgBox->exec();
        CHECK(!msgBox.isNull(), );
        return;
    }

    QVector<U2Region> searchRegionsOutsideExcluded = searchLocation.data()->regions;
    for (const U2Region& excludedRegion : qAsConst(excludeLocation.data()->regions)) {
        for (const U2Region& searchRegion : qAsConst(searchLocation.data()->regions)) {
            if (excludedRegion.contains(searchRegion)) {
                searchRegionsOutsideExcluded.removeAll(searchRegion);
            }
        }
    }
    if (searchRegionsOutsideExcluded.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("'Uncut' region/location fully contains 'Search in' inside it!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(tr("Nowhere to search!"));
        msgBox->exec();
        CHECK(!msgBox.isNull(), );
        return;
    }

    if (selectedEnzymes.isEmpty()) {
        int ret = QMessageBox::question(this,
                                        windowTitle(),
                                        tr("<html><body align=\"center\">No enzymes are selected! Do you want to turn off <br>enzymes annotations highlighting?</body></html>"),
                                        QMessageBox::Yes,
                                        QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            QAction* toggleAction = AutoAnnotationUtils::findAutoAnnotationsToggleAction(advSequenceContext.data(), ANNOTATION_GROUP_ENZYME);
            if (toggleAction) {
                toggleAction->setChecked(false);
            }
            saveSettings();
            QDialog::accept();
        }
        return;
    }

    if (FindEnzymesAutoAnnotationUpdater::isTooManyAnnotationsInTheResult(advSequenceContext->getSequenceLength(), selectedEnzymes.size())) {
        QString message = tr("Too many results to render. Please reduce the search region or number of selected enzymes.");
        QMessageBox::critical(this, tr("Error!"), message, QMessageBox::Ok);
        return;
    }

    saveSettings();

    AutoAnnotationUtils::triggerAutoAnnotationsUpdate(advSequenceContext.data(), ANNOTATION_GROUP_ENZYME);

    QDialog::accept();
}

void FindEnzymesDialog::initTitleAndLayout() {
    setObjectName("FindEnzymesDialog");
    setWindowTitle(tr("Find Restriction Sites"));

    auto layout = new QVBoxLayout(this);
    layout->setSpacing(0);
}

void FindEnzymesDialog::initEnzymesSelectorWidget() {
    enzSel = new EnzymesSelectorWidget(this, advSequenceContext);
    enzSel->setObjectName("enzymesSelectorWidget");
    layout()->addWidget(enzSel);
}

void FindEnzymesDialog::initResultsCountFilter() {
    SAFE_POINT_NN(advSequenceContext.data(), );

    countFilter = new ResultsCountFilter(this);
    layout()->addWidget(countFilter);
}

void FindEnzymesDialog::initRegionSelectorWithExclude() {
    SAFE_POINT_NN(advSequenceContext.data(), );

    regionSelector = new RegionSelectorWithExclude(this, advSequenceContext);
    layout()->addWidget(regionSelector);
}

void FindEnzymesDialog::saveSettings() {
    enzSel->saveSettings();
    countFilter->saveSettings();
    regionSelector->saveSettings();
}

void FindEnzymesDialog::initDialogButtonBox() {
    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName("buttonBox");
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Ok);
    new HelpButton(this, buttonBox, "65930747");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    layout()->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &FindEnzymesDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &FindEnzymesDialog::reject);
}

}  // namespace U2
