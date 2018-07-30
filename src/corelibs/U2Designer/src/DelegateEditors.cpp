/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ScriptEditorDialog.h>

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowUtils.h>

#include "DelegateEditors.h"
#include "PropertyWidget.h"

namespace U2 {

DelegateEditor::DelegateEditor(const DelegateEditor &other) : ConfigurationEditor(other) {
    foreach (const QString &id, other.delegates.keys()) {
        delegates[id] = other.delegates[id]->clone();
    }
}

void DelegateEditor::updateDelegates() {
    foreach (PropertyDelegate* delegate, delegates.values()) {
        delegate->update();
    }
}

void DelegateEditor::updateDelegate( const QString & name ) {
    if (delegates.contains(name)) {
        delegates[name]->update();
    }
}

/********************************
 * SpinBoxDelegate
 ********************************/
PropertyWidget * SpinBoxDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new SpinBoxWidget(getProperties(), parent);
}

QWidget * SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    SpinBoxWidget *editor = new SpinBoxWidget(getProperties(), parent);
    connect(editor, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged(int)));
    connect(editor, SIGNAL(valueChanged(int)), SLOT(sl_commit()));

    currentEditor = editor;

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, ConfigurationEditor::ItemValueRole).toInt();
    SpinBoxWidget *spinBox = static_cast<SpinBoxWidget*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    SpinBoxWidget *spinBox = static_cast<SpinBoxWidget*>(editor);
    int value = spinBox->value().toInt();
    model->setData(index, value, ConfigurationEditor::ItemValueRole);
}

QVariant SpinBoxDelegate::getDisplayValue( const QVariant& v) const {
    QSpinBox editor;
    WorkflowUtils::setQObjectProperties(editor, getProperties());
    editor.setValue(v.toInt());
    return editor.text();
}

void SpinBoxDelegate::setEditorProperty(const char* name, const QVariant& val) {
    spinProperties[name] = val;
    if (!currentEditor.isNull()) {
        currentEditor->setProperty(name, val);
    }
}

void SpinBoxDelegate::getItems(QVariantMap &items ) const {
    items = this->spinProperties;
}

QVariantMap SpinBoxDelegate::getProperties() const {
    QVariantMap result = spinProperties;
    DelegateTags *t = tags();
    CHECK(t != NULL, result);
    foreach (const QString &tagName, t->names()) {
        result[tagName] = t->get(tagName);
    }
    return result;
}

void SpinBoxDelegate::sl_commit() {
    SpinBoxWidget* editor = static_cast<SpinBoxWidget*>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

/********************************
* DoubleSpinBoxDelegate
********************************/
const int DoubleSpinBoxDelegate::DEFAULT_DECIMALS_VALUE = 5;

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(const QVariantMap &props, QObject *parent)
: PropertyDelegate(parent), spinProperties(props)
{
    if (!spinProperties.contains("decimals")) {
        spinProperties["decimals"] = DEFAULT_DECIMALS_VALUE;
    }
}

PropertyWidget * DoubleSpinBoxDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return (PropertyWidget*)createEditor(parent, QStyleOptionViewItem(), QModelIndex());
}

QWidget * DoubleSpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
    DoubleSpinBoxWidget* editor = new DoubleSpinBoxWidget(spinProperties, parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    DoubleSpinBoxWidget *spinBox = static_cast<DoubleSpinBoxWidget*>(editor);
    spinBox->setValue(value);
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    DoubleSpinBoxWidget *spinBox = static_cast<DoubleSpinBoxWidget*>(editor);
    double value = spinBox->value().toDouble();
    model->setData(index, value, ConfigurationEditor::ItemValueRole);
}

QVariant DoubleSpinBoxDelegate::getDisplayValue( const QVariant& v) const {
    QDoubleSpinBox editor;
    WorkflowUtils::setQObjectProperties(editor, spinProperties);
    editor.setValue(v.toDouble());
    return editor.text();
}

void DoubleSpinBoxDelegate::getItems( QVariantMap &items ) const {
    items = this->spinProperties;
}

void DoubleSpinBoxDelegate::sl_commit() {
    DoubleSpinBoxWidget* editor = static_cast<DoubleSpinBoxWidget*>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

/********************************
* ComboBoxDelegate
********************************/
ComboBoxDelegate::ComboBoxDelegate(const QVariantMap &items, QObject *parent)
    : PropertyDelegate(parent)
{
    foreach (QString key, items.keys()) {
        comboItems.append(qMakePair(key, items.value(key)));
    }
}
ComboBoxDelegate::ComboBoxDelegate(const QList<ComboItem> &items, QObject *parent)
    : PropertyDelegate(parent),
      comboItems(items)
{

}

PropertyWidget * ComboBoxDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new ComboBoxWidget(comboItems, parent);
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/* option */,
                                        const QModelIndex &/* index */) const
{
    QList<ComboItem> l;
    QVariantMap m = getAvailableItems();
    if (m.isEmpty()) {
         l = comboItems;
    } else {
        foreach (QString key, m.keys()) {
            l.append(qMakePair(key, m.value(key)));
        }
    }
    ComboBoxWidget *editor = new ComboBoxWidget(l, parent);
    connect(editor, SIGNAL(valueChanged(const QString &)),
        SLOT(sl_commit()));
    connect(editor, SIGNAL(valueChanged(const QString &)),
        SIGNAL(si_valueChanged(const QString &)));

    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    ComboBoxWidget *box = static_cast<ComboBoxWidget*>(editor);
    box->setValue(val);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    ComboBoxWidget *box = static_cast<ComboBoxWidget*>(editor);
    model->setData(index, box->value(), ConfigurationEditor::ItemValueRole);
}

QVariant ComboBoxDelegate::getDisplayValue(const QVariant& val) const {
    QVariantMap m; getItems(m);
    QString display = m.key(val);
    emit si_valueChanged( display );
    return QVariant( display );
}

void ComboBoxDelegate::getItems( QVariantMap &items ) const {
    items = getAvailableItems();
    if (items.isEmpty()) {
        foreach(ComboItem p, comboItems) {
            items.insert(p.first, p.second);
        }
    }
}

QVariantMap ComboBoxDelegate::getAvailableItems() const {
    DelegateTags *t = tags();
    if (t != NULL) {
        if (t->get("AvailableValues") != QVariant()) {
            return t->get("AvailableValues").toMap();
        }
    }
    return QVariantMap();
}

void ComboBoxDelegate::sl_commit() {
    ComboBoxWidget *editor = static_cast<ComboBoxWidget*>(sender());

    if (editor) {
        emit commitData(editor);
    }
}

/********************************
* ComboBoxWithUrlsDelegate
********************************/

PropertyWidget * ComboBoxWithUrlsDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new ComboBoxWithUrlWidget(items, isPath, parent);
}

QWidget *ComboBoxWithUrlsDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/* option */,
                                        const QModelIndex &/* index */) const
{
    ComboBoxWithUrlWidget *editor = new ComboBoxWithUrlWidget(items, isPath, parent);
    connect(editor, SIGNAL(valueChanged(const QString &)), SLOT(sl_valueChanged(const QString &)));
    return editor;
}

void ComboBoxWithUrlsDelegate::sl_valueChanged(const QString &newVal) {
    emit si_valueChanged(newVal);
    QWidget *editor = qobject_cast<QWidget *>(sender());
    SAFE_POINT(NULL != editor, "Invalid editor", );
    emit commitData(editor);
}

void ComboBoxWithUrlsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    ComboBoxWithUrlWidget *box = static_cast<ComboBoxWithUrlWidget*>(editor);
    box->setValue(val);
}

void ComboBoxWithUrlsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    ComboBoxWithUrlWidget *box = static_cast<ComboBoxWithUrlWidget*>(editor);
    model->setData(index, box->value(), ConfigurationEditor::ItemValueRole);
}

QVariant ComboBoxWithUrlsDelegate::getDisplayValue(const QVariant& val) const {
     QString display = items.key(val);
     emit si_valueChanged( display );
     return QVariant( display );
 }

/********************************
* ComboBoxEditableDelegate
********************************/

PropertyWidget * ComboBoxEditableDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
return new ComboBoxEditableWidget(items, parent);
}

QWidget *ComboBoxEditableDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem &/* option */,
                                                const QModelIndex &/* index */) const
{
    ComboBoxEditableWidget *editor = new ComboBoxEditableWidget(items, parent);
    connect(editor, SIGNAL(valueChanged(const QString &)), SLOT(sl_valueChanged(const QString &)));
    return editor;
}

void ComboBoxEditableDelegate::sl_valueChanged(const QString &newVal) {
    emit si_valueChanged(newVal);
    QWidget *editor = qobject_cast<QWidget *>(sender());
    SAFE_POINT(NULL != editor, "Invalid editor", );
    emit commitData(editor);
}

void ComboBoxEditableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    ComboBoxEditableWidget *box = static_cast<ComboBoxEditableWidget*>(editor);
    box->setValue(val);
}

void ComboBoxEditableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    ComboBoxEditableWidget *box = static_cast<ComboBoxEditableWidget*>(editor);
    model->setData(index, box->value(), ConfigurationEditor::ItemValueRole);
}

QVariant ComboBoxEditableDelegate::getDisplayValue(const QVariant& val) const {
    QString display = items.key(val);
    emit si_valueChanged( display );
    return QVariant( display );
}

/********************************
* ComboBoxWithDbUrlsDelegate
********************************/
ComboBoxWithDbUrlsDelegate::ComboBoxWithDbUrlsDelegate(QObject *parent)
    : PropertyDelegate(parent)
{

}

QWidget * ComboBoxWithDbUrlsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    ComboBoxWithDbUrlWidget *editor = new ComboBoxWithDbUrlWidget(parent);
    connect(editor, SIGNAL(valueChanged(const QString &)), SLOT(sl_valueChanged(const QString &)));
    const_cast<ComboBoxWithDbUrlsDelegate *>(this)->items = editor->getItems();
    return editor;
}

void ComboBoxWithDbUrlsDelegate::sl_valueChanged(const QString &newVal) {
    emit si_valueChanged(newVal);
    QWidget *editor = qobject_cast<QWidget *>(sender());
    SAFE_POINT(NULL != editor, "Invalid editor", );
    emit commitData(editor);
}

PropertyWidget * ComboBoxWithDbUrlsDelegate::createWizardWidget(U2OpStatus &, QWidget *parent) const {
    return new ComboBoxWithDbUrlWidget(parent);
}

void ComboBoxWithDbUrlsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    ComboBoxWithDbUrlWidget *box = qobject_cast<ComboBoxWithDbUrlWidget *>(editor);
    const QVariantMap items = box->getItems();
    if (val.isValid() && items.values().contains(val)) {
        box->setValue(val);
    } else if (!items.isEmpty()) {
        box->setValue(items.values().first());
    }
}

void ComboBoxWithDbUrlsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    ComboBoxWithDbUrlWidget *box = qobject_cast<ComboBoxWithDbUrlWidget *>(editor);
    model->setData(index, box->value(), ConfigurationEditor::ItemValueRole);
}

QVariant ComboBoxWithDbUrlsDelegate::getDisplayValue(const QVariant &val) const {
    QString display = items.key(val);
    emit si_valueChanged(display);
    return QVariant(display);
}

PropertyDelegate * ComboBoxWithDbUrlsDelegate::clone() {
    return new ComboBoxWithDbUrlsDelegate(parent());
}

PropertyDelegate::Type ComboBoxWithDbUrlsDelegate::type() const {
    return SHARED_DB_URL;
}

/********************************
* ComboBoxWithChecksDelegate
********************************/

PropertyWidget * ComboBoxWithChecksDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new ComboBoxWithChecksWidget(items, parent);
}

QWidget *ComboBoxWithChecksDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem &/* option */,
                                                const QModelIndex &/* index */) const
{
    ComboBoxWithChecksWidget* editor = new ComboBoxWithChecksWidget(items, parent);
    connect(editor, SIGNAL(valueChanged(const QString &)),
        this, SIGNAL(si_valueChanged(const QString &)));
    return editor;
}

void ComboBoxWithChecksDelegate::setEditorData(QWidget *editor,
                                             const QModelIndex &index) const
{
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    ComboBoxWithChecksWidget *box = static_cast<ComboBoxWithChecksWidget*>(editor);
    box->setValue(val);
}

void ComboBoxWithChecksDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                            const QModelIndex &index) const
{
    ComboBoxWithChecksWidget *box = static_cast<ComboBoxWithChecksWidget*>(editor);
    model->setData(index, box->value(), ConfigurationEditor::ItemValueRole);
}

void ComboBoxWithChecksDelegate::getItems( QVariantMap &items ) const {
    items = this->items;
}

QVariant ComboBoxWithChecksDelegate::getDisplayValue(const QVariant& val) const {
    QString display = val.toString();
    emit si_valueChanged( display );
    return QVariant( display );
}

/********************************
* ComboBoxWithBoolsDelegate
********************************/

ComboBoxWithBoolsDelegate::ComboBoxWithBoolsDelegate(QObject *parent)
    : ComboBoxDelegate(boolMap(), parent) {}

QVariantMap ComboBoxWithBoolsDelegate::boolMap() {
    QVariantMap map;
    map["False"] = false;
    map["True"] = true;
    return map;
}


/********************************
* URLDelegate
********************************/
URLDelegate::URLDelegate(const QString &filter, const QString &type, const Options &_options, QObject *parent, const QString &format)
    : PropertyDelegate(parent),
      lastDirType(type),
      options(_options)
{
    tags()->set(DelegateTags::FILTER, filter);
    tags()->set(DelegateTags::FORMAT, format);
}

URLDelegate::URLDelegate(const DelegateTags &_tags, const QString &type, const Options &_options, QObject *parent)
    : PropertyDelegate(parent),
      lastDirType(type),
      options(_options)
{
    *tags() = _tags;
}

URLDelegate::URLDelegate(const QString& filter, const QString& type, bool multi, bool isPath, bool saveFile, QObject *parent, const QString &format, bool noFilesMode)
    : PropertyDelegate(parent), lastDirType(type)
{
    tags()->set(DelegateTags::FILTER, filter);
    tags()->set(DelegateTags::FORMAT, format);

    options |= multi ? AllowSelectSeveralFiles : None;
    options |= isPath ? AllowSelectOnlyExistingDir : None;
    options |= saveFile ? SelectFileToSave : None;
    options |= noFilesMode ? SelectParentDirInsteadSelectedFile : None;
}

URLDelegate::URLDelegate(const DelegateTags &_tags, const QString &type, bool multi, bool isPath, bool saveFile, QObject *parent, bool noFilesMode) :
    PropertyDelegate(parent),
    lastDirType(type)
{
    *tags() = _tags;

    options |= multi ? AllowSelectSeveralFiles : None;
    options |= isPath ? AllowSelectOnlyExistingDir : None;
    options |= saveFile ? SelectFileToSave : None;
    options |= noFilesMode ? SelectParentDirInsteadSelectedFile : None;
}

QVariant URLDelegate::getDisplayValue(const QVariant &v) const {
    return v.toString().isEmpty() ? QVariant(DelegateTags::getString(tags(), DelegateTags::PLACEHOLDER_TEXT)) : v;
}

URLWidget *URLDelegate::createWidget(QWidget *parent) const {
    URLWidget *result;
    if (options.testFlag(SelectParentDirInsteadSelectedFile)) {
        bool isPath = false; // noFilesMode: choose a file but its dir will be committed
        result = new NoFileURLWidget(lastDirType,
                                     options.testFlag(AllowSelectSeveralFiles),
                                     isPath,
                                     options.testFlag(SelectFileToSave),
                                     tags(),
                                     parent);
    } else {
        result = new URLWidget(lastDirType,
                               options.testFlag(AllowSelectSeveralFiles),
                               options.testFlag(AllowSelectOnlyExistingDir),
                               options.testFlag(SelectFileToSave),
                               tags(),
                               parent);
    }
    if (options.testFlag(SelectFileToSave) && !options.testFlag(DoNotUseWorkflowOutputFolder)) {
        result->setSchemaConfig(schemaConfig);
    }
    return result;
}

PropertyWidget *URLDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return createWidget(parent);
}

QWidget *URLDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
    URLWidget *editor = createWidget(parent);
    connect(editor, SIGNAL(finished()), SLOT(sl_commit()));
    return editor;
}

void URLDelegate::sl_commit() {
    URLWidget *editor = static_cast<URLWidget*>(sender());

    text = editor->value().toString();
    emit commitData(editor);
}

void URLDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    URLWidget *lineEdit = dynamic_cast<URLWidget*>(editor);
    lineEdit->setValue(val);
}

void URLDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    URLWidget *lineEdit = dynamic_cast<URLWidget*>(editor);
    QString val = lineEdit->value().toString().replace('\\', '/').trimmed();
    QStringList urls = val.split(";", QString::SkipEmptyParts);
    val = urls.join(";");
    model->setData(index, val, ConfigurationEditor::ItemValueRole);
    if (options.testFlag(AllowSelectSeveralFiles)) {
        QVariantList vl;
        foreach(QString s, val.split(";")) {
            vl.append(s.trimmed());
        }
        model->setData(index, vl, ConfigurationEditor::ItemListValueRole);
    }
}

PropertyDelegate *URLDelegate::clone() {
    return new URLDelegate(*tags(), lastDirType, options, parent());
}

PropertyDelegate::Type URLDelegate::type() const {
    if (options.testFlag(AllowSelectOnlyExistingDir)) {
        return options.testFlag(SelectFileToSave) ? OUTPUT_DIR : INPUT_DIR;
    }
    return options.testFlag(SelectFileToSave) ? OUTPUT_FILE : INPUT_FILE;
}

/********************************
* FileModeDelegate
********************************/
FileModeDelegate::FileModeDelegate(bool appendSupported, QObject *parent)
: ComboBoxDelegate(QVariantMap(), parent) {
    comboItems.append(qMakePair(U2::WorkflowUtils::tr("Overwrite"), SaveDoc_Overwrite));
    comboItems.append(qMakePair(U2::WorkflowUtils::tr("Rename"), SaveDoc_Roll));
    if (appendSupported) {
        comboItems.append(qMakePair(U2::WorkflowUtils::tr("Append"), SaveDoc_Append));
    }
}

/********************************
 * SchemaRunModeDelegate
 ********************************/
const QString SchemaRunModeDelegate::THIS_COMPUTER_STR      = SchemaRunModeDelegate::tr( "This computer" );
const QString SchemaRunModeDelegate::REMOTE_COMPUTER_STR    = SchemaRunModeDelegate::tr( "Remote computer" );

SchemaRunModeDelegate::SchemaRunModeDelegate( QObject * parent )
: ComboBoxDelegate( QVariantMap(), parent ) {
    comboItems.append(qMakePair( THIS_COMPUTER_STR, true ));
    comboItems.append(qMakePair( REMOTE_COMPUTER_STR, false ));

    connect( this, SIGNAL( si_valueChanged( const QString & ) ), this,
        SLOT( sl_valueChanged( const QString & ) ) );
}

void SchemaRunModeDelegate::sl_valueChanged( const QString & val ) {
    emit si_showOpenFileButton( THIS_COMPUTER_STR == val );
}


/********************************
* ScriptSelectionWidget
********************************/
const int NO_SCRIPT_ITEM_ID = 0;
const int USER_SCRIPT_ITEM_ID = 1;
const QPair<QString, int> NO_SCRIPT_ITEM_STR("no script", NO_SCRIPT_ITEM_ID);
const QPair<QString, int> USER_SCRIPT_ITEM_STR("user script", USER_SCRIPT_ITEM_ID);
const QString SCRIPT_PROPERTY = "combo_script_property";

ScriptSelectionWidget::ScriptSelectionWidget(QWidget *parent) :
    PropertyWidget(parent)
{
    combobox = new QComboBox;
    combobox->addItem(NO_SCRIPT_ITEM_STR.first);
    combobox->addItem(USER_SCRIPT_ITEM_STR.first);
    connect(combobox, SIGNAL(activated(int)), SLOT(sl_comboActivated(int)));
    addMainWidget(combobox);
}

void ScriptSelectionWidget::setValue(const QVariant &value) {
    AttributeScript attrScript = value.value<AttributeScript>();
    if (attrScript.isEmpty()) {
        combobox->setCurrentIndex(NO_SCRIPT_ITEM_STR.second);
    } else {
        combobox->setCurrentIndex(USER_SCRIPT_ITEM_STR.second);
    }
    combobox->setProperty(SCRIPT_PROPERTY.toLatin1().constData(), qVariantFromValue<AttributeScript>(attrScript));
}

QVariant ScriptSelectionWidget::value() {
    return combobox->itemData(USER_SCRIPT_ITEM_ID, ConfigurationEditor::ItemValueRole);
}

void ScriptSelectionWidget::sl_comboActivated(int itemId) {
    switch(itemId) {
    case NO_SCRIPT_ITEM_ID: {
            combobox->setItemData(USER_SCRIPT_ITEM_ID, "", ConfigurationEditor::ItemValueRole);
            return;
        }
    case USER_SCRIPT_ITEM_ID: {
        AttributeScript attrScript = combobox->property(SCRIPT_PROPERTY.toLatin1().constData()).value<AttributeScript>();
        QObjectScopedPointer<ScriptEditorDialog> dlg = new ScriptEditorDialog(combobox, AttributeScriptDelegate::createScriptHeader(attrScript));
        dlg->setScriptText(attrScript.getScriptText());

        const int rc = dlg->exec();
        CHECK(!dlg.isNull(), );
        if(rc != QDialog::Accepted) {
            combobox->setItemData(USER_SCRIPT_ITEM_ID, qVariantFromValue<AttributeScript>(attrScript), ConfigurationEditor::ItemValueRole);
        } else {
            attrScript.setScriptText(dlg->getScriptText());
            combobox->setItemData(USER_SCRIPT_ITEM_ID, qVariantFromValue<AttributeScript>(attrScript), ConfigurationEditor::ItemValueRole);
        }

        emit si_finished();
        return;
    }
    default: {
        FAIL("Unexpected item", );
    }
    }
}

/********************************
* AttributeScriptDelegate
********************************/
AttributeScriptDelegate::AttributeScriptDelegate(QObject *parent) : PropertyDelegate(parent) {
}

AttributeScriptDelegate::~AttributeScriptDelegate() {
}

void AttributeScriptDelegate::sl_commit() {
    ScriptSelectionWidget *editor = static_cast<ScriptSelectionWidget*>(sender());
    emit commitData(editor);
}

QWidget * AttributeScriptDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    ScriptSelectionWidget *editor = new ScriptSelectionWidget(parent);
    connect(editor, SIGNAL(si_finished()), SLOT(sl_commit()));
    return editor;
}

void AttributeScriptDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    ScriptSelectionWidget * combo = qobject_cast<ScriptSelectionWidget*>(editor);
    assert(combo != NULL);
    combo->setValue(index.model()->data(index, ConfigurationEditor::ItemValueRole));
}

void AttributeScriptDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    ScriptSelectionWidget *combo = qobject_cast<ScriptSelectionWidget *>(editor);
    assert(combo != NULL);
    model->setData(index, combo->value(), ConfigurationEditor::ItemValueRole);
}

QVariant AttributeScriptDelegate::getDisplayValue(const QVariant& val) const{
    AttributeScript attrScript = val.value<AttributeScript>();
    QString ret = attrScript.isEmpty() ? NO_SCRIPT_ITEM_STR.first : USER_SCRIPT_ITEM_STR.first;
    return QVariant(ret);
}

QString AttributeScriptDelegate::createScriptHeader(const AttributeScript &attrScript) {
    QString header;
    foreach (const Descriptor & desc, attrScript.getScriptVars().keys()) {
        header += QString("var %1; // %2\n").arg(desc.getId()).arg(desc.getDisplayName());
    }
    return header;
}

/********************************
 * StringListDelegate
 ********************************/
void StingListEdit::sl_onExpand()
{
    QObjectScopedPointer<QDialog> editor = new QDialog(this);
    editor->setWindowTitle(StringListDelegate::tr("Enter items"));

    QPushButton *accept = new QPushButton(StringListDelegate::tr("OK"), editor.data());
    connect(accept, SIGNAL(clicked()), editor.data(), SLOT(accept()));
    QPushButton *reject = new QPushButton(StringListDelegate::tr("Cancel"), editor.data());
    connect(reject, SIGNAL(clicked()), editor.data(), SLOT(reject()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout(0);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(accept);
    buttonsLayout->addWidget(reject);

    QTextEdit *edit = new QTextEdit("", editor.data());

    foreach (const QString &item, text().split(";", QString::SkipEmptyParts)) {
        edit->append(item.trimmed());
    }

    QVBoxLayout *layout = new QVBoxLayout(editor.data());
    layout->addWidget(edit);
    layout->addLayout(buttonsLayout);

    editor->setLayout(layout);

    editor->exec();
    CHECK(!editor.isNull(), );

    if (editor->result() == QDialog::Accepted) {
        QString s = edit->toPlainText();
        s.replace("\n", "; ");
        setText(s);
        emit editingFinished();
    }
}

void StingListEdit::focusOutEvent ( QFocusEvent * ) {
    emit si_finished();
}

StingListWidget::StingListWidget(QWidget *parent)
: PropertyWidget(parent)
{
    edit = new StingListEdit(this);
    edit->setFrame(false);
    edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    addMainWidget(edit);

    QToolButton *button = new QToolButton(this);
    button->setText("...");
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(button, SIGNAL(clicked()), edit, SLOT(sl_onExpand()));
    layout()->addWidget(button);

    connect(edit, SIGNAL(si_finished()), SIGNAL(finished()));
}

QVariant StingListWidget::value() {
    return edit->text();
}

void StingListWidget::setValue(const QVariant &value) {
    edit->setText(value.toString());
}

void StingListWidget::setRequired() {
    edit->setPlaceholderText(L10N::required());
}

PropertyWidget * StringListDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new StingListWidget(parent);
}

QWidget * StringListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    StingListWidget *widget = new StingListWidget(parent);
    connect(widget, SIGNAL(finished()), SLOT(sl_commit()));

    currentEditor = widget;
    return widget;
}

void StringListDelegate::sl_commit() {
    emit commitData(currentEditor);
}

void StringListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    StingListWidget *lineEdit = dynamic_cast<StingListWidget*>(editor);
    lineEdit->setValue(val);
}

void StringListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    StingListWidget *lineEdit = dynamic_cast<StingListWidget*>(editor);

    QString val = lineEdit->value().toString();
    model->setData(index, val, ConfigurationEditor::ItemValueRole);

    QVariantList vl;
    foreach(const QString &s, val.split(";", QString::SkipEmptyParts)) {
        vl.append(s.trimmed());
    }

    model->setData(index, vl, ConfigurationEditor::ItemListValueRole);
}

/********************************
 * StringSelectorDelegate
********************************/
QWidget *StringSelectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    QWidget * editor = new QWidget(parent);
    valueEdit = new QLineEdit(editor);
    valueEdit->setObjectName("valueEdit");
    //valueEdit->setReadOnly(true);
    valueEdit->setFrame(false);
    valueEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred));
    editor->setFocusProxy(valueEdit);
    QToolButton * toolButton = new QToolButton(editor);
    toolButton->setVisible( true );
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_onClick()));

    QHBoxLayout* layout = new QHBoxLayout(editor);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(valueEdit);
    layout->addWidget(toolButton);

    currentEditor = editor;
    connect(valueEdit, SIGNAL(editingFinished()), SLOT(sl_commit()));

    return editor;
}

void StringSelectorDelegate::sl_commit() {
    emit commitData(currentEditor);
}

void StringSelectorDelegate::sl_onClick() {
    QObjectScopedPointer<QDialog> dlg = f->createSelectorDialog(initValue);

    const int dialogResult = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        valueEdit->setText(f->getSelectedString(dlg.data()));
        sl_commit();
    }
}

void StringSelectorDelegate::setEditorData(QWidget *, const QModelIndex &index) const {
    QString val = index.model()->data(index, ConfigurationEditor::ItemValueRole).toString();
    valueEdit->setText(val);
}

void StringSelectorDelegate::setModelData(QWidget *, QAbstractItemModel *model, const QModelIndex &index) const {
    QString val = valueEdit->text().trimmed();
    model->setData(index, val, ConfigurationEditor::ItemValueRole);
    if (multipleSelection) {
        QVariantList vl;
        foreach(QString s, val.split(",")) {
            vl.append(s.trimmed());
        }
        model->setData(index, vl, ConfigurationEditor::ItemListValueRole);
    }
}

/********************************
 * CharacterDelegate
 ********************************/
PropertyWidget * CharacterDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new DefaultPropertyWidget(1, parent);
}

QWidget * CharacterDelegate::createEditor(QWidget *parent,
                                   const QStyleOptionViewItem &/* option */,
                                   const QModelIndex &/* index */) const
{
    return new DefaultPropertyWidget(1, parent);
}

void CharacterDelegate::setEditorData(QWidget *editor,
                                const QModelIndex &index) const
{
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    DefaultPropertyWidget *lineEdit = dynamic_cast<DefaultPropertyWidget*>(editor);
    lineEdit->setValue(val);
}

void CharacterDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                               const QModelIndex &index) const
{
    DefaultPropertyWidget *lineEdit = dynamic_cast<DefaultPropertyWidget*>(editor);
    model->setData(index, lineEdit->value().toString(), ConfigurationEditor::ItemValueRole);
}

}//namespace U2
