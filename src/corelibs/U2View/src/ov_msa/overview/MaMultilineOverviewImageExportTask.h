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

#ifndef _U2_MSA_MULTILINE_OVERVIEW_IMAGE_EXPORT_TASK_H_
#define _U2_MSA_MULTILINE_OVERVIEW_IMAGE_EXPORT_TASK_H_

#include <U2Gui/ImageExportTask.h>

class QCheckBox;

namespace U2 {

class MaSimpleMultilineOverview;
class MaGraphMultilineOverview;

class MaMultilineOverviewImageExportSettings {
public:
    MaMultilineOverviewImageExportSettings(bool exportSimpleOverview = false,
                                  bool exportGraphOverview = true)
        : exportSimpleOverview(exportSimpleOverview),
          exportGraphOverview(exportGraphOverview) {
    }

    bool exportSimpleOverview;
    bool exportGraphOverview;
};

class MaMultilineOverviewImageExportToBitmapTask : public ImageExportTask {
    Q_OBJECT
public:
    MaMultilineOverviewImageExportToBitmapTask(MaSimpleMultilineOverview *simpleOverview, MaGraphMultilineOverview *graphOverview, const MaMultilineOverviewImageExportSettings &overviewSettings, const ImageExportTaskSettings &settings);
    void run();

private:
    MaSimpleMultilineOverview *simpleOverview;
    MaGraphMultilineOverview *graphOverview;
    MaMultilineOverviewImageExportSettings overviewSettings;
};

class MaMultilineOverviewImageExportController : public ImageExportController {
    Q_OBJECT
public:
    MaMultilineOverviewImageExportController(MaSimpleMultilineOverview *simpleOverview, MaGraphMultilineOverview *graphOverview);

    int getImageWidth() const;
    int getImageHeight() const;

protected:
    void initSettingsWidget();

    Task *getExportToBitmapTask(const ImageExportTaskSettings &settings) const;

private:
    MaSimpleMultilineOverview *simpleOverview;
    MaGraphMultilineOverview *graphOverview;

    QCheckBox *exportSimpleOverview;
    QCheckBox *exportGraphOverview;
};

}  // namespace U2

#endif  // _U2_MSA_MULTILINE_OVERVIEW_IMAGE_EXPORT_TASK_H_
