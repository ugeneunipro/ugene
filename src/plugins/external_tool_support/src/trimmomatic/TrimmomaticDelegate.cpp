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

#include <QAbstractItemView>
#include <QListView>
#include <QMenu>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SignalBlocker.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/MultiClickMenu.h>
#include <U2Gui/WidgetWithLocalToolbar.h>

#include "TrimmomaticDelegate.h"
#include "TrimmomaticStep.h"

namespace U2 {
namespace LocalWorkflow {

/********************************************************************/
/*TrimmomaticDelegate*/
/********************************************************************/

static const QString PLACEHOLDER("Configure steps");
static const QRegularExpression notQuotedSpaces("[^\\s\\\"']*\"[^\\\"]*\\\"[^\\s\\\"']*"
                                               "|"
                                               "[^\\s\\\"']*'[^']*'[^\\s\\\"']*"
                                               "|"
                                               "[^\\s\\\"']+");

TrimmomaticDelegate::TrimmomaticDelegate(QObject *parent)
    : PropertyDelegate(parent)
{
}

QVariant TrimmomaticDelegate::getDisplayValue(const QVariant &value) const {
    QString str = value.value<QStringList>().join(" ");
    return str.isEmpty() ? PLACEHOLDER : str;
}

PropertyDelegate* TrimmomaticDelegate::clone() {
    return new TrimmomaticDelegate(parent());
}

QWidget* TrimmomaticDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &,
                                       const QModelIndex &) const {
    TrimmomaticPropertyWidget* editor = new TrimmomaticPropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget* TrimmomaticDelegate::createWizardWidget(U2OpStatus &,
                                                 QWidget *parent) const {
    return new TrimmomaticPropertyWidget(parent);
}

void TrimmomaticDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const {
    const QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    TrimmomaticPropertyWidget* propertyWidget =
                    qobject_cast<TrimmomaticPropertyWidget*>(editor);
    propertyWidget->setValue(value);
}

void TrimmomaticDelegate::setModelData(QWidget *editor,
                                       QAbstractItemModel *model,
                                       const QModelIndex &index) const {
    TrimmomaticPropertyWidget* propertyWidget =
                    qobject_cast<TrimmomaticPropertyWidget*>(editor);
    model->setData(index, propertyWidget->value(),
                   ConfigurationEditor::ItemValueRole);
}

void TrimmomaticDelegate::sl_commit() {
    TrimmomaticPropertyWidget* editor =
        qobject_cast<TrimmomaticPropertyWidget*>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

/********************************************************************/
/*TrimmomaticPropertyWidget*/
/********************************************************************/

TrimmomaticPropertyWidget::TrimmomaticPropertyWidget(QWidget* parent,
                DelegateTags* tags) : PropertyWidget(parent, tags) {
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(PLACEHOLDER);
    lineEdit->setObjectName("trimmomaticPropertyLineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lineEdit->setReadOnly(true);
    connect(lineEdit, SIGNAL(textEdited(QString)), SLOT(sl_textEdited()));

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("trimmomaticPropertyToolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_showDialog()));
    layout()->addWidget(toolButton);

    setObjectName("TrimmomaticPropertyWidget");
}

QVariant TrimmomaticPropertyWidget::value() {
    QRegularExpressionMatchIterator capturedSteps = notQuotedSpaces.globalMatch(lineEdit->text());
    QStringList steps;
    while (capturedSteps.hasNext()) {
        const QString step = capturedSteps.next().captured();
        if (!step.isEmpty()) {
            steps << step;
        }
    }
    return steps;
}

void TrimmomaticPropertyWidget::setValue(const QVariant& value) {
    lineEdit->setText(value.value<QStringList>().join(" "));
}

void TrimmomaticPropertyWidget::sl_textEdited() {
    emit si_valueChanged(value());
}

void TrimmomaticPropertyWidget::sl_showDialog() {
    QObjectScopedPointer<TrimmomaticPropertyDialog> dialog
                            (new TrimmomaticPropertyDialog(lineEdit->text(), this));
    if (QDialog::Accepted == dialog->exec()) {
        CHECK(!dialog.isNull(), );
        lineEdit->setText(dialog->getValue());
        emit si_valueChanged(value());
    }
}

/********************************************************************/
/*TrimmomaticPropertyDialog*/
/********************************************************************/

const QString TrimmomaticPropertyDialog::DEFAULT_DESCRIPTION = QObject::tr("<html><head></head><body>"
                                                               "<p>Click the \"Add new step\" button and select a step. The following options are available:</p>"
                                                               "<ul>"
                                                               "<li>ILLUMINACLIP: Cut adapter and other illumina-specific sequences from the read.</li>"
                                                               "<li>SLIDINGWINDOW: Perform a sliding window trimming, cutting once the average quality within the window falls below a threshold.</li>"
                                                               "<li>LEADING: Cut bases off the start of a read, if below a threshold quality.</li>"
                                                               "<li>TRAILING: Cut bases off the end of a read, if below a threshold quality.</li>"
                                                               "<li>CROP: Cut the read to a specified length.</li>"
                                                               "<li>HEADCROP: Cut the specified number of bases from the start of the read.</li>"
                                                               "<li>MINLEN: Drop the read if it is below a specified length.</li>"
                                                               "<li>AVGQUAL: Drop the read if the average quality is below the specified level.</li>"
                                                               "<li>TOPHRED33: Convert quality scores to Phred-33.</li>"
                                                               "<li>TOPHRED64: Convert quality scores to Phred-64.</li>"
                                                               "</ul>"
                                                               "</body></html>");
const QString TrimmomaticPropertyDialog::DEFAULT_SETTINGS_TEXT = QObject::tr("Add a step.");

TrimmomaticPropertyDialog::TrimmomaticPropertyDialog(const QString &value,
                                      QWidget *parent) : QDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "22059547");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Apply"));

    menu = new QMenu(this);
    menu->setObjectName("stepsMenu");
    new MultiClickMenu(menu);

    foreach (TrimmomaticStepFactory *factory, TrimmomaticStepsRegistry::getInstance()->getAllEntries()) {
        QAction* step = new QAction(factory->getId(), menu->menuAction());
        step->setObjectName(factory->getId());
        menu->addAction(step);
    }

    buttonAdd->setMenu(menu);

    currentWidget = NULL;
    defaultSettingsWidget = new QLabel(DEFAULT_SETTINGS_TEXT);
    listSteps->setEditTriggers(QAbstractItemView::NoEditTriggers);

    enableButtons(false);
    emptySelection();

    connect(listSteps, SIGNAL(currentRowChanged(int)), SLOT(sl_currentRowChanged()));
    connect(menu, SIGNAL(triggered(QAction*)), SLOT(sl_addStep(QAction*)));
    connect(buttonUp, SIGNAL(pressed()), SLOT(sl_moveStepUp()));
    connect(buttonDown, SIGNAL(pressed()), SLOT(sl_moveStepDown()));
    connect(buttonRemove, SIGNAL(pressed()), SLOT(sl_removeStep()));

    parseCommand(value);
}

QString TrimmomaticPropertyDialog::getValue() const {
    QString result;
    foreach (TrimmomaticStep *step, steps) {
        result += step->getCommand();
        result += " ";
    }
    result.chop(1);
    return result;
}

void TrimmomaticPropertyDialog::sl_valuesChanged() {
    bool isValid = !steps.isEmpty();
    for (int i = 0 ; i < steps.size(); i++) {
        const bool isStepValid = steps[i]->validate();
        QListWidgetItem *item = listSteps->item(i);
        SAFE_POINT(NULL != item, QString("Item with number %1 is NULL").arg(i), );
        item->setBackgroundColor(isStepValid ? GUIUtils::OK_COLOR : GUIUtils::WARNING_COLOR);
        isValid = isValid && isStepValid;
    }
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

void TrimmomaticPropertyDialog::sl_currentRowChanged() {
    const int currentStepNumber = listSteps->currentRow();
    CHECK(-1 != currentStepNumber, );
    SAFE_POINT(0 <= currentStepNumber && currentStepNumber < listSteps->count(), "Unexpected selected item", );

    TrimmomaticStep *selectedStep = steps[currentStepNumber];

    textDescription->setText(selectedStep->getDescription());

    currentWidget->hide();
    currentWidget = selectedStep->getSettingsWidget();
    widgetStepSettings->layout()->addWidget(currentWidget);
    currentWidget->show();
}

void TrimmomaticPropertyDialog::emptySelection() {
    textDescription->setText(DEFAULT_DESCRIPTION);

    currentWidget = defaultSettingsWidget;
    widgetStepSettings->layout()->addWidget(currentWidget);
    currentWidget->show();
}

void TrimmomaticPropertyDialog::sl_addStep(QAction* a) {
    addStep(TrimmomaticStepsRegistry::getInstance()->getById(a->text())->createStep());
    listSteps->setCurrentRow(steps.size() - 1);
}

void TrimmomaticPropertyDialog::sl_moveStepUp() {
    CHECK(listSteps->selectedItems().size() != 0, );

    const int selectedStepNum = listSteps->currentRow();
    CHECK(selectedStepNum != -1, );

    const int size = listSteps->count();
    SAFE_POINT(0 <= selectedStepNum && selectedStepNum < size,
        "Unexpected selected item", );

    CHECK(selectedStepNum != 0, );

    {
        SignalBlocker signalBlocker(listSteps);
        Q_UNUSED(signalBlocker);
        listSteps->insertItem(selectedStepNum - 1, listSteps->takeItem(selectedStepNum));
    }

    steps.swap(selectedStepNum, selectedStepNum - 1);
    listSteps->setCurrentRow(selectedStepNum - 1);
}

void TrimmomaticPropertyDialog::sl_moveStepDown() {
    CHECK(listSteps->selectedItems().size() != 0, );

    const int selectedStepNum = listSteps->currentRow();
    CHECK(selectedStepNum != -1, );

    const int size = listSteps->count();
    SAFE_POINT(0 <= selectedStepNum && selectedStepNum < size,
        "Unexpected selected item", );

    CHECK(selectedStepNum != size - 1, );

    {
        SignalBlocker signalBlocker(listSteps);
        Q_UNUSED(signalBlocker);
        listSteps->insertItem(selectedStepNum + 1, listSteps->takeItem(selectedStepNum));
    }

    steps.swap(selectedStepNum, selectedStepNum + 1);
    listSteps->setCurrentRow(selectedStepNum + 1);
}

void TrimmomaticPropertyDialog::sl_removeStep() {
    CHECK(listSteps->selectedItems().size() != 0, );

    const int selectedStepNum = listSteps->currentRow();
    CHECK(selectedStepNum != -1, );

    const int size = listSteps->count();
    SAFE_POINT(0 <= selectedStepNum && selectedStepNum < size,
        "Unexpected selected item", );

    delete listSteps->takeItem(selectedStepNum);
    delete steps.takeAt(selectedStepNum);
    sl_valuesChanged();
    if (steps.size() == 0) {
        enableButtons(false);
        emptySelection();
    }
}

void TrimmomaticPropertyDialog::enableButtons(bool setEnabled) {
    buttonUp->setEnabled(setEnabled);
    buttonDown->setEnabled(setEnabled);
    buttonRemove->setEnabled(setEnabled);
}

void TrimmomaticPropertyDialog::addStep(TrimmomaticStep *step) {
    steps << step;

    connect(step, SIGNAL(si_valueChanged()), SLOT(sl_valuesChanged()));

    listSteps->addItem(step->getName());
    sl_valuesChanged();
    if (steps.size() == 1) {
        enableButtons(true);
        listSteps->setCurrentRow(0);
    }
}

void TrimmomaticPropertyDialog::parseCommand(const QString &command) {
    QRegularExpressionMatchIterator stepCommands = notQuotedSpaces.globalMatch(command);
    while (stepCommands.hasNext()) {
        const QString stepCommand = stepCommands.next().captured();
        const QString stepId = stepCommand.left(stepCommand.indexOf(":"));
        TrimmomaticStepFactory *stepFactory = TrimmomaticStepsRegistry::getInstance()->getById(stepId);
        CHECK_CONTINUE(NULL != stepFactory);

        TrimmomaticStep *step = stepFactory->createStep();
        step->setCommand(stepCommand);
        addStep(step);
    }
}

}   // namespace LocalWorkflow
}   // namespace U2
