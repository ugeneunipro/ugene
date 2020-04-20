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

#ifndef _U2_EXPORT_IMAGE_DIALOG_H_
#define _U2_EXPORT_IMAGE_DIALOG_H_

#include <QDialog>

#include "imageExport/ImageExportTask.h"

class Ui_ImageExportForm;
class QRadioButton;
class QAbstractButton;
class QButtonGroup;

namespace U2 {

class SaveDocumentController;

class U2GUI_EXPORT ExportImageDialog : public QDialog {
    Q_OBJECT
public:
    enum InvokedFrom{ WD, CircularView, MSA, SequenceView, AssemblyView, PHYTreeView, DotPlot, MolView};
    enum ImageScalingPolicy {
        NoScaling,
        SupportScaling
    };

    ExportImageDialog(QWidget* screenShotWidget,
                      InvokedFrom invoSource,
                      const QString& file,
                      ImageScalingPolicy scalingPolicy = NoScaling,
                      QWidget* parent = NULL);

    ExportImageDialog(ImageExportController *factory,
                      InvokedFrom invoSource,
                      const QString& file,
                      ImageScalingPolicy scalingPolicy = NoScaling,
                      QWidget* parent = NULL);

    ~ExportImageDialog();

    const QString& getFilename() const { return filename; }
    const QString& getFormat() const { return format; }
    int getWidth() const;
    int getHeight() const;

    bool hasQuality() const;
    int  getQuality() const;

public slots:
    void accept();

private slots:
    void sl_onFormatsBoxItemChanged(const QString& format);

    void sl_showMessage(const QString& message);
    void sl_disableExport(bool disable);

private:
    void init();
    void initSaveController(const QString& defaultFormat);
    void setSizeControlsEnabled(bool enabled);
    QStringList getFormats();
    QStringList getRasterFormats();
    QStringList getSvgAndPdfFormats();

    static bool isVectorGraphicFormat(const QString &formatName);
    static bool isLossyFormat(const QString &formatName);

    SaveDocumentController *saveController;
    ImageExportController*  exportController;
    ImageScalingPolicy      scalingPolicy;

    QString filename;
    QString origFilename;
    QString format;

    Ui_ImageExportForm* ui;
    InvokedFrom source;
}; // class ExportImageDialog

} // namespace

#endif
