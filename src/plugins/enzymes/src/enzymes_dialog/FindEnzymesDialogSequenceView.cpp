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

#include "FindEnzymesDialogSequenceView.h"

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AutoAnnotationUtils.h>

#include <QMessageBox>

#include "EnzymesSelectorWidget.h"
#include "FindEnzymesTask.h"
#include "ResultsCountFilter.h"
#include "RegionSelectorWithExclude.h"

namespace U2 {

FindEnzymesDialogSequenceView::FindEnzymesDialogSequenceView(QWidget* parent, const QPointer<ADVSequenceObjectContext>& _advSequenceContext)
    : FindEnzymesDialogBase(parent), advSequenceContext(_advSequenceContext) {

    initTitleAndLayout();
    initEnzymesSelectorWidget();
    enzSel->setSequenceContext(advSequenceContext);
    initResultsCountFilter();
    initRegionSelectorWithExclude();
    initDialogButtonBox();
}

bool FindEnzymesDialogSequenceView::acceptProtected() {
    if (advSequenceContext.isNull()) {
        QMessageBox::critical(this, tr("Error!"), tr("Sequence has been alredy closed."));
        return false;
    }

    QList<SEnzymeData> selectedEnzymes = enzSel->getSelectedEnzymes();
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
            return true;
        } else {
            return false;
        }
    }

    bool ok = false;
    U2Location searchLocation = regionSelector->getRegionSelectorLocation(&ok);
    U2Location excludeLocation;
    if (regionSelector->isExcludeCheckboxChecked()) {
        bool prevOk = ok;
        excludeLocation = regionSelector->getExcludeRegionSelectorLocation(&ok);
        ok = prevOk && ok;
    }
    if (!ok) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("Invalid 'Select sequence region' or 'Uncut area'!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(tr("Given region or genbank location is invalid, please correct it."));
        msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        return false;
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
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("'Uncut area' fully contains 'Select sequence region' inside it!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(tr("Nowhere to search!"));
        msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        return false;
    }

    if (FindEnzymesAutoAnnotationUpdater::isTooManyAnnotationsInTheResult(advSequenceContext->getSequenceLength(), selectedEnzymes.size())) {
        QString message = tr("Too many results to render. Please reduce the search region or number of selected enzymes.");
        QMessageBox::critical(this, tr("Error!"), message, QMessageBox::Ok);
        return false;
    }

    saveSettings();

    AutoAnnotationUtils::triggerAutoAnnotationsUpdate(advSequenceContext.data(), ANNOTATION_GROUP_ENZYME);

    return true;
}


void FindEnzymesDialogSequenceView::initResultsCountFilter() {
    SAFE_POINT_NN(advSequenceContext.data(), );

    countFilter = new ResultsCountFilter(this);
    layout()->addWidget(countFilter);
}

void FindEnzymesDialogSequenceView::initRegionSelectorWithExclude() {
    SAFE_POINT_NN(advSequenceContext.data(), );

    regionSelector = new RegionSelectorWithExclude(this, advSequenceContext);
    layout()->addWidget(regionSelector);
}

void FindEnzymesDialogSequenceView::saveSettings() {
    FindEnzymesDialogBase::saveSettings();

    countFilter->saveSettings();
    regionSelector->saveSettings();
}



}  // namespace U2
