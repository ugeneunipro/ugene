/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "IQTreeWidget.h"

#include <QDesktopServices>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <U2View/PhyTreeDisplayOptionsWidget.h>

namespace U2 {

static constexpr const char* IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY = "/extra-parameters";

IQTreeWidget::IQTreeWidget(const MultipleSequenceAlignment&, QWidget* parent)
    : CreatePhyTreeWidget(parent) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("tab_widget");
    layout->addWidget(tabWidget);

    auto iqTreeOptionsWidget = new QWidget();
    auto iqTreeOptionsTabLayout = new QVBoxLayout();
    iqTreeOptionsWidget->setLayout(iqTreeOptionsTabLayout);

    auto hintLayout = new QHBoxLayout();
    hintLayout->addWidget(new QLabel(tr("Extra command line options for IQ-TREE (1 per line):")));
    hintLayout->addStretch();
    auto docsButton = new QPushButton(tr("View all options"));
    docsButton->setToolTip(tr("Open official 'Command reference' guide for IQ-TREE in browser"));
    connect(docsButton, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("http://www.iqtree.org/doc/Command-Reference"));
    });
    hintLayout->addWidget(docsButton);
    iqTreeOptionsTabLayout->addLayout(hintLayout);

    extraParametersTextEdit = new QPlainTextEdit();
    extraParametersTextEdit->setToolTip(tr("Use one parameter per one line"));
    extraParametersTextEdit->setObjectName("extra_parameters_text_edit");
    auto savedParameters = AppContext::getSettings()->getValue(CreatePhyTreeWidget::getAppSettingsRoot() + IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY).toStringList();
    extraParametersTextEdit->setPlainText(savedParameters.join("\n"));
    iqTreeOptionsTabLayout->addWidget(extraParametersTextEdit);

    tabWidget->addTab(iqTreeOptionsWidget, tr("IQ-TREE options"));

    displayOptionsWidget = new PhyTreeDisplayOptionsWidget();
    displayOptionsWidget->setContentsMargins(10, 10, 10, 10);
    tabWidget->addTab(displayOptionsWidget, tr("Display Options"));
}

void IQTreeWidget::fillSettings(CreatePhyTreeSettings& settings) {
    settings.extToolArguments.clear();
    QStringList extraArguments = extraParametersTextEdit->toPlainText().split("\n");
    for (const QString& arg : qAsConst(extraArguments)) {
        QString trimmedArg = arg.trimmed();
        if (!trimmedArg.isEmpty()) {  // Do not add empty lines into the argument list.
            settings.extToolArguments << trimmedArg;
        }
    }
    displayOptionsWidget->fillSettings(settings);
}

void IQTreeWidget::storeSettings() {
    QStringList params = extraParametersTextEdit->toPlainText().split("\n");
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::getAppSettingsRoot() + IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY, params);
    displayOptionsWidget->storeSettings();
}

void IQTreeWidget::restoreDefault() {
    AppContext::getSettings()->remove(CreatePhyTreeWidget::getAppSettingsRoot() + IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY);
    extraParametersTextEdit->clear();
    displayOptionsWidget->restoreDefault();
}

bool IQTreeWidget::checkSettings(QString& message, const CreatePhyTreeSettings& settings) {
    return displayOptionsWidget->checkSettings(message, settings);
}

}  // namespace U2
