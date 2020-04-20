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

#ifndef _U2_CREATE_CMDLINE_BASED_WORKER_WIZARD_H_
#define _U2_CREATE_CMDLINE_BASED_WORKER_WIZARD_H_

#include <QStandardItem>
#include <QWizard>

#include "ui_CreateCmdlineBasedWorkerWizardCommandPage.h"
#include "ui_CreateCmdlineBasedWorkerWizardElementAppearancePage.h"
#include "ui_CreateCmdlineBasedWorkerWizardGeneralSettingsPage.h"
#include "ui_CreateCmdlineBasedWorkerWizardInputDataPage.h"
#include "ui_CreateCmdlineBasedWorkerWizardOutputDataPage.h"
#include "ui_CreateCmdlineBasedWorkerWizardParametersPage.h"
#include "ui_CreateCmdlineBasedWorkerWizardSummaryPage.h"
#include "library/CfgExternalToolModel.h"

namespace U2 {

class ExternalProcessConfig;
class ExternalTool;
class ExternalToolSelectComboBox;

class CreateCmdlineBasedWorkerWizard : public QWizard {
    Q_OBJECT
public:
    explicit CreateCmdlineBasedWorkerWizard(SchemaConfig *schemaConfig, QWidget* parent = nullptr);
    explicit CreateCmdlineBasedWorkerWizard(SchemaConfig* schemaConfig, ExternalProcessConfig* initialConfig, QWidget* parent = nullptr);
    ~CreateCmdlineBasedWorkerWizard() override;

    ExternalProcessConfig* takeConfig();

    static void saveConfig(ExternalProcessConfig* config);
    static bool isRequiredToRemoveElementFromScene(ExternalProcessConfig* actualConfig, ExternalProcessConfig* newConfig);

    static const QString PAGE_TITLE_STYLE_SHEET;

    static const QString ATTRIBUTES_DATA_FIELD;
    static const QString ATTRIBUTES_IDS_FIELD;
    static const QString ATTRIBUTES_NAMES_FIELD;
    static const QString COMMAND_TEMPLATE_DESCRIPTION_FIELD;
    static const QString COMMAND_TEMPLATE_FIELD;
    static const QString CUSTOM_TOOL_PATH_FIELD;
    static const QString INPUTS_DATA_FIELD;
    static const QString INPUTS_IDS_FIELD;
    static const QString INPUTS_NAMES_FIELD;
    static const QString INTEGRATED_TOOL_ID_FIELD;
    static const QString USE_INTEGRATED_TOOL_FIELD;
    static const QString OUTPUTS_DATA_FIELD;
    static const QString OUTPUTS_IDS_FIELD;
    static const QString OUTPUTS_NAMES_FIELD;
    static const QString WORKER_DESCRIPTION_FIELD;
    static const QString WORKER_ID_FIELD;
    static const QString WORKER_NAME_FIELD;

private slots:
    void accept() override;

private:
    void init();
    ExternalProcessConfig* createActualConfig() const;

    ExternalProcessConfig* initialConfig;
    ExternalProcessConfig* config;
    SchemaConfig* schemaConfig;
};

class CreateCmdlineBasedWorkerWizardGeneralSettingsPage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardGeneralSettingsPage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardGeneralSettingsPage(ExternalProcessConfig* initialConfig);

    void initializePage() override;
    bool isComplete() const override;
    bool validatePage() override;

signals:
    void si_integratedToolChanged();

private slots:
    void sl_browse();
    void sl_integratedToolChanged();

private:
    static void makeUniqueWorkerName(QString& name);

    ExternalProcessConfig* initialConfig;
    ExternalToolSelectComboBox *cbIntegratedTools;

    static char const* const INTEGRATED_TOOL_ID_PROPERTY;
    static char const* const WORKER_ID_PROPERTY;
    static const QString LOD_DOMAIN;
};

class CreateCmdlineBasedWorkerWizardInputDataPage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardInputDataPage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardInputDataPage(ExternalProcessConfig* initialConfig);

    void initializePage() override;
    bool isComplete() const override;

signals:
    void si_inputsChanged();

private slots:
    void sl_addInput();
    void sl_deleteInput();
    void sl_updateInputsProperties();

private:
    ExternalProcessConfig* initialConfig;
    CfgExternalToolModel* inputsModel;

    static char const* const INPUTS_DATA_PROPERTY;
    static char const* const INPUTS_IDS_PROPERTY;
    static char const* const INPUTS_NAMES_PROPERTY;
};

class CreateCmdlineBasedWorkerWizardParametersPage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardParametersPage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardParametersPage(ExternalProcessConfig* initialConfig, SchemaConfig* schemaConfig);

    void initializePage() override;
    bool isComplete() const override;

signals:
    void si_attributesChanged();

private slots:
    void sl_addAttribute();
    void sl_deleteAttribute();
    void sl_updateAttributes();

private:
    static void initAttributesModel(QAbstractItemModel* model, const QList<AttributeConfig>& attributeConfigs);

    ExternalProcessConfig* initialConfig;

    CfgExternalToolModelAttributes* model;

    static char const* const ATTRIBUTES_DATA_PROPERTY;
    static char const* const ATTRIBUTES_IDS_PROPERTY;
    static char const* const ATTRIBUTES_NAMES_PROPERTY;
};

class CreateCmdlineBasedWorkerWizardOutputDataPage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardOutputDataPage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardOutputDataPage(ExternalProcessConfig* initialConfig);

    void initializePage() override;
    bool isComplete() const override;

signals:
    void si_outputsChanged();

private slots:
    void sl_addOutput();
    void sl_deleteOutput();
    void sl_updateOutputsProperties();

private:
    ExternalProcessConfig* initialConfig;
    CfgExternalToolModel* outputsModel;

    static char const* const OUTPUTS_DATA_PROPERTY;
    static char const* const OUTPUTS_IDS_PROPERTY;
    static char const* const OUTPUTS_NAMES_PROPERTY;
};

class CommandValidator : public QObject {
    Q_OBJECT
public:
    CommandValidator(QTextEdit* textEdit);

private slots:
    void sl_textChanged();

private:
    QTextEdit* textEdit;
};

class CreateCmdlineBasedWorkerWizardCommandPage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardCommandPage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardCommandPage(ExternalProcessConfig* initialConfig);

    void initializePage() override;
    bool isComplete() const override;
    bool validatePage() override;

private:
    ExternalProcessConfig* initialConfig;
};

class CreateCmdlineBasedWorkerWizardElementAppearancePage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardElementAppearancePage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardElementAppearancePage(ExternalProcessConfig* initialConfig);

    void initializePage() override;

private:
    ExternalProcessConfig* initialConfig;
};

class CreateCmdlineBasedWorkerWizardSummaryPage : public QWizardPage, private Ui_CreateCmdlineBasedWorkerWizardSummaryPage {
    Q_OBJECT
public:
    CreateCmdlineBasedWorkerWizardSummaryPage();

private:
    void showEvent(QShowEvent* event) override;
};

class ExternalToolSelectComboBox : public QComboBox {
    Q_OBJECT
public:
    ExternalToolSelectComboBox(QWidget* parent = nullptr);

    virtual void hidePopup() override;

    void modifyMenuAccordingToData(const QString& data);
    void setDefaultMenuValue(const QString& defaultValue);
private:
    void addSupportedToolsPopupMenu();
    void initExternalTools();
    void initPopupMenu();
    void separateSupportedAndCustomTools(const QList<ExternalTool*>& tools);
    void makeSupportedToolsMapFromList(const QList<ExternalTool*>& tools);
    void sortCustomToolsList();
    void sortSupportedToolsMap();
    void initFirstClickableRow();

    // exclude module and runner tools
    static void excludeNotSuitableTools(QList<ExternalTool*>& tools);

    QMap<QString, QList<ExternalTool*> > supportedTools;
    QList<ExternalTool*> customTools;
    QString firstClickableRowData;

    static const QString SHOW_ALL_TOOLS;
    static const QString SHOW_CUSTOM_TOOLS;
};

}
// namespace U2

#endif // _U2_CREATE_CMDLINE_BASED_WORKER_WIZARD_H_
