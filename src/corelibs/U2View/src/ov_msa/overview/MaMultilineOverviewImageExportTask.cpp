/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MaMultilineOverviewImageExportTask.h"

#include <QCheckBox>
#include <QPainter>
#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include "MaGraphMultilineOverview.h"
#include "MaSimpleMultilineOverview.h"

namespace U2 {

MaMultilineOverviewImageExportToBitmapTask::MaMultilineOverviewImageExportToBitmapTask(MaSimpleMultilineOverview *simpleOverview,
                                                                     MaGraphMultilineOverview *graphOverview,
                                                                     const MaMultilineOverviewImageExportSettings &overviewSettings,
                                                                     const ImageExportTaskSettings &settings)
    : ImageExportTask(settings),
      simpleOverview(simpleOverview),
      graphOverview(graphOverview),
      overviewSettings(overviewSettings) {
    SAFE_POINT_EXT(simpleOverview != nullptr, setError(tr("Overview is NULL")), );
    SAFE_POINT_EXT(graphOverview != nullptr, setError(tr("Graph overview is NULL")), );
    CHECK_EXT(overviewSettings.exportGraphOverview || overviewSettings.exportSimpleOverview,
              setError(tr("Nothing to export. ") + EXPORT_FAIL_MESSAGE.arg(settings.fileName)), );
}

void MaMultilineOverviewImageExportToBitmapTask::run() {
    SAFE_POINT_EXT(settings.isBitmapFormat(),
                   setError(WRONG_FORMAT_MESSAGE.arg(settings.format).arg("MSAOverviewImageExportToBitmapTask")), );
    QPixmap pixmap(settings.imageSize.width(), settings.imageSize.height());
    QPainter p(&pixmap);

    if (overviewSettings.exportSimpleOverview) {
        QPixmap simpleOverviewPixmap = simpleOverview->getView();
        p.drawPixmap(simpleOverview->rect(), simpleOverviewPixmap);
        p.translate(0, simpleOverview->height());
    }
    if (overviewSettings.exportGraphOverview) {
        QPixmap graphOverviewPixmap = graphOverview->getView();
        p.drawPixmap(graphOverview->rect(), graphOverviewPixmap);
    }
    p.end();

    CHECK_EXT(pixmap.save(settings.fileName, qPrintable(settings.format), settings.imageQuality), setError(tr("FAIL")), );
}

MaMultilineOverviewImageExportController::MaMultilineOverviewImageExportController(MaSimpleMultilineOverview *simpleOverview,
                                                                 MaGraphMultilineOverview *graphOverview)
    : ImageExportController(),
      simpleOverview(simpleOverview),
      graphOverview(graphOverview) {
    SAFE_POINT(simpleOverview != nullptr, QObject::tr("Overview is NULL"), );
    SAFE_POINT(graphOverview != nullptr, QObject::tr("Graph overview is NULL"), );
    shortDescription = tr("Alignment overview");
    initSettingsWidget();
}

int MaMultilineOverviewImageExportController::getImageWidth() const {
    return graphOverview->width();
}

int MaMultilineOverviewImageExportController::getImageHeight() const {
    int h = 0;
    if (exportSimpleOverview->isChecked()) {
        h += simpleOverview->height();
    }
    if (exportGraphOverview->isChecked()) {
        h += graphOverview->height();
    }
    return h;
}

Task *MaMultilineOverviewImageExportController::getExportToBitmapTask(const ImageExportTaskSettings &settings) const {
    MaMultilineOverviewImageExportSettings overviewSettings(exportSimpleOverview->isChecked(),
                                                   exportGraphOverview->isChecked());
    // overview has fixed size
    ImageExportTaskSettings copySettings = settings;
    copySettings.imageSize = QSize(getImageWidth(), getImageHeight());
    return new MaMultilineOverviewImageExportToBitmapTask(simpleOverview, graphOverview, overviewSettings, copySettings);
}

void MaMultilineOverviewImageExportController::initSettingsWidget() {
    settingsWidget = new QWidget();
    exportSimpleOverview = new QCheckBox(QObject::tr("Export simple overview"), settingsWidget);
    exportGraphOverview = new QCheckBox(QObject::tr("Export graph overview"), settingsWidget);

    exportSimpleOverview->setObjectName("export_msa_simple_overview");
    exportGraphOverview->setObjectName("export_msa_graph_overview");

    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(exportSimpleOverview);
    layout->addWidget(exportGraphOverview);

    if (!simpleOverview->isValid() || !simpleOverview->isVisible()) {
        exportSimpleOverview->setDisabled(true);
    } else {
        exportSimpleOverview->setChecked(true);
    }
    exportGraphOverview->setChecked(true);

    settingsWidget->setLayout(layout);
}

}  // namespace U2
