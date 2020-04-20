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

#include <QAbstractButton>
#include <QMessageBox>
#include <QStandardItemModel>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/ScriptingToolRegistry.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GroupedComboBoxDelegate.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "CreateCmdlineBasedWorkerWizard.h"
#include "WorkflowEditorDelegates.h"
#include "util/CustomWorkerUtils.h"
#include "util/WorkerNameValidator.h"

namespace U2 {

/**********************************************/
/* CreateCmdlineBasedWorkerWizard */
/**********************************************/

#ifdef Q_OS_MAC
const QString CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET = "QLabel {margin-left: -5px; margin-bottom: -5px; margin-top: -5px; font-size: 20pt; padding-bottom: 10px; color: #0c3762}";
#else
const QString CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET = "QLabel {margin-left: -6px; margin-bottom: -5px; margin-top: -5px; font-size: 16pt; padding-bottom: 10px; color: #0c3762}";
#endif

const QString CreateCmdlineBasedWorkerWizard::ATTRIBUTES_DATA_FIELD = "attributes-data";
const QString CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD = "attributes-ids";
const QString CreateCmdlineBasedWorkerWizard::ATTRIBUTES_NAMES_FIELD = "attributes-names";
const QString CreateCmdlineBasedWorkerWizard::COMMAND_TEMPLATE_DESCRIPTION_FIELD = "command-template-description";
const QString CreateCmdlineBasedWorkerWizard::COMMAND_TEMPLATE_FIELD = "command-template";
const QString CreateCmdlineBasedWorkerWizard::CUSTOM_TOOL_PATH_FIELD = "custom-tool-path";
const QString CreateCmdlineBasedWorkerWizard::INPUTS_DATA_FIELD = "inputs-data";
const QString CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD = "inputs-ids";
const QString CreateCmdlineBasedWorkerWizard::INPUTS_NAMES_FIELD = "inputs-names";
const QString CreateCmdlineBasedWorkerWizard::INTEGRATED_TOOL_ID_FIELD = "integrated-tool-id";
const QString CreateCmdlineBasedWorkerWizard::OUTPUTS_DATA_FIELD = "outputs-data";
const QString CreateCmdlineBasedWorkerWizard::OUTPUTS_IDS_FIELD = "outputs-ids";
const QString CreateCmdlineBasedWorkerWizard::OUTPUTS_NAMES_FIELD = "outputs-names";
const QString CreateCmdlineBasedWorkerWizard::USE_INTEGRATED_TOOL_FIELD = "use-integrated-tool";
const QString CreateCmdlineBasedWorkerWizard::WORKER_DESCRIPTION_FIELD = "worker-description";
const QString CreateCmdlineBasedWorkerWizard::WORKER_ID_FIELD = "worker-id";
const QString CreateCmdlineBasedWorkerWizard::WORKER_NAME_FIELD = "worker-name";

CreateCmdlineBasedWorkerWizard::CreateCmdlineBasedWorkerWizard(SchemaConfig* _schemaConfig, QWidget *_parent)
    : QWizard(_parent),
    initialConfig(nullptr),
    config(nullptr),
    schemaConfig(_schemaConfig)
{
    GCOUNTER(cvar, tvar, "\"Configure Element with External Tool\" dialog is opened for creating");
    init();
}

CreateCmdlineBasedWorkerWizard::CreateCmdlineBasedWorkerWizard(SchemaConfig* _schemaConfig, ExternalProcessConfig *_initialConfig, QWidget *_parent)
    : QWizard(_parent),
    initialConfig(nullptr),
    config(nullptr),
    schemaConfig(_schemaConfig)
{
    SAFE_POINT(nullptr != _initialConfig, "Initial config of the element to edit is nullptr", );
    GCOUNTER(cvar, tvar, "\"Configure Element with External Tool\" dialog is opened for editing");
    initialConfig = new ExternalProcessConfig(*_initialConfig);
    init();
}

CreateCmdlineBasedWorkerWizard::~CreateCmdlineBasedWorkerWizard() {
    delete initialConfig;
    delete config;
}

ExternalProcessConfig *CreateCmdlineBasedWorkerWizard::takeConfig() {
    ExternalProcessConfig *toReturn = nullptr;
    qSwap(toReturn, config);
    return toReturn;
}

void CreateCmdlineBasedWorkerWizard::saveConfig(ExternalProcessConfig *config) {
    const QString serializedConfig = HRSchemaSerializer::actor2String(config);
    const QString dirPath = WorkflowSettings::getExternalToolDirectory();
    const QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(dirPath);
    }

    if (QFileInfo(config->filePath).dir().absolutePath() != dir.absolutePath()) {
        config->filePath = dirPath + GUrlUtils::fixFileName(config->name) + ".etc";
    }
    config->filePath = GUrlUtils::rollFileName(config->filePath, "_");

    QFile file(config->filePath);
    file.open(QIODevice::WriteOnly);
    file.write(serializedConfig.toLatin1());
    file.close();
}

bool CreateCmdlineBasedWorkerWizard::isRequiredToRemoveElementFromScene(ExternalProcessConfig* actualConfig, ExternalProcessConfig* newConfig) {
    CHECK(nullptr != actualConfig, false)
        CHECK(nullptr != newConfig, false);

    bool result = (newConfig->inputs != actualConfig->inputs)
        || (newConfig->outputs != actualConfig->outputs)
        || (newConfig->attrs != actualConfig->attrs);

    return result;
}

namespace {

static const int UNNECCESSARY_ARGUMENT = 0;

QString removeEmptyLines(const QString &str) {
    QStringList res;
    foreach(const QString &s, str.split(QRegularExpression("(\n|\r)"))) {
        if (!s.trimmed().isEmpty()) {
            res.append(s);
        }
    }
    return res.join("\r\n");
}

void initDataModel(QAbstractItemModel *model, const QList<DataConfig> &dataConfigs) {
    model->removeRows(0, model->rowCount());

    int row = 0;
    const int ignoredRowNumber = 0;
    foreach(const DataConfig &dataConfig, dataConfigs) {
        model->insertRow(ignoredRowNumber, QModelIndex());

        QModelIndex index = model->index(row, CfgExternalToolModel::COLUMN_NAME);
        model->setData(index, dataConfig.attrName);

        index = model->index(row, CfgExternalToolModel::COLUMN_ID);
        model->setData(index, dataConfig.attributeId);

        index = model->index(row, CfgExternalToolModel::COLUMN_DATA_TYPE);
        model->setData(index, dataConfig.type);

        index = model->index(row, CfgExternalToolModel::COLUMN_FORMAT);
        model->setData(index, dataConfig.format);

        index = model->index(row, CfgExternalToolModel::COLUMN_DESCRIPTION);
        model->setData(index, dataConfig.description);

        row++;
    }
}

bool checkNamesAndIds(const QStringList &names, const QStringList &ids) {
    bool res = true;

    foreach (const QString &id, ids) {
        if (id.isEmpty()) {
            res = false;
        }
    }

    foreach (const QString &name, names) {
        if (name.isEmpty()) {
            res = false;
        }
    }

    const bool areThereDuplicates = (ids.toSet().size() != ids.size());
    if (areThereDuplicates) {
        res = false;
    }

    return res;
}

}

void CreateCmdlineBasedWorkerWizard::accept() {
    QScopedPointer<ExternalProcessConfig> actualConfig(createActualConfig());
    CHECK(!actualConfig.isNull(), );

    if (isRequiredToRemoveElementFromScene(initialConfig, actualConfig.data())) {
        int res = QMessageBox::question(this,
                                        tr("Warning"),
                                        tr("You've changed the element structure (input data, parameters, or output data).\n\n"
                                           "If you apply the changes, all elements of this type will be removed from the scene."
                                           "You can then add a new such element to the scene by dragging it from the \"Custom Elements with External Tools\" group of the \"Elements\" palette.\n\n"
                                           "Would you like to apply the changes ? "),
                                        QMessageBox::Apply | QMessageBox::Cancel | QMessageBox::Reset,
                                        QMessageBox::Apply);
        if (QMessageBox::Cancel == res) {
            return;
        } else if (QMessageBox::Reset == res) {
            restart();
            return;
        }
    }
    if (nullptr != initialConfig) {
        GCOUNTER(cvar, tvar, "\"Configure Element with External Tool\" dialog is finished for editing");
    } else {
        GCOUNTER(cvar1, tvar1, "\"Configure Element with External Tool\" dialog is finished for creating");
    }
    config = actualConfig.take();
    done(QDialog::Accepted);
}

void CreateCmdlineBasedWorkerWizard::init() {
    addPage(new CreateCmdlineBasedWorkerWizardGeneralSettingsPage(initialConfig));
    addPage(new CreateCmdlineBasedWorkerWizardInputDataPage(initialConfig));
    addPage(new CreateCmdlineBasedWorkerWizardParametersPage(initialConfig, schemaConfig));
    addPage(new CreateCmdlineBasedWorkerWizardOutputDataPage(initialConfig));
    addPage(new CreateCmdlineBasedWorkerWizardCommandPage(initialConfig));
    addPage(new CreateCmdlineBasedWorkerWizardElementAppearancePage(initialConfig));
    addPage(new CreateCmdlineBasedWorkerWizardSummaryPage());

    setWindowTitle(tr("Configure Element with External Tool"));
    setObjectName("CreateExternalProcessWorkerDialog");
    setWizardStyle(ClassicStyle);
    setOption(IndependentPages);

    setOption(QWizard::HaveHelpButton, true);
    new U2::HelpButton(this, this->button(QWizard::HelpButton), "28967044");

    DialogUtils::setWizardMinimumSize(this, QSize(780, 350));
}

ExternalProcessConfig *CreateCmdlineBasedWorkerWizard::createActualConfig() const {
    ExternalProcessConfig *config = new ExternalProcessConfig();
    config->id = field(WORKER_ID_FIELD).toString();
    config->name = field(WORKER_NAME_FIELD).toString();
    config->description = removeEmptyLines(field(WORKER_DESCRIPTION_FIELD).toString());
    config->templateDescription = removeEmptyLines(field(COMMAND_TEMPLATE_DESCRIPTION_FIELD).toString());
    config->inputs = field(INPUTS_DATA_FIELD).value<QList<DataConfig> >();
    config->outputs = field(OUTPUTS_DATA_FIELD).value<QList<DataConfig> >();
    config->attrs = field(ATTRIBUTES_DATA_FIELD).value<QList<AttributeConfig> >();
    config->cmdLine = field(COMMAND_TEMPLATE_FIELD).toString();
    config->filePath = WorkflowSettings::getExternalToolDirectory() + GUrlUtils::fixFileName(config->name) + ".etc";
    config->useIntegratedTool = field(USE_INTEGRATED_TOOL_FIELD).toBool();
    config->integratedToolId = field(INTEGRATED_TOOL_ID_FIELD).toString();
    config->customToolPath = QDir::fromNativeSeparators(field(CUSTOM_TOOL_PATH_FIELD).toString());
    return config;
}

/**********************************************/
/* CreateCmdlineBasedWorkerWizardGeneralSettingsPage */
/**********************************************/

char const * const CreateCmdlineBasedWorkerWizardGeneralSettingsPage::INTEGRATED_TOOL_ID_PROPERTY = "integrated-tool-id-property";
char const * const CreateCmdlineBasedWorkerWizardGeneralSettingsPage::WORKER_ID_PROPERTY = "worker-id-property";
const QString CreateCmdlineBasedWorkerWizardGeneralSettingsPage::LOD_DOMAIN = "CreateCmdlineBasedWorkerWizard: select custom tool path";

CreateCmdlineBasedWorkerWizardGeneralSettingsPage::CreateCmdlineBasedWorkerWizardGeneralSettingsPage(ExternalProcessConfig* _initialConfig)
    : QWizardPage(nullptr),
      initialConfig(_initialConfig)
{
    setupUi(this);

    cbIntegratedTools = new ExternalToolSelectComboBox(gbTool);
    cbIntegratedTools->setEnabled(false);
    cbIntegratedTools->setObjectName("cbIntegratedTools");
    containerLayout->addWidget(cbIntegratedTools);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);
    leName->setValidator(new QRegularExpressionValidator(WorkflowEntityValidator::ACCEPTABLE_NAME, leName));

    connect(leToolPath, SIGNAL(textChanged(const QString&)), SIGNAL(completeChanged()));
    connect(tbBrowse, SIGNAL(clicked()), SLOT(sl_browse()));
    connect(rbIntegratedTool, SIGNAL(toggled(bool)), SIGNAL(completeChanged()));
    connect(rbIntegratedTool, SIGNAL(toggled(bool)), SLOT(sl_integratedToolChanged()));
    connect(rbIntegratedTool, SIGNAL(toggled(bool)), cbIntegratedTools, SLOT(setEnabled(bool)));
    connect(cbIntegratedTools, SIGNAL(currentIndexChanged(int)), SLOT(sl_integratedToolChanged()));

    registerField(CreateCmdlineBasedWorkerWizard::WORKER_NAME_FIELD + "*", leName);
    registerField(CreateCmdlineBasedWorkerWizard::WORKER_ID_FIELD, this, WORKER_ID_PROPERTY);
    registerField(CreateCmdlineBasedWorkerWizard::USE_INTEGRATED_TOOL_FIELD, rbIntegratedTool);
    registerField(CreateCmdlineBasedWorkerWizard::CUSTOM_TOOL_PATH_FIELD, leToolPath);
    registerField(CreateCmdlineBasedWorkerWizard::INTEGRATED_TOOL_ID_FIELD, this, INTEGRATED_TOOL_ID_PROPERTY, SIGNAL(si_integratedToolChanged()));
}

void CreateCmdlineBasedWorkerWizardGeneralSettingsPage::initializePage() {
    if (nullptr != initialConfig) {
        leName->setText(initialConfig->name);
        rbIntegratedTool->setChecked(initialConfig->useIntegratedTool);
        leToolPath->setText(QDir::toNativeSeparators(initialConfig->customToolPath));
        if (AppContext::getExternalToolRegistry()->getById(initialConfig->integratedToolId) == nullptr && rbIntegratedTool->isChecked()) {
            QObjectScopedPointer<QMessageBox> warningBox(new QMessageBox(
                QMessageBox::Warning,
                initialConfig->name,
                tr("UGENE can't find the tool specified in this element. Please specify another tool."),
                QMessageBox::Close));
            warningBox->exec();
            rbCustomTool->setChecked(true);
        } else if (!initialConfig->integratedToolId.isEmpty()) {
            cbIntegratedTools->setDefaultMenuValue(initialConfig->integratedToolId);
        }
    } else {
        QString name = "Custom Element";
        makeUniqueWorkerName(name);
        leName->setText(name);
    }
    sl_integratedToolChanged();
}

bool CreateCmdlineBasedWorkerWizardGeneralSettingsPage::isComplete() const {
    if (rbCustomTool->isChecked() && leToolPath->text().isEmpty()) {
        return false;
    }
    return QWizardPage::isComplete();
}

bool CreateCmdlineBasedWorkerWizardGeneralSettingsPage::validatePage() {
    QString name = field(CreateCmdlineBasedWorkerWizard::WORKER_NAME_FIELD).toString();

    const QMap<Descriptor, QList<ActorPrototype *> > groups = Workflow::WorkflowEnv::getProtoRegistry()->getProtos();
    QStringList reservedNames;
    QStringList reservedIds;

    foreach (const QList<ActorPrototype *> &group, groups) {
        foreach (ActorPrototype *proto, group) {
            reservedNames << proto->getDisplayName();
            reservedIds << proto->getId();
        }
    }

    if (nullptr == initialConfig || initialConfig->name != name) {
        name = WorkflowUtils::createUniqueString(name, " ", reservedNames);
        setField(CreateCmdlineBasedWorkerWizard::WORKER_NAME_FIELD, name);
    }

    QString id;
    if (nullptr == initialConfig) {
        id = WorkflowUtils::createUniqueString(WorkflowUtils::generateIdFromName(name), "-", reservedIds);
    } else {
        id = initialConfig->id;
    }

    setProperty(WORKER_ID_PROPERTY, id);
    return true;
}

void CreateCmdlineBasedWorkerWizardGeneralSettingsPage::sl_browse() {
    LastUsedDirHelper lod(LOD_DOMAIN);
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select an executable file"), lod.dir);
    CHECK(!lod.url.isEmpty(), );
    leToolPath->setText(QDir::toNativeSeparators(lod.url));
}

void CreateCmdlineBasedWorkerWizardGeneralSettingsPage::sl_integratedToolChanged() {
    setProperty(INTEGRATED_TOOL_ID_PROPERTY, cbIntegratedTools->currentData());
    emit si_integratedToolChanged();
}

void CreateCmdlineBasedWorkerWizardGeneralSettingsPage::makeUniqueWorkerName(QString& name) {
    const QMap<Descriptor, QList<ActorPrototype *> > groups = Workflow::WorkflowEnv::getProtoRegistry()->getProtos();
    QStringList reservedNames;
    foreach(const QList<ActorPrototype *> &group, groups) {
        foreach(ActorPrototype *proto, group) {
            reservedNames << proto->getDisplayName();
        }
    }
    name = WorkflowUtils::createUniqueString(name, " ", reservedNames);
}

/**********************************************/
/* CreateCmdlineBasedWorkerWizardInputDataPage */
/**********************************************/

char const * const CreateCmdlineBasedWorkerWizardInputDataPage::INPUTS_DATA_PROPERTY = "inputs-data-property";
char const * const CreateCmdlineBasedWorkerWizardInputDataPage::INPUTS_IDS_PROPERTY = "inputs-ids-property";
char const * const CreateCmdlineBasedWorkerWizardInputDataPage::INPUTS_NAMES_PROPERTY = "inputs-names-property";

CreateCmdlineBasedWorkerWizardInputDataPage::CreateCmdlineBasedWorkerWizardInputDataPage(ExternalProcessConfig *_initialConfig)
    : QWizardPage(nullptr),
      initialConfig(_initialConfig)
{
    setupUi(this);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);

    connect(pbAddInput, SIGNAL(clicked()), SLOT(sl_addInput()));
    connect(pbAddInput, SIGNAL(clicked()), tvInput, SLOT(setFocus()));
    connect(pbDeleteInput, SIGNAL(clicked()), SLOT(sl_deleteInput()));
    connect(pbDeleteInput, SIGNAL(clicked()), tvInput, SLOT(setFocus()));
    connect(this, SIGNAL(si_inputsChanged()), SIGNAL(completeChanged()));

    inputsModel = new CfgExternalToolModel(CfgExternalToolModel::Input, tvInput);
    connect(inputsModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(sl_updateInputsProperties()));
    connect(inputsModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(sl_updateInputsProperties()));
    connect(inputsModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), SLOT(sl_updateInputsProperties()));

    tvInput->setModel(inputsModel);
    tvInput->setItemDelegate(new ProxyDelegate());
    tvInput->horizontalHeader()->setStretchLastSection(true);
    tvInput->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    /*
    QFontMetrics fm = QFontMetrics(tvInput->font());
    int columnWidth = static_cast<int>(fm.width(SEQ_WITH_ANNS) * 1.5);
    tvInput->setColumnWidth(1, columnWidth);
    */
    registerField(CreateCmdlineBasedWorkerWizard::INPUTS_DATA_FIELD, this, INPUTS_DATA_PROPERTY, SIGNAL(si_inputsChanged()));
    registerField(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD, this, INPUTS_IDS_PROPERTY);
    registerField(CreateCmdlineBasedWorkerWizard::INPUTS_NAMES_FIELD, this, INPUTS_NAMES_PROPERTY);
    this->duplicateInputsWarningLabel->setVisible(false);
}

void CreateCmdlineBasedWorkerWizardInputDataPage::initializePage() {
    CHECK(nullptr != initialConfig, );
    initDataModel(inputsModel, initialConfig->inputs);
}

bool CreateCmdlineBasedWorkerWizardInputDataPage::isComplete() const {
    const QStringList ids = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList();
    const QStringList names = field(CreateCmdlineBasedWorkerWizard::INPUTS_NAMES_FIELD).toStringList();
    return checkNamesAndIds(names, ids);
}

void CreateCmdlineBasedWorkerWizardInputDataPage::sl_addInput() {
    const int ignoredRowNumber = 0;
    inputsModel->insertRow(ignoredRowNumber, QModelIndex());
    tvInput->setCurrentIndex(inputsModel->index(inputsModel->rowCount(QModelIndex()) - 1, 0));
}

void CreateCmdlineBasedWorkerWizardInputDataPage::sl_deleteInput() {
    inputsModel->removeRow(tvInput->currentIndex().row());
}

void CreateCmdlineBasedWorkerWizardInputDataPage::sl_updateInputsProperties() {
    QStringList ids;
    QStringList names;
    QList<DataConfig> data;
    bool hasDuplicates = false;
    foreach (CfgExternalToolItem *item, inputsModel->getItems()) {
        data << item->itemData;
        QString id = item->getId();
        hasDuplicates = hasDuplicates || (!id.isEmpty() && ids.contains(id));
        ids << id;
        names << item->getName();
    }
    setProperty(INPUTS_DATA_PROPERTY, QVariant::fromValue<QList<DataConfig> >(data));
    setProperty(INPUTS_IDS_PROPERTY, ids);
    setProperty(INPUTS_NAMES_PROPERTY, names);

    this->duplicateInputsWarningLabel->setVisible(hasDuplicates);

    emit si_inputsChanged();
}

/**********************************************/
/* CreateCmdlineBasedWorkerWizardParametersPage */
/**********************************************/

char const * const CreateCmdlineBasedWorkerWizardParametersPage::ATTRIBUTES_DATA_PROPERTY = "attributes-data-property";
char const * const CreateCmdlineBasedWorkerWizardParametersPage::ATTRIBUTES_IDS_PROPERTY = "attributes-ids-property";
char const * const CreateCmdlineBasedWorkerWizardParametersPage::ATTRIBUTES_NAMES_PROPERTY = "attributes-names-property";

CreateCmdlineBasedWorkerWizardParametersPage::CreateCmdlineBasedWorkerWizardParametersPage(ExternalProcessConfig *_initialConfig, SchemaConfig *_schemaConfig)
    : QWizardPage(nullptr),
    initialConfig(_initialConfig)
{
    setupUi(this);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);

    connect(pbAdd, SIGNAL(clicked()), SLOT(sl_addAttribute()));
    connect(pbAdd, SIGNAL(clicked()), tvAttributes, SLOT(setFocus()));
    connect(pbDelete, SIGNAL(clicked()), SLOT(sl_deleteAttribute()));
    connect(pbDelete, SIGNAL(clicked()), tvAttributes, SLOT(setFocus()));
    connect(this, SIGNAL(si_attributesChanged()), SIGNAL(completeChanged()));

    model = new CfgExternalToolModelAttributes(_schemaConfig);
    connect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(sl_updateAttributes()));
    connect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(sl_updateAttributes()));
    connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), SLOT(sl_updateAttributes()));

    tvAttributes->setModel(model);
    tvAttributes->setItemDelegate(new ProxyDelegate());
    tvAttributes->horizontalHeader()->setStretchLastSection(true);
    tvAttributes->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    registerField(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_DATA_FIELD, this, ATTRIBUTES_DATA_PROPERTY, SIGNAL(si_attributesChanged()));
    registerField(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD, this, ATTRIBUTES_IDS_PROPERTY);
    registerField(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_NAMES_FIELD, this, ATTRIBUTES_NAMES_PROPERTY);
    this->duplicateParametersWarningLabel->setVisible(false);
}

void CreateCmdlineBasedWorkerWizardParametersPage::initializePage() {
    CHECK(nullptr != initialConfig, );
    initAttributesModel(model, initialConfig->attrs);
}

bool CreateCmdlineBasedWorkerWizardParametersPage::isComplete() const {
    const QStringList ids = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList() +
                            field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD).toStringList();
    const QStringList names = field(CreateCmdlineBasedWorkerWizard::INPUTS_NAMES_FIELD).toStringList() +
                              field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_NAMES_FIELD).toStringList();
    return checkNamesAndIds(names, ids);
}

void CreateCmdlineBasedWorkerWizardParametersPage::sl_addAttribute() {
    const int ignoredRowNumber = 0;
    model->insertRow(ignoredRowNumber, QModelIndex());
    tvAttributes->setCurrentIndex(model->index(model->rowCount(QModelIndex()) - 1, 0));
}

void CreateCmdlineBasedWorkerWizardParametersPage::sl_deleteAttribute() {
    model->removeRow(tvAttributes->currentIndex().row());
}

void CreateCmdlineBasedWorkerWizardParametersPage::sl_updateAttributes() {
    QStringList ids;
    QStringList names;
    QList<AttributeConfig> data;
    // this is the second page in the wizard. Check for duplicates with the prev. page ids (inputs)
    QStringList inputIds = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList();
    bool hasDuplicates = false;
    foreach (AttributeItem *item, model->getItems()) {
        AttributeConfig attributeConfig;
        attributeConfig.attributeId = item->getId();
        attributeConfig.attrName = item->getName();
        attributeConfig.type = item->getDataType();
        attributeConfig.defaultValue = item->getDefaultValue().toString();
        attributeConfig.description = item->getDescription();
        if (attributeConfig.isOutputUrl()) {
            attributeConfig.flags |= AttributeConfig::AddToDashboard;
            if (attributeConfig.isFile()) {
                attributeConfig.flags |= AttributeConfig::OpenWithUgene;
            }
        }
        data << attributeConfig;
        QString id = item->getId();
        hasDuplicates = hasDuplicates || (!id.isEmpty() && (ids.contains(id) || inputIds.contains(id)));
        ids << id;
        names << item->getName();
    }
    setProperty(ATTRIBUTES_DATA_PROPERTY, QVariant::fromValue<QList<AttributeConfig> >(data));
    setProperty(ATTRIBUTES_IDS_PROPERTY, ids);
    setProperty(ATTRIBUTES_NAMES_PROPERTY, names);

    this->duplicateParametersWarningLabel->setVisible(hasDuplicates);

    emit si_attributesChanged();
}

void CreateCmdlineBasedWorkerWizardParametersPage::initAttributesModel(QAbstractItemModel *model, const QList<AttributeConfig> &attributeConfigs) {
    model->removeRows(0, model->rowCount());

    int row = 0;
    const int ignoredRowNumber = 0;
    foreach(const AttributeConfig &attributeConfig, attributeConfigs) {
        model->insertRow(ignoredRowNumber, QModelIndex());

        QModelIndex index = model->index(row, CfgExternalToolModelAttributes::COLUMN_NAME);
        model->setData(index, attributeConfig.attrName);

        index = model->index(row, CfgExternalToolModelAttributes::COLUMN_ID);
        model->setData(index, attributeConfig.attributeId);

        index = model->index(row, CfgExternalToolModelAttributes::COLUMN_DATA_TYPE);
        model->setData(index, attributeConfig.type);

        index = model->index(row, CfgExternalToolModelAttributes::COLUMN_DEFAULT_VALUE);
        model->setData(index, attributeConfig.defaultValue);

        index = model->index(row, CfgExternalToolModelAttributes::COLUMN_DESCRIPTION);
        model->setData(index, attributeConfig.description);

        row++;
    }
}

/**********************************************/
/* CreateCmdlineBasedWorkerWizardOutputDataPage */
/**********************************************/

char const * const CreateCmdlineBasedWorkerWizardOutputDataPage::OUTPUTS_DATA_PROPERTY = "outputs-data-property";
char const * const CreateCmdlineBasedWorkerWizardOutputDataPage::OUTPUTS_IDS_PROPERTY = "outputs-ids-property";
char const * const CreateCmdlineBasedWorkerWizardOutputDataPage::OUTPUTS_NAMES_PROPERTY = "outputs-names-property";

CreateCmdlineBasedWorkerWizardOutputDataPage::CreateCmdlineBasedWorkerWizardOutputDataPage(ExternalProcessConfig *_initialConfig)
    : QWizardPage(nullptr),
      initialConfig(_initialConfig)
{
    setupUi(this);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);

    connect(pbAddOutput, SIGNAL(clicked()), SLOT(sl_addOutput()));
    connect(pbAddOutput, SIGNAL(clicked()), tvOutput, SLOT(setFocus()));
    connect(pbDeleteOutput, SIGNAL(clicked()), SLOT(sl_deleteOutput()));
    connect(pbDeleteOutput, SIGNAL(clicked()), tvOutput, SLOT(setFocus()));
    connect(this, SIGNAL(si_outputsChanged()), SIGNAL(completeChanged()));

    outputsModel = new CfgExternalToolModel(CfgExternalToolModel::Output);
    connect(outputsModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(sl_updateOutputsProperties()));
    connect(outputsModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(sl_updateOutputsProperties()));
    connect(outputsModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), SLOT(sl_updateOutputsProperties()));

    tvOutput->setModel(outputsModel);
    tvOutput->setItemDelegate(new ProxyDelegate());
    tvOutput->horizontalHeader()->setStretchLastSection(true);
    tvOutput->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    registerField(CreateCmdlineBasedWorkerWizard::OUTPUTS_DATA_FIELD, this, OUTPUTS_DATA_PROPERTY, SIGNAL(si_outputsChanged()));
    registerField(CreateCmdlineBasedWorkerWizard::OUTPUTS_IDS_FIELD, this, OUTPUTS_IDS_PROPERTY);
    registerField(CreateCmdlineBasedWorkerWizard::OUTPUTS_NAMES_FIELD, this, OUTPUTS_NAMES_PROPERTY);

    this->duplicateOutputsWarningLabel->setVisible(false);
}

void CreateCmdlineBasedWorkerWizardOutputDataPage::initializePage() {
    CHECK(nullptr != initialConfig, );
    initDataModel(outputsModel, initialConfig->outputs);
}

bool CreateCmdlineBasedWorkerWizardOutputDataPage::isComplete() const {
    const QStringList ids = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList() +
                            field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD).toStringList() +
                            field(CreateCmdlineBasedWorkerWizard::OUTPUTS_IDS_FIELD).toStringList();
    const QStringList names = field(CreateCmdlineBasedWorkerWizard::INPUTS_NAMES_FIELD).toStringList() +
                              field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_NAMES_FIELD).toStringList() +
                              field(CreateCmdlineBasedWorkerWizard::OUTPUTS_NAMES_FIELD).toStringList();
    return checkNamesAndIds(names, ids);
}

void CreateCmdlineBasedWorkerWizardOutputDataPage::sl_addOutput() {
    outputsModel->insertRow(UNNECCESSARY_ARGUMENT, QModelIndex());
    tvOutput->setCurrentIndex(outputsModel->index(outputsModel->rowCount(QModelIndex()) - 1, 0));
}

void CreateCmdlineBasedWorkerWizardOutputDataPage::sl_deleteOutput() {
    outputsModel->removeRow(tvOutput->currentIndex().row());
}

void CreateCmdlineBasedWorkerWizardOutputDataPage::sl_updateOutputsProperties() {
    QStringList ids;
    QStringList names;
    QList<DataConfig> data;

    // this is the third page in the wizard. Check for duplicates with the prev. pages ids (inputs, attributes)
    QStringList inputIds = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList();
    QStringList attributeIds = field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD).toStringList();
    bool hasDuplicates = false;

    foreach (CfgExternalToolItem *item, outputsModel->getItems()) {
        data << item->itemData;
        QString id = item->getId();
        hasDuplicates = hasDuplicates || (!id.isEmpty() && (ids.contains(id) || inputIds.contains(id) || attributeIds.contains(id)));
        ids << id;
        names << item->getName();
    }
    setProperty(OUTPUTS_DATA_PROPERTY, QVariant::fromValue<QList<DataConfig> >(data));
    setProperty(OUTPUTS_IDS_PROPERTY, ids);
    setProperty(OUTPUTS_NAMES_PROPERTY, names);

    this->duplicateOutputsWarningLabel->setVisible(hasDuplicates);

    emit si_outputsChanged();
}

/**********************************************/
/* CreateCmdlineBasedWorkerWizardCommandPage */
/**********************************************/

CommandValidator::CommandValidator(QTextEdit *_textEdit)
    : QObject(_textEdit),
      textEdit(_textEdit)
{
    SAFE_POINT(nullptr != textEdit, "textEdit widget is nullptr", );
    connect(textEdit, SIGNAL(textChanged()), SLOT(sl_textChanged()));
}

void CommandValidator::sl_textChanged() {
    QSignalBlocker signalBlocker(textEdit);
    Q_UNUSED(signalBlocker);

    QTextCursor cursor = textEdit->textCursor();
    const int position = cursor.position();

    QString text = textEdit->toPlainText();
    text.replace("\"", "\'");
    textEdit->setPlainText(text);

    cursor.setPosition(position);
    textEdit->setTextCursor(cursor);
}

CreateCmdlineBasedWorkerWizardCommandPage::CreateCmdlineBasedWorkerWizardCommandPage(ExternalProcessConfig *_initialConfig)
    : QWizardPage(nullptr),
      initialConfig(_initialConfig)
{
    setupUi(this);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);

    teCommand->setWordWrapMode(QTextOption::WrapAnywhere);
    teCommand->document()->setDefaultStyleSheet("span { white-space: pre-wrap; }");
    new CommandValidator(teCommand);

    registerField(CreateCmdlineBasedWorkerWizard::COMMAND_TEMPLATE_FIELD + "*", teCommand, "plainText", SIGNAL(textChanged()));
}

void CreateCmdlineBasedWorkerWizardCommandPage::initializePage() {
    if (nullptr != initialConfig) {
        teCommand->setText(initialConfig->cmdLine);
    } else {
        QString commandTemplate = "<My tool>";
        bool isIntegratedTool = field(CreateCmdlineBasedWorkerWizard::USE_INTEGRATED_TOOL_FIELD).toBool();
        if (!isIntegratedTool) {
            commandTemplate = "%" + CustomWorkerUtils::TOOL_PATH_VAR_NAME + "%";
        } else {
            QString integatedToolId = field(CreateCmdlineBasedWorkerWizard::INTEGRATED_TOOL_ID_FIELD).toString();
            ExternalTool * tool = AppContext::getExternalToolRegistry()->getById(integatedToolId);
            if (tool) {
                QString toolRunnerProgramId = tool->getToolRunnerProgramId();
                if (!toolRunnerProgramId.isEmpty()) {
                    ExternalTool* toolRunnerProgram = AppContext::getExternalToolRegistry()->getById(toolRunnerProgramId);
                    if (nullptr != toolRunnerProgram) {
                        commandTemplate = "%" + CustomWorkerUtils::getVarName(toolRunnerProgram) + "% ";
                        foreach(const QString & param, toolRunnerProgram->getRunParameters()) {
                            commandTemplate += param + " ";
                        }
                    } else {
                        commandTemplate = "";
                    }
                } else {
                    commandTemplate = "";
                }
                commandTemplate +=  "%" + CustomWorkerUtils::getVarName(tool) + "%";
            }
        }

        const QStringList inputsNames = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList();
        foreach (const QString &name, inputsNames) {
            commandTemplate += " $" + name;
        }

        const QStringList outputsNames = field(CreateCmdlineBasedWorkerWizard::OUTPUTS_IDS_FIELD).toStringList();
        foreach (const QString &name, outputsNames) {
            commandTemplate += " $" + name;
        }

        const QStringList attributesNames = field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD).toStringList();
        int i = 0;
        foreach (const QString &name, attributesNames) {
            commandTemplate += " -p" + QString::number(++i) + " $" + name;
        }

        teCommand->setText(commandTemplate);
    }
}

bool CreateCmdlineBasedWorkerWizardCommandPage::isComplete() const {
    return !teCommand->toPlainText().isEmpty();
}

bool CreateCmdlineBasedWorkerWizardCommandPage::validatePage() {
    const QString command = teCommand->toPlainText();
    QStringList ids = field(CreateCmdlineBasedWorkerWizard::INPUTS_IDS_FIELD).toStringList() +
                      field(CreateCmdlineBasedWorkerWizard::OUTPUTS_IDS_FIELD).toStringList() +
                      field(CreateCmdlineBasedWorkerWizard::ATTRIBUTES_IDS_FIELD).toStringList();

    QString parameters;
    foreach(const QString &id, ids) {
        if (!command.contains("$" + id)) {
            parameters += " - " + id + "\n";
        }
    }

    if (parameters.isEmpty()) {
        return true;
    }

    QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(tr("Create Element"));
    msgBox->setText(tr("You don't use listed parameters in template string. Continue?"));
    msgBox->setDetailedText(parameters);
    QAbstractButton *detailsButton = NULL;
    foreach(QAbstractButton *button, msgBox->buttons()) {
        if (msgBox->buttonRole(button) == QMessageBox::ActionRole) {
            QString buttoText = button->text();
            detailsButton = button;
            break;
        }
    }
    if (detailsButton) {
        detailsButton->click();
    }
    msgBox->addButton(tr("Continue"), QMessageBox::ActionRole);
    QPushButton *cancel = msgBox->addButton(tr("Abort"), QMessageBox::ActionRole);
    msgBox->exec();
    CHECK(!msgBox.isNull(), false);
    if (msgBox->clickedButton() == cancel) {
        return false;
    }
    return true;
}

/**********************************************/
/* CreateCmdlineBasedWorkerWizardElementAppearancePage */
/**********************************************/

CreateCmdlineBasedWorkerWizardElementAppearancePage::CreateCmdlineBasedWorkerWizardElementAppearancePage(ExternalProcessConfig *_initialConfig)
    : QWizardPage(nullptr),
      initialConfig(_initialConfig)
{
    setupUi(this);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);

    new CommandValidator(tePrompter);
    new CommandValidator(teDescription);

    registerField(CreateCmdlineBasedWorkerWizard::COMMAND_TEMPLATE_DESCRIPTION_FIELD, tePrompter, "plainText", SIGNAL(textChanged()));
    registerField(CreateCmdlineBasedWorkerWizard::WORKER_DESCRIPTION_FIELD, teDescription, "plainText", SIGNAL(textChanged()));
}

void CreateCmdlineBasedWorkerWizardElementAppearancePage::initializePage() {
    CHECK(nullptr != initialConfig, );
    teDescription->setPlainText(initialConfig->description);
    tePrompter->setPlainText(initialConfig->templateDescription);
}

/*********************************************/
/* CreateCmdlineBasedWorkerWizardSummaryPage */
/*********************************************/

CreateCmdlineBasedWorkerWizardSummaryPage::CreateCmdlineBasedWorkerWizardSummaryPage()
    : QWizardPage(nullptr)
{
    setupUi(this);

    lblTitle->setStyleSheet(CreateCmdlineBasedWorkerWizard::PAGE_TITLE_STYLE_SHEET);
    QColor backGroundColor = palette().color(QPalette::Window);
    lblNameValue->setStyleSheet("background-color:" + backGroundColor.name() + ";");
    lblPrompterValue->setStyleSheet("background-color:" + backGroundColor.name() + ";");
    lblDescriptionValue->setStyleSheet("background-color:" + backGroundColor.name() + ";");
    lblCommandValue->setStyleSheet("background-color:" + backGroundColor.name() + ";");
}

void CreateCmdlineBasedWorkerWizardSummaryPage::showEvent(QShowEvent * /*event*/) {
    lblNameValue->setText(field(CreateCmdlineBasedWorkerWizard::WORKER_NAME_FIELD).toString());
    lblPrompterValue->setText(field(CreateCmdlineBasedWorkerWizard::COMMAND_TEMPLATE_DESCRIPTION_FIELD).toString());
    lblDescriptionValue->setText(field(CreateCmdlineBasedWorkerWizard::WORKER_DESCRIPTION_FIELD).toString());
    lblCommandValue->setText(field(CreateCmdlineBasedWorkerWizard::COMMAND_TEMPLATE_FIELD).toString());
}


/******************************/
/* ExternalToolSelectComboBox */
/******************************/

const QString ExternalToolSelectComboBox::SHOW_ALL_TOOLS = "SHOW_ALL";
const QString ExternalToolSelectComboBox::SHOW_CUSTOM_TOOLS = "SHOW_CUSTOM";

ExternalToolSelectComboBox::ExternalToolSelectComboBox(QWidget* parent)
    : QComboBox(parent) {
    initExternalTools();
    initPopupMenu();
};

void ExternalToolSelectComboBox::hidePopup() {
    QString data = model()->data(view()->currentIndex(), Qt::UserRole).toString();
    if (data == SHOW_ALL_TOOLS || data == SHOW_CUSTOM_TOOLS) {
        modifyMenuAccordingToData(data);
        QComboBox::showPopup();
    } else {
        QComboBox::hidePopup();
    }
}

void ExternalToolSelectComboBox::modifyMenuAccordingToData(const QString& data) {
    GroupedComboBoxDelegate* cbDelegate = qobject_cast<GroupedComboBoxDelegate*>(itemDelegate());
    SAFE_POINT(nullptr != cbDelegate, "GroupedComboBoxDelegate not found", );

    QStandardItemModel* standardModel = qobject_cast<QStandardItemModel*>(model());
    SAFE_POINT(nullptr != standardModel, "Can't cast combobox model to a QStandardItemModel", );

    if (data == SHOW_ALL_TOOLS) {
        model()->removeRows(model()->rowCount() - 2, 2);
        addSupportedToolsPopupMenu();
        insertSeparator(model()->rowCount() + 1);
        cbDelegate->addUngroupedItem(standardModel, tr("Show customs tools only"), SHOW_CUSTOM_TOOLS);
        setCurrentIndex(findData(firstClickableRowData));
    } else if (data == SHOW_CUSTOM_TOOLS) {
        model()->removeRows(customTools.size() + 1, model()->rowCount() - customTools.size() - 1);
        insertSeparator(customTools.size() + 1);
        cbDelegate->addUngroupedItem(standardModel, tr("Show all tools"), SHOW_ALL_TOOLS);
        setCurrentIndex(findData(firstClickableRowData));
    }
}

void ExternalToolSelectComboBox::addSupportedToolsPopupMenu() {
    GroupedComboBoxDelegate* cbDelegate = qobject_cast<GroupedComboBoxDelegate*>(itemDelegate());
    SAFE_POINT(nullptr != cbDelegate, "GroupedComboBoxDelegate not found", );

    QStandardItemModel* standardModel = qobject_cast<QStandardItemModel*>(model());
    SAFE_POINT(nullptr != standardModel, "Can't cast combobox model to a QStandardItemModel", );

    cbDelegate->addParentItem(standardModel, tr("Supported tools"), false);
    QList<QString> keys = supportedTools.keys();
    std::sort(keys.begin(), keys.end(), [](const QString& a, const QString& b) {return a.compare(b, Qt::CaseInsensitive) < 0; });
    foreach(const QString & toolKitName, keys) {
        QList<ExternalTool*> currentToolKitTools = supportedTools.value(toolKitName);
        if (currentToolKitTools.size() == 1) {
            ExternalTool* tool = currentToolKitTools.first();
            cbDelegate->addUngroupedItem(standardModel, tool->getName(), tool->getId());
        } else {
            cbDelegate->addParentItem(standardModel, toolKitName, false, false);
            foreach(ExternalTool * tool, currentToolKitTools) {
                cbDelegate->addChildItem(standardModel, tool->getName(), tool->getId());
            }
        }
    }
}

void ExternalToolSelectComboBox::initExternalTools() {
    QList<ExternalTool*> tools = AppContext::getExternalToolRegistry()->getAllEntries();
    excludeNotSuitableTools(tools);
    separateSupportedAndCustomTools(tools);
}

void ExternalToolSelectComboBox::initPopupMenu() {
    GroupedComboBoxDelegate* cbDelegate = new GroupedComboBoxDelegate();
    setItemDelegate(cbDelegate);

    QStandardItemModel* standardModel = qobject_cast<QStandardItemModel*>(model());
    SAFE_POINT(nullptr != standardModel, "Can't cast combobox model to a QStandardItemModel", );

    if (!customTools.isEmpty()) {
        cbDelegate->addParentItem(standardModel, tr("Custom tools"), false);
        foreach(ExternalTool * tool, customTools) {
            cbDelegate->addUngroupedItem(standardModel, tool->getName(), tool->getId());
        }
        insertSeparator(customTools.size() + 1);
        cbDelegate->addUngroupedItem(standardModel, tr("Show all tools"), SHOW_ALL_TOOLS);
    } else {
        addSupportedToolsPopupMenu();
    }
    setCurrentIndex(findData(firstClickableRowData));
}

void ExternalToolSelectComboBox::excludeNotSuitableTools(QList<ExternalTool*>& tools) {
    foreach(ExternalTool * tool, tools) {
        CHECK_CONTINUE(tool->isModule() || tool->isRunner());
        tools.removeOne(tool);
    }
}

void ExternalToolSelectComboBox::separateSupportedAndCustomTools(const QList<ExternalTool*>& tools) {
    customTools.clear();
    supportedTools.clear();
    QList<ExternalTool*> supportedToolsList;
    foreach(ExternalTool * tool, tools) {
        if (tool->isCustom()) {
            customTools << tool;
        } else {
            supportedToolsList << tool;
        }
    }
    makeSupportedToolsMapFromList(supportedToolsList);
    sortCustomToolsList();
    sortSupportedToolsMap();
    initFirstClickableRow();
}

void ExternalToolSelectComboBox::makeSupportedToolsMapFromList(const QList<ExternalTool*>& tools) {
    foreach(ExternalTool * tool, tools) {
        const QString toolKitName = tool->getToolKitName();
        QList<ExternalTool*>& currentToolKitTools = supportedTools[toolKitName];
        currentToolKitTools << tool;
    }
}

void ExternalToolSelectComboBox::sortCustomToolsList() {
    std::sort(customTools.begin(), customTools.end(), [](ExternalTool* a, ExternalTool* b) {return a->getName().compare(b->getName(), Qt::CaseInsensitive) < 0; });
}

void ExternalToolSelectComboBox::sortSupportedToolsMap() {
    QMap<QString, QList<ExternalTool*> > resultMap;
    foreach(const QString & toolKitName, supportedTools.keys()) {
        QList<ExternalTool*> currentToolKitTools = supportedTools.value(toolKitName);
        if (currentToolKitTools.size() == 1) {
            resultMap.insert(currentToolKitTools.first()->getName(), currentToolKitTools);
        } else {
            std::sort(currentToolKitTools.begin(), currentToolKitTools.end(), [](ExternalTool* a, ExternalTool* b) {return a->getName().compare(b->getName(), Qt::CaseInsensitive) < 0; });
            resultMap.insert(toolKitName, currentToolKitTools);
        }
    }
    supportedTools = resultMap;
}

void ExternalToolSelectComboBox::initFirstClickableRow() {
    if (!customTools.isEmpty()) {
        firstClickableRowData = customTools.first()->getId();
    } else {
        QStringList keys = supportedTools.keys();
        std::sort(keys.begin(), keys.end(), [](const QString& a, const QString& b) {return a.compare(b, Qt::CaseInsensitive) < 0; });
        QList<ExternalTool*> tools = supportedTools.value(keys.first());
        firstClickableRowData = tools.first()->getId();
    }
}

void ExternalToolSelectComboBox::setDefaultMenuValue(const QString& defaultValue) {
    int index = findData(defaultValue);
    if (index > -1) {
        setCurrentIndex(index);
    } else {
        modifyMenuAccordingToData(SHOW_ALL_TOOLS);
        index = findData(defaultValue);
        setCurrentIndex(index != -1 ? index : 1);
    }
}

}   // namespace U2
