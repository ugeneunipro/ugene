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

#ifndef _U2_CIRCULAR_VIEW_IMAGE_EXPORT_TASK_H_
#define _U2_CIRCULAR_VIEW_IMAGE_EXPORT_TASK_H_

#include <U2Gui/ImageExportTask.h>

class QComboBox;
class QCheckBox;
namespace U2 {

class CircularView;

class CircularViewImageExportSettings {
public:
    CircularViewImageExportSettings(bool includeMarker = false, bool includeSelection = true)
        : includeMarker(includeMarker),
          includeSelection(includeSelection) {}

    bool includeMarker;
    bool includeSelection;
};

class CircularViewImageExportTask : public ImageExportTask {
public:
    CircularViewImageExportTask(CircularView* cv,
                                const CircularViewImageExportSettings &cvExportSettings,
                                const ImageExportTaskSettings& settings)
        : ImageExportTask(settings),
          cvWidget(cv),
          cvExportSettings(cvExportSettings) {}
    virtual void run() = 0;
protected:
    CircularView*   cvWidget;
    CircularViewImageExportSettings     cvExportSettings;
};

class CircularViewImageExportToSVGTask : public CircularViewImageExportTask {
public:
    CircularViewImageExportToSVGTask(CircularView *cv,
                                     const CircularViewImageExportSettings &cvExportSettings,
                                     const ImageExportTaskSettings& settings)
        : CircularViewImageExportTask(cv, cvExportSettings, settings) {}
    void run();
};

class CircularViewImageExportToPDFTask : public CircularViewImageExportTask {
public:
    CircularViewImageExportToPDFTask(CircularView *cv,
                                     const CircularViewImageExportSettings &cvExportSettings,
                                     const ImageExportTaskSettings& settings)
        : CircularViewImageExportTask(cv, cvExportSettings, settings) {}
    void run();
};

class CircularViewImageExportToBitmapTask : public CircularViewImageExportTask {
public:
    CircularViewImageExportToBitmapTask(CircularView *cv,
                                        const CircularViewImageExportSettings &cvExportSettings,
                                        const ImageExportTaskSettings& settings)
        : CircularViewImageExportTask(cv, cvExportSettings, settings) {}
    void run();
};

class CircularViewImageExportController : public ImageExportController {
    Q_OBJECT
public:
    CircularViewImageExportController(CircularView *cv);

    CircularViewImageExportController(const QList<CircularView*> &list,
                                       CircularView* defaultCV = NULL);

    int getImageWidth() const;
    int getImageHeight() const;

protected:
    void initSettingsWidget();

    Task* getExportToSvgTask(const ImageExportTaskSettings &settings) const;
    Task* getExportToPdfTask(const ImageExportTaskSettings &settings) const;
    Task* getExportToBitmapTask(const ImageExportTaskSettings &settings) const;

private:
    void updateCvWidget() const;

    mutable CircularView*   cvWidget;
    QList<CircularView*>    cvList;

    QCheckBox*      includeMarkerCheckbox;
    QCheckBox*      includeSelectionCheckbox;

    QComboBox*      sequenceComboBox;
};

} // namespace

#endif // _U2_CIRCULAR_VIEW_IMAGE_EXPORT_TASK_H_
