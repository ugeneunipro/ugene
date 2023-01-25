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

#include "FastTreeWidget.h"

#include <QCheckBox>
#include <QDesktopServices>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2View/PhyTreeDisplayOptionsWidget.h>

#include "../iqtree/CmdlineParamsParser.h"

namespace U2 {

static constexpr const char* FAST_TREE_EXTRA_PARAMETERS_SETTINGS_KEY = "/extra-parameters";

FastTreeWidget::FastTreeWidget(const MultipleSequenceAlignment& msa, QWidget* parent)
    : CreatePhyTreeWidget(parent) {
    auto layout = new QVBoxLayout();
    setLayout(layout);

    auto tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("tabWidget");
    layout->addWidget(tabWidget);

    auto fastTreeOptionsWidget = new QWidget();
    auto fastTreeOptionsTabLayout = new QVBoxLayout();
    fastTreeOptionsWidget->setLayout(fastTreeOptionsTabLayout);

    auto hintLayout = new QHBoxLayout();

    auto hintLabel = new QLabel(tr("Command line options for FastTree:"));
    hintLabel->setToolTip(tr("Example: -fastest -pseudo\n Use double-quotes (\") for values with spaces."));
    hintLayout->addWidget(hintLabel);

    hintLayout->addStretch();

    auto docsButton = new QPushButton(tr("View all options"));
    docsButton->setToolTip(tr("Open official guide for FastTree in browser"));
    connect(docsButton, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("http://www.microbesonline.org/fasttree/#Usage"));
    });
    hintLayout->addWidget(docsButton);

    fastTreeOptionsTabLayout->addLayout(hintLayout);

    extraParametersTextEdit = new QPlainTextEdit();
    extraParametersTextEdit->setToolTip(tr("Use one parameter per one line"));
    extraParametersTextEdit->setObjectName("extraParametersTextEdit");
    auto savedParameters = AppContext::getSettings()->getValue(CreatePhyTreeWidget::getAppSettingsRoot() + FAST_TREE_EXTRA_PARAMETERS_SETTINGS_KEY).toStringList();
    extraParametersTextEdit->setPlainText(savedParameters.join("\n"));
    connect(extraParametersTextEdit, &QPlainTextEdit::textChanged, this, [this] {
        CHECK(!isInsideChangeCallback, );
        isInsideChangeCallback = true;
        propagateTextParametersToWidgetValues();
        isInsideChangeCallback = false;
    });
    fastTreeOptionsTabLayout->addWidget(extraParametersTextEdit);

    auto fastTreeOptionsComponentLayout = new QVBoxLayout();

    useFastestCheckBox = new QCheckBox(tr("Speed up the neighbor joining phase (>50,000 sequences)"));
    useFastestCheckBox->setObjectName("useFastestCheckBox");
    connect(useFastestCheckBox, &QCheckBox::stateChanged, this, &FastTreeWidget::propagateWidgetValuesToTextParameters);
    fastTreeOptionsComponentLayout->addWidget(useFastestCheckBox);

    usePseudoCountsCheckBox = new QCheckBox(tr("Use pseudo-counts (recommended for highly gapped sequences)"));
    usePseudoCountsCheckBox->setObjectName("usePseudoCountsCheckBox");
    connect(usePseudoCountsCheckBox, &QCheckBox::stateChanged, this, &FastTreeWidget::propagateWidgetValuesToTextParameters);
    fastTreeOptionsComponentLayout->addWidget(usePseudoCountsCheckBox);

    fastTreeOptionsTabLayout->addLayout(fastTreeOptionsComponentLayout);
    tabWidget->addTab(fastTreeOptionsWidget, tr("FastTree options"));

    isNucleotideAlignment = msa->getAlphabet()->isNucleic();

    displayOptionsWidget = new PhyTreeDisplayOptionsWidget();
    displayOptionsWidget->setContentsMargins(10, 10, 10, 10);
    tabWidget->addTab(displayOptionsWidget, tr("Display Options"));

    propagateTextParametersToWidgetValues();
}

static QString findIncompatibleParameter(const QStringList& parameterList, bool isNucleotideAlignment) {
    QStringList incompatibleParameterList = isNucleotideAlignment
                                                ? QStringList({"-lg", "-wag"})
                                                : QStringList({"-gtr"});
    for (const auto& p : qAsConst(parameterList)) {
        if (incompatibleParameterList.contains(p))
            return p;
    }
    return "";
}

void FastTreeWidget::fillSettings(CreatePhyTreeSettings& settings) {
    settings.extToolArguments.clear();
    QString parametersString = extraParametersTextEdit->toPlainText();
    U2OpStatusImpl os;
    QStringList parameterList = CmdlineParamsParser::parse(os, parametersString);
    if (os.hasError()) {
        QMessageBox::critical(this, L10N::errorTitle(), os.getError());
        return;
    }
    QString incompatibleParameter = findIncompatibleParameter(parameterList, isNucleotideAlignment);
    if (!incompatibleParameter.isEmpty()) {
        QString message = isNucleotideAlignment
                              ? tr("Parameter can't be used with nucleotide alignment: %1")
                              : tr("Parameter can't be used with amino acid alignment: %1");
        QMessageBox::critical(this, L10N::errorTitle(), message.arg(incompatibleParameter));
        return;
    }

    for (const QString& parameter : qAsConst(parameterList)) {
        settings.extToolArguments << parameter;
    }
    displayOptionsWidget->fillSettings(settings);
}

void FastTreeWidget::storeSettings() {
    QString parametersString = extraParametersTextEdit->toPlainText();
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::getAppSettingsRoot() + FAST_TREE_EXTRA_PARAMETERS_SETTINGS_KEY, parametersString);
    displayOptionsWidget->storeSettings();
}

void FastTreeWidget::restoreDefault() {
    AppContext::getSettings()->remove(CreatePhyTreeWidget::getAppSettingsRoot() + FAST_TREE_EXTRA_PARAMETERS_SETTINGS_KEY);
    extraParametersTextEdit->clear();
    displayOptionsWidget->restoreDefault();
}

bool FastTreeWidget::checkSettings(QString& message, const CreatePhyTreeSettings& settings) {
    return displayOptionsWidget->checkSettings(message, settings);
}

void FastTreeWidget::propagateWidgetValuesToTextParameters() {
    CHECK(!isInsideChangeCallback, );
    U2OpStatusImpl os;
    QStringList oldParameters = CmdlineParamsParser::parse(os, extraParametersTextEdit->toPlainText());
    CHECK_OP(os, );

    isInsideChangeCallback = true;

    QStringList parameters = oldParameters;

    if (isNucleotideAlignment) {
        parameters.removeOne("-nt");
        parameters << "-nt";
    }

    parameters.removeOne("-fastest");
    if (useFastestCheckBox->isChecked()) {
        parameters << "-fastest";
    }

    parameters.removeOne("-pseudo");
    if (usePseudoCountsCheckBox->isChecked()) {
        parameters << "-pseudo";
    }
    extraParametersTextEdit->setPlainText(parameters.join(' '));

    isInsideChangeCallback = false;
}

void FastTreeWidget::propagateTextParametersToWidgetValues() {
    U2OpStatusImpl os;
    QString parametersText = extraParametersTextEdit->toPlainText();
    QStringList parameters = CmdlineParamsParser::parse(os, parametersText);
    CHECK_OP(os, );

    useFastestCheckBox->setChecked(parameters.contains("-fastest"));
    usePseudoCountsCheckBox->setChecked(parameters.contains("-pseudo"));
}

}  // namespace U2
