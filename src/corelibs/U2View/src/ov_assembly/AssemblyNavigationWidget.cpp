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

#include <QLabel>
#include <QVBoxLayout>

#include <U2Core/FormatUtils.h>
#include <U2Core/Theme.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/PositionSelector.h>
#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include "AssemblyBrowser.h"

#include "AssemblyNavigationWidget.h"

namespace U2 {

AssemblyNavigationWidget::AssemblyNavigationWidget(AssemblyBrowser *_browser, QWidget *p)
    : QWidget(p), browser(_browser), savableTab(this, GObjectViewUtils::findViewByName(_browser->getName()))
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(5);
    setLayout(mainLayout);

    U2OpStatus2Log os;
    qint64 modelLen = browser->getModel()->getModelLength(os);
    if(!os.isCoR()) {
        posSelector = new PositionSelector(this, 1, modelLen, false);
        connect(posSelector, SIGNAL(si_positionChanged(int)), browser, SLOT(sl_onPosChangeRequest(int)));
        posSelector->setContentsMargins(0,0,10,0);

        mainLayout->addWidget(new QLabel(tr("Enter position in assembly:"), this));
        mainLayout->addWidget(posSelector);

        connect(browser, SIGNAL( si_zoomOperationPerformed() ), SLOT( sl_updateZoomingState() ));
        sl_updateZoomingState();
    }

    CoveredRegionsLabel * coveredLabel = new CoveredRegionsLabel(browser, this);
    QWidget * coveredGroup = new ShowHideSubgroupWidget("COVERED", tr("Most Covered Regions"), coveredLabel, true);
    mainLayout->addWidget(coveredGroup);

    U2WidgetStateStorage::restoreWidgetState(savableTab);
}

void AssemblyNavigationWidget::sl_updateZoomingState(){
    if (browser && posSelector){
        posSelector->setEnabled(browser->canPerformZoomOut());
    }
}

// ----- CoveredRegionsLabel -----

CoveredRegionsLabel::CoveredRegionsLabel(AssemblyBrowser * ab, QWidget *p)
    : QLabel(p), browser(ab)
{
    connect(this, SIGNAL(linkActivated(QString)), browser, SLOT(sl_coveredRegionClicked(QString)));
    connect(browser, SIGNAL(si_coverageReady()), SLOT(sl_updateContent()));
    setAlignment(Qt::AlignTop);
    setContentsMargins(0, 0, 0, 0);
    setObjectName("CoveredRegionsLabel");
    sl_updateContent();
}

void CoveredRegionsLabel::setAdditionalText(QString prefix_, QString postfix_) {
    prefix = prefix_;
    postfix = postfix_;
    sl_updateContent();
}

void CoveredRegionsLabel::sl_updateContent() {
    QString text = "<style>a:link { color: " + Theme::linkColorLabelStr() + " }</style>";
    text += prefix;

    QList<CoveredRegion> coveredRegions = browser->getCoveredRegions();
    if(!browser->areCoveredRegionsReady()) {
        text += tr("Computing coverage...");
    } else if(!coveredRegions.empty()) {
        QString coveredRegionsText = "<style>td { padding-right: 8px;}</style>";
        coveredRegionsText += "<table cellspacing='2'>";
        /*
        * |   | Region | Coverage |
        * | 1 | [x,y]  | z        |
        */
        coveredRegionsText += tr("<tr><th/><th align='left'><div style='margin-right: 5px;'>Position</div></th><th align = 'center'>Coverage</th></tr>");
        for(int i = 0; i < coveredRegions.size(); ++i) {
            const CoveredRegion & cr = coveredRegions.at(i);
            QString crRegion = FormatUtils::splitThousands(cr.region.center());
            QString crCoverage = FormatUtils::splitThousands(cr.coverage);
            coveredRegionsText += "<tr>";
            coveredRegionsText += QString("<td align='right'>%1&nbsp;&nbsp;</td>").arg(i+1);
            coveredRegionsText += QString("<td><a href=\"%1\">%2</a></td>").arg(i).arg(crRegion);
            coveredRegionsText += tr("<td align=\"center\">%4</td>").arg(crCoverage);
            coveredRegionsText += "</tr>";
        }
        coveredRegionsText += "</table>";
        text += coveredRegionsText;
    }

    text += postfix;

    setText(text);
}

////////////////////////////////////
// AssemblyNavigationWidgetFactory
////////////////////////////////////
const QString AssemblyNavigationWidgetFactory::GROUP_ID = "OP_ASS_NAVIGATION";
const QString AssemblyNavigationWidgetFactory::GROUP_ICON_STR = ":core/images/goto.png";
const QString AssemblyNavigationWidgetFactory::GROUP_DOC_PAGE = "24748911";

AssemblyNavigationWidgetFactory::AssemblyNavigationWidgetFactory() {
    objectViewOfWidget = ObjViewType_AssemblyBrowser;
}

QWidget* AssemblyNavigationWidgetFactory::createWidget(GObjectView* objView) {
    SAFE_POINT(NULL != objView,
        QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
        NULL);

    AssemblyBrowser* assemblyBrowser = qobject_cast<AssemblyBrowser*>(objView);
    SAFE_POINT(NULL != assemblyBrowser,
        QString("Internal error: unable to cast object view to Assembly Browser for group '%1'.").arg(GROUP_ID),
        NULL);

    AssemblyNavigationWidget* widget = new AssemblyNavigationWidget(assemblyBrowser, assemblyBrowser->getMainWidget());
    return widget;
}

OPGroupParameters AssemblyNavigationWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Navigation"), GROUP_DOC_PAGE);
}

} // namespace U2
