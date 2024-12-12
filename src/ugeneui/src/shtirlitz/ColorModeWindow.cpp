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

#include "ColorModeWindow.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/Theme.h>

#include "app_settings/user_apps_settings/UserApplicationsSettingsGUIController.h"

#include <QRadioButton>
#include <QStyleFactory>

namespace U2 {

ColorModeWindow::ColorModeWindow(QWidget* parent)
    : QDialog(parent) {
    setupUi(this);

    setWindowTitle(tr("UGENE color mode"));
    errorLabel->setStyleSheet(QString("color: %1;").arg(Theme::errorColorLabelColor().name()));

    auto keys = QStyleFactory::keys();
    for (const auto& key : qAsConst(keys)) {
        cbStyle->addItem(UserApplicationsSettingsPageWidget::FIXED_CASE_QSTYLE_KEY_MAP.value(key, key));
    }
    auto userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();
    QString styleName = userAppSettings->getVisualStyle();
    int styleIdx = cbStyle->findText(styleName, Qt::MatchFixedString);  // case insensitive
    if (styleIdx != -1) {
        cbStyle->setCurrentIndex(styleIdx);
    }
    connect(cbStyle, &QComboBox::currentTextChanged, this, &ColorModeWindow::sl_updateState);

    QStringList colorModes = {tr("Light"),
                              tr("Dark")};
#if defined(Q_OS_WIN) | defined(Q_OS_DARWIN)
    if (StyleFactory::isDarkStyleAvaliable()) {
        colorModes.append(tr("Auto"));
    }
#endif

    static const QStringList interfixes = {"/light",
                                           "/dark",
                                           ""};

    int colorModeIndex = userAppSettings->getColorModeIndex();
    auto hBoxLayout = new QHBoxLayout(this);
    for (int i = 0; i < colorModes.size(); i++) {
        const auto& mode = colorModes[i];
        const auto& interfix = interfixes[i];
        QString imagePath = QString(":/ugene/images%1/color_mode_pic.png").arg(interfix);
        QPixmap pix(imagePath);//

        auto pic = new QLabel(this);
        pic->setPixmap(pix);

        auto vBoxLayout = new QVBoxLayout(this);

        auto h3BoxLayout = new QHBoxLayout(this);
        h3BoxLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        h3BoxLayout->addWidget(pic);
        h3BoxLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        vBoxLayout->addLayout(h3BoxLayout);

        auto h2BoxLayout = new QHBoxLayout(this);

        h2BoxLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        auto rbColor = new QRadioButton(mode, this);
        auto f = rbColor->font();
        f.setPointSize(label_5->font().pointSize());
        rbColor->setFont(f);
        rbColor->setChecked(i == colorModeIndex);
        if (i == 0) {
            lightRb = rbColor;
        } else {
            darkRbs.append(rbColor);
        }
        h2BoxLayout->addWidget(rbColor);
        connect(rbColor, &QRadioButton::toggled, this, &ColorModeWindow::sl_updateState);

        h2BoxLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        vBoxLayout->addLayout(h2BoxLayout);

        hBoxLayout->addLayout(vBoxLayout);
    }
    wgtColorModes->setLayout(hBoxLayout);

    sl_updateState();

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void ColorModeWindow::sl_updateState() {
    static const QString WINDOWS_VISTA_STYLE = "WindowsVista";
    if (cbStyle->currentText() == WINDOWS_VISTA_STYLE) {
        for (auto darkRb : qAsConst(darkRbs)) {
            darkRb->setEnabled(false);
        }
        errorLabel->setText(tr("Note: WindowsVista style is incompatible with Dark color mode. We suggest using Fusion"));
    } else if (!lightRb->isChecked()) {
        int styleIdx = cbStyle->findText(WINDOWS_VISTA_STYLE, Qt::MatchFixedString);
        if (styleIdx != -1) {
            cbStyle->removeItem(styleIdx);
        }
        errorLabel->setText("");
    } else {
        for (auto darkRb : qAsConst(darkRbs)) {
            darkRb->setEnabled(true);
            int styleIdx = cbStyle->findText(WINDOWS_VISTA_STYLE, Qt::MatchFixedString);
            if (styleIdx == -1) {
                cbStyle->addItem(WINDOWS_VISTA_STYLE);
            }
        }
        errorLabel->setText("");
    }
}

QPair<QString, int> ColorModeWindow::getNewStyle() const {
    QPair<QString, int> res = {"", 0};
    res.first = cbStyle->currentText();
    QList<QRadioButton*> allRbs = {lightRb};
    allRbs.append(darkRbs);
    for (int i = 0; i < allRbs.size(); i++) {
        const auto& cmrb = allRbs.at(i);
        if (cmrb->isChecked()) {
            res.second = i;
        }
    }
    return res;
}

}  // namespace U2
