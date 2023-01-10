/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QCheckBox>
#include <QDesktopServices>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2View/PhyTreeDisplayOptionsWidget.h>

#include "CmdlineParamsParser.h"

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

    auto hintLabel = new QLabel(tr("Command line options for IQ-TREE:"));
    hintLabel->setToolTip(tr("Example: -lmap 2000 -n 0 -m\n Use double-quotes (\") for values with spaces."));
    hintLayout->addWidget(hintLabel);

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
    extraParametersTextEdit->setObjectName("extraParametersTextEdit");
    auto savedParameters = AppContext::getSettings()->getValue(CreatePhyTreeWidget::getAppSettingsRoot() + IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY).toStringList();
    extraParametersTextEdit->setPlainText(savedParameters.join("\n"));
    connect(extraParametersTextEdit, &QPlainTextEdit::textChanged, this, [this] {
        CHECK(!isInsideChangeCallback, );
        isInsideChangeCallback = true;
        propagateTextParametersToWidgetValues();
        isInsideChangeCallback = false;
    });
    iqTreeOptionsTabLayout->addWidget(extraParametersTextEdit);

    auto iqTreeOptionsComponentRowLayout = new QHBoxLayout();

    iqTreeOptionsComponentRowLayout->addWidget(new QLabel(tr("Subst. model")));
    substModelEdit = new QLineEdit();
    substModelEdit->setObjectName("substModelEdit");
    substModelEdit->setMinimumWidth(60);
    connect(substModelEdit, &QLineEdit::textChanged, this, &IQTreeWidget::propagateWidgetValuesToTextParameters);
    iqTreeOptionsComponentRowLayout->addWidget(substModelEdit);
    iqTreeOptionsComponentRowLayout->addSpacing(10);

    iqTreeOptionsComponentRowLayout->addWidget(new QLabel(tr("Ultrafast bootstrap")));
    ultrafastBootstrapEdit = new QLineEdit();
    ultrafastBootstrapEdit->setObjectName("ultrafastBootstrapEdit");
    ultrafastBootstrapEdit->setMinimumWidth(60);
    connect(ultrafastBootstrapEdit, &QLineEdit::textChanged, this, &IQTreeWidget::propagateWidgetValuesToTextParameters);
    iqTreeOptionsComponentRowLayout->addWidget(ultrafastBootstrapEdit);
    iqTreeOptionsComponentRowLayout->addSpacing(10);

    iqTreeOptionsComponentRowLayout->addWidget(new QLabel(tr("alrt")));
    alrtEdit = new QLineEdit();
    alrtEdit->setObjectName("alrtEdit");
    alrtEdit->setMinimumWidth(60);
    connect(alrtEdit, &QLineEdit::textChanged, this, &IQTreeWidget::propagateWidgetValuesToTextParameters);
    iqTreeOptionsComponentRowLayout->addWidget(alrtEdit);
    iqTreeOptionsComponentRowLayout->addSpacing(10);

    iqTreeOptionsComponentRowLayout->addWidget(new QLabel(tr("Ancestral reconstruction")));
    ancestralReconstructionCheckBox = new QCheckBox();
    ancestralReconstructionCheckBox->setObjectName("ancestralReconstructionCheckBox");
    connect(ancestralReconstructionCheckBox, &QCheckBox::stateChanged, this, &IQTreeWidget::propagateWidgetValuesToTextParameters);
    iqTreeOptionsComponentRowLayout->addWidget(ancestralReconstructionCheckBox);
    iqTreeOptionsTabLayout->addLayout(iqTreeOptionsComponentRowLayout);

    tabWidget->addTab(iqTreeOptionsWidget, tr("IQ-TREE options"));

    displayOptionsWidget = new PhyTreeDisplayOptionsWidget();
    displayOptionsWidget->setContentsMargins(10, 10, 10, 10);
    tabWidget->addTab(displayOptionsWidget, tr("Display Options"));

    propagateTextParametersToWidgetValues();
}

void IQTreeWidget::fillSettings(CreatePhyTreeSettings& settings) {
    settings.extToolArguments.clear();
    QString parametersString = extraParametersTextEdit->toPlainText();
    U2OpStatusImpl os;
    QStringList parameterList = CmdlineParamsParser::parse(os, parametersString);
    if (os.hasError()) {
        QMessageBox::critical(this, L10N::errorTitle(), os.getError());
        return;
    }
    for (const QString& parameter : qAsConst(parameterList)) {
        settings.extToolArguments << parameter;
    }
    displayOptionsWidget->fillSettings(settings);
}

void IQTreeWidget::storeSettings() {
    QString parametersString = extraParametersTextEdit->toPlainText();
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::getAppSettingsRoot() + IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY, parametersString);
    displayOptionsWidget->storeSettings();
}

void IQTreeWidget::restoreDefault() {
    AppContext::getSettings()->remove(CreatePhyTreeWidget::getAppSettingsRoot() + IQTREE_EXTRA_PARAMETERS_SETTINGS_KEY);
    extraParametersTextEdit->clear();
    displayOptionsWidget->restoreDefault();
}

bool IQTreeWidget::checkSettings(QString& message, const CreatePhyTreeSettings& settings) {
    // TODO: parse parameters and return error message (change the API & all impls) not a bool!
    return displayOptionsWidget->checkSettings(message, settings);
}

void IQTreeWidget::propagateWidgetValuesToTextParameters() {
    CHECK(!isInsideChangeCallback, );
    U2OpStatusImpl os;
    QStringList oldParameters = CmdlineParamsParser::parse(os, extraParametersTextEdit->toPlainText());
    CHECK_OP(os, );

    isInsideChangeCallback = true;  // TODO: create an utility class that will call lambda in destructor.

    QStringList parameters = oldParameters;

    CmdlineParamsParser::removeParameterNameAndValue("-m", parameters);
    if (!substModelEdit->text().isEmpty()) {
        parameters << "-m" << substModelEdit->text();
    }

    CmdlineParamsParser::removeParameterNameAndValue("-bb", parameters);
    if (!ultrafastBootstrapEdit->text().isEmpty()) {
        parameters << "-bb" << ultrafastBootstrapEdit->text();
    }

    CmdlineParamsParser::removeParameterNameAndValue("-alrt", parameters);
    if (!alrtEdit->text().isEmpty()) {
        parameters << "-alrt" << alrtEdit->text();
    }

    parameters.removeOne("-asr");
    if (ancestralReconstructionCheckBox->isChecked()) {
        parameters << "-asr";
    }

    extraParametersTextEdit->setPlainText(parameters.join(' '));

    isInsideChangeCallback = false;
}

void IQTreeWidget::propagateTextParametersToWidgetValues() {
    U2OpStatusImpl os;
    QString parametersText = extraParametersTextEdit->toPlainText();
    QStringList parameters = CmdlineParamsParser::parse(os, parametersText);
    CHECK_OP(os, );

    substModelEdit->setText(CmdlineParamsParser::getParameterValue("-m", parameters));
    ultrafastBootstrapEdit->setText(CmdlineParamsParser::getParameterValue("-bb", parameters));
    alrtEdit->setText(CmdlineParamsParser::getParameterValue("-alrt", parameters));
    ancestralReconstructionCheckBox->setChecked(parameters.contains("-asr"));
}

}  // namespace U2
