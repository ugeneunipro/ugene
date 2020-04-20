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

#include <QLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QMessageBox>

#include "U2Algorithm/GenomeAssemblyRegistry.h"

#include <U2Core/QObjectScopedPointer.h>
#include <U2Gui/HelpButton.h>

#include "SpadesDelegate.h"
#include "SpadesWorker.h"

namespace U2 {
namespace LocalWorkflow {

/********************************************************************/
/*SpadesDelegate*/
/********************************************************************/

const QString SpadesDelegate::PLACEHOLDER = QApplication::translate("SpadesDelegate", "Configure input type");

SpadesDelegate::SpadesDelegate(QObject *parent)
    : PropertyDelegate(parent)
{
}

QVariant SpadesDelegate::getDisplayValue(const QVariant &) const {
    return PLACEHOLDER;
}

PropertyDelegate* SpadesDelegate::clone() {
    return new SpadesDelegate(parent());
}

QWidget* SpadesDelegate::createEditor(QWidget *parent,
                        const QStyleOptionViewItem &/*option*/,
                        const QModelIndex &/*index*/) const {
    SpadesPropertyWidget* editor = new SpadesPropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget* SpadesDelegate::createWizardWidget(U2OpStatus &,
                                                   QWidget *parent) const {
    return new SpadesPropertyWidget(parent);
}

void SpadesDelegate::setEditorData(QWidget *editor,
                                const QModelIndex &index) const {
    const QVariant value = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    SpadesPropertyWidget* propertyWidget =
                            qobject_cast<SpadesPropertyWidget*>(editor);
    propertyWidget->setValue(value);
}

void SpadesDelegate::setModelData(QWidget *editor,
                                       QAbstractItemModel *model,
                                       const QModelIndex &index) const {
    SpadesPropertyWidget* propertyWidget =
                            qobject_cast<SpadesPropertyWidget*>(editor);
    model->setData(index, propertyWidget->value(),
                            ConfigurationEditor::ItemValueRole);
}

void SpadesDelegate::sl_commit() {
    SpadesPropertyWidget* editor =
                        qobject_cast<SpadesPropertyWidget*>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

/********************************************************************/
/*SpadesPropertyWidget*/
/********************************************************************/

SpadesPropertyWidget::SpadesPropertyWidget(QWidget* parent, DelegateTags* tags)
                : PropertyWidget(parent, tags) {
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(SpadesDelegate::PLACEHOLDER);
    lineEdit->setObjectName("spadesPropertyLineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lineEdit->setReadOnly(true);

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("spadesPropertyToolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_showDialog()));
    layout()->addWidget(toolButton);

    setObjectName("spadesPropertyWidget");
}

QVariant SpadesPropertyWidget::value() {
    return QVariant::fromValue<QVariantMap>(dialogValue);
}

void SpadesPropertyWidget::setValue(const QVariant& value) {
    dialogValue = value.toMap();
}

void SpadesPropertyWidget::sl_showDialog() {
    QObjectScopedPointer<SpadesPropertyDialog> dialog
        (new SpadesPropertyDialog(dialogValue, this));

    if (QDialog::Accepted == dialog->exec()) {
        CHECK(!dialog.isNull(), );

        dialogValue = dialog->getValue();
        emit si_valueChanged(value());
    }
}

/********************************************************************/
/*SpadesPropertyDialog*/
/********************************************************************/

SpadesPropertyDialog::SpadesPropertyDialog(const QMap<QString, QVariant> &value,
    QWidget *parent) : QDialog(parent) {
    setupUi(this);

    new HelpButton(this, buttonBox, "28967231");
    setItemsData();
    setValue(value);
}

void SpadesPropertyDialog::accept() {
    CHECK_EXT(isSomeRequiredParemeterChecked(),
        QMessageBox::critical(this, windowTitle(), QApplication::translate("SpadesPropertyDialog", "At least one of the required input ports should be set in the \"Input data\" parameter.")), );

    QDialog::accept();
}

QVariantMap SpadesPropertyDialog::getValue() const {
    QMap<QString, QVariant> result;

    //required
    if (needRequiredSequencingPlatform()) {
        result.insert(SpadesWorkerFactory::SEQUENCING_PLATFORM_ID,
                      sequencingPlatformComboBox->currentData());

        if (pairEndCheckBox->isChecked()) {
            QStringList params = getDataFromComboBoxes(pairEndReadsDirectionComboBox,
                                                       pairEndReadsTypeComboBox);
            SAFE_POINT(params.size() == 2, QApplication::translate("SpadesPropertyDialog", "Incorrect parameters, can't parse"), QVariantMap());

            result.insert(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[0],
                          QString("%1:%2").arg(params.first()).arg(params.last()));
        }
        if (hightQualityCheckBox->isChecked()) {
            QStringList params = getDataFromComboBoxes(hightQualityReadsDirectionComboBox,
                                                       hightQualityReadsTypeComboBox);
            SAFE_POINT(params.size() == 2, QApplication::translate("SpadesPropertyDialog", "Incorrect parameters, can't parse"), QVariantMap());

            result.insert(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[2],
                          QString("%1:%2").arg(params.first()).arg(params.last()));
        }
        if (unpairedReadsCheckBox->isChecked()) {
            result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[0], "");
        }
    }
    if (pacBioCcsCheckBox->isChecked()) {
        result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[1], "");
    }

    //additional
    if (needAdditionalSequencingPlatform()) {
        if (!result.contains(SpadesWorkerFactory::SEQUENCING_PLATFORM_ID))
            result.insert(SpadesWorkerFactory::SEQUENCING_PLATFORM_ID,
                          sequencingPlatformComboBox->currentData());

        if (matePairsCheckBox->isChecked()) {
            QStringList params = getDataFromComboBoxes(matePairsReadsDirectionComboBox,
                                                       matePairsTypeComboBox);
            SAFE_POINT(params.size() == 2, QApplication::translate("SpadesPropertyDialog", "Incorrect parameters, can't parse"), QVariantMap());

            result.insert(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[1],
                          QString("%1:%2").arg(params.first()).arg(params.last()));
        }
    }

    if (pacBioClrCheckBox->isChecked()) {
        result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[2], "");
    }
    if (oxfordNanoporeCheckBox->isChecked()) {
        result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[3], "");
    }
    if (sangerReadsCheckBox->isChecked()) {
        result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[4], "");
    }
    if (trustedContigsCheckBox->isChecked()) {
        result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[5], "");
    }
    if (untrustedContigsCheckBox->isChecked()) {
        result.insert(SpadesWorkerFactory::IN_PORT_ID_LIST[6], "");
    }

    return result;
}

void SpadesPropertyDialog::setValue(const QMap<QString, QVariant> &value) {
    //required
    if (value.contains(SpadesWorkerFactory::SEQUENCING_PLATFORM_ID)) {
        const QVariant platformVariant = value.value(SpadesWorkerFactory::SEQUENCING_PLATFORM_ID);
        SAFE_POINT(platformVariant.canConvert<QString>(), QApplication::translate("SpadesPropertyDialog", "Incorrect parameters, can't parse"), );

        sequencingPlatformComboBox->setCurrentIndex(sequencingPlatformComboBox->findData(platformVariant.toString()));

        if (value.contains(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[0])) {
            pairEndCheckBox->setChecked(true);
            const QVariant dataValue = value.value(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[0]);
            setDataForComboBoxes(pairEndReadsDirectionComboBox, pairEndReadsTypeComboBox, dataValue);
        }
        if (value.contains(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[2])) {
            hightQualityCheckBox->setChecked(true);
            const QVariant dataValue = value.value(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[2]);
            setDataForComboBoxes(hightQualityReadsDirectionComboBox, hightQualityReadsTypeComboBox, dataValue);
        }

        unpairedReadsCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[0]));
    }
    pacBioCcsCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[1]));

    //additional
    if (value.contains(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[1])) {
        matePairsCheckBox->setChecked(true);
        const QVariant dataValue = value.value(SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST[1]);
        setDataForComboBoxes(matePairsReadsDirectionComboBox, matePairsTypeComboBox, dataValue);
    }
    pacBioClrCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[2]));
    oxfordNanoporeCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[3]));
    sangerReadsCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[4]));
    trustedContigsCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[5]));
    untrustedContigsCheckBox->setChecked(value.contains(SpadesWorkerFactory::IN_PORT_ID_LIST[6]));
}

void SpadesPropertyDialog::setItemsData() {
    sequencingPlatformComboBox->setItemData(0, PLATFORM_ILLUMINA);
    sequencingPlatformComboBox->setItemData(1, PLATFORM_ION_TORRENT);

    QList<QComboBox*> directionComboBoxes = QList<QComboBox*>() <<
                                            pairEndReadsDirectionComboBox <<
                                            hightQualityReadsDirectionComboBox <<
                                            matePairsReadsDirectionComboBox;
    foreach(QComboBox* dirCombo, directionComboBoxes) {
        dirCombo->setItemData(0, QString("fr"));
        dirCombo->setItemData(1, QString("rf"));
        dirCombo->setItemData(2, QString("ff"));
    }
    QList<QComboBox*> typeComboBoxes = QList<QComboBox*>() <<
                                       pairEndReadsTypeComboBox <<
                                       hightQualityReadsTypeComboBox <<
                                       matePairsTypeComboBox;
    foreach(QComboBox* typeCombo, typeComboBoxes) {
        typeCombo->setItemData(0, QString("single reads"));
        typeCombo->setItemData(1, QString("interlaced reads"));
    }

}

bool SpadesPropertyDialog::isSomeRequiredParemeterChecked() const {
    return pairEndCheckBox->isChecked() ||
           hightQualityCheckBox->isChecked() ||
           unpairedReadsCheckBox->isChecked() ||
           pacBioCcsCheckBox->isChecked();
}

bool SpadesPropertyDialog::needRequiredSequencingPlatform() const {
    return pairEndCheckBox->isChecked() ||
           hightQualityCheckBox->isChecked() ||
           unpairedReadsCheckBox->isChecked();
}

bool SpadesPropertyDialog::needAdditionalSequencingPlatform() const {
    return matePairsCheckBox->isChecked();
}

QStringList SpadesPropertyDialog::getDataFromComboBoxes(QComboBox* directionComboBox, QComboBox* typeComboBox) {
    QStringList res;
    foreach(QComboBox* comboBox, QList<QComboBox*>() << directionComboBox << typeComboBox) {
        res << comboBox->currentData().toString();
    }

    return res;
}

void SpadesPropertyDialog::setDataForComboBoxes(QComboBox* directionComboBox, QComboBox* typeComboBox, const QVariant& value) {
    SAFE_POINT(value.canConvert<QString>(), QApplication::translate("SpadesPropertyDialog", "Incorrect parameters, can't parse"), );

    const QString stringValue = value.toString();
    const QStringList valueList = stringValue.split(":");
    SAFE_POINT(valueList.size() == 2, QApplication::translate("SpadesPropertyDialog", "Incorrect parameters, can't parse"), );

    directionComboBox->setCurrentIndex(directionComboBox->findData(valueList.first()));
    typeComboBox->setCurrentIndex(typeComboBox->findData(valueList.last()));
}

} // namespace LocalWorkflow
} // namespace U2
