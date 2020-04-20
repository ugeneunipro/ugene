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


#include "SettingsDialog.h"
#include <U2Core/Log.h>
#include <U2Gui/HelpButton.h>

namespace U2 {


BioStruct3DSettingsDialog::BioStruct3DSettingsDialog()
        : anaglyphStatus(NOT_AVAILABLE), anaglyphSettings(AnaglyphSettings::defaultSettings())
{
    setupUi(this);

    new HelpButton(this, buttonBox, "24748806");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    glWidget = NULL;
    initColorSchemes();
}

void BioStruct3DSettingsDialog::setWidget(BioStruct3DGLWidget *glWidget)
{
    this->glWidget = glWidget;

    state = glWidget->getState();
}

void BioStruct3DSettingsDialog::initColorSchemes()
{
    glassesColorSchemes.insert(0, GlassesColorScheme(QString(tr("Custom")), QColor(0, 0, 0), QColor(0, 0, 0)));

    glassesColorSchemes.append(GlassesColorScheme(QString(tr("Red - Blue")), QColor(255, 0, 0), QColor(0, 0, 255)));
    glassesColorSchemes.append(GlassesColorScheme(QString(tr("Cyan - Red")), QColor(0, 255, 255), QColor(255, 0, 0)));
    glassesColorSchemes.append(GlassesColorScheme(QString(tr("Red - Dark Cyan")), QColor(255, 0, 0), QColor(0, 139, 139)));
    glassesColorSchemes.append(GlassesColorScheme(QString(tr("Red - Green")), QColor(255, 0, 0), QColor(0, 150, 0)));
    glassesColorSchemes.append(GlassesColorScheme(QString(tr("Magenta - Green")), QColor(255, 0, 150), QColor(0, 150, 0)));

    foreach (const GlassesColorScheme &scheme, glassesColorSchemes)
        BioStruct3DSettingsDialog::glassesColorSchemeComboBox->addItem(scheme.name);
}


void BioStruct3DSettingsDialog::sl_setBackgroundColor()
{
    backgroundColor=QColorDialog::getColor(backgroundColor,this);

    state[glWidget->BACKGROUND_COLOR_NAME] = QVariant::fromValue(backgroundColor);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::sl_setSelectionColor()
{
    selectionColor=QColorDialog::getColor(selectionColor,this);

    state[glWidget->SELECTION_COLOR_NAME] = QVariant::fromValue(selectionColor);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::sl_setRenderDetailLevel()
{
    renderDetailLevel=renderDetailLevelSlider->sliderPosition();

    state[glWidget->RENDER_DETAIL_LEVEL_NAME] = QVariant::fromValue((float) renderDetailLevel/100.0);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::sl_setShadingLevel()
{
    shadingLevel = shadingSlider->sliderPosition();
    state[glWidget->SHADING_LEVEL_NAME] = QVariant::fromValue(shadingLevel);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::sl_setAnaglyph() {
    anaglyphStatus = (anaglyphViewGroupBox->isChecked()) ? ENABLED : DISABLED;

    state[glWidget->ANAGLYPH_STATUS_NAME] = qVariantFromValue((int)anaglyphStatus);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::sl_setEyesShift() {
    anaglyphSettings.eyesShift = (float) eyesShiftSlider->sliderPosition() / 100.0;

    anaglyphSettings.toMap(state);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::sl_setGlassesColorScheme() {
    int itemNum = glassesColorSchemeComboBox->currentIndex();
    setGlassesColorScheme(itemNum);
}

void BioStruct3DSettingsDialog::sl_setLeftEyeColor() {
    QColor changed(QColorDialog::getColor(anaglyphSettings.leftEyeColor, this));

    if (changed.red()!=0 || changed.green()!=0 || changed.blue()!=0) {
        setGlassesColorScheme(changed, anaglyphSettings.rightEyeColor);
    }
}

void BioStruct3DSettingsDialog::sl_setRightEyeColor() {
    QColor changed(QColorDialog::getColor(anaglyphSettings.rightEyeColor, this));

    if (changed.red()!=0 || changed.green()!=0 || changed.blue()!=0) {
        setGlassesColorScheme(anaglyphSettings.leftEyeColor, changed);
    }
}

void BioStruct3DSettingsDialog::sl_swapColors() {
    QColor right(anaglyphSettings.rightEyeColor);
    QColor left(anaglyphSettings.leftEyeColor);

    setGlassesColorScheme(right, left);
}

QColor BioStruct3DSettingsDialog::getBackgroundColor()const
{
    return backgroundColor;
}

QColor BioStruct3DSettingsDialog::getSelectionColor()const
{
    return selectionColor;
}

float BioStruct3DSettingsDialog::getRenderDetailLevel()const
{
    return renderDetailLevel / 100.0;
}

void BioStruct3DSettingsDialog::setGlassesColorScheme(QColor &leftEyeColor, QColor &rightEyeColor)
{
    setLeftEyeColor(leftEyeColor);
    setRightEyeColor(rightEyeColor);

    int currentNumber=0;
    foreach(const GlassesColorScheme& scheme, glassesColorSchemes)
    {
        if (scheme.leftEyeColor==leftEyeColor && scheme.rightEyeColor==rightEyeColor)
        {
            glassesColorSchemeComboBox->setCurrentIndex(currentNumber);
            return;
        }
        currentNumber++;
    }

    glassesColorSchemeComboBox->setCurrentIndex(0);
}

void BioStruct3DSettingsDialog::setGlassesColorScheme(int num)
{
    if (num<=0 || num>glassesColorSchemes.size())
        return;

    GlassesColorScheme need(glassesColorSchemes.at(num));

    setLeftEyeColor(need.leftEyeColor);
    setRightEyeColor(need.rightEyeColor);
}

void BioStruct3DSettingsDialog::setBackgroundColor(QColor color)
{
    this->backgroundColor=color;
}

void BioStruct3DSettingsDialog::setSelectionColor(QColor color)
{
    this->selectionColor=color;
}

void BioStruct3DSettingsDialog::setRenderDetailLevel(float renderDetailLevel_)
{
    renderDetailLevel = renderDetailLevel_ * 100;
    renderDetailLevelSlider->setSliderPosition(renderDetailLevel);
}

static const QString COLOR_STYLE("QPushButton { background-color : %1;}");// color : %2;

void BioStruct3DSettingsDialog::setLeftEyeColor(QColor leftEyecolor)
{
    anaglyphSettings.leftEyeColor = leftEyecolor;
    leftEyeColorChangeButton->setStyleSheet(COLOR_STYLE.arg(anaglyphSettings.leftEyeColor.name()));

    anaglyphSettings.toMap(state);
    glWidget->setState(state);
}

void BioStruct3DSettingsDialog::setRightEyeColor(QColor rightEyecolor)
{
    anaglyphSettings.rightEyeColor = rightEyecolor;
    rightEyeColorChangeButton->setStyleSheet(COLOR_STYLE.arg(anaglyphSettings.rightEyeColor.name()));

    anaglyphSettings.toMap(state);
    glWidget->setState(state);
}

int BioStruct3DSettingsDialog::getShadingLevel() const {
    return shadingLevel;
}

void BioStruct3DSettingsDialog::setShadingLevel(int shading) {
    shadingLevel = shading;
    shadingSlider->setSliderPosition(shading);
}

// anaglyph related settings
AnaglyphStatus BioStruct3DSettingsDialog::getAnaglyphStatus() const {
    return anaglyphStatus;
}

const AnaglyphSettings& BioStruct3DSettingsDialog::getAnaglyphSettings() const {
    return anaglyphSettings;
}

void BioStruct3DSettingsDialog::setAnaglyphStatus(AnaglyphStatus status) {
    anaglyphStatus = status;

    if (anaglyphStatus == NOT_AVAILABLE) {
        anaglyphViewGroupBox->setDisabled(true);

        QString anaglyphTitile = anaglyphViewGroupBox->title();
        anaglyphViewGroupBox->setTitle(anaglyphTitile + QString(" ") + QString(tr("(not supported by your videocard)")));
    }
    else if (anaglyphStatus == DISABLED) {
        anaglyphViewGroupBox->setEnabled(true);
        anaglyphViewGroupBox->setChecked(false);
    }
    else if (anaglyphStatus == ENABLED) {
        anaglyphViewGroupBox->setEnabled(true);
        anaglyphViewGroupBox->setChecked(true);
    }
    else {
        assert(!"Invalid AnaglyphStatus value");
    }
}

void BioStruct3DSettingsDialog::setAnaglyphSettings(const AnaglyphSettings &settings) {
    anaglyphSettings = settings;

    setLeftEyeColor(anaglyphSettings.leftEyeColor);
    setRightEyeColor(anaglyphSettings.rightEyeColor);

    eyesShiftSlider->setSliderPosition(anaglyphSettings.eyesShift * 100.0);
}

} // namespace
