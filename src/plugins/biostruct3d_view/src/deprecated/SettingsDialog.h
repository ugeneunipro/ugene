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

#ifndef _U2_SETTINGSDIALOG_H_
#define _U2_SETTINGSDIALOG_H_

#include "BioStruct3DGLWidget.h"
#include "ui_SettingsDialog.h"
#include "AnaglyphRenderer.h"

#include <QColorDialog>

namespace U2 {


class GlassesColorScheme
{
public:
    QColor leftEyeColor;
    QColor rightEyeColor;

    QString name;

    GlassesColorScheme()
    {
        name="";
        leftEyeColor=QColor(0, 0, 0);
        rightEyeColor = leftEyeColor;
    }

    GlassesColorScheme(QString name, QColor leftEyeColor, QColor rightEyeColor)
    {
        this->name = name;
        this->leftEyeColor = leftEyeColor;
        this->rightEyeColor = rightEyeColor;
    }
};


class BioStruct3DSettingsDialog : public QDialog, private Ui_SettingsDialog
{
    Q_OBJECT

public:
    BioStruct3DSettingsDialog();

private slots:
    void sl_setBackgroundColor();
    void sl_setSelectionColor();
    void sl_setShadingLevel();
    void sl_setRenderDetailLevel();
    void sl_setAnaglyph();

    void sl_setEyesShift();

    void sl_setGlassesColorScheme();
    void sl_setLeftEyeColor();
    void sl_setRightEyeColor();
    void sl_swapColors();


public:
    // anaglyph related settings
    AnaglyphStatus getAnaglyphStatus() const;
    const AnaglyphSettings& getAnaglyphSettings() const;

    void setAnaglyphStatus(AnaglyphStatus status);
    void setAnaglyphSettings(const AnaglyphSettings &settings);

    void setRightEyeColor(QColor rightEyecolor);
    void setLeftEyeColor(QColor leftEyecolor);

    QColor getBackgroundColor()const;
    QColor getSelectionColor()const;

    float getRenderDetailLevel()const;
    int getShadingLevel()const;

    void setBackgroundColor(QColor color);
    void setSelectionColor(QColor color);

    void setGlassesColorScheme(QColor &leftEyeColor, QColor &rightEyeColor);
    void setGlassesColorScheme(int num);

    void setRenderDetailLevel(float renderDetailLevel);
    void setShadingLevel(int shading);

    void setWidget(BioStruct3DGLWidget *glWidget);

private:
    QVariantMap state;

    BioStruct3DGLWidget *glWidget;
    QList<GlassesColorScheme> glassesColorSchemes;

    void initColorSchemes();

    QColor backgroundColor;
    QColor selectionColor;


    int renderDetailLevel;
    int shadingLevel;

    AnaglyphStatus anaglyphStatus;
    AnaglyphSettings anaglyphSettings;
};

} // namespace

#endif // _U2_SETTINGSDIALOG_H_
