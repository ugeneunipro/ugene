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

#pragma once

#include <U2Gui/ImageExportTask.h>

class QComboBox;
class QCheckBox;
namespace U2 {

class CircularView;

class CircularViewImageExportSettings {
public:
    CircularViewImageExportSettings(bool includeMarker = false, bool includeSelection = true)
        : includeMarker(includeMarker),
          includeSelection(includeSelection) {
    }

    bool includeMarker;
    bool includeSelection;
};

class CircularViewImageExportTask : public ImageExportTask {
public:
    CircularViewImageExportTask(CircularView* cv,
                                const CircularViewImageExportSettings& cvExportSettings,
                                const ImageExportTaskSettings& settings)
        : ImageExportTask(settings),
          cvWidget(cv),
          cvExportSettings(cvExportSettings) {
    }
    void run() override = 0;

protected:
    CircularView* cvWidget;
    CircularViewImageExportSettings cvExportSettings;
};

class CircularViewImageExportToSVGTask : public CircularViewImageExportTask {
public:
    CircularViewImageExportToSVGTask(CircularView* cv,
                                     const CircularViewImageExportSettings& cvExportSettings,
                                     const ImageExportTaskSettings& settings)
        : CircularViewImageExportTask(cv, cvExportSettings, settings) {
    }
    void run() override;
};

class CircularViewImageExportToPDFTask : public CircularViewImageExportTask {
public:
    CircularViewImageExportToPDFTask(CircularView* cv,
                                     const CircularViewImageExportSettings& cvExportSettings,
                                     const ImageExportTaskSettings& settings)
        : CircularViewImageExportTask(cv, cvExportSettings, settings) {
    }
    void run() override;
};

class CircularViewImageExportToBitmapTask : public CircularViewImageExportTask {
public:
    CircularViewImageExportToBitmapTask(CircularView* cv,
                                        const CircularViewImageExportSettings& cvExportSettings,
                                        const ImageExportTaskSettings& settings)
        : CircularViewImageExportTask(cv, cvExportSettings, settings) {
    }
    void run() override;
};

class CircularViewImageExportController : public ImageExportController {
    Q_OBJECT
public:
    CircularViewImageExportController(CircularView* cv);

    CircularViewImageExportController(const QList<CircularView*>& list,
                                      CircularView* defaultCV = nullptr);

    int getImageWidth() const override;
    int getImageHeight() const override;

protected:
    void initSettingsWidget() override;

    Task* getExportToSvgTask(const ImageExportTaskSettings& settings) const override;
    Task* getExportToPdfTask(const ImageExportTaskSettings& settings) const override;
    Task* getExportToBitmapTask(const ImageExportTaskSettings& settings) const override;

private:
    void updateCvWidget() const;

    mutable CircularView* cvWidget;
    QList<CircularView*> cvList;

    QCheckBox* includeMarkerCheckbox;
    QCheckBox* includeSelectionCheckbox;

    QComboBox* sequenceComboBox;
};

}  // namespace U2
